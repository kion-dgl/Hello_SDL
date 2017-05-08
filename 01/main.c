#include <stdio.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>

GLuint program;
GLint attribute_coord2d;

bool init_resources();
void render(SDL_Window*);
void free_resources();
void main_loop(SDL_Window*);

int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(
		"My First Triangle",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
	);
	SDL_GL_CreateContext(window);

	GLenum glew_status = glewInit();
	if(glew_status != GLEW_OK) {
		fprintf(stderr, "Error: glewInit %s\n", glewGetErrorString(glew_status));
		exit(1);
	}
	
	if(!init_resources()) {
		fprintf(stderr, "Could not initialize resources\n");
		exit(1);
	}

	main_loop(window);

	free_resources();
	return 0;

}

bool init_resources() {
	
	GLint compile_ok = GL_FALSE;
	GLint link_ok = GL_FALSE;

	// Initialize Shader Version

	const char* version;
	int profile;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);

	if(profile == SDL_GL_CONTEXT_PROFILE_ES) {
		version = "#version 100\n"; // OpenGL ES 2.0
	} else {
		version = "#version 120\n"; // OpenGL 2.1
	}

	// Vertex Shader

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	
	const char *vs_source[] = {
		version,
		"attribute vec2 coord2d;                   "
		"void main (void) {                        "
		"	gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}                                         "
	};

	glShaderSource(vs, 2, vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if(!compile_ok) {
		fprintf(stderr, "Error in vertex shader\n");
		return false;
	}

	// Fragment Shader
	
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fs_source[] = {
		version,
		"void main(void) {                        "
		"gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0); "
		"}                                        "
	};

	glShaderSource(fs, 2, fs_source, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if(!compile_ok) {
		fprintf(stderr, "Error in fragment shader\n");
		return false;
	}

	// Create Program

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok) {
		fprintf(stderr, "Error linking program\n");
		return false;
	}

	// Bind attribute

	const char *attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return false;
	}

	// Initialize Clear color

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glUseProgram(program);

	return true;

}

void render(SDL_Window* window) {

	glClear(GL_COLOR_BUFFER_BIT);

	glEnableVertexAttribArray(attribute_coord2d);
	GLfloat triangle_vertices[] = {
		 0.0,  0.8,
		-0.8, -0.8,
		 0.8, -0.8
	};

	glVertexAttribPointer(
		attribute_coord2d,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		triangle_vertices
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);

	SDL_GL_SwapWindow(window);


}

void free_resources() {

	glDeleteProgram(program);

}

void main_loop(SDL_Window* window) {
	
	while(true) {
		SDL_Event ev;

		while(SDL_PollEvent(&ev)) {
			
			if(ev.type == SDL_QUIT) {
				return;
			}

			render(window);
		}

	}

}


