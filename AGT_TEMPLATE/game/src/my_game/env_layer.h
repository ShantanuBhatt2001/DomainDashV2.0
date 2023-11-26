#pragma once

#include<engine.h>
#include<vector>
#include<string>
#include "cross_fade.h"

class env_layer : public engine::layer
{

private:
	// count of enemy ships
	int enemy_count =100;
	// count of protection spheres
	int prot_count = 400;
	//environment blocks
	//used for boid behaviour
	struct Boid {
		glm::vec3 position{ 0 };
		glm::vec3 velocity{ 0 };
	};
	engine::ref<engine::game_object> protection_sphere;
	std::vector<glm::vec3> sphere_pos;
	//paths for mesh models location
	std::vector<std::string> meshStrings = { "assets/models/static/core.fbx",
		"assets/models/static/gear1.fbx",
		"assets/models/static/gear2.fbx",
		"assets/models/static/gear3.fbx",
		"assets/models/static/gear4.fbx",
		"assets/models/static/topHalf.fbx",
		"assets/models/static/lowerHalf.fbx",
		"assets/models/static/innerRing.fbx",
		"assets/models/static/outerRing.fbx" };

	std::string ship_loc = "assets/models/static/ship.fbx";
	std::string play_loc = "assets/models/static/play_model.fbx";
	std::string help_loc = "assets/models/static/help_model.fbx";
	std::string exit_loc = "assets/models/static/exit_model.fbx";
	glm::mat4 rotateEuler(glm::vec3 rot_angles,glm::mat4 matrix);//rotate object with euler angles
	glm::vec3 closest_center= glm::vec3(0.f,0.f,0.f);// centre of gravity and camera rotation for game
	
	std::vector<engine::ref<engine::game_object>> planet; // array of all meshes for planet
	engine::ref<engine::game_object> enemy;// enemy ships
	engine::ref<engine::game_object> gun;//gun cylinder
	std::vector<Boid> boids;//boid positions and vectors in 2d


	// meshes for menu

	engine::ref<engine::game_object> PLAY;
	engine::ref<engine::game_object> HELP;
	engine::ref<engine::game_object> EXIT;

	engine::ref<engine::material> base_mat{}; //base material for spheres
	engine:: ref<engine::material> gun_mat{};//mat for guns
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::text_manager>	m_text_manager{};//UI for input
	engine::ref<cross_fade>							m_cross_fade{};
	// cameras
	engine::perspective_camera m_3d_cam;
	engine::orthographic_camera m_2d_cam;
	//lights
	engine::DirectionalLight            m_directionalLight;//world directional light
	engine::PointLight m_pointLight;
	uint32_t num_point_lights = 1;
	engine::ref<engine::material> m_lightsource_material{ };

	//player
	engine::ref<engine::game_object> player;
	bool canJump = true;// bool for start of render
public:
	env_layer();
	~env_layer();
	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
	void updateBoids(std::vector<Boid>& boids, float deltaTime);

	bool isPlay();
	bool isExit();
	bool isHelp();
	glm::vec3 map2DToSphere(double x, double y, double screenWidth, double screenHeight, double sphereRadius);
	glm::vec3 get_random_inside_unit_sphere();

	glm::vec3 player_accel{ 0 };
	glm::vec3 player_velocity{ 0 };
	glm::vec3 player_position{ 0 };
};
