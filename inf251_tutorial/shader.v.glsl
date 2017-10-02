#version 330	// GLSL version

// model-view transformation
uniform mat4 transformation;

// vertex position
layout (location = 0) in vec3 position; 

// vertex texture coordinates
layout (location = 1) in vec2 tex_coords;

// pass the texture coordinates to the fragment shader
out vec2 cur_tex_coords;

// Copied from tutorial 3 shader
// Output vertex color (per-vertex, interpolated and passed to frag shader)
out vec4 color;

void main() {
	// transform the vertex
    gl_Position = transformation * vec4(position, 1.);	
	
	// pass the texture coordinates to the fragment shader
	cur_tex_coords = tex_coords;

	// Copied from tutorial 3 shader
	color.r = clamp(gl_Position.x, 0., 1.);
	color.g = clamp(gl_Position.y, 0., 1.);
	color.b = clamp(gl_Position.z, 0., 1.);
	color.a = 1.;
}