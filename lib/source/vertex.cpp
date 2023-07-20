//#ifndef VERTEX_HPP
//#define VERTEX_HPP
#include "vertex.h"

vertex::vertex(const vec3& v, const vec3& normal, const vec3& color)
{
	this->color = color;
	this->position = v;
	this->normal = normal;
	this->degree = 0;
}

vertex::~vertex() {
	for (vertex* v : connections) {
		if (v != nullptr) v->connections.erase(this);
	}

	for (face* F : adjacent_faces) {
		for (vertex* v : F->vertex_array()) {
			if (v != this) v->adjacent_faces.erase(F);
		}
	}
}



void vertex::connect(vertex* other)
{
	//check to avoid incrementing degree unnecessarily
	if (this == other || this->connections.find(other) != this->connections.end()) {
		return;
	}
	else {
		this->connections.insert(other);
		other->connections.insert(this);
		this->degree++;
		other->degree++;
	}
	return;
}

void vertex::disconnect(vertex* other)
{
	//check to avoid decrementing degree unnecessarily
	if (this->connections.find(other) == this->connections.end()) {
		return;
	}
	else {
		this->connections.erase(other);
		other->connections.erase(this);
		this->degree--;
	}
	return;
}

bool vertex::comparator::operator () (const vertex* v1, const vertex* v2) const {
	vec3 pos1 = v1->position;
	vec3 pos2 = v2->position;
	for (int i = 0; i < 3; i++) {
		if (abs(pos1[0][i] - pos2[0][i]) > 0.01) {
			return false;
		}
	}
	return true;
}

size_t vertex::hasher::operator ()(const vertex* v) const {
	vec3 pos = v->position;
	size_t seed = 3;
	for (int i = 0; i < 3; i++) {
		seed ^= std::hash<GLfloat>()(pos[0][i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

//EDGE
void edge::init() {
	this->_size = 2;
	this->_vertices = vector<vertex*>(2);
}

edge::edge(vertex* first, vertex* second) {
	init();
	this->_vertices[0] = first;
	this->_vertices[1] = second;
}

size_t edge::Hasher::operator () (const mesh_elem* E) const {
	vec3 pos_a = E->get(0)->position;
	vec3 pos_b = E->get(1)->position;
	size_t seed = 3;
	for (int i = 0; i < 3; i++) {
		seed ^= std::hash<GLfloat>()(pos_a[0][i] * pos_b[0][i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

bool edge::Comparator::operator () (const mesh_elem* E1, const mesh_elem* E2) const {
	return (E1->get(0) == E2->get(0) && E1->get(1) == E2->get(1)) || (E1->get(0) == E2->get(0) && E1->get(1) == E2->get(0));
}

//FACE
face::face(std::initializer_list<vertex*> args) {
	this->_size = args.size();
	this->_vertices = vector<vertex*>(_size);
	this->adjacent_edges = vector<edge>(_size);

	int i = 0;
	for (vertex* v : args) {
		_vertices[i] = v;
		v->adjacent_faces.insert(this);
		i++;
	}
	for (int i = 0; i < _size; i++) {
		adjacent_edges[i] = edge(_vertices[i], _vertices[(i + 1) % (_size)]);
	}
}

size_t face::Hasher::operator () (const mesh_elem* F) const {
	size_t seed = 3;

	for (int i = 0; i < F->size(); i++) {
		seed ^= std::hash<GLfloat>()(F->get(i)->position[0][0]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}



bool face::Comparator::operator () (const mesh_elem* F1, const mesh_elem* F2)  const {
	if (F1->size() != F2->size()) return false;

	int size = F1->size();
	vertex** data_F2 = F2->data();

	for (int i = 0; i < size; i++) {
		if (!linear_search<vertex*>(data_F2, size, F1->get(i))) {
			return false;
		}
	}

	return true;
}


//#endif

