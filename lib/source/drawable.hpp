#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include "drawable.h"

template<unsigned int NUM_ATTRIBUTES>
void drawable<NUM_ATTRIBUTES>::draw(Shader shader)
{
	glUseProgram(shader.program);
	shader.set_uniform("geom_model", _model, GL_TRUE);
	glBindVertexArray(_vao);
	glDrawArrays(_draw_mode, 0, _object_count());
	shader.set_uniform("geom_model", mat4::id(), GL_FALSE);
}

template<unsigned int NUM_ATTRIBUTES>
drawable<NUM_ATTRIBUTES>::drawable()
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
void drawable<NUM_ATTRIBUTES>::init_buffers(GLenum usage)
{
	float* attribute_buffers[NUM_ATTRIBUTES];

	for (int i = 0; i < NUM_ATTRIBUTES; i++) {
		attribute_buffers[i] = new float[_bufsize(i)](0);
	}

	_copy_attributes(attribute_buffers);

	glBindVertexArray(_vao);

	//copy data in vertex attributes to vertex array buffers
	for (int i = 0; i < NUM_ATTRIBUTES; i++) {
		glEnableVertexAttribArray(i);
		glBindBuffer(GL_ARRAY_BUFFER, _vbos[i]);
		glBufferData(GL_ARRAY_BUFFER, _bufsize(i) * sizeof(float), attribute_buffers[i], usage);
		glVertexAttribPointer(i, _primitive_sizes[i], GL_FLOAT, GL_FALSE, 0, NULL);

	}
}



#endif