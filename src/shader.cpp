#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include "shader.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>

#define MAX_LINE_LENGTH 100

inline char* read_text_file(FILE* file) {
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

inline string read_text_file_2(const char* src){
	ifstream file(src);

	if (!file.is_open()) {
		fprintf(stderr, "ERROR: could not open file:\n",src,"\n");
		return "";
	}

	string line;
	string text;

	while (getline(file,line)) {
		text.append(line).append("\n");
	}

	return text;
}


ShaderProgram::ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path) {
	init();

	//// read shader data from files
	//FILE* fragment_shader_data;
	//FILE* vertex_shader_data;
//
	//fragment_shader_data = fopen(fragment_shader_path, "r");
	//vertex_shader_data = fopen(vertex_shader_path, "r");

	printf("shader?\n");

	string vertex_shader_source = read_text_file_2(vertex_shader_path);
	string fragment_shader_source = read_text_file_2(fragment_shader_path);

	if (fragment_shader_source == "" || vertex_shader_source == "") {
		printf("no shader\n"); 
		fprintf(stderr, "ERROR: could not open shader files\n");
		glfwTerminate();
	}

	printf("yes shader\n");
	
	//compile shaders
	_compileShader(GL_VERTEX_SHADER, vertex_shader_source.c_str());
	_compileShader(GL_FRAGMENT_SHADER, fragment_shader_source.c_str());

	glLinkProgram(program);
}

void ShaderProgram::setUniform(const char* name, int value)
{
	glUniform1i(this->getUniform(name), value);
}

void ShaderProgram::setUniform(const char* name, float value)
{
	glUniform1f(this->getUniform(name), value);
}

void ShaderProgram::setUniform(const char* name, vec3 value)
{
	glUniform3fv(this->getUniform(name), 1, value.data());
}

void ShaderProgram::setUniform(const char* name, mat3 value, GLboolean transpose)
{
	glUniformMatrix3fv(this->getUniform(name), 1, transpose, value.data());
}

void ShaderProgram::setUniform(const char* name, mat4 value, GLboolean transpose)
{
	glUniformMatrix4fv(this->getUniform(name), 1, transpose, value.data());
}

void ShaderProgram::_compileShader(GLenum type, const char* source)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &source, NULL);
	glCompileShader(s);

	GLint success = 0;
	glGetShaderiv(s, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint length = 0;
		glGetShaderiv(s, GL_INFO_LOG_LENGTH, &length);

		std::vector<GLchar> error_message(length);
		glGetShaderInfoLog(s, length, NULL, &error_message[0]);

		printf(&error_message[0]);

		glDeleteShader(s);
	}
	else {
		glAttachShader(program, s);
	}


}

computeShader::computeShader(const char* shader_path) {
	init();

	FILE* shader_data;
	shader_data = fopen(shader_path, "r");

	if (shader_data == NULL) {
		fprintf(stderr, "ERROR: could not open shader files\n");
		glfwTerminate();
	}

	const char* shader_source = read_text_file(shader_data);

	_compileShader(GL_COMPUTE_SHADER, shader_source);

	glLinkProgram(program);
}