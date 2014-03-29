#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "log.h"
#include "assert.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
int width = 800;
int height = 600;

void _update_fps_counter(GLFWwindow* window) {
	static double previous = glfwGetTime();
	static int frame;
	double current = glfwGetTime();
	double elapsed = current - previous;
	if (elapsed > 0.25) {
		previous = current;
		double fps = (double) frame / elapsed;
		char tmp[128];
		sprintf_s(tmp, 128, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame = 0;
	}
	frame++;
}

void glfw_resize(GLFWwindow* window, int w, int h) {
	height = h;
	width = w;
	glViewport(0, 0, width, height);
}

char* fileToChar(const char* filename) {
	long length;
	ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		length = in.tellg();
		char* buffer = new char[length + 1];
		in.seekg(0, std::ios::beg);
		in.read(buffer, length);
		in.close();
		buffer[length] = 0;
		return buffer;
	}
	throw(errno);
}

int main(int argc, char **argv) {
	restart_gl_log();
	char message[256];
	sprintf_s(message, 256, "starting GLFW %s", glfwGetVersionString());
	assert(gl_log(message, __FILE__, __LINE__));
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "error: could not start GLFW3\n");
		return 1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", NULL, NULL);
	if (!window) {
		fprintf(stderr, "error: could not create window\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, glfw_resize);
	glewExperimental = GL_TRUE;
	glewInit();
	log_gl_params();

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("renderer: %s\n", renderer);
	printf("OpenGL version: %s\n", version);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float points [] = {
		-0.25f, 0.25f, 0.f,
		-0.25f, -0.25f, 0.f,
		0.25f, 0.25f, 0.f,
		0.25f, -0.25f, 0.f
	};

	float points2 [] = {
		0.25f, 0.75f, 0.f,
		0.25f, 0.25f, 0.f,
		0.75f, 0.75f, 0.f,
		0.75f, 0.25f, 0.f
	};

	GLuint vbo[2];
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points2, GL_STATIC_DRAW);
	

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	GLuint vao2 = 1;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	const char* vertexShader = fileToChar("../ogl/vertex.glsl");
	const char* fragmentShader = fileToChar("../ogl/frag.glsl");
	const char* fragmentShader2 = fileToChar("../ogl/frag1.glsl");
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShader, NULL);
	glCompileShader(vs);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShader, NULL);
	glCompileShader(fs);
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);

	GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs2, 1, &fragmentShader2, NULL);
	glCompileShader(fs2);
	GLuint shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, fs2);
	glAttachShader(shaderProgram2, vs);
	glLinkProgram(shaderProgram2);

	glClearColor(0.6, 0.6, 0.6, 1.0);
	while (!glfwWindowShouldClose(window)) {
		_update_fps_counter(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUseProgram(shaderProgram2);
		glBindVertexArray(vao2);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwPollEvents();
		glfwSwapBuffers(window);
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)){
			glfwSetWindowShouldClose(window, 1);
		}
	}

	glfwTerminate();
	return 0;
}