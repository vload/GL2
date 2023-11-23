#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in float vert_speed[];
out float speed;

out vec2 center;
out vec2 geometry_pos;

uniform float size;

uniform mat4 projection;

void main()
{
    speed = vert_speed[0];
    
    center = gl_in[0].gl_Position.xy;

    gl_Position = gl_in[0].gl_Position + projection * vec4(-size, -size, 0.0, 0.0);
    geometry_pos = (gl_in[0].gl_Position + vec4(-size, -size, 0.0, 0.0)).xy;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + projection * vec4(-size, size, 0.0, 0.0);
    geometry_pos = (gl_in[0].gl_Position + vec4(-size, size, 0.0, 0.0)).xy;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + projection * vec4(size, -size, 0.0, 0.0); 
    geometry_pos = (gl_in[0].gl_Position + vec4(size, -size, 0.0, 0.0)).xy;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + projection * vec4(size, size, 0.0, 0.0);
    geometry_pos = (gl_in[0].gl_Position + vec4(size, size, 0.0, 0.0)).xy;
    EmitVertex();
    
    EndPrimitive();
} 