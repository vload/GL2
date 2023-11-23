#version 430 core
out vec4 FragColor;

in vec2 geometry_pos;
in vec2 center;

in float speed;

uniform float size;

void main()
{
    float stp = 1.5;

    vec3 color = vec3(0.0f);

    if(speed<stp){
        color = vec3(0, speed / stp, 1 - speed / stp);
    }
    else if(speed < 2 * stp){
        color = vec3(speed / stp - 1, 2 - speed / stp, 0);
    }
    else if(speed < 3 * stp){
        color = vec3(3 - speed / stp, 0, speed / stp - 2);
    }
    else{
        color = vec3(1.0f);
    }


    float inside_circle = 1 - step(size, length(center - geometry_pos));

    FragColor = vec4(color, inside_circle);
}