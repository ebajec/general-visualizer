#pragma once
#include "matrix.hpp"
#include "shader.h"
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 

#ifndef CAMERA_H
#define CAMERA_H


using vec3 = matrix<3, 1, GLfloat>;
using vec4 = matrix<4, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;
using mat4 = matrix<4, 4, GLfloat>;

struct camera {
public:
	camera() { this->near_dist = 0; this->far_dist = 0; }

	camera(
		vec3 normal,
		vec3 pos,
		GLfloat focus = 1,
		GLfloat far = 100,
		int w_screen = 1920,
		int h_screen = 1080
	);

	void connect_uniforms(const Shader& shader);

	void set_velocity(vec3 v) {
		velocity = v;
	}
	void update() {
		*position[2] = *position[2] / 1.001f;
		view_mat = (
			change_of_basis |
			-1 * (change_of_basis * (position - basis[2] * near_dist))).transpose() |
			vec4(0.0f);
	}

private:
	int _w_screen;
	int _h_screen;

	vec3 velocity;

	GLfloat near_dist;
	GLfloat far_dist;
	vec3 position;
	vec3 basis[3];
	mat3 change_of_basis;
	mat4 model_mat;
	mat4 view_mat;
	mat4 projection_2D;
};

#endif