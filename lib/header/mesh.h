#pragma once
#ifndef MESH_H
#define MESH_H

#include <initializer_list>
#include <vector>
#include <map>
#include <GL/glew.h>
#include "drawable.h"
#include "vertex.h"
#include "matrix.hpp"
#include "shapes.h"

void center(vertex* start);
vec3 centroid(vertex* start);

enum ShapeType { LINE = 2, TRIANGLE = 3 };

class mesh : public drawable<VERTEX_ATTRIBUTES> {

public:
	mesh();
	template<int n>
	mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices);
	mesh(vertex* vertex, bool center_vertices = true);
	template<typename paramFunc>
	mesh(surface<paramFunc> S, int N_s, int N_t);

	~mesh();

	void init_buffers(GLenum usage);
	void reinit_buffer(GLenum usage, unsigned int attribute);
	void center();
	void color_with_curvature();
	void compute_normals();
	void set_type(ShapeType type);

	// F must be a lambda taking in an argument the size of each object in the buffer
	template<typename func> void transform_buffer(VERTEX_ATTRIBUTE attribute, func F);

	//F must be a lamda taking in a pointer to a vertex
	template<typename func> void transform_vertices(func F);

protected:
	vector<vertex*> _vertex_list;
	list<face*> _face_list;
	list<edge*> _edge_list;

	map<face*, int> _face_indices;
	GLuint face_normal_buffer;
	GLuint position_storage_buffer;

	ShapeType _type = TRIANGLE;

	void _init();
	unsigned long _object_count();
	void _find_faces_triangular();
	void _find_edges();
	void _copy_attributes(float** attribute_buffers);
	//void _copy_attribute(vertex* v, float* mem, int offset, unsigned int attribute);
};

#include "mesh.hpp"

#endif