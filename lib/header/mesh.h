#pragma once
#ifndef MESH_H
#define MESH_H

#include <initializer_list>
#include <vector>
#include <GL/glew.h>
#include "vertex.h"
#include "GL/glew.h"
#include "matrix.hpp"

void center(Vertex* start);
vec3 centroid(Vertex* start);

class Mesh {

public:
	Mesh();
	template<int n>
	Mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices);
	Mesh(Vertex* vertex);

	~Mesh();

	void initialize_buffers(GLenum usage, ShapeType type);
	void update_buffer(VERTEX_ATTRIBUTE attribute);
	void center();
	void draw();
	void color_with_curvature();
	void compute_normals();

	void transform_geometry(mat3 A);
	void transform_buffer(int buffer, mat3 A);
	template<typename func> void transform_buffer(int buffer, func F);
	template<typename func> void transform_vertices(func F);

	unsigned int face_count() { return this->_face_list.size(); }
	size_t buffer_size(int index) { return this->_buffer_sizes[index]; }


private:
	vector<Vertex*> _vertex_list;
	list<Face*> _face_list;
	list<Edge*> _edge_list;

	size_t _buffer_sizes[ATTRIBUTES];
	GLuint vao;
	GLuint vbos[ATTRIBUTES];
	GLuint face_normal_buffer;
	GLuint position_storage_buffer;

	unsigned int _num_vertices = 0;
	unsigned int _draw_mode = NULL;

	void _init();
	void _find_faces_triangular();
	void _find_edges();
};



template<typename func>
void Mesh::transform_buffer(int buffer, func F) {
	float* mem;
	glBindBuffer(GL_ARRAY_BUFFER, (vbos)[buffer]);
	mem = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, _buffer_sizes[buffer] * sizeof(float), GL_MAP_WRITE_BIT);
	transform_pts_3<func>(mem, _buffer_sizes[buffer] / 3, F);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

template<typename func>
void Mesh::transform_vertices(func F) {
	for (Vertex* v : this->_vertex_list) {
		F(v);
	}
}

#endif