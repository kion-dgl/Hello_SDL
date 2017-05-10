/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */

#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include "SDL.h"
#include "lib/dashgl.h"

int screen_width=800, screen_height=600;
GLuint vbo_cube_vertices, vbo_cube_colors;
GLuint ibo_cube_elements;
GLuint program;
GLint attribute_coord3d, attribute_v_color;
GLint uniform_mvp;

bool init_resources();
void logic();
void render(SDL_Window* window);
void onResize(int width, int height);
void free_resources();
void mainLoop(SDL_Window* window);

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow(
		"My Rotating Cube",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		screen_width, 
		screen_height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	);

	if (window == NULL) {
		fprintf(stderr, "Cannot create window\n");
		return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	if (SDL_GL_CreateContext(window) == NULL) {
		fprintf(stderr, "Cannot create context");
		return EXIT_FAILURE;
	}

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		fprintf(stderr, "Error: glewInit: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}
	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
		return EXIT_FAILURE;
	}
	
	if (!init_resources())
		return EXIT_FAILURE;

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mainLoop(window);
	
	free_resources();
	return EXIT_SUCCESS;

}

bool init_resources() {

	GLfloat cube_vertices[] = {
		// front
		-1.0, -1.0,  1.0, 1.0, 0.0, 0.0,
		 1.0, -1.0,  1.0, 0.0, 1.0, 0.0,
		 1.0,  1.0,  1.0, 0.0, 0.0, 1.0,
		-1.0,  1.0,  1.0, 1.0, 1.0, 1.0, 
		// back
		-1.0, -1.0, -1.0, 1.0, 0.0, 0.0,
		 1.0, -1.0, -1.0, 0.0, 1.0, 0.0,
		 1.0,  1.0, -1.0, 0.0, 0.0, 1.0,
		-1.0,  1.0, -1.0, 1.0, 1.0, 1.0
	};

	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	GLushort cube_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
	
	GLint link_ok = GL_FALSE;
	program = dash_create_program("sdr/vertex.glsl", "sdr/fragment.glsl");
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram\n");
		return false;
	}
	
	const char* attribute_name;
	attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute 1\n");
		return false;
	}
	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if (attribute_v_color == -1) {
		fprintf(stderr, "Could not bind attribute 2\n");
		return false;
	}
	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if (uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform\n");
		return false;
	}
	
	glUseProgram(program);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	return true;

}

void logic() {
	
	GLfloat mtx[] = {
		1.341,
		0.056,
		0.064,
		0.062,
		0.000,
		1.603,
		-0.456,
		-0.447,
		0.094,
		-0.800,
		-0.910,
		-0.892,
		0.000,
		0.000,
		4.360,
		4.472
	};

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, mtx);

}

void render(SDL_Window* window) {
	
	int size;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);

	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		sizeof(float)*6,                 // no extra data between each position
		0                  // offset of first element
	);
	
	glVertexAttribPointer(
		attribute_v_color, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		sizeof(float)*6,                 // no extra data between each position
		(void*)(sizeof(float) * 3)                  // offset of first element
	);
	
	/* Push each element in buffer_vertices to the vertex shader */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	
	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);
	SDL_GL_SwapWindow(window);

}

void onResize(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}

void free_resources() {

	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_cube_vertices);
	glDeleteBuffers(1, &ibo_cube_elements);

}


void mainLoop(SDL_Window* window) {
	while (true) {

		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			
			if (ev.type == SDL_QUIT) {
				return;
			}

			if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
				onResize(ev.window.data1, ev.window.data2);
			}

		}

		logic();
		render(window);
	}

}

