#include "MainMenu.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

MainMenu::MainMenu()
	:m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{
	// Hide the mouse and lock it inside the window
	//engine::input::anchor_mouse(true);
	engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce.wav", engine::sound_type::spatialised, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/DST-impuretechnology.mp3", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->play("music");
	


	// Initialise the shaders, materials and lights
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

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
			(float)engine::application::window().height()));
	m_Planet_mat = engine::material::create(1.0f, glm::vec3(1.0f, 1.f, 1.f),
		glm::vec3(1.0f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_Player_mat = engine::material::create(1.0f, glm::vec3(1.0f, 0.01f, 0.07f),
		glm::vec3(1.0f, 0.01f, 0.07f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});
	Planet tempPlanet;

	Planets.push_back(tempPlanet);
	activePlanet = Planets[0];


	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.05f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 10.f, 0.f, 0.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_player= engine::game_object::create(sphere_props);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	glm::vec3 gravDir = activePlanet.Position() - m_player->position();
	glm::vec3 camPos = activePlanet.Position() - (glm::normalize(gravDir) * radius);
	std::cout << "cam pos: " << camPos << std::endl;
	m_3d_camera.on_update(camPos, activePlanet.Position());
}

MainMenu::~MainMenu() {}

void MainMenu::on_update(const engine::timestep& time_step) {

	glm::vec3 gravDir = activePlanet.Position() - m_player->position();
	glm::vec3 camPos = activePlanet.Position() - (glm::normalize(gravDir) * radius);
	std::cout << "cam pos: "<< camPos<<std::endl;
	m_3d_camera.on_update(camPos, activePlanet.Position());
	
	glm::vec3 velocity = m_3d_camera.right_vector() * InputVector.x + m_3d_camera.up_vector() * InputVector.y;
	
	glm::vec3 newPos = m_player->position() + (velocity * (float)time_step);
	if (glm::length(newPos - activePlanet.Position()) > activePlanet.Radius());
	{
		newPos = glm::normalize(newPos - activePlanet.Position()) * activePlanet.Radius();
	}
	m_player->set_position(newPos);
	InputVector = { 0.f,0.f };

	





}

void MainMenu::on_render()
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();

	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_camera, mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	

	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);
	m_Planet_mat->submit(mesh_shader);
	for (Planet planet : Planets)
	{
		
		glm::mat4 planetTransform = glm::mat4(1.f);
		planetTransform = glm::translate(planetTransform, glm::vec3(0.f, 0.f, 0.f));
		engine::renderer::submit(mesh_shader,planetTransform,planet.m_Planet);
	}
	m_Player_mat->submit(mesh_shader);
	
	engine::renderer::submit(mesh_shader, m_player);
	

}

void MainMenu::on_event(engine::event& event)
{
	
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_A)
		{
			InputVector += glm::vec2{-1.f, 0.f};
		}
		if (e.key_code() == engine::key_codes::KEY_D)
		{
			InputVector += glm::vec2{ 1.f, 0.f };
		}
		if (e.key_code() == engine::key_codes::KEY_W)
		{
			InputVector += glm::vec2{ 0.f, 1.f };
		}
		if (e.key_code() == engine::key_codes::KEY_S)
		{
			InputVector += glm::vec2{ 0.f, -1.f };
		}
		
	}
}

void MainMenu::CheckCollision()
{

}
