
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <complex>
#include "matrix.hpp"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "shapes.h"
#include "point_cloud.h"

import misc;

#ifndef PI
#define PI 3.141592654f
#endif

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1080

float t = 0;

int main() {
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	Shader meshShader("lib/shader/vertex.glsl", "lib/shader/frag.glsl");

	camera cam(vec3({ 0,-0.5,1 }), vec3({ 0,3,-10 }), PI / 3);


	auto torus = [=](float s, float t) {
		float r = 0.75f;
		float R = 3.0f;
		return R * vec3{ sin(s) * cos(t),sin(s) * sin(t), cos(s) };
		/*vec3{ cos(s) * (R + r * cos(t)), sin(s) * (R + r * cos(t)), r * sin(t) };*/
	};

	auto boys = [=](float tf, float rf) {

		std::complex<double> w = (double)tf * exp((double)rf * 1i);
		double rt5 = sqrt(5);

		std::complex<double> g_1 = -3.0f / 2.0f *
			imag(
				w * (1.0 - pow(w, 4)) /
				(pow(w, 6) + rt5 * pow(w, 3) - 1.0)
			);
		std::complex<double> g_2 = -3.0f / 2.0f * real(
			w * (1.0 + pow(w, 4)) /
			(pow(w, 6) + rt5 * pow(w, 3) - 1.0)
		);
		std::complex<double> g_3 = imag(
			(1.0 + pow(w, 6)) /
			(pow(w, 6) + rt5 * pow(w, 3) - 1.0)
		) - 0.5;

		matrix<3, 1, std::complex<double>> v = { g_1,g_2,g_3 };
		v = (1.0 / (g_1 * g_1 + g_2 * g_2 + g_3 * g_3)) * v;

		return vec3{
			(float)real(*v[0]),
			(float)real(*v[1]),
			(float)real(*v[2]) };
	};

	mesh torus_model(surface(torus, PI, 2 * PI), 40, 4);
	torus_model.set_type(LINE);
	torus_model.init_buffers(GL_STREAM_DRAW);
	torus_model.transform(mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
		});
	//torus_model.set_mode(GL_POINTS);
	//torus_model.transform(rotateyz<GLfloat>(-PI / 2));

	mat3 R = rotatexz<GLfloat>(PI / 1024);// *rotateyz<GLfloat>(PI / 512)* rotatexy<GLfloat>(PI / 1024);

	while (!glfwWindowShouldClose(window)) {
		//set up shaders
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		meshShader.use();
		cam.connect_uniforms(meshShader);

		torus_model.draw(meshShader);
		torus_model.transform(R);

		glfwPollEvents();
		glfwSwapBuffers(window);
		//cam.update();
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}


