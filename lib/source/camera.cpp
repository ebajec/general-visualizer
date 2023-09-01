#include "camera.h"

Camera::Camera(
	vec3 normal,
	vec3 pos,
	int w_screen,
	int h_screen,
	GLfloat FOV,
	GLfloat far)
	:
	_w_screen(w_screen),
	_h_screen(h_screen),
	_near_dist(1 / tan(FOV / 2)),
	_far_dist(far),
	_pos(pos)
{
	//generate orthonormal, right-handed basis for camera coordinates, with Z
	//as the normal vector.
	basis[2] = normalize(normal);
	basis[0] = cross(basis[2], vec3({ 0,1,0 }));
	basis[1] = cross(basis[0], basis[2]);
	basis[0] = normalize(basis[0]);
	basis[1] = normalize(basis[1]);

	change_of_basis = inv(basis[0] | basis[1] | basis[2]);

	float screen_ratio = (float)_h_screen / (float)_w_screen;
	_proj = {
		screen_ratio,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	_view = mat4(change_of_basis | -1 * (change_of_basis * (_pos - basis[2] * _near_dist)));

	_model = mat4{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

void Camera::connectUniforms(const ShaderProgram& shader)
{
	glUniform1f(shader.getUniform("near_dist"), _near_dist);
	glUniform1f(shader.getUniform("far_dist"), _far_dist);
	glUniformMatrix4fv(shader.getUniform("cam_projection"), 1, GL_FALSE, _proj.data());
	glUniformMatrix4fv(shader.getUniform("cam_modelview"), 1, GL_TRUE, (_model * _view).data());
}

void Camera::rotate(float pitch, float yaw)
{
	_model = _model * mat4(rotateyz<GLfloat>(pitch) * rotatexz<GLfloat>(yaw));
}

void Camera::translate(vec3 delta) {
	_pos = _pos + delta;
	_updateViewMat();
}

