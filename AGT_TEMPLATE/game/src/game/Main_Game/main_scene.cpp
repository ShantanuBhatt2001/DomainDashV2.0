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
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("gNumPointLights",0 );
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
	engine::ref<engine::sphere> planet_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties planet_props;
	planet_props.meshes = { planet_shape->mesh() };
	planet_props.bounding_shape = glm::vec3(0.5f);


	world_planets.push_back(planet{ glm::vec3{0.f},10.f });
	planet_gameObjects.push_back(engine::game_object::create(planet_props));
	active_planet = world_planets[0];

 
	world_planets.push_back(planet{ glm::vec3{20.f},10.f });
	planet_gameObjects.push_back(engine::game_object::create(planet_props));

	world_planets.push_back(planet{ glm::vec3{-20.f},10.f });
	planet_gameObjects.push_back(engine::game_object::create(planet_props));

	planet_mat=engine::material::create(1.0f, glm::vec3(1.0f, 1.f, 1.f),
		glm::vec3(1.0f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


	//player initialization

	player_mat = engine::material::create(1.0f, glm::vec3(0.0f, 0.f, 0.f),
		glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


	engine::ref<engine::sphere> player_shape = engine::sphere::create(10, 20, 0.1f);
	engine::game_object_properties player_props;
	player_props.meshes = { planet_shape->mesh() };
	player_props.bounding_shape = glm::vec3(0.1f);
	player_object = engine::game_object::create(player_props);

	m_player.position= glm::normalize(m_3d_cam.position() - active_planet.position) * 6.f;

	// spawn ships initialisation

	engine::ref <engine::model> ship_model = engine::model::create(ship_loc);
	engine::game_object_properties ship_props;
	ship_props.meshes = ship_model->meshes();
	ship_props.textures = ship_model->textures();
	ship_props.position = { 0.f,0.f, 0.f };
	ship_props.scale = glm::vec3(0.01f);
	spawn_object = engine::game_object::create(ship_props);
	for (int i = 0; i < count; i++)
	{
		// boid setup

		spawn_ship temp_boid;
		temp_boid.position = get_random_inside_unit_sphere() * 6.f;
		
		glm::vec3 origin_vec = active_planet.position - temp_boid.position;
		glm::vec3 up_vec = glm::cross(get_random_inside_unit_sphere(), origin_vec);
		glm::vec3 right_vec = glm::cross(origin_vec, up_vec);

		temp_boid.velocity = glm::normalize(up_vec + right_vec);
		spawn_ships.push_back(temp_boid);
		
	}


	//follow enemies initialization
	follow_mat= engine::material::create(1.0f, glm::vec3(1.0f, 0.f, 0.f),
		glm::vec3(1.0f, 0.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f),1.f);
	engine::ref<engine::sphere> follow_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties follow_props;
	follow_props.meshes = { follow_shape->mesh() };
	follow_props.bounding_shape = glm::vec3(0.1f);
	follow_object = engine::game_object::create(follow_props);

	follow_enemy temp;
	temp.position = get_random_inside_unit_sphere() * 10.f;
	temp.active_planet = active_planet;
	follow_enemies.push_back(temp);

}

mainscene_layer::~mainscene_layer()
{

}

void mainscene_layer::on_update(const engine::timestep& time_step)
{
	update_player(time_step);
		update_boid(time_step);

		
			update_follow( time_step);
	

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
		
		obj_transform = glm::translate(obj_transform, world_planets[i].position);
		obj_transform = glm::scale(obj_transform, glm::vec3{ world_planets[i].radius });
		engine::renderer::submit(mesh_shader, obj_transform, planet_gameObjects[i]);
	}


	player_mat->submit(mesh_shader);
	glm::mat4 player_transform(1.f);

	player_transform=glm::translate(player_transform, m_player.position);
	engine::renderer::submit(mesh_shader, player_transform, player_object);


	// spawn ships render
	for (int i = 0; i < spawn_ships.size(); i++)
	{
		spawn_object->turn_towards(spawn_ships[i].velocity);
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, spawn_ships[i].position);
		obj_transform = glm::rotate(obj_transform, spawn_object->rotation_amount(), spawn_object->rotation_axis());
		obj_transform = glm::scale(obj_transform, spawn_object->scale());
		
		engine::renderer::submit(mesh_shader, obj_transform, spawn_object);
	}


	// render follow enenmies
	follow_mat->submit(mesh_shader);
	for (int i = 0; i < follow_enemies.size(); i++)
	{
		
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, follow_enemies[i].position);
		obj_transform = glm::scale(obj_transform, follow_object->scale());
		std::cout << follow_enemies[i].position << std::endl;
		

		engine::renderer::submit(mesh_shader, obj_transform, follow_object);
	}
	
}

void mainscene_layer::on_event(engine::event& event)
{

}

