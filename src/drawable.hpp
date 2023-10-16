#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "drawable.h"

template<unsigned int NUM_ATTRIBUTES>
void Drawable<NUM_ATTRIBUTES>::draw(ShaderProgram shader,int count)
{
	if (count > _pointCount()/3) count = _pointCount()/3;
	if (count == -1) count = _pointCount()/3;

	glUseProgram(shader.program);
	shader.setUniform("geom_model", _model, GL_TRUE);
	glBindVertexArray(_vao);
	glDrawArrays(_draw_mode, 0, 3*count);
	shader.setUniform("geom_model", mat4::id(), GL_FALSE);
}

template<unsigned int NUM_ATTRIBUTES>
Drawable<NUM_ATTRIBUTES>::Drawable()
{
	glGenVertexArrays(1, &_vao);
	for (int i = 0; i < NUM_ATTRIBUTES; i++) {
		glGenBuffers(1, _vbos + i);
	}

	_draw_mode = GL_POINTS;
	_model = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

template<unsigned int NUM_ATTRIBUTES>
void Drawable<NUM_ATTRIBUTES>::initBuffers(GLenum usage)
{
	int n = NUM_ATTRIBUTES;
	float** attribute_buffers = new float*[n] ;

	for (int i = 0; i < n ; i++) {
		int size = _bufSize(i);
		attribute_buffers[i] = new float[size];
	}

	_copyAttributes(attribute_buffers);

	glBindVertexArray(_vao);

	//copy data in vertex attributes to vertex array buffers
	for (int i = 0; i < n ; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, _bufSize(i) * sizeof(float), attribute_buffers[i], usage);
		glVertexAttribPointer(i, _primitive_sizes[i], GL_FLOAT, GL_FALSE, 0, NULL);

	}
}



#endif