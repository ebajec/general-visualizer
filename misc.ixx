#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include "linked_list.h"
#include "matrix.hpp"

export module misc;

using mat3 = matrix<3, 3, GLfloat>;
using vec3 = matrix<3, 1, GLfloat>;

export template<typename T> int sign(T val) {
	return (val > 0) - (val < 0);
}

export double sigmoid(float t);

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

export void transform_pts_3(float* points, int num_pts, mat3 A) {
	int n = 3;
	for (int i = 0; i < num_pts; i++) {
		vec3 p(points + i * n);
		p = A * p;
		for (int j = 0; j < n; j++) {
			points[i * n + j] = p[0][j];
		}
	}
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
}
