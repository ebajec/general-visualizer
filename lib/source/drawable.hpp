#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "drawable.h"

template<unsigned int NUM_ATTRIBUTES>
void Drawable<NUM_ATTRIBUTES>::draw(ShaderProgram shader,float t)
{
	glUseProgram(shader.program);
	shader.setUniform("geom_model", _model, GL_TRUE);
	glBindVertexArray(_vao);
	glDrawArrays(_draw_mode, 0, (int)(t*(float)_objectCount()));
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
	float* attribute_buffers[NUM_ATTRIBUTES];

	for (int i = 0; i < NUM_ATTRIBUTES; i++) {
		attribute_buffers[i] = new float[_bufSize(i)](0);
	}

	_copyAttributes(attribute_buffers);

	glBindVertexArray(_vao);

	//copy data in vertex attributes to vertex array buffers
	for (int i = 0; i < NUM_ATTRIBUTES; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, _bufSize(i) * sizeof(float), attribute_buffers[i], usage);
		glVertexAttribPointer(i, _primitive_sizes[i], GL_FLOAT, GL_FALSE, 0, NULL);

	}
}



#endif