#pragma once
#include "matrix.h"
#include "shader.h"
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 

#ifndef CAMERA_H
#define CAMERA_H
#define PI 3.141592654f

using vec3 = matrix<3, 1, GLfloat>;
using vec4 = matrix<4, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;
using mat4 = matrix<4, 4, GLfloat>;

struct Camera {
public:
	Camera() { this->_near_dist = 0; this->_far_dist = 0; }

	Camera(
		vec3 normal,
		vec3 pos,
		int w_screen = 1920,
		int h_screen = 1080,
		GLfloat focus = PI / 3,
		GLfloat far = 100
	);

	void connectUniforms(const ShaderProgram& shader);
	void rotate(float pitch, float yaw);
	void translate(vec3 delta);
private:
	int _w_screen;
	int _h_screen;

	GLfloat _near_dist;
	GLfloat _far_dist;

	vec3 _pos;
	vec3 basis[3];
	mat3 change_of_basis;
	mat4 _model;
	mat4 _view;
	mat4 _proj;

	void _updateViewMat() {
		_view = mat4(change_of_basis | -1 * (change_of_basis * (_pos - basis[2] * _near_dist)));
	}
};



#endif