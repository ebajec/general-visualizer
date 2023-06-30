#version 400 core

in vec3 frag_color;
in vec3 frag_pos;
in vec3 normal;
out vec4 FragColor;

float PI = 3.141592654;

vec3 calc_light(vec3 light, vec3 color) {
    vec3 ray = light - frag_pos;
    float dist_sq = dot(ray,ray);
    float cosine = dot(normal,ray)/sqrt(dist_sq);
    float theta = acos(cosine);

    float brightness = 200*cos(theta)/dist_sq;

    if (theta > PI/2) {
        return vec3(0);
    }

    return brightness * color;
}

void main()
{ 
    vec3 light1 = vec3(10,1,10);
    vec3 light2 = vec3(-10,10,4);
    vec3 light3 = vec3(-5,-6,7);

    vec3 color = calc_light(light1,frag_color) + calc_light(light2,frag_color);// + calc_light(light3,frag_color);

    FragColor = vec4(color,1);
} 