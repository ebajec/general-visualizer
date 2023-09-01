
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "matrix.h"
#include "view_window.h"
#include "math.h"

import misc;

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920

struct CameraManager {
	vec3 _motion_dir;


};

#include "mesh.h"
class inputViewWindow : public BaseViewWindow {
protected:
	vec3 _motion_dir = { 0,0,0 };

	void _main() {
		_main_shader = ShaderProgram("lib/shader/vertex.glsl", "lib/shader/frag.glsl");
		Mesh torus_model(Surface([](float s, float t) {
			
			return vec3{(s)*cos(t),sin(s*erf(s))*sin(5*t),(s)*sin(t)};

			}, 2*PI, 2 * PI), 1, 100, 300);
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

		_key_manager.mapKeyFunc(GLFW_KEY_R, GLFW_PRESS, movtorus);

		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			torus_model.draw(_main_shader);
			//torus_model.transform(R);

			_cam.translate(_motion_dir * 0.01);

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	void _mapMovementKeys() {
		auto map_keys = [this](int action) {
			int mval = (action == GLFW_PRESS) - (action == GLFW_RELEASE);
			vec3& dir = this->_motion_dir;
			this->_key_manager.mapKeyFunc(GLFW_KEY_W, action, [&dir, mval]() {dir[0][2] += mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_A, action, [&dir, mval]() {dir[0][0] += mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_S, action, [&dir, mval]() {dir[0][2] += -mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_D, action, [&dir, mval]() {dir[0][0] += -mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_LEFT_SHIFT, action, [&dir, mval]() {dir[0][1] += -mval; });
			this->_key_manager.mapKeyFunc(GLFW_KEY_SPACE, action, [&dir, mval]() {dir[0][1] += mval; });
		};
		map_keys(GLFW_PRESS);
		map_keys(GLFW_RELEASE);
		return;
	}
public:
	inputViewWindow(int width, int height) : BaseViewWindow(width, height) {
		_mapMovementKeys();
		
	}
};


int main() {
	std::cout << "Welcome to random stuff.\n\n";

	inputViewWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

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
