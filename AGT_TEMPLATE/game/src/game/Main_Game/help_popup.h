#pragma once
#include <engine.h>

class quad;

class help_popup
{
public:
	help_popup(const std::string& path, float max_time, float width, float height);
	~help_popup();

	void on_update(const engine::timestep& time_step);
	void on_render(engine::ref<engine::shader> shader);
	void activate();
	void deactivate();
	bool is_active() { return s_active; }
	static engine::ref<help_popup> create(const std::string& path, float max_time, float width, float height);

private:
	bool s_active;

	float m_timer;
	float m_max_time;

	engine::ref<engine::texture_2d> m_texture;
	float m_transparency;
	engine::ref<quad> m_quad;
};