void mainscene_layer::update_boid( const engine::timestep& time_step)
{
	float separationRadius = 10.0f;
	float alignmentRadius = 5.0f;
	float cohesionRadius = 10.f;
	float maxSpeed = 50.0f;

	for (spawn_ship& boid :spawn_ships) {
		// Update the boid's position based on its velocity and time step
		boid.position += boid.velocity * (float)time_step;

		// simple boid behaviour implemented here

		glm::vec3 separation(0.0f);
		glm::vec3 alignment(0.0f);
		glm::vec3 cohesion(0.0f);

		for (const spawn_ship& other : spawn_ships) {
			if (&boid != &other) {
				float distance = glm::distance(boid.position, other.position);
				if (distance < separationRadius) {
					separation -= (other.position - boid.position) / distance;
				}
				if (distance < alignmentRadius) {
					alignment += other.velocity;
				}
				if (distance < cohesionRadius) {
					cohesion += other.position;
				}
			}
		}

		// Adjust the boid's velocity based on the behavior rules
		boid.velocity += 0.8f * separation + 0.05f * alignment + 0.001f * cohesion;
		float speed = glm::length(boid.velocity);

		//limiting the movement to be near 20 units away from center( could be not hard coded)
		if (glm::length(boid.position - active_planet.position) > 10.f)
			boid.velocity -= (boid.position - active_planet.position) * 0.1f;
		if (glm::length(boid.position - active_planet.position) <= 10.f)
		{
			boid.velocity += (boid.position - active_planet.position) * 2.f;
		}
		//limiting the max speed of the boid
		if (speed > maxSpeed) {
			boid.velocity = (boid.velocity / speed) * maxSpeed;
		}



	}
}
void mainscene_layer::update_follow( const engine::timestep& time_step)
{

	for (follow_enemy& follow_instance : follow_enemies)
	{
		follow_instance.accel = glm::vec3{ 0.f };
		glm::vec3  player_vec = m_player.position - follow_instance.position;
		glm::vec3 origin_vec = follow_instance.active_planet.position - follow_instance.position;
		follow_instance.accel = glm::normalize(player_vec+origin_vec) * follow_accel;
		follow_instance.accel += origin_vec * 20.f;

		follow_instance.accel += -(follow_instance.velocity - glm::dot(follow_instance.velocity, glm::normalize(origin_vec)) * glm::normalize(origin_vec)) * 5.f;

		follow_instance.position += follow_instance.velocity * (float)time_step;
		follow_instance.velocity += follow_instance.accel * (float)time_step;

		if (glm::length(follow_instance.position - follow_instance.active_planet.position) <= 6.f)
		{
			follow_instance.velocity -= glm::dot(follow_instance.velocity, glm::normalize(follow_instance.active_planet.position - follow_instance.position)) * glm::normalize(follow_instance.active_planet.position - follow_instance.position);
		}
	}
	

}

void mainscene_layer::update_grenadier(grenadier_enemy& grenadier_instance, const engine::timestep& time_step)
{

}

void mainscene_layer::update_grenade(grenade& grenade_instance, const engine::timestep& time_step)
{

}

void mainscene_layer::update_trapper(trapper_enemy& trapper_instance, const engine::timestep& time_step)
{

}

void mainscene_layer::update_trap(trap& trap_instance, const engine::timestep& time_step)
{

}

void mainscene_layer::update_player(const engine::timestep& time_step)
{
	m_player.accel = glm::vec3{ 0 };
	glm::vec3 input_accel = glm::vec3{ 0 };
	//player acceleration for movement
	// player update
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
		m_player.accel -= m_3d_cam.right_vector();
	if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
		m_player.accel += m_3d_cam.right_vector();
	if (engine::input::key_pressed(engine::key_codes::KEY_W)) // Up
		m_player.accel += m_3d_cam.up_vector();
	if (engine::input::key_pressed(engine::key_codes::KEY_S)) // Down
		m_player.accel -= m_3d_cam.up_vector();
	if (glm::length(m_player.accel) != 0.f)
	{
		
		m_player.accel = glm::normalize(m_player.accel) * 15.f * glm::length(active_planet.position - m_player.position);
	}

	//player acceleration for gravity
	m_player.accel += glm::normalize(active_planet.position - m_player.position) * 20.f * glm::length(active_planet.position - m_player.position);


	//drag force: not affecting the force towards the center
	m_player.accel += -(m_player.velocity - glm::dot(m_player.velocity, glm::normalize(active_planet.position - m_player.position)) * glm::normalize(active_planet.position - m_player.position)) * 5.f;

	//updating player position
	m_player.position += m_player.velocity * (float)time_step;

	//updating player velocity
	m_player.velocity += m_player.accel * (float)time_step;


	//collision detection between player and planet
	if (glm::length(m_player.position - active_planet.position) <= active_planet.radius)
	{
		m_player.can_jump = true;
		m_player.accel -= glm::dot(m_player.accel, (active_planet.position - m_player.position)) * glm::normalize(active_planet.position - m_player.position);
		m_player.velocity -= glm::dot(m_player.velocity, glm::normalize(active_planet.position - m_player.position)) * glm::normalize(active_planet.position- m_player.position);
	}
	else
		m_player.can_jump= false;

	if (engine::input::key_pressed(engine::key_codes::KEY_SPACE) && m_player.can_jump)
	{
		std::cout << "called jump";
		m_player.velocity += m_3d_cam.front_vector() * -40.f;
	}
	m_3d_cam.pos_update(active_planet.position + glm::normalize(m_player.position - active_planet.position) * 50.f, active_planet.position);
}



//utility functions

glm::vec3 mainscene_layer::get_random_inside_unit_sphere()
{
	float theta = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * glm::pi<float>())));
	float z = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 2.f))) - 1.f;
	glm::vec3 random_vec(1.f);
	random_vec.x = sqrt(1 - (z * z)) * glm::cos(theta);
	random_vec.y = sqrt(1 - (z * z)) * glm::sin(theta);
	random_vec.z = z;
	return random_vec;
}

