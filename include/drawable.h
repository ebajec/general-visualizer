#pragma once

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "GL/glew.h"
#include "shader.h"
#include "matrix.h"



template<unsigned int NUM_ATTRIBUTES>
class Drawable {
public:
	Drawable();

	void initBuffers(GLenum usage);
	virtual void reinitBuffer(GLenum usage, unsigned int attribute) = 0;

	void transformAffine(mat4 A) { _model = _model * A; }
	void draw(ShaderProgram shader, float t = 1);

	void setMode(GLenum mode) { _draw_mode = mode; }

protected:
	/*identifies each attribute with location in vertex array.  Mainly useful if
	using the same shader for different objects*/
	int _layout_map[NUM_ATTRIBUTES];

	//Size of primitives for each attribute, in number of floating point values.  NOT size in bytes.
	int _primitive_sizes[NUM_ATTRIBUTES];

	//transformation applied to points of object in vertex shader
	mat4 _model;

	GLenum _draw_mode;
	GLuint _vao;
	GLuint _vbos[NUM_ATTRIBUTES];

	//Size of buffer in number of floating point values.  NOT size in bytes.
	long unsigned int _bufSize(int attribute) { return _objectCount() * _primitive_sizes[attribute]; }

	//define layout map and primitive sizes here
	virtual void _init() = 0;

	//Number of individual points in vertex array, i.e, the vertex shader will
	//run this many times.
	virtual unsigned long _objectCount() { return 0; }

	/*copies attributes into NUM_ATTRIBUTES blocks of memory in attribute_buffers
	each block will be of size bufsize(attribute)*/
	virtual void _copyAttributes(float** attribute_buffers) = 0;
};

#include "drawable.hpp"

#endif