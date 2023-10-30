#pragma once

#include<engine.h>
#include<vector>
#include<string>

class env_layer : public engine::layer
{

private:
	//environment blocks
	struct Boid {
		glm::vec3 position{ 0 };
		glm::vec3 velocity{ 0 };
	};
	std::vector<std::string> meshStrings = { "assets/models/static/core.fbx",
		"assets/models/static/gear1.fbx",
		"assets/models/static/gear2.fbx",
		"assets/models/static/gear3.fbx",
		"assets/models/static/gear4.fbx",
		"assets/models/static/topHalf.fbx",
		"assets/models/static/lowerHalf.fbx",
		"assets/models/static/innerRing.fbx",
		"assets/models/static/outerRing.fbx" };
	
	glm::mat4 rotateEuler(glm::vec3 rot_angles,glm::mat4 matrix);
	glm::vec3 closest_center= glm::vec3(0.f,0.f,0.f);// centre of gravity and camera rotation for game
	
	std::vector<engine::ref<engine::game_object>> planet; // array of all meshes for planet
	std::vector<engine::ref<engine::game_object>> enemies;// enemy ships
	std::vector<Boid> boids;//boid positions and vectors in 2d
	engine::ref<engine::material> base_mat{}; //base material for all objects
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::text_manager>	m_text_manager{};//UI for input
	// cameras
	engine::perspective_camera m_cam;
	//lights
	engine::DirectionalLight            m_directionalLight;//world directional light
	engine::PointLight m_pointLight;
	uint32_t num_point_lights = 1;
	engine::ref<engine::material> m_lightsource_material{ };
	bool start = true;// bool for start of render
public:
	env_layer();
	~env_layer();
	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void updateBoids(std::vector<Boid>& boids, float deltaTime);
	glm::vec3 map2DToSphere(double x, double y, double screenWidth, double screenHeight, double sphereRadius);
	glm::vec3 get_random_inside_unit_sphere();
};
