#include "pch.h"
#include "help_popup.h"
#include "quad.h"

help_popup::help_popup(const std::string& path, float max_time, float width, float height)
{
	m_max_time = max_time;
	m_texture = engine::texture_2d::create(path, true);

	m_transparency = 1.0f;

	m_quad = quad::create(glm::vec2(width, height));
	s_active = false;
}

help_popup::~help_popup()
{}

void help_popup::on_update(const engine::timestep& time_step)
{
	if (!s_active)
		return;

	m_timer += (float)time_step;

	
	
}

void help_popup::on_render(engine::ref<engine::shader> shader)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("transparency", m_transparency);
	if (!s_active)
		return;
	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.1f));
	//setup  cross fade material. create it unlit so as to be unaffected by world lights
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("lighting_on", false);
	m_texture->bind();
	engine::renderer::submit(shader, m_quad->mesh(), transform);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->
		set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("has_texture", false);

}

void help_popup::activate()
{
	s_active = true;
	m_timer = 0.0f;
}

void help_popup::deactivate()
{
	s_active = false;
}

engine::ref<help_popup> help_popup::create(const std::string& path, float max_time, float width, float height)
{
	return std::make_shared<help_popup>(path, max_time, width, height);
}
