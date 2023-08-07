#pragma once
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <thread>
#include <functional>
#include "camera.h"
#include "drawable.h"


#ifndef VIEW_WINDOW_H
#define VIEW_WINDOW_H

/*
* GLFW window for viewing scenes with a camera.
*/
class BaseViewWindow {
protected:
	GLFWwindow* _window = NULL;
	const GLubyte* _renderer = glGetString(GL_RENDERER);
	const GLubyte* _version = glGetString(GL_VERSION);
	int _height;
	int _width;

	Camera _cam;
	ShaderProgram _main_shader;
	
	//main thread
	thread _main_thread;
	void _launchProcess(
		const char* title,
		GLFWmonitor* monitor,
		GLFWwindow* share);

	virtual void _main() = 0;
public:
	BaseViewWindow(
		int width,
		int height
	);

	void launch(
		const char* title,
		GLFWmonitor* monitor,
		GLFWwindow* share
	);

	void close();

	//void createShader
	
};


#endif