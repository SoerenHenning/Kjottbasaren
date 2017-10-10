#version 330	// GLSL version

// model-view transformation
uniform mat4 transformation;

// Copied from tutorial 6 shader
// Camera position
uniform vec3 camera_position;

// Copied from tutorial 6 shader
// Directional light
uniform vec3 d_light_direction;
uniform vec3 d_light_a_color;
uniform float d_light_a_intensity;
uniform vec3 d_light_d_color;
uniform float d_light_d_intensity;
uniform vec3 d_light_s_color;
uniform float d_light_s_intensity;

// Copied from tutorial 6 shader
// Object material
// Notice that all of this values may be also specified per-vertex or 
//  through a texture.
uniform vec3 material_a_color;
uniform vec3 material_d_color;
uniform vec3 material_s_color;
uniform float material_shininess;

// vertex position
layout (location = 0) in vec3 position; 

// vertex texture coordinates
//layout (location = 1) in vec2 tex_coords;

layout (location = 2) in vec3 normal; 

// pass the texture coordinates to the fragment shader
//out vec2 cur_tex_coords;

// Copied from tutorial 3 shader
// Output vertex color (per-vertex, interpolated and passed to frag shader)
out vec4 fcolor;



void main() {

	// transform the vertex
    gl_Position = transformation * vec4(position, 1.);	


	// DEBUG
	vec3 debugColor;

	

	
	
	// pass the texture coordinates to the fragment shader
	//cur_tex_coords = tex_coords;

	// Compute the vertex color according to the phong lighting model
	// For each light compute the sum of:
	// ambient component = m_ambient * l_ambient
	// -> the * denote element-by-element multiplication
	// diffuse component = m_diffuse * (-l_dir.normal) * l_diffuse
	// -> light direction and normal are assumed to be already normalized
	// reflect component = m_specular * (reflect_dir.view_dir_nn)^m_shininess
	// -> with reflect_dir = l_dir - 2 * (l_dir.normal) * normal 
	// -> the Blinn-phong version can be used here
	// This computation has to be repeated for every light in the scene.
	// The final color is given by the sum of contributions from every light.
	
	// --- directional light ----
	// compute the required values and vectors
	// notice that input variables cannot be modified, so copy them first
	vec3 normal_nn = normalize(normal);	// TODO NaN's returned
	vec3 d_light_dir_nn = normalize(d_light_direction);
	vec3 view_dir_nn = normalize(camera_position - position); //OUT
	//d_light_dir_nn = view_dir_nn;

	debugColor.r = 0.0;
	debugColor.g = 0.0;
	debugColor.b = 0.0;
	//debugColor.b = normal_nn.x;
	
	if (position.x != 0.0 && position.y != 0.0 && position.z != 0.0) {
		//debugColor.r = 1.0;
	}
	/*
	if (tex_coords.x == 0.0 && tex_coords.y == 0.0) {
		//debugColor.g = 1.0;
	}
	*/
	if (normal.x == 0.0) {
		//debugColor.b = 1.0;
	}
	//debugColor.b = normal_nn.z;
	if (d_light_a_color.z == 0.0) {
		//debugColor.r = 1.0;
	}
	
	
	float dot_d_light_normal = dot(-d_light_dir_nn, normal);   // notice the minus!
	vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal; // OUT
	// should be already normalized, but we "need" to correct numerical errors
	d_reflected_dir_nn = normalize(d_reflected_dir_nn);  // OUT
	
	// compute the color contribution	
	vec3 color;
	vec3 amb_color = clamp(
			material_a_color * d_light_a_color * d_light_a_intensity,
			0.0, 1.0);
	//amb_color = vec3(0.0,0.0,0.0); // TODO
	vec3 diff_color = clamp(
			material_d_color * dot_d_light_normal * d_light_d_intensity,
			0.0, 1.0);
	//diff_color = vec3(0.0,0.0,0.0);
	vec3 spec_color = clamp(
			material_s_color *  
			pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess),
			0.0, 1.0);
	//spec_color = vec3(0.0,0.0,0.0); // TODO Camera not right

	
	color = clamp(
			amb_color + diff_color + spec_color,
			0.0, 1.0);
	

	//color = debugColor; // DEBUG

	fcolor = vec4(color, 1.0);

	//DEBUG
	//color = material_a_color;

	// TODO: do the same for the headlight!
	// notice that for the headlight dot(view_dir, light_dir) = ...
	
	
	/* OUT
	
	//p_light_dir_nn = view_dir_nn;
	
	vec3 p_light_dir_nn = -view_dir_nn;
	
	float dot_p_light_normal = dot(-p_light_dir_nn, normal);   // notice the minus!
	vec3 p_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal;
	// should be already normalized, but we "need" to correct numerical errors
	d_reflected_dir_nn = normalize(d_reflected_dir_nn); 
	
	// compute the color contribution
	amb_color = clamp(
			material_a_color * d_light_a_color * d_light_a_intensity,
			0.0, 1.0);
	amb_color = vec3(0.0,0.0,0.0); // TODO
	diff_color = clamp(
			material_d_color * dot_p_light_normal * d_light_d_intensity,
			0.0, 1.0);
	//diff_color = vec3(0.0,0.0,0.0);
	spec_color = clamp(
			material_s_color *  
			pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess),
			0.0, 1.0);
	spec_color = vec3(0.0,0.0,0.0); // TODO
	color = clamp(
			amb_color + diff_color + spec_color,
			0.0, 1.0);

	color = clamp(color, 0.0, 1.0);
	
		
	// pass the reuslt to the fragment shader
	//fcolor = vec4(color, 1.0); //TODO

	OUT END*/

	// Copied from tutorial 3 shader
	//color.r = clamp(gl_Position.x, 0., 1.);
	//color.g = clamp(gl_Position.y, 0., 1.);
	//color.b = clamp(gl_Position.z, 0., 1.);
	//color.r = clamp(gl_Position.z, 0., 1.);
	//fcolor.r = 1.;
	//fcolor.g = 1.;
	//fcolor.b = 1.;
	//fcolor.a = 1.;



}