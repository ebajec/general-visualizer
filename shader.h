#pragma once
#include <GL/glew.h> 
#include <GLFW/glfw3.h> 
#include "matrix.hpp"

#ifndef SHADER_H
#define SHADER_H

using vec3 = matrix<3, 1, GLfloat>;
using mat3 = matrix<3, 3, GLfloat>;
using mat4 = matrix<4, 4, GLfloat>;

class Shader {
public:
	Shader() {}
	Shader(const char* vertex_shader_path, const char* fragment_shader_path);

	void use() const { glUseProgram(program); }

	void set_uniform(const char* name, int value);
	void set_uniform(const char* name, float value);
	void set_uniform(const char* name, vec3 value);
	void set_uniform(const char* name, mat3 value, GLboolean transpose);
	void set_uniform(const char* name, mat4 value, GLboolean transpose);

	GLint get_uniform(const char* name) const { return glGetUniformLocation(program, name); }

	GLuint program;
protected:

	void compile_shader(GLenum type, const char* source);

	//create shader program
	void init() { program = glCreateProgram(); }

};

class ComputeShader : public Shader {
	ComputeShader(const char* shader_path);
};

#endif

