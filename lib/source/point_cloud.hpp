#ifndef POINT_CLOUD_HPP
#define POINT_CLOUD_HPP
#include "point_cloud.h"

pointCloud::pointCloud(vector<vec3> points) : drawable<POINT_ATTRIBUTES>() {
	_init();
	this->_points = points;
}

template<typename paramFunc>
pointCloud::pointCloud(surface<paramFunc> S, int fineness) : drawable<POINT_ATTRIBUTES>()
{
	_init();
	this->_points = S.gen_points(fineness);
}


void pointCloud::reinit_buffer(GLenum usage, unsigned int attribute)
{

}

void pointCloud::_init()
{
	_draw_mode = GL_POINTS;

	_layout_map[0] = 0;
	_layout_map[1] = 2;

	_primitive_sizes[0] = 3;
	_primitive_sizes[1] = 3;
}

unsigned long pointCloud::_object_count()
{
	return _points.size();
}

void pointCloud::_copy_attributes(float** attribute_buffers)
{
	int counter = 0;
	float* color_data = _color.data();

	for (vec3 v : _points) {
		float* pos_data = v.data();
		copy(pos_data, pos_data + 3, attribute_buffers[0] + 3 * counter);
		copy(color_data, color_data + 3, attribute_buffers[1] + 3 * counter);
		counter++;
	}
}


#endif