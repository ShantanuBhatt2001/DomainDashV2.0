#pragma once

#include<engine.h>
#include<vector>
#include<string>
class env_layer : public engine::layer
{
public:
	env_layer();
	~env_layer();
	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
private:
	//environment blocks
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
	std::vector<glm::vec3> rotation_vectors;// should be the same 
	engine::ref<engine::material> base_mat{}; //base material for all objects
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::text_manager>	m_text_manager{};//UI for input
	// cameras
	engine::perspective_camera m_cam;
	//lights
	engine::DirectionalLight            m_directionalLight;//world directional light
};
