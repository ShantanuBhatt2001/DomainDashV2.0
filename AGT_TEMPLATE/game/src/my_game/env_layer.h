#pragma once

#include<engine.h>
#include<vector>
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
	std::vector<engine::ref<engine::game_object>> environment;
	engine::ref<engine::material> base_mat{};
	engine::ref<engine::skybox>			m_skybox{};
	engine::ref<engine::text_manager>	m_text_manager{};
	// cameras
	engine::perspective_camera m_cam;
	//lights
	engine::DirectionalLight            m_directionalLight;
};
