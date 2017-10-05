#version 330	// GLSL version

// Sampler to access the texture
//uniform sampler2D sampler;

// Time
//uniform float time;

// Per fragment texture coordinates
//in vec2 cur_tex_coords;

// Copied from tutorial 3 shader
// Per-fragment color coming from the vertex shader
in vec4 fcolor;

// Per-frgament output color
out vec4 FragColor;


void main() { 
	// Set the output color according to the input
    //FragColor = texture2D(sampler, cur_tex_coords.ts);

	//FragColor.r += 0.25 * cos(2. * time + 3.1415) + 0.25;
	
	// Copied from tutorial 3 shader
	// Set the output color according to the input
    FragColor = fcolor;
	//FragColor.r = 0.1;
	//FragColor = vec3(1,0,0);
	
}