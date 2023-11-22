#version 330 core
out vec4 FragColor;

in vec2 geometry_pos;
in vec2 center;

void main()
{
    float test = 1 - step(0.03, length(center - geometry_pos));

    FragColor = vec4(test);
}