#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include "matrix.hpp"

export module misc;

using mat3 = matrix<3, 3, GLfloat>;
using vec3 = matrix<3, 1, GLfloat>;

export vec3 hue(float s, float t = 1) {
	return vec3{ 0.5f * cos(t) * cos(s) + 0.5f,0.5f * cos(t) * sin(s) + 0.5f, 0.5f * sin(t) + 0.5f };
}

export template<typename T> int sign(T val) {
	return (val > 0) - (val < 0);
}

export double sigmoid(float t);

export int modulo(int dividend, int divisor) {
	return (dividend % divisor + divisor) % divisor;
}

//Random floating point value between 0 and 1.
export float uniform_rand(int fineness = RAND_MAX) {
	if (fineness == RAND_MAX) {
		return (float)rand() / (float)RAND_MAX;
	}
	return (float)(rand() % fineness) / (float)fineness;
}


export template<typename T> bool linear_search(T* begin, size_t size, T elem) {
	for (T* p = begin; p < begin + size; p++) {
		if (*p == elem) return true;
	}
	return false;
}

export template<typename F, int n> matrix<n, 1, F>
centroid(initializer_list<matrix<n, 1, F>> vectors) {
	matrix<n, 1, F> c = { 0,0,0 };
	for (matrix<n, 1, F> v : vectors) {
		c = c + v;
	}

	return c * (1 / (float)vectors.size());
}

export template<typename func>
void transform_pts_3(float* points, int num_pts, func F) {
	int n = 3;
	for (int i = 0; i < num_pts; i++) {
		vec3 p(points + i * n);
		p = F(p);
		for (int j = 0; j < n; j++) {
			points[i * n + j] = p[0][j];
		}
	}
	return;
}

export void transform_pts_3(float* points, int num_pts, mat3 F) {
	int n = 3;
	for (int i = 0; i < num_pts; i++) {
		vec3 p(points + i * n);
		p = F * p;
		for (int j = 0; j < n; j++) {
			points[i * n + j] = p[0][j];
		}
	}
	return;
}


