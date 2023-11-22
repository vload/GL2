#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 geometry_pos;
out vec2 center;

void main()
{
    center = gl_in[0].gl_Position.xy;

    gl_Position = gl_in[0].gl_Position + vec4(-0.1, -0.1, 0.0, 0.0);
    geometry_pos = gl_Position.xy;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);
    geometry_pos = gl_Position.xy;
    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(0.1, -0.1, 0.0, 0.0); 
    geometry_pos = gl_Position.xy;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
    geometry_pos = gl_Position.xy;
    EmitVertex();
    
    EndPrimitive();

} 