#version 330	// GLSL version

// model-view transformation
uniform mat4 transformation;

// Camera position
uniform vec3 camera_position;

// Directional light
uniform vec3 d_light_direction;
uniform vec3 d_light_a_color;
uniform float d_light_a_intensity;
uniform vec3 d_light_d_color;
uniform float d_light_d_intensity;
uniform vec3 d_light_s_color;
uniform float d_light_s_intensity;

// Head-mounted light
uniform vec3 p_light_a_color;
uniform float p_light_a_intensity;
uniform float p_light_d_intensity_k_const;
uniform float p_light_d_intensity_k_linear;
uniform float p_light_d_intensity_k_square;
uniform vec3 p_light_d_color;
uniform float p_light_d_intensity;
uniform vec3 p_light_s_color;
uniform float p_light_s_intensity;

// Object material
uniform vec3 material_a_color;
uniform vec3 material_d_color;
uniform vec3 material_s_color;
uniform float material_shininess;

// vertex position
layout (location = 0) in vec3 position; 

// vertex texture coordinates
layout (location = 1) in vec2 tex_coords;

layout (location = 2) in vec3 normal; 

// pass the texture coordinates to the fragment shader
//out vec2 cur_tex_coords;

// Output vertex color (per-vertex, interpolated and passed to frag shader)
out vec4 fcolor;


vec3 computeHeadlight(vec3);
vec3 computeDirectionalLight(vec3);


void main() {

	// transform the vertex
    gl_Position = transformation * vec4(position, 1.);	

	// pass the texture coordinates to the fragment shader
	//cur_tex_coords = tex_coords;

	vec3 view_dir_nn = normalize(camera_position - position);


	vec3 directionalLight = computeDirectionalLight(view_dir_nn);

	vec3 headlight = computeHeadlight(view_dir_nn);

	vec3 color = directionalLight + headlight;

	// pass the result to the fragment shader
	fcolor = vec4(color, 1.0); //TODO
}

vec3 computeDirectionalLight(vec3 view_dir_nn) {
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
	//d_light_dir_nn = view_dir_nn;


	float dot_d_light_normal = dot(-d_light_dir_nn, normal);   // notice the minus!
	vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal; // OUT
	// should be already normalized, but we "need" to correct numerical errors
	d_reflected_dir_nn = normalize(d_reflected_dir_nn);  // OUT
	
	// compute the color contribution	
	vec3 amb_color = clamp(
			material_a_color * d_light_a_color * d_light_a_intensity,
			0.0, 1.0);
	//amb_color = vec3(0.0,0.0,0.0); // TODO
	vec3 diff_color = clamp(
			material_d_color * dot_d_light_normal * d_light_d_intensity,
			0.0, 1.0);
	//diff_color = vec3(0.0,0.0,0.0);  // TODO
	vec3 spec_color = clamp(
			material_s_color *  
			pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess),
			0.0, 1.0);
	//spec_color = vec3(0.0,0.0,0.0); //TODO

	return clamp(amb_color + diff_color + spec_color, 0.0, 1.0);
}

vec3 computeHeadlight(vec3 view_dir_nn) {
	
	//p_light_dir_nn = view_dir_nn;
	
	//vec3 p_light_dir_nn = -view_dir_nn;

	//TODO style
	// position mins cam position
	vec3 p_light_dir = position - camera_position;
	vec3 p_light_dir_nn = normalize(p_light_dir);
	

	float dot_p_light_normal = dot(-p_light_dir_nn, normal);   // notice the minus!
	vec3 p_reflected_dir_nn = p_light_dir_nn + 2. * dot_p_light_normal * normal;
	// should be already normalized, but we "need" to correct numerical errors
	p_reflected_dir_nn = normalize(p_reflected_dir_nn); 

	float distance = length(p_light_dir);
	float distance_intensity = p_light_d_intensity_k_const + (p_light_d_intensity_k_linear * distance) + (p_light_d_intensity_k_square * distance * distance);

	// compute the color contribution
	vec3 amb_color = clamp(
			material_a_color * p_light_a_color * p_light_a_intensity,
			0.0, 1.0);
	//amb_color = vec3(0.0,0.0,0.0); // TODO
	vec3 diff_color = clamp(
			material_d_color * dot_p_light_normal * (p_light_d_intensity / distance_intensity),
			0.0, 1.0);
	//diff_color = vec3(0.0,0.0,0.0); // TODO
	vec3 spec_color = clamp(
			material_s_color *
			pow(dot(p_reflected_dir_nn, view_dir_nn), material_shininess) *
			(p_light_s_intensity / distance_intensity),
			0.0, 1.0);
	//spec_color = vec3(0.0,0.0,0.0); // TODO

	return clamp(amb_color + diff_color + spec_color, 0.0, 1.0);
}