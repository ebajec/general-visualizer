#pragma once
#ifndef VERTEX_H
#define VERTEX_H
#include "matrix.h"
//#include "linked_list.h"
#include <queue>
#include "misc.h"
#include <unordered_set>
#include <GL/glew.h>
#include <vector>


#define VERTEX_ATTRIBUTES 4

using namespace std;

enum VERTEX_ATTRIBUTE {
	POSITION,
	NORMAL,
	COLOR,
	ADJACENCY
};

//Vertex
using vec3 = matrix<3, 1, GLfloat>;

struct Vertex;
struct MeshElem;
struct Edge;
struct Face;

struct Vertex {
public:

	struct comparator {
		bool operator () (const Vertex* v1, const Vertex* v2) const;
	};

	struct hasher {
		size_t operator ()(const Vertex* v) const;
	};

	vec3 position;
	vec3 normal;
	vec3 color = { 1.0f,1.0f,1.0f };
	unordered_set<Vertex*> connections;
	unordered_set<Face*> adjacent_faces;

	Vertex() { degree = 0; }
	Vertex(const vec3& v, const vec3& normal, const vec3& color = { 1.0f,1.0f,1.0f });
	~Vertex();

	void connect(Vertex* other);
	void disconnect(Vertex* other);
	template<typename... Args>
	void connect(Vertex* arg, Args... args);
	template<typename... Args>
	void disconnect(Vertex* arg, Args... args);
	//template<typename func>
	//void transform(func f);

	size_t deg() { return degree; }


private:
	size_t degree;
};


//Mesh elements
using mat3 = matrix<3, 3, GLfloat>;

struct MeshElem {

public:
	vector<Vertex*> vertexArray() const { return _vertices; }
	Vertex* get(int index) const { return _vertices[index]; }
	int size() const { return _size; }
	Vertex** data() const { return (Vertex**)_vertices.data(); }

	struct Hasher {
		size_t operator () (const MeshElem* E) const;
	};

protected:
	int _size;
	vector<Vertex*> _vertices;
};

/* Stores connectivity data for an edge
*/
struct Edge : public MeshElem {
public:
	Edge() { init(); }
	Edge(Vertex* first, Vertex* second);

	struct Comparator {
		bool operator () (const MeshElem* E1, const MeshElem* E2) const;
	};
private:
	void init();
};


/*Stores connectivity data for a face. */
struct Face : public MeshElem {
	Face() { _size = 0; }

	Face(std::initializer_list<Vertex*> args);

	struct Comparator {
		bool operator () (const MeshElem* F1, const MeshElem* F2) const;
	};

	void computeNormal() {
		vec3 v1 = _vertices[1]->position - _vertices[0]->position;
		vec3 v2 = _vertices[2]->position - _vertices[0]->position;
		normal = cross(v1, v2);
	}

	vector<Edge> adjacent_edges;
	vec3 normal = { 0,0,0 };
};

template<typename func>
void bfs(Vertex* start, func s) {
	std::queue<Vertex*> queue;
	unordered_set<Vertex*> visited;

	queue.push(start);
	visited.insert(start);

	while (!queue.empty()) {
		Vertex* dequeued = (Vertex*)queue.front();
		queue.pop();
		s(dequeued);

		//Mark connections as visited and add unvisited to queue
		for (Vertex* v : dequeued->connections) {

			//Only queue the vertex if insert() returns true for new element insertion
			bool new_insertion = visited.insert(v).second;

			if (new_insertion) {
				queue.push(v);
			}
		}

	}
}

template<typename ...Args>
void Vertex::connect(Vertex* arg, Args ...args)
{
	connect(arg);
	connect(args...);
}

template<typename ...Args>
void Vertex::disconnect(Vertex* arg, Args ...args)
{
	disconnect(arg);
	disconnect(args...);
}

#endif


