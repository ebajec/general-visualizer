#ifndef MESH_HPP
#define MESH_HPP

#include "mesh.h"
#include "math.h"
#include "linked_list.h"
#include <forward_list>
#include <unordered_set>
#include <queue>

import misc;

vec3 centroid(vertex* start) {
	int count = 0;
	vec3 sum = { 0,0,0 };
	auto add_to_sum = [&](vertex* v) {
		count++;
		sum = sum + v->position;
	};

	bfs(start, add_to_sum);

	return sum * (1 / ((float)count));
}

void center(vertex* start)
{
	vec3 c = centroid(start);

	auto move_to_origin = [&](vertex* v) {

	};

	bfs(start, move_to_origin);
}

float discrete_laplacian(vertex* v) {
	vec3 centroid = { 0,0,0 };

	for (vertex* w : v->connections) {
		centroid = centroid + w->position;
	}

	centroid = centroid * (1.0f / v->deg());

	vec3 vp = v->position;

	vec3 diff = centroid - vp;
	float val = 0;

	for (vertex* w : v->connections) {
		val += dot(diff, w->position - vp);
	}

	return val / v->deg();
}

vec3 red_blue_hue(float t) {
	return vec3{ (float)sigmoid(t),0,(float)sigmoid(-t) } + vec3{0, 0, 0};
}

void mesh::_init() {
	_vertex_list = vector<vertex*>(0);
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

mesh::mesh() : drawable<VERTEX_ATTRIBUTES>() {
	_init();
}

mesh::mesh(vertex* v_start, bool center_vertices) : drawable<VERTEX_ATTRIBUTES>() {
	_init();
	vector<vertex*> temp_list;
	auto add_to_vertices = [&](vertex* v) {
		temp_list.push_back(v);
	};

	bfs(v_start, add_to_vertices);
	this->_vertex_list = temp_list;

	this->_find_edges();
	this->_find_faces_triangular();
	this->compute_normals();
	if (center_vertices) {
		this->center();
	}
}


template<int n>
mesh::mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices) : drawable<VERTEX_ATTRIBUTES>() {
	_init();

	if (vertices.size() != n) {
		throw std::invalid_argument("Adjacency matrix size must match vertex list.");
	}

	this->_vertex_list = vector<vertex*>(n);

	int i = 0;
	for (vec3 v : vertices) {
		this->_vertex_list[i] = new vertex(v, normalize(v));
		i++;
	}

	for (int i = 0; i < n; i++) {
		for (int j = i; j < n; j++) {
			if (adjacency[i][j]) {
				this->_vertex_list[i]->connect(this->_vertex_list[j]);
			}
		}
	}

	this->_find_edges();
	this->_find_faces_triangular();
	this->compute_normals();
}

template<typename paramFunc>
mesh::mesh(surface<paramFunc> S, int N_s, int N_t) : drawable<VERTEX_ATTRIBUTES>()
{
	_init();

	float s_max = S.s_max();
	float t_max = S.t_max();

	float ds = s_max / N_s;
	float dt = t_max / N_t;

	int size = N_s * N_t;

	this->_vertex_list = vector<vertex*>(size);
	for (int i = 0; i < size; i++) {
		_vertex_list[i] = new vertex({ 0,0,0 }, { 1, 0, 0 });
	}

	for (int i = 0; i < N_s; i++) {
		for (int j = 0; j < N_t; j++) {
			int i_next = modulo(i + 1, N_s);
			int j_next = modulo(j + 1, N_t);

			vertex* current = _vertex_list[i * N_t + j];
			vertex* left = _vertex_list[i_next * N_t + j];
			vertex* behind = _vertex_list[i * N_t + j_next];
			vertex* diag = _vertex_list[i_next * N_t + j_next];

			current->position = S.eval(i * ds, j * dt);
			current->color = hue((i * ds), 0.5);// (j * dt) * 2 * PI / t_max);

			if (i == N_s - 1) {
				//current->connect(left);
				current->connect(behind);
				current->connect(_vertex_list[(N_s - 1) * N_t + N_t - 2]);
			}
			else {
				current->connect(left);
				current->connect(behind);
				current->connect(diag);
			}
		}
	}

	this->_find_edges();
	this->_find_faces_triangular();
	this->compute_normals();
}

mesh::~mesh() {
	for (vertex* v : this->_vertex_list) {
		delete v;
	}
	for (edge* E : this->_edge_list) {
		delete E;
	}
}

void mesh::center() {
	vec3 c = centroid(_vertex_list[0]);
	for (vertex* v : _vertex_list) {
		v->position = v->position - c;
	}
}

unsigned long mesh::_object_count() {
	switch (_type) {
	case LINE:
		return 2 * _edge_list.size();
		break;
	case TRIANGLE:
		return 3 * _face_list.size();
		break;
	}
}

void mesh::init_buffers(GLenum usage) {
	this->drawable<VERTEX_ATTRIBUTES>::init_buffers(usage);
}

