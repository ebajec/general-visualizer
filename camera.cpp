#include "camera.h"

camera::camera(vec3 normal, vec3 pos, GLfloat FOV, GLfloat far)
{
	this->near_dist = 1 / tan(FOV / 2);
	this->far_dist = far;
	this->position = pos;

	//generate orthonormal, right-handed basis for camera coordinates, with Z
	//as the normal vector.
	basis[2] = normalize(normal);
	basis[0] = cross(basis[2], vec3({ 0,1,0 }));
	basis[1] = cross(basis[0], basis[2]);

	basis[0] = normalize(basis[0]);
	basis[1] = normalize(basis[1]);

	change_of_basis = inv(basis[0] | basis[1] | basis[2]);

	projection_matrix_2D = (
		basis[0] | basis[1] | vec3(0.0f) | vec3(0.0f)).transpose() | vec4(0.0f);
}

void camera::connect_uniforms(const Shader& shader)
{
	//affine transformation from normal space to camera coordinates.  Allows easy 
	//projection onto camera plane.
	mat4 coordinate_mat = (
		change_of_basis |
		-1 * (change_of_basis * (position - basis[2] * near_dist))).transpose() |
		vec4(0.0f);

	glUniform1f(shader.get_uniform("near_dist"), near_dist);
	glUniform1f(shader.get_uniform("far_dist"), far_dist);
	glUniformMatrix4fv(shader.get_uniform("projection_mat"), 1, GL_FALSE, projection_matrix_2D.data());
	glUniformMatrix4fv(shader.get_uniform("coordinate_mat"), 1, GL_FALSE, coordinate_mat.data());
}
