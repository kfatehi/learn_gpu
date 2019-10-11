#include <iostream>
//#include "kernel.cuh"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include "getopt.h"
#include "linmath.h"

using namespace std;

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec2 vPos;\n"
"varying vec2 texcoord;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    texcoord = vPos;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"uniform sampler2D texture;\n"
"uniform vec3 color;\n"
"varying vec2 texcoord;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color * texture2D(texture, texcoord).rgb, 1.0);\n"
"}\n";

static const vec2 vertices[4] =
{
	{ 0.f, 0.f },
	{ 1.f, 0.f },
	{ 1.f, 1.f },
	{ 0.f, 1.f }
};

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// setup the window hints

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLuint texture, program, vertex_buffer;
	GLint mvp_location, vpos_location, color_location, texture_location;

	// create window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == nullptr) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);

	// Only enable vsync for the first of the windows to be swapped to
	// avoid waiting out the interval for each window
	glfwSwapInterval(1);

	
	// The contexts are created with the same APIs so the function
	// pointers should be re-usable between them
	//GLADloadproc *loadProc = (GLADloadproc) gladLoadGLLoader;
	//;
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

	int x, y;
	char pixels[16 * 16];
	GLuint vertex_shader, fragment_shader;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	srand((unsigned int)glfwGetTimerValue());

	for (y = 0; y < 16; y++)
	{
		for (x = 0; x < 16; x++)
			pixels[y * 16 + x] = rand() % 256;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 16, 16, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");
	color_location = glGetUniformLocation(program, "color");
	texture_location = glGetUniformLocation(program, "texture");
	vpos_location = glGetAttribLocation(program, "vPos");

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glUseProgram(program);
	glUniform1i(texture_location, 0);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);
	
	while (!glfwWindowShouldClose(window))
	{
		int width, height;
		mat4x4 mvp;
		const vec3 colors = { 0.8f, 0.4f, 1.f };

		glfwGetFramebufferSize(window, &width, &height);
		glfwMakeContextCurrent(window);

		glViewport(0, 0, width, height);

		mat4x4_ortho(mvp, 0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
		glUniform3fv(color_location, 1, colors);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glfwSwapBuffers(window);

		glfwWaitEvents();
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
	return 0;
}