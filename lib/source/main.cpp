
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "matrix.h"
#include "mesh.h"
#include "shapes.h"
#include "shader.h"
#include "camera.h"
#include "view_window.h"

import misc;

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1080

class inputViewWindow : public BaseViewWindow {
public:
	inputViewWindow(int width,int height) : BaseViewWindow(width, height) {}
protected:
	void _main() {
		_main_shader = ShaderProgram("lib/shader/vertex.glsl", "lib/shader/frag.glsl");
		_cam = Camera(vec3({ 0,-1,1 }), vec3({ 0,5,-5 }), PI / 3);

		Mesh torus_model(Surface([](float s, float t) {

			return Torus(1.0f, 0.5f)(s, t);

			}, 2 * PI, 2 * PI), 1, 50, 50);
		torus_model.setType(LINE);
		torus_model.transformVertices([](Vertex* v) {
			v->position = rotateyz<GLfloat>(-PI / 2) * v->position;
			});
		torus_model.initBuffers(GL_STREAM_DRAW);

		mat3 R = rotatexy<GLfloat>(PI / 4024);

		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			torus_model.draw(_main_shader);
			torus_model.transform(R);

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

};

int main() {
	inputViewWindow  window(800, 800);
	window.launch("DONUT", NULL, NULL);
	
	std::cout << "running\n";

	std::string x;
	while (x != "close") std::cin >> x;
	
	window.close();

	return 0;
}
