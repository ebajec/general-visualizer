
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "matrix.h"
#include "view_window.h"

import misc;

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1080

enum DIRECTIONS { UP, DOWN, LEFT, RIGHT, FORWARD, BACK };

#include "mesh.h"
class inputViewWindow : public BaseViewWindow {
protected:
	vec3 _motion_dir = { 0,0,0 };

	void _main() {
		_main_shader = ShaderProgram("lib/shader/vertex.glsl", "lib/shader/frag.glsl");
		_cam = Camera(vec3({ 0,-1,1 }), vec3({ 0,5,-5 }), PI / 3);

		Mesh torus_model(Surface([](float s, float t) {

			return Torus(1.0f, 0.5f)(s, t);

			}, 2 * PI, 2 * PI), 1, 50, 50);
		//torus_model.setType(LINE);
		torus_model.transformVertices([](Vertex* v) {
			v->position = rotateyz<GLfloat>(-PI / 2) * v->position;
			});
		torus_model.initBuffers(GL_STREAM_DRAW);

		mat3 R = rotatexy<GLfloat>(PI / 2048);

		auto movtorus = [&]() {
			torus_model.transform(rotateyz<GLfloat>(PI / 12));
			return;
		};

		_key_manager.mapKeyFunc(GLFW_KEY_R, GLFW_REPEAT, movtorus);

		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			torus_model.draw(_main_shader);
			torus_model.transform(R);

			_cam.translate(_motion_dir * 0.01);

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}
	void _mapMovementKeys(int action) {
		int mval = (action == GLFW_PRESS || action == GLFW_REPEAT);
		vec3& dir = _motion_dir;
		_key_manager.mapKeyFunc(GLFW_KEY_W, action, [&dir, mval]() {dir[0][2] = mval; });
		_key_manager.mapKeyFunc(GLFW_KEY_A, action, [&dir, mval]() {dir[0][0] = mval; });
		_key_manager.mapKeyFunc(GLFW_KEY_S, action, [&dir, mval]() {dir[0][2] = -mval; });
		_key_manager.mapKeyFunc(GLFW_KEY_D, action, [&dir, mval]() {dir[0][0] = -mval; });
		_key_manager.mapKeyFunc(GLFW_KEY_LEFT_SHIFT, action, [&dir, mval]() {dir[0][1] = -mval; });
		_key_manager.mapKeyFunc(GLFW_KEY_SPACE, action, [&dir, mval]() {dir[0][1] = mval; });
	}
public:
	inputViewWindow(int width, int height) : BaseViewWindow(width, height) {
		_mapMovementKeys(GLFW_PRESS);
		_mapMovementKeys(GLFW_REPEAT);
		_mapMovementKeys(GLFW_RELEASE);
	}
};


int main() {
	std::cout << "Welcome to random stuff.\n\n";

	inputViewWindow  window(800, 800);

	std::string command;

	while (true) {
		std::cin >> command;

		if (command == "exit") break;

		else if (command == "launch" && !window.isRunning()) window.launch("DONUT", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else std::cout << "invalid command\n";

	}

	return 0;
}
