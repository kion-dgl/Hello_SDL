#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "lib/dashgl.h"

GLuint vbo_triangle;
GLuint program;
GLint attribute_coord2d, attribute_v_color;
GLint uniform_m_transform;

bool init_resources();
void render(SDL_Window*);
void free_resources();
void main_loop(SDL_Window*);

int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(
		"My Transformed Triangle",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	);

	if(window == NULL) {
		fprintf(stderr, "Error can't create window %s\n", SDL_GetError());
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	main_loop(window);
	free_resources();

	return 0;

}

bool init_resources() {

	GLfloat triangle_vertices[] = {
		 0.0,  0.8, 1.0, 1.0, 0.0,
		-0.8, -0.8, 0.0, 0.0, 1.0,
		 0.8, -0.8, 1.0, 0.0, 0.0
	};

	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(triangle_vertices),
		triangle_vertices,
		GL_STATIC_DRAW
	);

	program = dash_create_program("sdr/vertex.glsl", "sdr/fragment.glsl");
	if(program == 0) {
		fprintf(stderr, "Program creation error\n");
		return false;
	}

	const char *attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1) {
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
		fprintf(stderr, "Could not bind uniform fade\n");
		return false;
	}

	glUseProgram(program);
	glClearColor(1.0, 1.0, 1.0, 1.0);


	return true;

}

void render(SDL_Window *window) {

	glClear(GL_COLOR_BUFFER_BIT);
	
	glEnableVertexAttribArray(attribute_coord2d);
	glEnableVertexAttribArray(attribute_v_color);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5,
		0
	);

	glVertexAttribPointer(
		attribute_v_color,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(float) * 5,
		(void*)(sizeof(float) * 2)
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(attribute_coord2d);
	glDisableVertexAttribArray(attribute_v_color);

	SDL_GL_SwapWindow(window);

}

void free_resources() {
	
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo_triangle);

}

void main_loop(SDL_Window *window) {

	while(true) {

		SDL_Event ev;
		
		while(SDL_PollEvent(&ev)) {

			if(ev.type == SDL_QUIT) {
				return;
			}

		}
		
		float move = sinf(SDL_GetTicks() / 1000.0 * (2*3.14) / 5);
		float angle = SDL_GetTicks() / 1000.0; 
		
		vec3 pos = { move, 0.0, 0.0 };
		vec3 rot = { 0.0, 0.0, angle };

		mat4 mvp, m_pos, m_rot;
		mat4_rotate(rot, m_rot);
		mat4_translate(pos, m_pos);
		mat4_multiply(m_pos, m_rot, mvp);

		glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, mvp);

		render(window);
	}

}


