#include "env_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

env_layer::env_layer():
m_cam((float)engine::application::window().width(), (float)engine::application::window().height())
{
	//world setup
	engine::application::window().hide_mouse_cursor();// cursor lock
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");//setup mesh shader
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");//setup text shader
	//light setup
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 1.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);
	//skybox setup
	m_skybox = engine::skybox::create(100.f,
		{ engine::texture_2d::create("assets/textures/skybox/front.png", true),
		  engine::texture_2d::create("assets/textures/skybox/right.png", true),
		  engine::texture_2d::create("assets/textures/skybox/back.png", true),
		  engine::texture_2d::create("assets/textures/skybox/left.png", true),
		  engine::texture_2d::create("assets/textures/skybox/top.png", true),
		  engine::texture_2d::create("assets/textures/skybox/bottom.png", true)
		});
	
	for (auto i : meshStrings)
	{
		engine::ref <engine::model> core_model = engine::model::create(i);
	engine::game_object_properties core_props;
	core_props.meshes =core_model->meshes();
	core_props.textures = core_model->textures();
	core_props.position = { 0.f,0.f, 0.f };
	core_props.scale = glm::vec3(0.01f);
	planet.push_back(engine::game_object::create(core_props));
	}
	
	base_mat = engine::material::create(1.0f, glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_text_manager = engine::text_manager::create();
}
env_layer::~env_layer() {}
void env_layer::on_update(const engine::timestep& time_step)
{
	glm::vec3 rot_euler;
	//top half rotate
	rot_euler = planet[5]->rotation_euler();
	rot_euler.y += 1.f * time_step;
	planet[5]->set_rotation_euler(rot_euler);
	//bottom half rotate
	rot_euler = planet[6]->rotation_euler();
	rot_euler.y += -1.f * time_step;
	planet[6]->set_rotation_euler(rot_euler);
	//
	// gears rotate

	rot_euler = planet[1]->rotation_euler();
	rot_euler.z -= 1 * time_step;
	planet[1]->set_rotation_euler(rot_euler);

	rot_euler = planet[2]->rotation_euler();
	rot_euler.z += 1 * time_step;
	planet[2]->set_rotation_euler(rot_euler);

	rot_euler = planet[3]->rotation_euler();
	rot_euler.x += 1 * time_step;
	planet[3]->set_rotation_euler(rot_euler);

	rot_euler = planet[4]->rotation_euler();
	rot_euler.x -= 1 * time_step;
	planet[4]->set_rotation_euler(rot_euler);

	
	rot_euler = planet[7]->rotation_euler();
	rot_euler.x += 1.f * time_step;
	rot_euler.y += 0.3f * time_step;
	rot_euler.z += 0.1f * time_step;
	planet[7]->set_rotation_euler(rot_euler);
	//ring rotation
	rot_euler = planet[8]->rotation_euler();
	rot_euler.x += -1.f * time_step;
	rot_euler.y += 0.2f * time_step;
	rot_euler.z += -0.7f * time_step;
	planet[8]->set_rotation_euler(rot_euler);


	m_cam.on_update(time_step,closest_center,30.f);

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
	
	for(engine::ref<engine::game_object> obj: planet)
	{
			
		base_mat->submit(mesh_shader);
		
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, obj->position());
		obj_transform = rotateEuler(obj->rotation_euler(),obj_transform);
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
glm::mat4 env_layer::rotateEuler(glm::vec3 rot_angles, glm::mat4 matrix)
{
	matrix = glm::rotate(matrix, rot_angles.x, glm::vec3(1.f, 0.f, 0.f));
	matrix = glm::rotate(matrix, rot_angles.y, glm::vec3(0.f, 1.f, 0.f));
	matrix = glm::rotate(matrix, rot_angles.z, glm::vec3(0.f, 0.f, 1.f));
	return matrix;
}
