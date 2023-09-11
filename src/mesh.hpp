#ifndef MESH_HPP
#define MESH_HPP

#include "mesh.h"
#include "math.h"
#include "linked_list.h"
#include <forward_list>
#include <unordered_set>
#include <queue>
#include <functional>
#include "misc.h"

vec3 centroid(Vertex* start) {
	int count = 0;
	vec3 sum = { 0,0,0 };
	auto add_to_sum = [&](Vertex* v) {
		count++;
		sum = sum + v->position;
	};

	bfs(start, add_to_sum);

	return sum * (1 / ((float)count));
}

void center(Vertex* start)
{
	vec3 c = centroid(start);

	auto move_to_origin = [&](Vertex* v) {

	};

	bfs(start, move_to_origin);
}

float discrete_laplacian(Vertex* v) {
	vec3 centroid = { 0,0,0 };

	for (Vertex* w : v->connections) {
		centroid = centroid + w->position;
	}

	centroid = centroid * (1.0f / v->deg());

	vec3 vp = v->position;

	vec3 diff = centroid - vp;
	float val = 0;

	for (Vertex* w : v->connections) {
		val += dot(diff, w->position - vp);
	}

	return val / v->deg();
}

vec3 red_blue_hue(float t) {
	return vec3{ (float)sigmoid(t),0,(float)sigmoid(-t) } + vec3{0, 0, 0};
}

void Mesh::_init() {
	_vertex_list = vector<Vertex*>(0);
	_draw_mode = GL_TRIANGLES;

	//define primitive sizes for each vertex buffer and create empty array for each one
	_primitive_sizes[POSITION] = 3;
	_primitive_sizes[NORMAL] = 3;
	_primitive_sizes[COLOR] = 3;
	_primitive_sizes[ADJACENCY] = 3 * _face_list.size();

	for (int i = 0; i < VERTEX_ATTRIBUTES; i++) {
		_layout_map[i] = i;
	}
}

Mesh::Mesh() : Drawable<VERTEX_ATTRIBUTES>() {
	_init();
}

Mesh::Mesh(Vertex* v_start, bool center_vertices) : Drawable<VERTEX_ATTRIBUTES>() {
	_init();
	vector<Vertex*> temp_list;
	auto add_to_vertices = [&](Vertex* v) {
		temp_list.push_back(v);
	};

	bfs(v_start, add_to_vertices);
	this->_vertex_list = temp_list;

	this->_findEdges();
	this->_findFacesTriangular();
	this->computeNormals();
	if (center_vertices) {
		this->center();
	}
}


template<int n>
Mesh::Mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices) : Drawable<VERTEX_ATTRIBUTES>() {
	_init();

	if (vertices.size() != n) {
		throw std::invalid_argument("Adjacency matrix size must match vertex list.");
	}

	this->_vertex_list = vector<Vertex*>(n);

	int i = 0;
	for (vec3 v : vertices) {
		this->_vertex_list[i] = new Vertex(v, normalize(v));
		i++;
	}

	for (int i = 0; i < n; i++) {
		for (int j = i; j < n; j++) {
			if (adjacency[i][j]) {
				this->_vertex_list[i]->connect(this->_vertex_list[j]);
			}
		}
	}

	this->_findEdges();
	this->_findFacesTriangular();
	this->computeNormals();
}


