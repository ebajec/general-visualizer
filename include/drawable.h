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
  
	/*Copies data for each vertex attribute into GPU buffers.*/
	void initBuffers(GLenum usage);
	virtual void refreshBuffer(GLenum usage, unsigned int attribute) = 0;

	void transformAffine(mat4 A) { _model = _model * A; }
	void draw(ShaderProgram shader, int count = -1);

	void setMode(GLenum mode) { _draw_mode = mode; }

	//Number of individual points in vertex array, i.e, the vertex shader will
	//run this many times.
	virtual unsigned long _pointCount() { return 0; }
protected:
	//identifies each attribute with location in vertex array.  Only really needed
	//if using the same shader for different types objects.
	int _layout_map[NUM_ATTRIBUTES];

	//Size of primitives for each attribute, in number of floating point values.  NOT size in bytes.
	//ex: if attribute 0 was a 3D position, _primitive_sizes[0] = 1. 
	int _primitive_sizes[NUM_ATTRIBUTES];

	//transformation applied to vertices in vertex shader
	mat4 _model;

	//specifies first argument in glDrawArrays
	GLenum _draw_mode;
	GLuint _vao;
	GLuint _vbos[NUM_ATTRIBUTES];

	//Size of a buffer in number of floating point values.  NOT size in bytes.
	//If the position buffer had 100 points, this would be 100 for.
	//@param attribute index of vertex buffer object to be sized
	long unsigned int _bufSize(int attribute) { return _pointCount() * _primitive_sizes[attribute]; }

	//define layout map and primitive sizes here
	virtual void _init() = 0;

	

	/*copies attributes into NUM_ATTRIBUTES blocks of memory in attribute_buffers
	each block will be of size bufsize(attribute)*/
	virtual void _copyAttributes(float** attribute_buffers) = 0;
};

#include "drawable.hpp"

#endif