#pragma once
#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

#include <shapes.h>
#include <drawable.h>

#define POINT_ATTRIBUTES 2

class pointCloud : public drawable<POINT_ATTRIBUTES> {
public:
	pointCloud() {}
	pointCloud(vector<vec3> points);
	template<typename paramFunc>
	pointCloud(surface<paramFunc> S, int fineness);

	void set_color(vec3 color) { _color = color; }
	void reinit_buffer(GLenum usage, unsigned int attribute);
private:
	vector<vec3> _points;
	vec3 _color = { 1,1,1 };

	void _init();
	inline unsigned long _object_count();
	void _copy_attributes(float** attribute_buffers);
};

#include "point_cloud.hpp"

#endif


