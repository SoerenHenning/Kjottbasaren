#version 330	// GLSL version

// Sampler to access the texture
uniform sampler2D sampler;

// Per-fragment color coming from the vertex shader
in vec4 fcolor;

// Per-frgament output color
out vec4 FragColor;


void main() { 
    // Compute output color by using the luminosity method: Brightness = 0.21 R + 0.72 G + 0.07 B
	float avg = 0.21 * fcolor.r + 0.72 * fcolor.g + 0.07 * fcolor.b;
    FragColor = vec4(avg, avg, avg, fcolor.a);
}