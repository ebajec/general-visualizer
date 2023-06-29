#version 400 core

layout(std430, binding = 0) buffer FaceData {
    vec3 vertices[3];
};

layout(std430, binding = 1) buffer NormalData {
    vec3 normal;
};

void main(){
	vec3 v1 = vertices[1] - vertices[0];
	vec3 v2 = vertices[2] - vertices[0];
	normal = cross(v1,v2);
}