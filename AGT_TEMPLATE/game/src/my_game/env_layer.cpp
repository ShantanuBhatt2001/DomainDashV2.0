#include "env_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

env_layer::env_layer():
m_cam((float)engine::application::window().width(), (float)engine::application::window().height())
{
	engine::application::window().hide_mouse_cursor();
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	m_skybox = engine::skybox::create(100.f,
		{ engine::texture_2d::create("assets/textures/skybox/front.png", true),
		  engine::texture_2d::create("assets/textures/skybox/right.png", true),
		  engine::texture_2d::create("assets/textures/skybox/back.png", true),
		  engine::texture_2d::create("assets/textures/skybox/left.png", true),
		  engine::texture_2d::create("assets/textures/skybox/top.png", true),
		  engine::texture_2d::create("assets/textures/skybox/bottom.png", true)
		});
	engine::ref<engine::cuboid> floor_cube = engine::cuboid::create(glm::vec3(3.f, 0.01f, 100.f), false);
	engine::game_object_properties floor_props;
	floor_props.position = { 0.f,-3.f,-100.f };
	floor_props.meshes = { floor_cube->mesh() };
	floor_props.type = 1;
	floor_props.scale = glm:: vec3(1.f);
	floor_props.bounding_shape = glm::vec3(1.f);
	environment.push_back(engine::game_object::create(floor_props));
	base_mat = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_text_manager = engine::text_manager::create();
}
env_layer::~env_layer() {}
void env_layer::on_update(const engine::timestep& time_step)
{
	m_cam.on_update(time_step);

}
void env_layer::on_render()
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_cam, mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_cam.position());
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_cam.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);
	engine::renderer::submit(mesh_shader, environment[0]);
	for(engine::ref<engine::game_object> obj: environment)
	{
			
		base_mat->submit(mesh_shader);
		std::cout << "length of environment" << environment.size();
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, obj->position());
		obj_transform = glm::rotate(obj_transform, obj->rotation_amount(),obj->rotation_axis());
		obj_transform = glm::scale(obj_transform, obj->scale());
		engine::renderer::submit(mesh_shader,obj_transform, obj);
		
	}
	engine::renderer::end_scene();
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Orange Text", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
	
	
}
void env_layer::on_event(engine::event& event)
{
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_TAB)
		{
			engine::render_command::toggle_wireframe();
			
		}
	}
}
