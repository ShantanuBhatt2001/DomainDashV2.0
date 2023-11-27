#include "main_scene.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"


mainscene_layer::mainscene_layer() :
	m_2d_cam(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_cam((float)engine::application::window().width(), (float)engine::application::window().height())
{
	this->set_active(false);
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->play("music");



	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	m_directionalLight.submit(mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gMatSpecularIntensity", 1.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gSpecularPower", 10.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("transparency", 1.0f);

//skybox initialisation
	m_skybox = engine::skybox::create(100.f,
		{ engine::texture_2d::create("assets/textures/skybox/front.png", true),
		  engine::texture_2d::create("assets/textures/skybox/right.png", true),
		  engine::texture_2d::create("assets/textures/skybox/back.png", true),
		  engine::texture_2d::create("assets/textures/skybox/left.png", true),
		  engine::texture_2d::create("assets/textures/skybox/top.png", true),
		  engine::texture_2d::create("assets/textures/skybox/bottom.png", true)
		});


	//planet initialisation
	//current initialisation with one sphere and 3 planets
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.bounding_shape = glm::vec3(0.5f);


	world_planets.push_back(planet{ glm::vec3{0.f },10.f });
	planet_gameObjects.push_back(engine::game_object::create(sphere_props));
active_planet = planet{ glm::vec3{0.f },5.f };

 
	world_planets.push_back(planet{ glm::vec3{0.5f},10.f });
	planet_gameObjects.push_back(engine::game_object::create(sphere_props));

	world_planets.push_back(planet{ glm::vec3{-0.5f},10.f });
	planet_gameObjects.push_back(engine::game_object::create(sphere_props));

	planet_mat=engine::material::create(1.0f, glm::vec3(1.0f, 1.f, 1.f),
		glm::vec3(1.0f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


}

mainscene_layer::~mainscene_layer()
{

}

void mainscene_layer::on_update(const engine::timestep& time_step)
{

}
void mainscene_layer::on_render()
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	// Set up  shader. (renders textures and materials)
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_cam, mesh_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_cam.position());

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_cam.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);

	//planet render

	planet_mat->submit(mesh_shader);
	for (int i = 0; i < world_planets.size(); i++)
	{
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::scale(obj_transform, glm::vec3{ world_planets[i].radius });
		obj_transform = glm::translate(obj_transform, world_planets[i].position);
		engine::renderer::submit(mesh_shader, obj_transform, planet_gameObjects[i]);
	}

}

void mainscene_layer::on_event(engine::event& event)
{

}

void mainscene_layer::update_boid(spawn_ship& spawn_ship_instance)
{

}
void mainscene_layer::update_follow(follow_enemy& follow_instance)
{

}

void mainscene_layer::update_grenadier(grenadier_enemy& grenadier_instance)
{

}

void mainscene_layer::update_grenade(grenade& grenade_instance)
{

}

void mainscene_layer::update_trapper(trapper_enemy& trapper_instance)
{

}

void mainscene_layer::update_trap(trap& trap_instance)
{

}

void mainscene_layer::update_player()
{

}
