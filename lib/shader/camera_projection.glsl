#version 400 core

uniform float near_dist;
uniform float far_dist;
uniform mat4 projection_mat;
uniform mat4 coordinate_mat;

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 color;
out vec3 frag_color;
out vec3 frag_pos;
out vec3 normal;

void main() {
	
	vec4 v_pos_new = vec4(v_pos,1);

	v_pos_new = coordinate_mat*v_pos_new;

	float w = v_pos_new.z/near_dist;
	float depth = 2*(v_pos_new.z - near_dist)/(far_dist - near_dist) - 1;

	frag_color = color;
	frag_pos = v_pos;
	normal = v_normal;
    gl_Position = vec4(v_pos_new.xy,w*depth,w);
}