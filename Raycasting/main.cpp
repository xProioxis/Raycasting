#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
# define PI 3.14159265359
# define P2 PI/2
# define P3 3*PI/2
# define DR 0.0174533

const GLuint WIDTH = 1024, HEIGHT = 512;
float px = 300, py = 300, p_angle = 0, pdelta_x = cos(p_angle)*5, pdelta_y = sin(p_angle)*5; // p for player

int map_x = 8, map_y = 8, mapS = 64;
int map[] =
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 1, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1
};

float dist(float x1, float y1, float x2, float y2, float angle)
{
	return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));
}

void draw_map_2D()
{
	int x, y, xo, yo;
	for (y = 0; y < map_y; y++)
	{	
		for (x = 0; x < map_x; x++)
		{
			if (map[(y * map_x) + x] == 1) { glColor3f(1, 1, 1); }
			else { glColor3f(0, 0, 0); }; // to draw the wall, or to not draw the wall, that is the question
			xo = x * mapS; yo = y * mapS;
			glBegin(GL_QUADS);
			glVertex2i(0 + xo + 1, 0 + yo + 1);
			glVertex2i(0 + xo + 1, mapS + yo - 1);
			glVertex2i(mapS + xo - 1, mapS + yo - 1);
			glVertex2i(mapS + xo - 1, 0 + yo + 1);


			glEnd();
		}
	}
}

void draw_rays_2D()
{
	int r, mx, my, mp, dof; // depth of field 
	float rx, ry, hx, hy, vx, vy, r_angle, x_offset, y_offset, distance_h, distance_v, distance_final; // r for ray
	r_angle = p_angle;
	r_angle = p_angle - DR * 30;
	if (r_angle < 0) { r_angle += 2 * PI; }
	if (r_angle > 2 * PI) { r_angle -= 2 * PI; }
	//if (ra > 2*PI) 
	for (r = 0; r < 60; r++)
	{
		// ---- Check Horizontal Lines -----
		dof = 0;
		float distance_h = 1000000, hx = px, hy = px;
		float aTan = -1 / tan(r_angle);
		// next line get the nearest 64th value by bitshifing right 6 (dividing by 64) then bitshifting left (multiplying by 64)
		if (r_angle > PI) { // looking down (aka more than pi)
			ry = (((int)py >> 6) << 6) - 0.0001; 
			rx = (py - ry) * aTan + px; // calculating the initial ry, rx using the players angle and arctan, y is the nearest 64th number location
			y_offset = -64; /*-64 corresponds to next line on map */ 
			x_offset = -y_offset * aTan; // offset can be just -64, must be ratioed by aTan(angle), 
		}	
		if (r_angle < PI) { // looking up (aka less than pi)
			ry = (((int)py >> 6) << 6) + 64 ; 
			rx = (py - ry) * aTan + px; // calculating the initial ry, rx using the players angle and arctan, y is the nearest 64th number location
			y_offset = 64; /*-64 corresponds to next line on map */ 
			x_offset = -y_offset * aTan; // offset can be just 64, must be ratioed by aTan(angle)
		}
		if (r_angle == 0 || r_angle == PI) { rx = px; ry = py; dof = 8; } // looking staright left or right (can never hit a horizonatal line)
		while (dof < 8)
		{	
			mx = (int)(rx) >> 6; 
			my=(int)(ry) >> 6; 
			mp = (my * map_x) + mx; // mp is the coords, remember >> 6 is just dividing by 64
			if (0 < mp && mp < map_x * map_y && map[mp] == 1) 
			{ 
				hx = rx; hy = ry; // horizontal ray
				distance_h = dist(px, py, hx, hy, r_angle);
				dof = 8;
			}
			else { rx += x_offset; ry += y_offset; dof += 1; } // just at the the points, retains ration between x and y (angle), no need to recalulate
		}

		/*HlColor3f(0.05, 0.9, 1);
		glLineWidth(10);
		glBegin(GL_LINES);
		glVertex2i(px, py); // starts on player
		glVertex2i(rx, ry); // ends where loop ended
		glEnd();*/
		
		
		// ---- Check vertical Lines -----
		dof = 0;
		distance_v = 1000000; vx = px; vy = px;
		float nTan = -tan(r_angle); // negative tangent 
		// next line get the nearest 64th value by bitshifing right 6 (dividing by 64) then bitshifting left (multiplying by 64)
		if (r_angle > P2 && r_angle < P3) { // looking left (aka more than pi) 
			rx = (((int)px >> 6) << 6) - 0.0001; 
			ry = (px - rx) * nTan + py; // calculating the initial ry, rx using the players angle and arctan, y is the nearest 64th number location
			x_offset = -64; /*-64 corresponds to next line on map */ 
			y_offset = -x_offset * nTan; // offset can be just -64, must be ratioed by nTan(angle)
		}
		if (r_angle < P2 || r_angle > P3) { // looking right (aka less than pi)
			rx = (((int)px >> 6) << 6) + 64; 
			ry = (px - rx) * nTan + py; // calculating the initial ry, rx using the players angle and arctan, y is the nearest 64th number location
			x_offset = 64; /*-64 corresponds to next line on map */ 
			y_offset = -x_offset * nTan; // offset can be just 64, must be ratioed by nTan(angle)
		}
		if (r_angle == 0 || r_angle == PI) { rx = px; ry = py; dof = 8; } // looking up or down (can never hit a vertical line)
		while (dof < 8)
		{
			mx = (int)(rx) >> 6; 
			my = (int)(ry) >> 6; 
			mp = (my * map_x) + mx; // mp is the coords, remember >> 6 is just dividing by 64
			if (0 < mp && mp < map_x * map_y && map[mp] == 1) 
			{ 
				vx = rx; vy = ry; // vertical ray
				distance_v = dist(px, py, vx, vy, r_angle);
				dof = 8; 
			}
			else { rx += x_offset; ry += y_offset; dof += 1; } // just at the the points, retains ration between x and y (angle), no need to recalulate
		}


		if (distance_v < distance_h) { rx = vx; ry = vy; distance_final = distance_v; glColor3f(0.9, 0, 0);}
		// next line aka (distance_h < distance_v)
		else  { rx = hx; ry = hy; distance_final = distance_h; glColor3f(0.75, 0, 0);
		}
		
	
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2i(px, py); // starts on player
		glVertex2i(rx, ry); // ends where loop ended
		glEnd();

		// ---- Draw 3d Walls ----
		float cast_angle = p_angle - r_angle;
		if (cast_angle < 0) { cast_angle += 2 * PI; }
		if (cast_angle > 2 * PI) { cast_angle -= 2 * PI; }
		distance_final *= cos(cast_angle);
		float line_h = (mapS * 320) / distance_final;
		float line_offset = 160 - line_h / 2;
		if (line_h > 320) { line_h = 320; }
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(r * 8 + 530, line_offset);
		glVertex2i(r * 8 + 530, line_h+line_offset);
		glEnd();

		r_angle += DR;
		if (r_angle < 0) { r_angle += 2 * PI; }
		if (r_angle > 2 * PI) { r_angle -= 2 * PI; }
	}

	
}