template<typename paramFunc>
Mesh::Mesh(Surface<paramFunc> S, int genus, int N_s, int N_t) : Drawable<VERTEX_ATTRIBUTES>()
{
	_init();

	float s_max = S.sMax();
	float t_max = S.tMax();

	float ds = s_max / N_s;
	float dt = t_max / N_t;

	//number of vertices
	int size;

	//quotient map
	std::function<pair<int, int>(int, int)> quot;

	//index converter
	std::function<int(pair<int, int>)> indexer;

	auto gen_vertices = [&](int size) {
		_vertex_list = vector<Vertex*>(size);
		for (Vertex*& v : _vertex_list) v = new Vertex({ 0,0,0 }, { 1, 0, 0 });
	};

	switch (genus) {
		//equivalent to sphere
	case 2:
		size = (N_s - 1) * N_t + 2; // +2 for bottom point and top point (i.e., infinity)
		gen_vertices(size);
		_vertex_list[1]->position = S.eval(s_max, t_max); //top

		quot = [=](int i, int j) {
			return pair<int, int>(i, modulo(j, N_t));
		};

		indexer = [=](pair<int, int> ind) {
			int i = ind.first;
			return (i == N_s) + (i != N_s) * (i != 0) * (2 + (i - 1) * N_t + ind.second);
		};

		break;
	case 1:
		size = N_s * N_t;
		gen_vertices(size);
		quot = [=](int i, int j) {
			return pair<int, int>(i - (i >= N_s), N_t - 1 - modulo(j, N_t));
		};

		indexer = [=](pair<int, int> ind) {
			return ind.first * N_t + ind.second;
		};

		break;

		//equivalent to torus
	case 0:
		size = N_s * N_t;
		gen_vertices(size);

		quot = [=](int i, int j) {
			return pair<int, int>((i != N_s) * i, (j != N_t) * j);
		};

		indexer = [=](pair<int, int> ind) {
			return ind.first * N_t + ind.second;
		};

		break;
	}

	for (int i = 0; i < N_s; i++) {
		for (int j = 0; j < N_t; j++) {
			auto current_index = quot(i, j);

			Vertex* current = _vertex_list[indexer(current_index)];
			Vertex* next_s = _vertex_list[indexer(quot(i + 1, j))];
			Vertex* next_t = _vertex_list[indexer(quot(i, j + 1))];
			Vertex* diag = _vertex_list[indexer(quot(i + 1, j + 1))];

			float s = current_index.first * ds;
			float t = current_index.second * dt;

			current->position = S.eval(s, t);
			current->color = hue(s, t * 2 * PI / t_max);

			current->connect(next_s);
			current->connect(next_t);
			current->connect(diag);
		}
	}

	this->_findEdges();
	this->_findFacesTriangular();
	this->computeNormals();
}

Mesh::~Mesh() {
	for (Vertex* v : this->_vertex_list) {
		delete v;
	}
	for (Edge* E : this->_edge_list) {
		delete E;
	}
}

void Mesh::center() {
	vec3 c = centroid(_vertex_list[0]);
	for (Vertex* v : _vertex_list) {
		v->position = v->position - c;
	}
}

unsigned long Mesh::_objectCount() {
	switch (_type) {
	case LINE:
		return 2 * _edge_list.size();
		break;
	case TRIANGLE:
		return 3 * _face_list.size();
		break;
	}
	return 0;
}

void Mesh::initBuffers(GLenum usage) {
	this->Drawable<VERTEX_ATTRIBUTES>::initBuffers(usage);
}

void Mesh::reinitBuffer(GLenum usage, unsigned int attribute)
{

	//float* mem = new float[_primitive_sizes[attribute] * _object_count](0);

	//int counter = 0;
	//auto copy_attributes = [&](mesh_elem* E) {
	//	vector<vertex*> vertices = E->vertex_array();

	//	for (vertex* v : vertices) {
	//		_copy_attribute(v, mem, counter, attribute);
	//		counter++;
	//	}
	//};

	//switch (_draw_mode) {
	//case LINE:
	//	for (edge* E : _edge_list) {
	//		copy_attributes(E);
	//	}
	//	break;

	//case TRIANGLE:
	//	for (face* F : _face_list) {
	//		copy_attributes(F);
	//	}
	//	break;
	//}

	//glBindBuffer(GL_ARRAY_BUFFER, _vbos[attribute]);
	//glBufferData(GL_ARRAY_BUFFER, _bufsize(attribute) * sizeof(float), mem, usage);
	//delete[] mem;
}

template<typename func>
void Mesh::transformBuffer(VERTEX_ATTRIBUTE attribute, func F) {
	float* mem;
	glBindBuffer(GL_ARRAY_BUFFER, (_vbos)[attribute]);
	mem = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, _bufSize(attribute) * sizeof(float), GL_MAP_WRITE_BIT);
	transform_pts_3<func>(mem, _bufSize(attribute) / 3, F);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

template<typename func>
void Mesh::transformVertices(func F) {
	for (Vertex* v : this->_vertex_list) {
		F(v);
	}
}

const int VEC_ATTR = 3;
void Mesh::_copyAttributes(float** attribute_buffers)
{
	int counter = 0;

	//copy vertex data to respective memory block before copying to buffer
	auto copy_attributes = [&](MeshElem* E) {
		vector<Vertex*> vertices = E->vertexArray();

		for (Vertex* v : vertices) {

			//copy vec3 data into buffers
			float* vector_data[VEC_ATTR] = {
				v->position.data(),
				v->normal.data(),
				v->color.data()//hue(PI * (float)counter / ((float)3 * _face_list.size()),0).data()
			};

			for (int i = 0; i < VEC_ATTR; i++) {
				copy(vector_data[i], vector_data[i] + 3, attribute_buffers[i] + 3 * counter);
			}

			//copy vertex-face adjacency data
			//for (face* C : v->adjacent_faces) {
			//	//attribute_buffers[ADJACENCY][counter * _face_list.size() + _face_indices[C]] = 1.0f;
			//}
			counter++;
		}
	};

	switch (_type) {
	case LINE:
		for (Edge* E : _edge_list) {
			copy_attributes(E);
		}
		break;

	case TRIANGLE:
		for (Face* F : _face_list) {
			copy_attributes(F);
		}
		break;
	}
}

