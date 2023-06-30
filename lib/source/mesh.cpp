//#ifndef MESH_HPP
//#define MESH_HPP

#include "mesh.h"
#include "math.h"
#include "linked_list.h"
#include <forward_list>
#include <unordered_set>
#include <map>
#include <queue>
import misc;

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

//MESH
void Mesh::_init() {
	glGenVertexArrays(1, &vao);
	for (int i = 0; i < ATTRIBUTES; i++) {
		glGenBuffers(1, vbos + i);
	}

	_vertex_list = vector<Vertex*>(0);
}

Mesh::Mesh() {
	_init();
}

Mesh::Mesh(Vertex* vertex) {
	_init();
	vector<Vertex*> temp_list;
	auto add_to_vertices = [&](Vertex* v) {
		temp_list.push_back(v);
	};

	bfs(vertex, add_to_vertices);
	this->_vertex_list = temp_list;

	this->_find_edges();
	this->_find_faces_triangular();

	vec3 c = centroid(_vertex_list[0]);
	for (Vertex* v : _vertex_list) {
		v->normal = normalize(v->position - c);
	}
}

Mesh::~Mesh() {
	for (Vertex* v : this->_vertex_list) {
		delete v;
	}
	for (Edge* E : this->_edge_list) {
		delete E;
	}
}

template<int n>
Mesh::Mesh(matrix<n, n, int> adjacency, initializer_list<vec3> vertices) {
	_init();

	if (vertices.size() != n) {
		throw std::invalid_argument("Adjacency matrix size must match vertex list.");
	}

	this->_num_vertices = n;
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

	this->_find_edges();
	this->_find_faces_triangular();
}

void Mesh::center() {
	vec3 c = centroid(_vertex_list[0]);
	for (Vertex* v : _vertex_list) {
		v->position = v->position - c;
	}
}

void Mesh::initialize_buffers(GLenum usage, ShapeType type)
{
	this->_draw_mode = type;

	size_t n_points;
	float* vertex_attributes[ATTRIBUTES];

	//define sizes for each vertex buffer and create empty array for each one
	auto initialize_attributes = [&](size_t n_points) {
		_buffer_sizes[POSITION] = 3 * n_points;
		_buffer_sizes[NORMAL] = 3 * n_points;
		_buffer_sizes[COLOR] = 3 * n_points;
		_buffer_sizes[ADJACENCY] = 3 * _face_list.size() * n_points;

		for (int i = 0; i < ATTRIBUTES; i++) {
			vertex_attributes[i] = new float[_buffer_sizes[i]](0);
		}
	};

	map<Face*, int> face_indices; {
		int i = 0;
		for (Face* F : _face_list) {
			face_indices.insert({ F, i });
			i++;
		}
	}

	int counter = 0;

	//copy vertex data to respective memory block before copying to buffer
	auto copy_attributes = [&](MeshElem* E) {
		vector<Vertex*> vertices = E->vertex_array();

		for (Vertex* v : vertices) {
			float* position_data = v->position.data();
			float* normal_data = v->normal.data();
			float* color_data = v->color.data();

			for (int j = 0; j < 3; j++) {
				int index = 3 * counter + j;
				vertex_attributes[POSITION][index] = position_data[j];
				vertex_attributes[NORMAL][index] = normal_data[j];
				vertex_attributes[COLOR][index] = color_data[j];
			}

			for (Face* C : v->adjacent_faces) {
				vertex_attributes[ADJACENCY][counter * _face_list.size() + face_indices[C]] = 1.0f;
			}

			counter++;
		}
	};

	switch (type) {
	case LINE:
		n_points = _draw_mode * _edge_list.size();
		initialize_attributes(n_points);
		for (Edge* E : _edge_list) {
			copy_attributes(E);
		}
		break;

	case TRIANGLE:
		n_points = _draw_mode * _face_list.size();
		initialize_attributes(n_points);
		for (Face* F : _face_list) {
			copy_attributes(F);
		}
		break;
	}

	glBindVertexArray(vao);

	//copy data in vertex attributes to vertex array buffers
	for (int i = 0; i < ATTRIBUTES; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, (vbos)[i]);
		glBufferData(GL_ARRAY_BUFFER, _buffer_sizes[i] * sizeof(float), vertex_attributes[i], usage);
		switch (i) {
		case POSITION:
			glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			break;
		case NORMAL:
			glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			break;
		case COLOR:
			glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			break;
		case ADJACENCY:
			glVertexAttribPointer(i, _face_list.size(), GL_FLOAT, GL_FALSE, 0, NULL);
			break;
		}

		delete[] vertex_attributes[i];
	}
}

void Mesh::update_buffer(VERTEX_ATTRIBUTE attribute)
{
	glBindBuffer(GL_ARRAY_BUFFER, (vbos)[attribute]);
}

void Mesh::transform_buffer(int buffer, mat3 A)
{
	float* mem;

	//transforms only normals and positions
	glBindBuffer(GL_ARRAY_BUFFER, (vbos)[buffer]);
	mem = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, _buffer_sizes[buffer] * sizeof(float), GL_MAP_WRITE_BIT);
	transform_pts_3(mem, _buffer_sizes[buffer] / 3, A);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Mesh::transform_geometry(mat3 A)
{
	transform_buffer(0, A);
	transform_buffer(1, A);
}

void Mesh::draw()
{
	GLint mode = GL_LINES;

	switch (this->_draw_mode) {
	case LINE:
		mode = GL_LINES;
		break;
	case TRIANGLE:
		mode = GL_TRIANGLES;
		break;
	}

	glBindVertexArray(vao);
	glDrawArrays(mode, 0, _buffer_sizes[0]);
}

void Mesh::_find_faces_triangular() {
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
	Face* F_0 = new Face({ _vertex_list[0],*common_vertices(_vertex_list[0],_vertex_list[1]).begin() ,_vertex_list[1] });

	int i = 0;
	while (F_0->size() == 0) {
		*F_0 = find_adjacent_face(_vertex_list[i]);
		i++;
	}

	//throw exception if none exist
	if (F_0->size() == 0) throw std::exception("mesh is not locally planar");

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

	return;
}

void Mesh::_find_edges() {
	Vertex* start = _vertex_list[0];
	linked_list<Vertex> queue;
	unordered_set<Vertex*> visited;

	queue.push(start);
	visited.insert(start);

	while (queue.head != nullptr) {
		Vertex* dequeued = (Vertex*)queue.pop();

		//Mark connections as visited and add unvisited to queue
		for (Vertex* v : dequeued->connections) {
			Edge* E = new Edge(v, dequeued);
			_edge_list.push_back(E);

			//Only queue the vertex if insert() returns true for new element insertion
			bool new_insertion = visited.insert(v).second;

			if (new_insertion) {
				queue.push(v);
			}
		}

	}
}

void Mesh::color_with_curvature() {
	for (Vertex* v : this->_vertex_list) {
		v->color = red_blue_hue(6.0f * discrete_laplacian(v));
	}
}

void Mesh::compute_normals()
{
	for (Face* F : _face_list) {
		F->compute_normal();
	}

	for (Vertex* v : _vertex_list) {
		vec3 normal_avg = { 0,0,0 };
		for (Face* F : v->adjacent_faces) {
			normal_avg = normal_avg + F->normal;
		}
		v->normal = normalize(normal_avg);
	}



}

//
//#endif