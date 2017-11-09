#version 330	// GLSL version

// Sampler to access the texture
uniform sampler2D sampler;

// Directional light
uniform vec3 d_light_direction;
uniform vec3 d_light_direction_temp;
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
uniform bool material_texture;

// Per fragment texture coordinates
in vec2 cur_tex_coords;

// Per-fragment color coming from the vertex shader
in vec4 fcolor; // TODO

in vec3 cur_normal;

in vec3 view_dir;

in vec3 view_dir_nn;

in vec4 cur_camera_position;

// Per-frgament output color
out vec4 FragColor;

vec3 material_d_color_2;
vec3 material_a_color_2;

vec3 computeHeadlight();
vec3 computeDirectionalLight();


void main() {

	vec4 texture = texture2D(sampler, cur_tex_coords.st);

	if (material_texture) {
		float texture_intensity = 0.;
		material_d_color_2 = (texture_intensity * texture.xyz) + ((1-texture_intensity) * material_d_color);
		material_a_color_2 = (texture_intensity * texture.xyz) + ((1-texture_intensity) * material_a_color);
		//material_d_color_2 = vec3(0.0, 1.0, 0.0);
	} else {
		material_d_color_2 = material_d_color;
		material_a_color_2 = material_a_color;
		//material_d_color_2 = vec3(0.0, 1.0, 0.0);
	}

	vec3 directionalLight = computeDirectionalLight();
	vec3 headlight = computeHeadlight();
	vec3 color = directionalLight + headlight;
	
	FragColor = vec4(color, 1.0); //TODO

	//color = normalize(cur_normal) * -1.0;
	//if (material_texture) {
	/*
	if (false) {
		FragColor = texture * vec4(color, 1.0); //TODO
	} else {
		FragColor = vec4(color, 1.0); //TODO
	}
	*/

	//FragColor = fcolor;
	//FragColor = vec4(material_d_color_2, 1.0);
}


vec3 computeDirectionalLight() {
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
	vec3 normal_nn = normalize(cur_normal);
	vec3 d_light_dir_nn = normalize(d_light_direction);

	float dot_d_light_normal = dot(-d_light_dir_nn, normal_nn);   // notice the minus!
	vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal_nn;
	// should be already normalized, but we "need" to correct numerical errors
	d_reflected_dir_nn = normalize(d_reflected_dir_nn);
	
	// compute the color contribution	
	vec3 amb_color = clamp(
			material_a_color_2 * d_light_a_color * d_light_a_intensity,
			0.0, 1.0);
	//amb_color = vec3(0.0,0.0,0.0); // TODO
	vec3 diff_color = clamp(
			material_d_color_2 * d_light_d_color * dot_d_light_normal * d_light_d_intensity,
			0.0, 1.0);
	//diff_color = vec3(0.0,0.0,0.0);  // TODO
	vec3 spec_color = clamp(
			material_s_color *
			d_light_s_color *
			pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess) *
			d_light_s_intensity,
			0.0, 1.0);
	//spec_color = vec3(0.0,0.0,0.0); //TODO

	return clamp(amb_color + diff_color + spec_color, 0.0, 1.0);
}



vec3 computeHeadlight() {

	//TODO remove p light dir nn from passing
	// TODO: do the same for the headlight!
	// notice that for the headlight dot(view_dir, light_dir) = ...
	
	//p_light_dir_nn = view_dir_nn;
	
	//vec3 p_light_dir_nn = -view_dir_nn;

	//TODO style
	// position mins cam position
	vec3 p_light_dir = view_dir;
	vec3 p_light_dir_nn = view_dir_nn;
	
	vec3 normal_nn = normalize(cur_normal);

	float dot_p_light_normal = dot(-p_light_dir_nn, cur_normal);   // notice the minus!
	vec3 p_reflected_dir_nn = p_light_dir_nn + 2. * dot_p_light_normal * cur_normal;
	// should be already normalized, but we "need" to correct numerical errors
	p_reflected_dir_nn = normalize(p_reflected_dir_nn); 

	float distance = length(p_light_dir);
	float distance_intensity = p_light_d_intensity_k_const + (p_light_d_intensity_k_linear * distance) + (p_light_d_intensity_k_square * distance * distance);

	//return dot_p_light_normal;
	//return vec3(distance_intensity, distance_intensity, distance_intensity);

	// compute the color contribution
	vec3 amb_color = clamp(
			material_a_color_2 * p_light_a_color * p_light_a_intensity,
			0.0, 1.0);
	//amb_color = vec3(0.0,0.0,0.0); // TODO
	vec3 diff_color = clamp(
			material_d_color_2 * p_light_d_color * dot_p_light_normal * (p_light_d_intensity / distance_intensity),
			0.0, 1.0);
	//diff_color = vec3(0.0,0.0,0.0); // TODO
	vec3 spec_color = clamp(
			material_s_color *
			p_light_s_color * 
			pow(dot(p_reflected_dir_nn, -view_dir_nn), material_shininess) *
			(p_light_s_intensity / distance_intensity),
			0.0, 1.0);
	//spec_color = vec3(0.0,0.0,0.0); // TODO

	return clamp(amb_color + diff_color + spec_color, 0.0, 1.0);
}