void norm_rgb(float& r, float& g, float& b)
{
	r = r / 255.0f;
	g = g / 255.0f;
	b = b / 255.0f;
}

void draw_player()
{	
	float r = 23.0f,g = 61.0f, b = 1.0f;
	norm_rgb(r, g, b);
	glColor3f(r, g, b);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px, py);
	glEnd();

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px, py);
	glVertex2i(px + pdelta_x * 5, py + pdelta_y * 5);
	glEnd();
}

void display(GLFWwindow* main_window)
{
	glClearColor(0.3f, 0.3f, 0.3f,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw_map_2D();
	draw_rays_2D();
	draw_player();
	glfwSwapBuffers(main_window);
}

void buttons(GLFWwindow* window, int key, int scancode, int x, int y)
{
	switch (key)
	{
	case(GLFW_KEY_A):
		p_angle -= 0.1; if (p_angle < 0) { p_angle += 2 * PI; }
		pdelta_x = cos(p_angle) * 5; pdelta_y = sin(p_angle) * 5;
		break;
	case(GLFW_KEY_D):
		p_angle += 0.1; if (p_angle > 2*PI) { p_angle -= 2 * PI; }
		pdelta_x = cos(p_angle) * 5; pdelta_y = sin(p_angle) * 5;
		break;
	case(GLFW_KEY_W):
		px += pdelta_x; py += pdelta_y; break; // angle is add because u move in the direction ur facing
		// cos gives u the appropriate x distance to move, sin gives appropriate y distance to move
	case(GLFW_KEY_S):
		px -= pdelta_x; py -= pdelta_y;  break;
	}
}

int main()
{
	glfwInit();
	

	int bufferWidth, bufferHeight;
	GLFWwindow* main_window = glfwCreateWindow(WIDTH, HEIGHT, "Raycasting", NULL, NULL);
	glfwGetFramebufferSize(main_window, &bufferWidth, &bufferHeight); // retrives size in pixels, stores in bufferwidth and budderheight

	// Set Context for GLEW to use
	glfwMakeContextCurrent(main_window);
	//glViewport(0, 0, bufferHeight, bufferHeight);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1024, 512, 0, -1, 1);

	while (!glfwWindowShouldClose(main_window))
	{
		glfwPollEvents();
		glfwSetKeyCallback(main_window, buttons);
		display(main_window);
	}

}