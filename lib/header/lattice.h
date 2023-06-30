#pragma once

#ifndef LATTICE_H
#define LATTICE_H

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
using vertex_set = unordered_set <Vertex*, Vertex::hasher, Vertex::comparator>;

/*Generate connected lattice of vertices.*/
Vertex** generate_lattice(int N, int L);

Vertex* generate_polygon(int penis);



#endif