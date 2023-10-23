#pragma once
#include<engine.h>

class env_layer : public engine::layer
{
public:
	env_layer();
	~env_layer();
	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;
private:
	std::vector<engine::ref<engine::game_object>> environment;
	engine::ref<engine::material>		base_mat{};
	engine::perspective_camera m_cam;
	engine::DirectionalLight m_global_light;
};
