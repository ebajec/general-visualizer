#pragma once

#ifndef SHAPES_H
#define SHAPES_H

#include <stdlib.h>
#include "vertex.h"
#include "linked_list.h"
#include <unordered_set>

#ifndef PI
#define PI 3.14159265359f
#endif
#define ROOT3OVER2 1.0f

constexpr GLfloat EPSILON = std::numeric_limits<GLfloat>::epsilon();
using mat3 = matrix<3, 3, GLfloat>;
using vertex_set = unordered_set <vertex*, vertex::hasher, vertex::comparator>;

struct shape {};

template<typename func>
struct surface : public shape {
public:
	surface() {}
	surface(func F, float s_max, float t_max) : _param_func(F), _s_max(s_max), _t_max(t_max) {}
	vec3 eval(float s, float t) { return _param_func(s, t); }
	vector<vec3> gen_points(int fineness);
	float s_max() { return _s_max; }
	float t_max() { return _t_max; }

protected:
	func _param_func;
	float _s_max, _t_max;
};

template<typename func>
vector<vec3> surface<func>::gen_points(int fineness)
{
	vector<vec3> points = vector<vec3>(fineness);

	for (int i = 0; i < fineness; i++) {

		float s = _s_max * uniform_rand();
		float t = _t_max * uniform_rand();

		points[i] = eval(s, t);
	}

	return points;
}

/*Generate connected lattice of vertices.*/
vertex** generate_lattice(int N, int L);

vertex* generate_polygon(int penis);


#endif