#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include "matrix.h"

//header library for random functions

using mat3 = matrix<3, 3, GLfloat>;
using vec3 = matrix<3, 1, GLfloat>;

inline vec3 hue(float s, float t = 1) {
	return vec3{ 0.5f * cos(t) * cos(s) + 0.5f,0.5f * cos(t) * sin(s) + 0.5f, 0.5f * sin(t) + 0.5f };
}

template<typename T> 
inline int sign(T val) {
	return (val > 0) - (val < 0);
}

inline double sigmoid(float t) {
	return 1 / (1 + exp(t));
}

inline int modulo(int dividend, int divisor) {
	return (dividend % divisor + divisor) % divisor;
}

//Random floating point value between 0 and 1.
inline float uniform_rand(int fineness = RAND_MAX) {
	if (fineness == RAND_MAX) {
		return (float)rand() / (float)RAND_MAX;
	}
	return (float)(rand() % fineness) / (float)fineness;
}

template<typename T> 
inline bool linear_search(T* begin, size_t size, T elem) {
	for (T* p = begin; p < begin + size; p++) {
		if (*p == elem) return true;
	}
	return false;
}

template<typename F, int n> matrix<n, 1, F>
inline centroid(initializer_list<matrix<n, 1, F>> vectors) {
	matrix<n, 1, F> c = { 0,0,0 };
	for (matrix<n, 1, F> v : vectors) {
		c = c + v;
	}

	return c * (1 / (float)vectors.size());
}

template<typename func>
inline void transform_pts_3_lam(float* points, size_t num_pts, func F) {
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

inline void transform_pts_3(float* points, size_t num_pts, mat3 F) {
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

#endif