void Mesh::_findFacesTriangular() {
	auto common_vertices = [](Vertex* v, Vertex* w) {
		forward_list<Vertex*> common;

		for (Vertex* cv : v->connections) {
			for (Vertex* cw : w->connections) {
				if (cv == cw) {
					common.push_front(cv);
				}
			}
		}
		return common;
	};

	auto find_adjacent_face = [](Vertex* v) {
		Vertex* w = *(v->connections.begin());

		for (Vertex* u : v->connections) {
			if (u->connections.find(w) != u->connections.end()) {
				return Face({ u,v,w });
			}
		}
		return Face();
	};

	//try to find at least one vertex with an adjacent triangular face
	int temp = 1;
	auto common = common_vertices(_vertex_list[0], _vertex_list[temp]);

	while (std::distance(common.begin(), common.end()) == 0) {
		if (temp == _vertex_list.size()) return;

		common = common_vertices(_vertex_list[0], _vertex_list[temp]);
		temp++;
	}

	Face* F_0 = new Face({ _vertex_list[0], *common.begin() ,_vertex_list[temp] });
	{
		int i = 0;
		while (F_0->size() == 0) {
			*F_0 = find_adjacent_face(_vertex_list[i]);
			i++;
		}
	}

	//throw exception if none exist
	if (F_0->size() == 0) return;

	unordered_set<Edge*, Edge::Hasher, Edge::Comparator> visited_edges;

	queue<Face*> face_queue;
	face_queue.push(F_0);
	this->_face_list.push_back(F_0);

	while (!face_queue.empty()) {
		Face* F = face_queue.front();
		face_queue.pop();

		//loop through edges of face
		for (Edge& E : F->adjacent_edges) {
			//mark them as visited as we go through;
			if (visited_edges.find(&E) == visited_edges.end()) {
				visited_edges.insert(&E);

				Vertex* edge_start = E.get(0);
				Vertex* edge_end = E.get(1);

				//identify common vertex for endpoints of our edge which is
				//not already part of F
				Vertex* common = nullptr;
				forward_list<Vertex*> common_list = common_vertices(edge_start, edge_end);
				for (Vertex* v : common_list) {
					if (!linear_search(F->data(), F->size(), v)) {
						common = v;
						break;
					}
				}

				//make new face and add to queue if the previous part succeeds
				if (common != nullptr) {
					Face* face_new = new Face({ edge_start,common,edge_end });
					this->_face_list.push_back(face_new);
					face_queue.push(face_new);
				}
			}
		}
	}

	//this is to save computation time later
	int i = 0;
	for (Face* F : _face_list) {
		_face_indices.insert({ F, i });
		i++;
	}

	return;
}

void Mesh::_findEdges() {
	Vertex* start = _vertex_list[0];
	queue<Vertex*> queue;
	unordered_set<Vertex*> visited;
	unordered_set<Vertex*> prev_dequeued;

	queue.push(start);
	visited.insert(start);

	while (!queue.empty()) {
		Vertex* dequeued = (Vertex*)queue.front();
		queue.pop();
		prev_dequeued.insert(dequeued);

		//Mark connections as visited and add unvisited to queue
		for (Vertex* v : dequeued->connections) {
			if (prev_dequeued.find(v) == prev_dequeued.end()) {
				Edge* E = new Edge(v, dequeued);
				_edge_list.push_back(E);
			}

			//Only queue the vertex if insert() returns true for new element insertion
			bool new_insertion = visited.insert(v).second;

			if (new_insertion) {
				queue.push(v);

			}
		}

	}
}



void Mesh::colorCurvature() {
	for (Vertex* v : this->_vertex_list) {
		v->color = red_blue_hue(6.0f * discrete_laplacian(v));
	}
}

void Mesh::computeNormals()
{
	for (Face* F : _face_list) {
		F->computeNormal();
	}

	for (Vertex* v : _vertex_list) {
		vec3 normal_avg = { 0,0,0 };
		for (Face* F : v->adjacent_faces) {
			normal_avg = normal_avg + F->normal;
		}
		v->normal = normalize(normal_avg);
	}



}

void Mesh::setType(ShapeType type)
{
	this->_type = type;

	switch (type) {
	case TRIANGLE:
		_draw_mode = GL_TRIANGLES;
		break;
	case LINE:
		_draw_mode = GL_LINES;
		break;
	}
}


#endif