void mesh::reinit_buffer(GLenum usage, unsigned int attribute)
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
void mesh::transform_buffer(VERTEX_ATTRIBUTE attribute, func F) {
	float* mem;
	glBindBuffer(GL_ARRAY_BUFFER, (_vbos)[attribute]);
	mem = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, _bufsize(attribute) * sizeof(float), GL_MAP_WRITE_BIT);
	transform_pts_3<func>(mem, _bufsize(attribute) / 3, F);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

template<typename func>
void mesh::transform_vertices(func F) {
	for (vertex* v : this->_vertex_list) {
		F(v);
	}
}

const int VEC_ATTR = 3;
void mesh::_copy_attributes(float** attribute_buffers)
{
	int counter = 0;

	//copy vertex data to respective memory block before copying to buffer
	auto copy_attributes = [&](mesh_elem* E) {
		vector<vertex*> vertices = E->vertex_array();

		for (vertex* v : vertices) {

			//copy vec3 data into buffers
			float* vector_data[VEC_ATTR] = {
				v->position.data(),
				v->normal.data(),
				hue(PI * (float)counter / ((float)3 * _face_list.size()),0).data()
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
		for (edge* E : _edge_list) {
			copy_attributes(E);
		}
		break;

	case TRIANGLE:
		for (face* F : _face_list) {
			copy_attributes(F);
		}
		break;
	}
}

void mesh::_find_faces_triangular() {
	auto common_vertices = [](vertex* v, vertex* w) {
		forward_list<vertex*> common;

		for (vertex* cv : v->connections) {
			for (vertex* cw : w->connections) {
				if (cv == cw) {
					common.push_front(cv);
				}
			}
		}
		return common;
	};

	auto find_adjacent_face = [](vertex* v) {
		vertex* w = *(v->connections.begin());

		for (vertex* u : v->connections) {
			if (u->connections.find(w) != u->connections.end()) {
				return face({ u,v,w });
			}
		}
		return face();
	};

	//try to find at least one vertex with an adjacent triangular face
	auto common = common_vertices(_vertex_list[0], _vertex_list[1]);
	if (std::distance(common.begin(), common.end()) == 0) {
		return;
	}

	face* F_0 = new face({ _vertex_list[0], *common.begin() ,_vertex_list[1] });
	{
		int i = 0;
		while (F_0->size() == 0) {
			*F_0 = find_adjacent_face(_vertex_list[i]);
			i++;
		}
	}

	//throw exception if none exist
	if (F_0->size() == 0) throw std::exception("mesh is not locally planar");

	unordered_set<edge*, edge::Hasher, edge::Comparator> visited_edges;

	queue<face*> face_queue;
	face_queue.push(F_0);
	this->_face_list.push_back(F_0);

	while (!face_queue.empty()) {
		face* F = face_queue.front();
		face_queue.pop();

		//loop through edges of face
		for (edge& E : F->adjacent_edges) {
			//mark them as visited as we go through;
			if (visited_edges.find(&E) == visited_edges.end()) {
				visited_edges.insert(&E);

				vertex* edge_start = E.get(0);
				vertex* edge_end = E.get(1);

				//identify common vertex for endpoints of our edge which is
				//not already part of F
				vertex* common = nullptr;
				forward_list<vertex*> common_list = common_vertices(edge_start, edge_end);
				for (vertex* v : common_list) {
					if (!linear_search(F->data(), F->size(), v)) {
						common = v;
						break;
					}
				}

				//make new face and add to queue if the previous part succeeds
				if (common != nullptr) {
					face* face_new = new face({ edge_start,common,edge_end });
					this->_face_list.push_back(face_new);
					face_queue.push(face_new);
				}
			}
		}
	}

	//this is to save computation time later
	int i = 0;
	for (face* F : _face_list) {
		_face_indices.insert({ F, i });
		i++;
	}

	return;
}

void mesh::_find_edges() {
	vertex* start = _vertex_list[0];
	linked_list<vertex> queue;
	unordered_set<vertex*> visited;

	queue.push(start);
	visited.insert(start);

	while (queue.head != nullptr) {
		vertex* dequeued = (vertex*)queue.pop();

		//Mark connections as visited and add unvisited to queue
		for (vertex* v : dequeued->connections) {
			edge* E = new edge(v, dequeued);
			_edge_list.push_back(E);

			//Only queue the vertex if insert() returns true for new element insertion
			bool new_insertion = visited.insert(v).second;

			if (new_insertion) {
				queue.push(v);
			}
		}

	}
}



void mesh::color_with_curvature() {
	for (vertex* v : this->_vertex_list) {
		v->color = red_blue_hue(6.0f * discrete_laplacian(v));
	}
}

void mesh::compute_normals()
{
	for (face* F : _face_list) {
		F->compute_normal();
	}

	for (vertex* v : _vertex_list) {
		vec3 normal_avg = { 0,0,0 };
		for (face* F : v->adjacent_faces) {
			normal_avg = normal_avg + F->normal;
		}
		v->normal = normalize(normal_avg);
	}



}

void mesh::set_type(ShapeType type)
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