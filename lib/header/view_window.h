#pragma once

#ifndef VIEW_WINDOW_H
#define VIEW_WINDOW_H
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include <thread>
#include <functional>
#include "camera.h"
#include "shader.h"
#include "drawable.h"
#include <map>


class KeyManager {
private:
	map<pair<int, int>, std::function<void()>> keymap;
public:
	KeyManager() {}
	//maps function to key action
	void mapKeyFunc(int key, int action, std::function<void()> func) { keymap.insert({ {key,action},func }); }
	//calls function for key action, if one exists  
	void callKeyFunc(int key, int action);
};

/*
* GLFW window for viewing scenes with a camera.
*/
class BaseViewWindow {
protected:
	GLFWwindow* _window = NULL;

	int _height;
	int _width;
	bool _is_running = false;

	Camera _cam;
	ShaderProgram _main_shader;
	KeyManager _key_manager;

	//main thread
	thread _main_thread;

	//Intializes window, runs main loop until close, then terminates OpenGL context
	void _windowProgram(
		const char* title,
		GLFWmonitor* monitor,
		GLFWwindow* share);

	//Main loop of program. 
	virtual void _main() = 0;

	static void _keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	BaseViewWindow(
		int width,
		int height
	);

	bool isRunning() { return _is_running; }

	void launch(
		const char* title,
		GLFWmonitor* monitor,
		GLFWwindow* share
	);

	void close();

	//void createShader

};






#endif