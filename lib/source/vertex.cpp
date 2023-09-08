//#ifndef VERTEX_HPP
//#define VERTEX_HPP
#include "vertex.h"

Vertex::Vertex(const vec3& v, const vec3& normal, const vec3& color)
{
	this->color = color;
	this->position = v;
	this->normal = normal;
	this->degree = 0;
}

Vertex::~Vertex() {
	for (Vertex* v : connections) {
		if (v != nullptr) v->connections.erase(this);
	}

	for (Face* F : adjacent_faces) {
		for (Vertex* v : F->vertexArray()) {
			if (v != this) v->adjacent_faces.erase(F);
		}
	}
}



void Vertex::connect(Vertex* other)
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

void Vertex::disconnect(Vertex* other)
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

bool Vertex::comparator::operator () (const Vertex* v1, const Vertex* v2) const {
	vec3 pos1 = v1->position;
	vec3 pos2 = v2->position;
	for (int i = 0; i < 3; i++) {
		if (abs(pos1[0][i] - pos2[0][i]) > 0.01) {
			return false;
		}
	}
	return true;
}

size_t Vertex::hasher::operator ()(const Vertex* v) const {
	vec3 pos = v->position;
	size_t seed = 3;
	for (int i = 0; i < 3; i++) {
		seed ^= std::hash<GLfloat>()(pos[0][i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

//EDGE
void Edge::init() {
	this->_size = 2;
	this->_vertices = vector<Vertex*>(2);
}

Edge::Edge(Vertex* first, Vertex* second) {
	init();
	this->_vertices[0] = first;
	this->_vertices[1] = second;
}

size_t MeshElem::Hasher::operator () (const MeshElem* E) const {

	size_t pointer_sum = 0;
	//modulo UNINT64_MAX/size is to avoid overflow
	for (Vertex* v : E->vertexArray()) pointer_sum += (size_t)v % (UINT64_MAX / E->size());

	return pointer_sum;
}

bool Edge::Comparator::operator () (const MeshElem* E1, const MeshElem* E2) const {
	return (E1->get(0) == E2->get(0) && E1->get(1) == E2->get(1)) || (E1->get(0) == E2->get(1) && E1->get(1) == E2->get(0));
}

//FACE
Face::Face(std::initializer_list<Vertex*> args) {
	this->_size = args.size();
	this->_vertices = vector<Vertex*>(_size);
	this->adjacent_edges = vector<Edge>(_size);

	int i = 0;
	for (Vertex* v : args) {
		_vertices[i] = v;
		v->adjacent_faces.insert(this);
		i++;
	}
	for (int i = 0; i < _size; i++) {
		adjacent_edges[i] = Edge(_vertices[i], _vertices[(i + 1) % (_size)]);
	}
}


bool Face::Comparator::operator () (const MeshElem* F1, const MeshElem* F2)  const {
	if (F1->size() != F2->size()) return false;

	int size = F1->size();
	Vertex** data_F2 = F2->data();

	for (int i = 0; i < size; i++) {
		if (!linear_search<Vertex*>(data_F2, size, F1->get(i))) {
			return false;
		}
	}

	return true;
}


//#endif

