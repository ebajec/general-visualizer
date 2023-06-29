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

	vec3 position;
	GLfloat near_dist;
	GLfloat far_dist;
	vec3 basis[3];
	mat3 change_of_basis;
	mat4 projection_matrix_2D;

	camera() { this->near_dist = 0; this->far_dist = 0; }

	camera(vec3 normal, vec3 pos, GLfloat focus = 1, GLfloat far = 100);

	void connect_uniforms(const Shader& shader);

	//void rotate(GLfloat pitch, GLfloat yaw) {

	//	mat3 rotation = rotatex<GLfloat, GLfloat>(-yaw)* rot_axis(this->basis[1], pitch);

	//	for (int i = 0; i < 3; i++) {
	//		basis[i] = rotation * basis[i];
	//	}

	//	//this makes it so the focal point serves as the 'joint' for rotation
	//	this->pos = focal_point + planenorm * focal_dist;
	//	this->cam_plane = hyperplane<R3>(planenorm);
	//}
};

#endif