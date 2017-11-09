#version 330	// GLSL version

// model-view transformation
uniform mat4 transformation; //TODO rename to camera_transformation

uniform mat4 world_transformation;
uniform mat4 world_normals_transformation;

//TODO temp here
// Sampler to access the texture
//uniform vec3 sampler;

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
out vec2 cur_tex_coords;

out vec3 cur_normal;

out vec3 view_dir;

out vec3 view_dir_nn;

vec3 cur_camera_position;



//TODO temp
out vec4 fcolor;

void main() {

	// transform the vertex
	vec4 world_position = world_transformation * model_transformation * vec4(position, 1.);
    gl_Position = transformation * world_position;

	vec4 world_camera_position = vec4(camera_position, 1.);
	cur_camera_position = world_camera_position.xyz;

	view_dir = world_position.xyz - world_camera_position.xyz;
	view_dir_nn = normalize(view_dir);

	//TODO Temp
	float distance = length(view_dir);
	float x = 1 / (distance * distance);
	fcolor = vec4(x, x, x, 1.0);
	//fcolor = vec4(0., 0., 1., 1.);


	//vec4 world_normal = world_normals_transformation * model_normals_transformation * vec4(normal, 0.); //TODO
	vec4 world_normal = model_normals_transformation * vec4(normal, 0.); //TODO
	cur_normal = normalize(world_normal.xyz);

	// pass the texture coordinates to the fragment shader
	cur_tex_coords = tex_coords;

}