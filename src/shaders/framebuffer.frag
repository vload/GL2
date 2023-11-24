#version 430 core

out vec4 FragColor;
in vec2 tex_coords;

uniform sampler2D screen_texture;

uniform float rangle = 0.0;
uniform float gangle = radians(15.0);
uniform float bangle = radians(45.0);
uniform int circle_count = 80; // per 1 unit (1000 px)

vec3 sampling(float angle){
    mat2 rotate = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    mat2 inverse_rotate = mat2(cos(-angle), -sin(-angle), sin(-angle), cos(-angle));

    // 2d grid that is rotated by angle
    // find out what are the coordinates on the grid
    // 0 - circle_count, 0 - circle_count (not really there are extras because of the rotation)
    vec2 circle_coords = floor(rotate * tex_coords * circle_count);

    // calculate the distance from the center but on the rotated grid
    vec2 center = ((circle_coords + 0.5) / circle_count);
    float dist = length(tex_coords - inverse_rotate * center);

    // sample values in the cirlces square to see what color should be drawn
    vec4 color = vec4(0.0f);

    float radius = 1.0f / circle_count / 2.0;

    int samples = 0;

    for(float i = center.x - radius; i < center.x + radius; i += 1.0 / 1000.0){
        for(float j = center.y - radius; j < center.y + radius; j += 1.0 / 1000.0){
            color += texture(screen_texture, inverse_rotate * vec2(i, j));
            samples++;
        }   
    }

    color = color / samples; // average color (simple but not efficient)

    float inside_circler = 1.0 - step(color.r, dist * circle_count * 1.41);
    float inside_circleg = 1.0 - step(color.g, dist * circle_count * 1.41);
    float inside_circleb = 1.0 - step(color.b, dist * circle_count * 1.41);

    return vec3(inside_circler, inside_circleg, inside_circleb);
}

void main() {
    FragColor = vec4(sampling(rangle).r, sampling(gangle).g, sampling(bangle).b, 1.0);
}