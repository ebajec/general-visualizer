#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#define MAX_LINE_LENGTH 100

char* read_text_file(FILE* file) {
	if (file == nullptr) {
		return nullptr;
	}

	char line[MAX_LINE_LENGTH];
	linked_list<char> line_list;

	size_t total_size = 0;

	while (fgets(line, MAX_LINE_LENGTH, file)) {
		size_t line_length = strlen(line);
		char* newline = (char*)malloc((line_length + 1) * sizeof(char));

		if (newline == nullptr) {
			return nullptr;
		}

		strcpy(newline, line);
		line_list.push(newline);
		total_size += line_length;
	}



	char* text = (char*)malloc((total_size + 1) * sizeof(char));

	int counter = 0;

	node<char>* current = line_list.head;

	while (current != nullptr) {
		char* line = (char*)current->address;

		size_t size = strlen(line);

		for (int i = 0; i < size; i++) {
			text[counter + i] = line[i];
		}
		counter += size;
		current = current->next;
	}

	text[counter] = '\0';

	line_list.destroy("t");

	return text;
}

Shader::Shader(const char* vertex_shader_path, const char* fragment_shader_path) {
	init();

	// read shader data from files
	FILE* fragment_shader_data;
	FILE* vertex_shader_data;

	fragment_shader_data = fopen(fragment_shader_path, "r");
	vertex_shader_data = fopen(vertex_shader_path, "r");

	if (fragment_shader_data == NULL || vertex_shader_data == NULL) {
		fprintf(stderr, "ERROR: could not open shader files\n");
		glfwTerminate();
	}

	const char* vertex_shader_source = read_text_file(vertex_shader_data);
	const char* fragment_shader_source = read_text_file(fragment_shader_data);

	//compile shaders
	compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
	compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);

	glLinkProgram(program);
}

void Shader::set_uniform(const char* name, int value)
{
	glUniform1i(this->get_uniform(name), value);
}

void Shader::set_uniform(const char* name, float value)
{
	glUniform1f(this->get_uniform(name), value);
}

void Shader::set_uniform(const char* name, vec3 value)
{
	glUniform3fv(this->get_uniform(name), 1, value.data());
}

void Shader::set_uniform(const char* name, mat3 value, GLboolean transpose)
{
	glUniformMatrix3fv(this->get_uniform(name), 1, transpose, value.data());
}

void Shader::set_uniform(const char* name, mat4 value, GLboolean transpose)
{
	glUniformMatrix4fv(this->get_uniform(name), 1, transpose, value.data());
}

void Shader::compile_shader(GLenum type, const char* source)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &source, NULL);
	glCompileShader(s);
	glAttachShader(program, s);
}

ComputeShader::ComputeShader(const char* shader_path) {
	init();

	FILE* shader_data;
	shader_data = fopen(shader_path, "r");

	if (shader_data == NULL) {
		fprintf(stderr, "ERROR: could not open shader files\n");
		glfwTerminate();
	}

	const char* shader_source = read_text_file(shader_data);

	compile_shader(GL_COMPUTE_SHADER, shader_source);

	glLinkProgram(program);
}