#ifndef POINT_CLOUD_HPP
#define POINT_CLOUD_HPP
#include "point_cloud.h"

point_cloud::point_cloud(vector<vec3> points) : drawable<POINT_ATTRIBUTES>() {
	this->_points = points;
	this->_object_count = points.size();
}

template<typename paramFunc>
point_cloud::point_cloud(surface<paramFunc> S, int fineness) : drawable<POINT_ATTRIBUTES>()
{
	this->_points = S.gen_points(fineness);
	this->_object_count = points.size();
	this.
}

void point_cloud::set_color(vec3 color)
{
}

void point_cloud::init_buffers(GLenum usage)
{
	_buffer_sizes[0] = 3 * _object_count;
	_buffer_sizes[1] = 3 * _object_count;

	float* attributes[POINT_ATTRIBUTES];

	//works because vec3 only stores data for values
	attributes[0] = (float*)_points.data();
	for (int i = 1; i < POINT_ATTRIBUTES; i++) {
		attributes[i] = new float[_buffer_sizes[i]];
	}

	//copy color data
	float* color_data = _color.data();
	for (int i = 0; i < _object_count; i++) {
		for (int j = 0; j < 3; j++) {
			attributes[1][i * 3 + j] = color_data[j];
		}

	}

	glBindVertexArray(_vao);
	for (int i = 0; i < POINT_ATTRIBUTES; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, _buffer_sizes[i] * sizeof(float), attributes[i], usage);
	}

	//I don't want to delete points
	delete[] attributes[1];
}

void point_cloud::reinit_buffer(GLenum usage, unsigned int attribute)
{

}


#endif