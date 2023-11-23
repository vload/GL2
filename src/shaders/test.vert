#version 430 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 velocity;

uniform mat4 projection;
uniform mat4 zoom;

out float vert_speed;

void main()
{
    gl_Position = zoom * projection * vec4(position, 0.0f, 1.0f);
    vert_speed = length(velocity);
}