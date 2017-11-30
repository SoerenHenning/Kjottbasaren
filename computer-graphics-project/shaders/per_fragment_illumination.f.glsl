#version 330	// GLSL version

uniform int shading_effect;

uniform int window_width;
uniform int window_height;

// Sampler to access the texture
uniform sampler2D sampler;

// Time
//uniform float time;

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
uniform float material_texture_intensity;

// Per fragment texture coordinates
in vec2 cur_tex_coords;

in vec3 cur_normal;

in vec3 view_dir;

in vec3 view_dir_nn;

//TODO
in vec4 temp_world_pos;

in vec4 cur_camera_position;

// Per-frgament output color
out vec4 FragColor;

vec3 material_d_color_2;
vec3 material_a_color_2;

vec3 computeHeadlight();
vec3 computeDirectionalLight();


float discretize(float f, float d) {
    return floor(f*d)/d;
}

vec4 discretize(vec4 v, float d) {
    return vec4(discretize(v.x, d), discretize(v.y, d), discretize(v.z, d), discretize(v.w, d));
}

float rand(vec2 seed) {
    return fract(sin(dot(seed, vec2(12.9898,78.233))) * 43758.5453);
}

void main() {

	//vec4 texture = vec4(0.8,0.8,0.8,1.0);
	vec4 texture = texture2D(sampler, cur_tex_coords.st);

	FragColor = vec4(cur_tex_coords.s, cur_tex_coords.t,0.0,0.0);
	FragColor = texture;

	//FragColor = vec4(cur_tex_coords.s, cur_tex_coords.t, 0.0, 0.0);
	if (cur_tex_coords.s > 0.9) {
		//FragColor = vec4(0.0,0.0,1.0, 0.0);
	}
	//FragColor = temp_world_pos;

	return;

	material_d_color_2 = (material_texture_intensity * texture.xyz) + ((1-material_texture_intensity) * material_d_color);
	material_a_color_2 = (material_texture_intensity * texture.xyz) + ((1-material_texture_intensity) * material_a_color);

	vec3 directionalLight = computeDirectionalLight();
	vec3 headlight = computeHeadlight();
	vec3 color = directionalLight + headlight;

	FragColor = vec4(color, 1.0);



	/*
	// Some temporary code for gausisan blur
	float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
	float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
	if (material_texture_intensity) {
		vec4 blur_color = texture2D( sampler, cur_tex_coords.st * weight[0]);
		for (int i=1; i<5; i++) {
			blur_color += texture2D( sampler, ( cur_tex_coords.st+vec2(0.0, offset[i]) )) * weight[i];
			blur_color += texture2D( sampler, ( cur_tex_coords.st-vec2(0.0, offset[i]) ) ) * weight[i];
		}
		
		blur_color = texture2D( sampler, cur_tex_coords.st * 0.1);
		blur_color += texture2D( sampler, (cur_tex_coords.st + vec2(0.0, 1.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st + vec2(0.0, 2.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st + vec2(0.0, 3.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st + vec2(0.0, 4.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st + vec2(0.0, 5.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st - vec2(0.0, 1.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st - vec2(0.0, 2.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st - vec2(0.0, 3.0) )) * 0.1;
		blur_color += texture2D( sampler, (cur_tex_coords.st - vec2(0.0, 4.0) )) * 0.1;
		
		FragColor = blur_color;
		//FragColor = texture2D(sampler, cur_tex_coords.st);
	} else {
		FragColor = vec4(color, 1.0); //TODO
	}
	*/
	
	
	if (shading_effect == 1) {
		// Foggy Shader
		float distance = length(view_dir);
		float fog_intensity = 1 / (2.5 * distance * distance);
		FragColor = (FragColor * fog_intensity) + (vec4(0.5, 0.5, 0.5, 1.0) * (1-fog_intensity));
	} else if (shading_effect == 2) {
		// Black and White Shader
		float avg = 0.21 * FragColor.r + 0.72 * FragColor.g + 0.07 * FragColor.b;
		FragColor = vec4(avg, avg, avg, FragColor.a);
	} else if (shading_effect == 3) {
		// Discretizer Shader
		FragColor = discretize(FragColor, 2);
	} else if (shading_effect == 4) {
		// Noise Shader
		float random = rand(texture.xy);
		vec4 noise = vec4(random,random,random,1.0) / 2;
		FragColor = (vec4(1.0,1.0,1.0,1.0) - noise) * FragColor;
	} else if (shading_effect == 5) {
		// Vignette Shader
		float relative_width = (gl_FragCoord.x/window_width);
		float relative_height = (gl_FragCoord.y/window_height);
		float i = (-4.0 * relative_width * relative_width) + (4.0 * relative_width);
		float j = (-4.0 * relative_height * relative_height) + (4.0 * relative_height);
		FragColor = FragColor * vec4(i,i,i,1.0) * vec4(j,j,j,1.0);
	} else if (shading_effect == 6) {
		// Normal Highlight Shader
		FragColor = vec4(((cur_normal/2) + vec3(0.5, 0.5, 0.5)), 1.0);
	}

	if (temp_world_pos.x > 0) {
		//FragColor = FragColor + vec4(0.0,1.0,0.0,0.0); // green
	} 

	if (temp_world_pos.z > 0) {
		//FragColor = FragColor + vec4(0.0,0.0,1.0,0.0); // blue
	}

	if (temp_world_pos.y > 0) {
		//FragColor = FragColor + vec4(1.0,0.0,0.0,0.0); // red
	}
	//FragColor = 

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

	float dot_p_light_normal = dot(-p_light_dir_nn, normal_nn);   // notice the minus!
	vec3 p_reflected_dir_nn = p_light_dir_nn + 2. * dot_p_light_normal * normal_nn;
	// should be already normalized, but we "need" to correct numerical errors
	p_reflected_dir_nn = normalize(p_reflected_dir_nn); 

	float distance = length(p_light_dir);
	float distance_intensity = p_light_d_intensity_k_const + (p_light_d_intensity_k_linear * distance) + (p_light_d_intensity_k_square * distance * distance);

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
