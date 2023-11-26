#pragma once
#include <engine.h>

class MainMenu : public engine::layer
{
public:
	MainMenu();
	~MainMenu();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;

private:
	const float radius = 3.f;
		Planet activePlanet;

		glm::vec2 InputVector = { 0.f,0.f };

		float forceAmount=10.f;
	void CheckCollision();

	engine::ref<engine::skybox>			m_skybox{};
	std::vector<Planet>	Planets;
	engine::ref<engine::game_object>    m_player{};

	engine::ref<engine::material>		m_Planet_mat{};
	engine::ref<engine::material>		m_Player_mat{};
	engine::DirectionalLight            m_directionalLight;

	std::vector<engine::ref<engine::game_object>>     m_game_objects{};

	engine::ref<engine::bullet_manager> m_physics_manager{};
	engine::ref<engine::audio_manager>  m_audio_manager{};
	engine::orthographic_camera       m_2d_camera;
	engine::perspective_camera        m_3d_camera;
};
