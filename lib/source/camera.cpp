#include "camera.h"

camera::camera(
	vec3 normal,
	vec3 pos,
	GLfloat FOV,
	GLfloat far,
	int w_screen,
	int h_screen)
	:
	_w_screen(w_screen),
	_h_screen(h_screen),
	near_dist(1 / tan(FOV / 2)),
	far_dist(far),
	position(pos)
{
	//generate orthonormal, right-handed basis for camera coordinates, with Z
	//as the normal vector.
	basis[2] = normalize(normal);
	basis[0] = cross(basis[2], vec3({ 0,1,0 }));
	basis[1] = cross(basis[0], basis[2]);
	basis[0] = normalize(basis[0]);
	basis[1] = normalize(basis[1]);

	change_of_basis = inv(basis[0] | basis[1] | basis[2]);

	projection_2D = (
		basis[0] | basis[1] | vec3(0.0f) | vec3(0.0f)).transpose() | vec4(0.0f);

	view_mat = (
		change_of_basis |
		-1 * (change_of_basis * (position - basis[2] * near_dist))).transpose() |
		vec4(0.0f);

	model_mat = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

void camera::connect_uniforms(const Shader& shader)
{
	glUniform1f(shader.get_uniform("near_dist"), near_dist);
	glUniform1f(shader.get_uniform("far_dist"), far_dist);
	glUniformMatrix4fv(shader.get_uniform("cam_projection"), 1, GL_FALSE, projection_2D.data());
	glUniformMatrix4fv(shader.get_uniform("cam_modelview"), 1, GL_FALSE, (view_mat * model_mat).data());
}
