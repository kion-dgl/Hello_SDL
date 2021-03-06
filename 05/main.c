#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include "SDL.h"
#include "lib/dashgl.h"

#define WIDTH 800
#define HEIGHT 600

GLuint program;
GLint attribute_coord3d, attribute_v_color;
GLuint vbo_cube_vertices, ibo_cube_elements;
GLint uniform_perspective, uniform_lookat, uniform_mvp;

bool init_resources();
void render(SDL_Window*);
void logic();
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
	// SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

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
	glEnable(GL_DEPTH_TEST);
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
	glLinkProgram(program);

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

	const char *uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if(uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}

	uniform_name = "lookat";
	uniform_lookat = glGetUniformLocation(program, uniform_name);
	if(uniform_lookat == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return false;
	}

	uniform_name = "perspective";
	uniform_perspective = glGetUniformLocation(program, uniform_name);
	if (uniform_perspective == -1) {
		fprintf(stderr, "Could not bind uniform: %s\n", uniform_name);
		return false;
	}

	glUseProgram(program);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	
	vec3 eye = { 0.0f, 2.0f, 0.0f };
	vec3 target = { 0.0f, 0.0f, -4.0f };
	vec3 axis = { 0.0f, 1.0f, 0.0f };

	mat4 projection, view;
	mat4_perspective(45.0f, 1.0f*WIDTH/HEIGHT, 0.1f, 10.0f, projection);
	mat4_look_at(eye, target, axis, view);

	glUniformMatrix4fv(uniform_perspective, 1, GL_FALSE, projection);
	glUniformMatrix4fv(uniform_lookat, 1, GL_FALSE, view);

	return true;

}

void logic() {
	
	float angle = SDL_GetTicks() / 1000.0;

	mat4 mvp, pos, rot;

	vec3 t = { 0.0, 0.0, -4.0f };
	mat4_translate(t, pos);
	mat4_rotate_y(angle, rot);

	mat4_identity(mvp);
	mat4_multiply(mvp, pos, mvp);
	mat4_multiply(mvp, rot, mvp);

	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, mvp);

}

void render(SDL_Window *window) {
	
    int size;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(attribute_coord3d);
    glEnableVertexAttribArray(attribute_v_color);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);

    glVertexAttribPointer(
        attribute_coord3d,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float)*6,
        0
    );

    glVertexAttribPointer(
        attribute_v_color,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float)*6,
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
    glDeleteBuffers(1, &ibo_cube_elements);

}

void main_loop(SDL_Window *window) {

    while (true) {

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {

            if (ev.type == SDL_QUIT) {
                return;
            }

        }

        logic();
        render(window);
    }

}


