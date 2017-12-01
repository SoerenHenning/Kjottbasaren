#version 330	// GLSL version

// model-view transformation
uniform mat4 transformation; //TODO rename to camera_transformation

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

uniform mat4 model_transformation;
uniform mat4 model_normals_transformation;

uniform int model_id;

// Object material
uniform vec3 material_a_color;
uniform vec3 material_d_color;
uniform vec3 material_s_color;
uniform float material_shininess;

// vertex position
layout (location = 0) in vec3 position; 

// vertex texture coordinates
layout (location = 1) in vec2 tex_coords;

// vertex normals
layout (location = 2) in vec3 normal; 

// pass the texture coordinates to the fragment shader
out vec2 cur_tex_coords;

out vec3 cur_normal;

out vec3 view_dir;

out vec3 view_dir_nn;

out vec4 temp_world_pos;

vec3 cur_camera_position;


void main() {
	// transform the vertex
	vec4 world_position = model_transformation * vec4(position.x, position.y, position.z, 1.);
    world_position = vec4(-world_position.x ,world_position.y,world_position.z,world_position.w); // Reflect on x-axis

	gl_Position = transformation * world_position;

	//TODO
	temp_world_pos = world_position;

	vec4 world_camera_position = vec4(camera_position, 1.);
	cur_camera_position = world_camera_position.xyz;

	view_dir = world_position.xyz - world_camera_position.xyz;
	view_dir_nn = normalize(view_dir);

	vec4 world_normal = model_normals_transformation * vec4(normal, 0.);
	cur_normal = normalize(world_normal.xyz);

	// pass the texture coordinates to the fragment shader
	cur_tex_coords = tex_coords;
}