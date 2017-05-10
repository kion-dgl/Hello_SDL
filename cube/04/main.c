#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "lib/dashgl.h"

#define WIDTH 800
#define HEIGHT 600

GLuint program;
GLint attribute_coord3d, attribute_v_color;
GLuint vbo_cube_vertices, ibo_cube_elements;
GLint uniform_lookat, uniform_m_transform;

bool init_resources();
void render(SDL_Window*);
void free_resources();
void main_loop(SDL_Window*);

int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(
		"My Rotating Cube",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	);

	if(window == NULL) {
		fprintf(stderr, "Error can't create window %s\n", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

	if(SDL_GL_CreateContext(window) == NULL) {
		fprintf(stderr, "Error can't create context %s\n", SDL_GetError());
		exit(1);
	}

	GLenum glew_status = glewInit();
	if(glew_status != GLEW_OK) {
		fprintf(stderr, "Error glewInit: %s\n", glewGetErrorString(glew_status));
		exit(1);
	}

	if(!GLEW_VERSION_2_0) {
		fprintf(stderr, "Your GPU does not support OpenGL 2.0\n");
		exit(1);
	}

	if(!init_resources()) {
		exit(1);
	}

	glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	main_loop(window);
	free_resources();

	return 0;

}

bool init_resources() {

	GLfloat cube_vertices[] = {
		-1.0, -1.0,  1.0, 1.0, 0.0, 0.0,
		 1.0, -1.0,  1.0, 0.0, 1.0, 0.0,
		 1.0,  1.0,  1.0, 0.0, 0.0, 1.0,
		-1.0,  1.0,  1.0, 1.0, 1.0, 1.0,
		-1.0, -1.0, -1.0, 1.0, 0.0, 0.0,
		 1.0, -1.0, -1.0, 0.0, 1.0, 0.0,
		 1.0,  1.0, -1.0, 0.0, 0.0, 1.0,
		-1.0,  1.0, -1.0, 1.0, 1.0, 1.0
	};

	GLushort cube_elements[] = {
		// Front
		0, 1, 2,
		2, 3, 0,
		// Top
		1, 5, 6,
		6, 2, 1,
		// Back
		7, 6, 5,
		5, 4, 7,
		// Bottom
		4, 0, 3,
		3, 7, 4,
		// Left
		4, 5, 1,
		1, 0, 4,
		// Right
		3, 2, 6,
		6, 7, 3
	};

	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(cube_vertices),
		cube_vertices,
		GL_STATIC_DRAW
	);

	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(cube_elements), 
		cube_elements, 
		GL_STATIC_DRAW
	);

	program = dash_create_program("sdr/vertex.glsl", "sdr/fragment.glsl");
	if(program == 0) {
		fprintf(stderr, "Program creation error\n");
		return false;
	}

	const char *attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return false;
	}

	attribute_name = "v_color";
	attribute_v_color = glGetAttribLocation(program, attribute_name);
	if(attribute_v_color == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return false;
	}

	const char *uniform_name = "m_transform";
	uniform_m_transform = glGetUniformLocation(program, uniform_name);
	if(uniform_m_transform == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}
	
	/*
	uniform_name = "lookat";
	uniform_lookat = glGetUniformLocation(program, uniform_name);
	if(uniform_lookat == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}
	*/

	glUseProgram(program);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	mat4 perspective, lookat, link;
	
	/*
	vec3 eye = { 0.0f, 2.0f, 0.0f };
	vec3 target = { 0.0f, 0.0f, -4.0f };
	vec3 axis = { 0.0f, 1.0f, 0.0f };
	
	mat4_perspective(45.0f, 1.0f*WIDTH/HEIGHT, 0.1f, 100.0f, perspective);
	mat4_look_at(eye, target, axis, lookat);
	mat4_multiply(perspective, lookat, link);
	*/
	
	/*
	mat4_identity(link);
	glUniformMatrix4fv(uniform_lookat, 1, GL_FALSE, link);
	*/

	return true;

}

void render(SDL_Window *window) {
	
	int size;

	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnableVertexAttribArray(attribute_coord3d);
	glEnableVertexAttribArray(attribute_v_color);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glVertexAttribPointer(
		attribute_coord3d,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 6,
		0
	);

	glVertexAttribPointer(
		attribute_v_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 6,
		(void*)(sizeof(float) * 3)
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_v_color);

	SDL_GL_SwapWindow(window);

}

void free_resources() {
	
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_cube_vertices);

}

void main_loop(SDL_Window *window) {

	while(true) {

		SDL_Event ev;
		
		while(SDL_PollEvent(&ev)) {

			if(ev.type == SDL_QUIT) {
				return;
			}

		}
		
		float angle = SDL_GetTicks() / 1000.0 * 45.0f;; 
		
		float rad = angle * M_PI / 180.0;
	
	/*
	vec3 eye = { 0.0f, 2.0f, 0.0f };
	vec3 target = { 0.0f, 0.0f, -4.0f };
	vec3 axis = { 0.0f, 1.0f, 0.0f };
	vec3 t = { 0.0, 0.0, -4.0f };

	mat4 mvp, pos, rot, projection, view;
	mat4_identity(mvp);
	mat4_perspective(45.0f, 1.0f*WIDTH/HEIGHT, 0.1f, 10.0f, projection);
	mat4_look_at(eye, target, axis, view);
	mat4_translate(t, pos);
	mat4_rotate_y(rad, rot);

	mat4_multiply(mvp, projection, mvp);
	mat4_multiply(mvp, view, mvp);
	mat4_multiply(mvp, pos, mvp);
	mat4_multiply(mvp, rot, mvp);

	glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, mvp);
	*/

		vec3 pos = { 0.0, 0.0, 2.0 };
		vec3 rot = { 0.0, angle, 0.0 };

		mat4 mvp, m_pos, m_rot;
		mat4_rotate(rot, m_rot);
		mat4_translate(pos, m_pos);
		mat4_multiply(m_pos, m_rot, mvp);
	mat4 mtx = {
1.337,                                                                                       
0.084,                                                                                      
0.096,                                                                                       
0.094,                                                                                       
0.000,                                                                                       
1.603,                                                                                       
-0.456,                                                                                      
-0.447,                                                                                      
0.141,                                                                                       
-0.797,                                                                                      
-0.907,
-0.889,
0.000,
0.000,
4.360,
4.472
};
		glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, mtx);

		render(window);
	}

}


