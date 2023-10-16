#pragma once
#ifndef MESH_H
#define MESH_H

#include <initializer_list>
#include <vector>
#include <map>
#include <GL/glew.h>
#include "drawable.h"
#include "vertex.h"
#include "matrix.h"
#include "shapes.h"

void center(Vertex* start);
vec3 centroid(Vertex* start);

enum ShapeType { LINE = 2, TRIANGLE = 3 };

class Mesh : public Drawable<VERTEX_ATTRIBUTES> {

public:
	Mesh();
	template<int n>
	Mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices);
	Mesh(Vertex* vertex, bool center_vertices = true);
	template<typename paramFunc>
	Mesh(Surface<paramFunc> S, int genus, int N_s, int N_t);

	~Mesh();

	void refreshBuffer(GLenum usage, unsigned int attribute);
	void center();
	void colorCurvature();
	void computeNormals();
	void setType(ShapeType type);

	//checks V - E + F to assess whether meshes are being processed properly
	void checkChar();

	// F must be a lambda taking in an argument the size of each object in the buffer
	template<typename func> void transformBuffer(VERTEX_ATTRIBUTE attribute, func F);

	//F must be a lambda taking in a pointer to a vertex
	template<typename func> void transformVertices(func F);
	unsigned long _pointCount();
protected:
	vector<Vertex*> _vertex_list;
	list<Face*> _face_list;
	list<Edge*> _edge_list;

	map<Face*, int> _face_indices;
	GLuint face_normal_buffer;
	GLuint position_storage_buffer;

	ShapeType _type = TRIANGLE;

	void _init();
	
	void _findFacesTriangular2();
	void _findFacesTriangular();
	void _findEdges();
	void _copyAttributes(float** attribute_buffers);
};

#include "mesh.hpp"

#endif
