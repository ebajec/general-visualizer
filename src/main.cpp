
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

std::complex<double> to_complex(float a, float b) {
	return ((double)a) + ((double)b)*1i;
}

//This is a small example program where I graph part of the imaginary of a mobius
//transformation on a disk in the complex plane.
class programWindow : public BaseViewWindow {
protected:
	void _main() {
		_main_shader = ShaderProgram("../shader/vertex.glsl", "../shader/frag.glsl");

		//random function
		Mesh graph(Surface([=](float s, float t) {
			C z = to_complex(s-1,t-1);
			return 10*vec3{(float)real(z),0,(float)imag(z)};

			},2, 2), 1, 100, 100);

		//graph.setType(LINE);
		graph.initBuffers(GL_STREAM_DRAW);
		graph.checkChar();
		draw_count = graph._pointCount();
		//graph.setMode(GL_POINTS);

		auto mobius = [=](matrix<2,2,C> M, C z) {
			return (M[0][0]*z + M[0][1])/(M[1][0]*z + M[1][1]);
		};

		//main loop
		while (!glfwWindowShouldClose(_window)) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			_main_shader.use();
			_cam.connectUniforms(_main_shader);

			graph.draw(_main_shader,draw_count);

			graph.transformPositionsCPU([&](vec3 v) {
				C z = to_complex(*v[0],*v[2]);
				C f = mobius(this->mobius_mat,z);
				return vec3{(float)real(z),(float)imag(f),(float)imag(z)};
			});
			
			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
	}

	
public:
	//this is just for debugging and lets you specify how many primitives to draw
	size_t draw_count = 0;

	matrix<2,2,C> mobius_mat = {
			2,-5,
			3,1
		};
	//one day something will be here
	programWindow(int width, int height) : BaseViewWindow(width, height){}
};


int main() {
	std::cout << "Welcome.\n\n";

	programWindow  window(WINDOW_WIDTH, WINDOW_HEIGHT);

	std::string command;

	//move the poles of the mobius transformation
	auto move_mobius = [&]() {
		while (true) {
			window.mobius_mat = {
			1,1,
			1,(C)2*cos(glfwGetTime())+ (C)2*sin(glfwGetTime())*1i
			};
		}
		return;
	};

	std::thread mobius_mover(move_mobius);

	while (true) {
	
		std::cin >> command;

		if (command == "exit") {
			window.close();
			return 0;
		}

		else if (command == "launch" && !window.isRunning()) window.launch("test", NULL, NULL);

		else if (command == "close" && window.isRunning()) window.close();

		else if (command == "drawcount") {
			int val;
			std::cin >> val;
			window.draw_count = val; 
		}

		else std::cout << "invalid command\n";

	}

	return 0;
}
