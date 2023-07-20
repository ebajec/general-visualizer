#pragma once
#ifndef VERTEX_H
#define VERTEX_H
#include "matrix.hpp"
#include "linked_list.h"
#include <unordered_set>
#include <GL/glew.h>
#define VERTEX_ATTRIBUTES 4

import misc;

enum VERTEX_ATTRIBUTE {
	POSITION,
	NORMAL,
	COLOR,
	ADJACENCY
};

//Vertex
using vec3 = matrix<3, 1, GLfloat>;

struct vertex;
struct mesh_elem;
struct edge;
struct face;

struct vertex {
public:

	struct comparator {
		bool operator () (const vertex* v1, const vertex* v2) const;
	};

	struct hasher {
		size_t operator ()(const vertex* v) const;
	};

	vec3 position;
	vec3 normal;
	vec3 color = { 1.0f,1.0f,1.0f };
	unordered_set<vertex*> connections;
	unordered_set<face*> adjacent_faces;

	vertex() { degree = 0; }
	vertex(const vec3& v, const vec3& normal, const vec3& color = { 1.0f,1.0f,1.0f });
	~vertex();

	void connect(vertex* other);
	void disconnect(vertex* other);
	template<typename... Args>
	void connect(vertex* arg, Args... args);
	template<typename... Args>
	void disconnect(vertex* arg, Args... args);
	//template<typename func>
	//void transform(func f);

	size_t deg() { return degree; }


private:
	size_t degree;
};


//Mesh elements
using mat3 = matrix<3, 3, GLfloat>;

struct mesh_elem {

public:
	struct BaseHasher {
		virtual size_t operator () (const mesh_elem* E) const = 0;
	};
	struct BaseComparator {
		virtual bool operator () (const mesh_elem* E1, const mesh_elem* E2) const = 0;
	};

	vector<vertex*> vertex_array() const { return _vertices; }
	vertex* get(int index) const { return _vertices[index]; }
	int size() const { return _size; }
	vertex** data() const { return (vertex**)_vertices.data(); }

protected:
	int _size;
	vector<vertex*> _vertices;
};

/* Stores connectivity data for an edge
*/
struct edge : public mesh_elem {
public:
	edge() { init(); }
	edge(vertex* first, vertex* second);

	struct Hasher : public BaseHasher {
		size_t operator () (const mesh_elem* E) const;
	};
	struct Comparator : public BaseComparator {
		bool operator () (const mesh_elem* E1, const mesh_elem* E2) const;
	};
private:
	void init();
};


/*Stores connectivity data for a face. */
struct face : public mesh_elem {
	face() { _size = 0; }

	face(std::initializer_list<vertex*> args);

	struct Hasher : public BaseHasher {
		size_t operator () (const mesh_elem* F) const;
	};

	struct Comparator : public BaseComparator {
		bool operator () (const mesh_elem* F1, const mesh_elem* F2) const;
	};

	void compute_normal() {
		vec3 v1 = _vertices[1]->position - _vertices[0]->position;
		vec3 v2 = _vertices[2]->position - _vertices[0]->position;
		normal = cross(v1, v2);
	}

	vector<edge> adjacent_edges;
	vec3 normal = { 0,0,0 };
};

template<typename func>
void bfs(vertex* start, func s) {
	linked_list<vertex> queue;
	unordered_set<vertex*> visited;

	queue.push(start);
	visited.insert(start);

	while (queue.head != nullptr) {
		vertex* dequeued = (vertex*)queue.pop();
		s(dequeued);

		//Mark connections as visited and add unvisited to queue
		for (vertex* v : dequeued->connections) {

			//Only queue the vertex if insert() returns true for new element insertion
			bool new_insertion = visited.insert(v).second;

			if (new_insertion) {
				queue.push(v);
			}
		}

	}
}

template<typename ...Args>
void vertex::connect(vertex* arg, Args ...args)
{
	connect(arg);
	connect(args...);
}

template<typename ...Args>
void vertex::disconnect(vertex* arg, Args ...args)
{
	disconnect(arg);
	disconnect(args...);
}

#endif


