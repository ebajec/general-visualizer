
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
#include "mesh.h"
#include "misc.h"
 
using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;

#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH 960 

typedef std::complex<double> C;

class programWindow : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");

		//disk 
		Mesh disk(Surface([](float s, float t) {
			std::complex<double> z = (double)s*exp(((double)t)*1i);

			return vec3{(float)real(z),0,(float)imag(z)};

		}, 1, 2*PI), 1, 100, 100);

		//morphed disk
		Mesh graph(Surface([](float s, float t) {
			std::complex<double> z = (double)(s)*exp(((double)t)*1i);
			std::complex<double> f = (z-(C)1)*((C)2*z*z+2i)/(z-(C)2);

			return 10.0f*vec3{(float)real(f),(float)real(z),(float)imag(f)};

			}, 1.0f, 2*PI), 1, 100, 100);
		graph.setType(LINE);
		graph.initBuffers(GL_STREAM_DRAW);

		disk.initBuffers(GL_STREAM_DRAW);

		//this is only to test out keyboard input 
		auto movshape = [&]() {
			graph.transformAffine(rotateyz<GLfloat>(PI / 12));
			return;
		};
		_key_manager.map(GLFW_KEY_R, GLFW_PRESS, movshape);

		//main loop
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);
		
			disk.draw(_main_shader);
			graph.draw(_main_shader);

			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
		
		//if a key mapping references out-of-scope variables, it will break
		//the program. So we must make sure to unmap any keys bound here.
		_key_manager.unmap(GLFW_KEY_R, GLFW_PRESS);
	}

	
public:
	//one day something will be here
	programWindow(int width, int height) : BaseViewWindow(width, height) {}
};


int main() {
	std::cout << "Welcome.\n\n";

	programWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

	std::string command;

	while (true) {
		std::cin >> command;

		if (command == "exit") {
			window.close();
			return 0;
		}

		else if (command == "launch" && !window.isRunning()) window.launch("test", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else std::cout << "invalid command\n";

	}

	return 0;
}
