
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "matrix.hpp"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "lattice.h"

import misc;

#ifndef PI
#define PI 3.141592654f;
#endif

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

camera cam(vec3({ 0,-0.2,-1 }), vec3({ 0,2,30 }), PI / 3);


int main() {
	// start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(1000, 1000, "test", NULL, NULL);
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

	Shader myShader("lib/shader/camera_projection.glsl", "lib/shader/frag.glsl");

	auto penisify = [](vec3 v) {
		float x, y, z;
		x = v[0][0]; y = v[1][0]; z = v[2][0];

		float r = sqrt(x * x + y * y + z * z);

		//normalize
		/*x = x / r;
		y = y / r;
		z = z / r;*/

		//transform
		return vec3{ x + 0.03f * sin(y), y - 0.03f * sin(x),z + 0.02f * sin(x * y) };
	};

	auto smooth = [](Vertex* v) {
		vec3 temp = v->position;
		float x, y, z;
		x = temp[0][0]; y = temp[1][0]; z = temp[2][0];

		float r = sqrt(x * x + y * y + z * z);

		//normalize
		x = x / r;
		y = y / r;
		z = z / r;

		//transform
		v->position = 5 * vec3{ x, y, z };
	};

	Mesh ball1(generate_polygon(15));
	ball1.center();
	ball1.transform_vertices(smooth);
	ball1.compute_normals();
	ball1.color_with_curvature();
	ball1.initialize_buffers(GL_STREAM_DRAW, TRIANGLE);
	ball1.transform_geometry(rotateyz<GLfloat>(PI / 2));

	mat3 R = rotatexz<GLfloat>(PI / 128);

	while (!glfwWindowShouldClose(window)) {
		//set up shaders
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		myShader.use();
		cam.connect_uniforms(myShader);

		ball1.transform_geometry(R);
		ball1.transform_buffer(POSITION, penisify);

		ball1.draw();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}


