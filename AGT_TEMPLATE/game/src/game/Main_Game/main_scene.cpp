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
	m_audio_manager->load_sound("assets/audio/Freesound-GrenaderLauncher.wav", engine::sound_type::spatialised, "launch_grenade"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/Freesound-Health.wav", engine::sound_type::spatialised, "health"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/Freesound-trap.wav", engine::sound_type::spatialised, "trap"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/freesound-damage.wav", engine::sound_type::spatialised, "damage"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/Freesound-grenadeExplosion.wav", engine::sound_type::spatialised, "grenade_explosion"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/Freesound-bgmusic.wav", engine::sound_type::track, "music");  // Royalty free music from http://www.nosoapradio.us/
	m_audio_manager->play("music");


	
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("gNumPointLights",0 );
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity =0.65f;
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
	m_skybox = engine::skybox::create(200.f,
		{ engine::texture_2d::create("assets/textures/skybox/front.png", true),
		  engine::texture_2d::create("assets/textures/skybox/right.png", true),
		  engine::texture_2d::create("assets/textures/skybox/back.png", true),
		  engine::texture_2d::create("assets/textures/skybox/left.png", true),
		  engine::texture_2d::create("assets/textures/skybox/top.png", true),
		  engine::texture_2d::create("assets/textures/skybox/bottom.png", true)
		});


	//planet initialisation
	//current initialisation with one sphere and 3 planets
	engine::ref<engine::sphere> planet_shape = engine::sphere::create(10, 20,1.f);
	engine::game_object_properties planet_props;
	planet_props.meshes = { planet_shape->mesh() };
	planet_props.bounding_shape = glm::vec3(0.5f);


	world_planets.push_back(planet{ glm::vec3{0.f},10.f });
	planet_gameObjects.push_back(engine::game_object::create(planet_props));
	active_planet = world_planets[0];

	planet_mat=engine::material::create(1.0f, glm::vec3(0.0f, 0.f, 0.f),
		glm::vec3(0.0f, 0.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


	//player initialization

	player_mat = engine::material::create(1.0f, glm::vec3(1.0f,1.f, 1.f),
		glm::vec3(1.0f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


	damage =cross_fade::create("assets/textures/Red.bmp", 0.4f, 1.6f, 0.9f);
	trapped = cross_fade::create("assets/textures/Yellow.bmp", 0.5f, 1.6f, 0.9f);
	engine::ref<engine::sphere> player_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties player_props;
	player_props.meshes = { planet_shape->mesh() };
	player_props.bounding_shape = glm::vec3(0.1f);
	player_object = engine::game_object::create(player_props);

	m_player.position= glm::normalize(m_3d_cam.position() - active_planet.position) * 6.f;
	m_player.current_hitp = m_player.max_hitp;



	// spawn ships initialisation

	engine::ref <engine::model> ship_model = engine::model::create(ship_loc);
	engine::game_object_properties ship_props;
	ship_props.meshes = ship_model->meshes();
	ship_props.textures = ship_model->textures();
	ship_props.position = { 0.f,0.f, 0.f };
	ship_props.scale = glm::vec3(0.005f);
	spawn_object = engine::game_object::create(ship_props);
	for (int i = 0; i < count; i++)
	{
		// boid setup

		spawn_ship temp_boid;
		temp_boid.position = get_random_inside_unit_sphere() *20.f;
		
		glm::vec3 origin_vec = active_planet.position - temp_boid.position;
		glm::vec3 up_vec = glm::cross(get_random_inside_unit_sphere(), origin_vec);
		glm::vec3 right_vec = glm::cross(origin_vec, up_vec);

		temp_boid.velocity = glm::normalize(up_vec + right_vec);
		spawn_ships.push_back(temp_boid);
		
	}


	//follow enemies initialization
	follow_mat= engine::material::create(1.0f, glm::vec3(1.0f, 0.0f, 0.f),
		glm::vec3(1.0f, 0.0f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f),1.f);

	//gremadier initialization
	grenadier_mat = engine::material::create(1.0f, glm::vec3(0.5f, 1.f, 0.5f),
		glm::vec3(0.5f, 1.f, 0.5f), glm::vec3(0.5f, 0.5f, 0.5f), 1.f);


	//enemy base model
	engine::ref<engine::sphere> follow_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties follow_props;
	follow_props.meshes = { follow_shape->mesh() };
	follow_props.bounding_shape = glm::vec3(0.1f);
	follow_object = engine::game_object::create(follow_props);

	//grenade initialization

	grenade_mat= engine::material::create(1.0f, glm::vec3(1.f, 0.f, 0.f),
		glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.f);

	engine::ref<engine::sphere> grenade_shape = engine::sphere::create(10, 20, 0.3f);
	engine::game_object_properties grenade_props;
	grenade_props.meshes = { grenade_shape->mesh() };
	grenade_props.bounding_shape = glm::vec3(0.1f);
	grenade_object = engine::game_object::create(grenade_props);
	
	//trapper initialization

	trapper_mat = engine::material::create(1.0f, glm::vec3(1.f, 1.f, 0.f),
		glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.f);
	//trap initialization
	engine::ref<engine::cylinder> trap_shape = engine::cylinder::create(15, 0.5, 1.f);
	engine::game_object_properties trap_props;
	trap_props.meshes = { trap_shape->mesh() };
	trap_props.scale = glm::vec3(0.5f);
	trap_props.bounding_shape = glm::vec3(0.1f);
	trap_object = engine::game_object::create(trap_props);


	engine::ref<engine::cylinder> trigger_model = engine::cylinder::create(6, 0.3f, 0.6f);
	engine::game_object_properties trigger_props;
	trigger_props.meshes = { trigger_model->mesh() };
	trigger_props.scale = glm::vec3(0.5f);
	trigger_props.bounding_shape = glm::vec3(0.1f);
	trap_trigger= engine::game_object::create(trigger_props);

	//gun initialization

	engine::ref<engine::prism> cyl_shape = engine::prism::create(3, 0.5f, 0.1f);
	engine::game_object_properties cyl_props;
	cyl_props.meshes = { cyl_shape->mesh() };
	cyl_props.scale = glm::vec3(1.5f);
	gun_object = engine::game_object::create(cyl_props);

	
	engine::ref<engine::pointedcyl> health_shape = engine::pointedcyl::create(6, 0.2f, 0.07f);
	engine::game_object_properties health_props;
	health_props.meshes = { health_shape->mesh() };
	health_props.scale = glm::vec3(4.f);
	pickup_object = engine::game_object::create(health_props);

	health_timer.start();
}

mainscene_layer::~mainscene_layer()
{

}

void mainscene_layer::on_update(const engine::timestep& time_step)
{
	if (isStart) {
		follow_timer.start();
		grenadier_timer.start();
		trapper_timer.start();
		score_timer.start();
		isStart = false;
	}
	update_player(time_step);
	update_boid(time_step);
	update_follow( time_step);
	update_grenadier(time_step);
	update_grenade(time_step);
	update_trapper(time_step);
	update_trap(time_step);
	update_health(time_step);
	damage->on_update(time_step);
	trapped->on_update(time_step);



	//spawn health pickups
	if (health_timer.elapsed() >= spawn_health)
	{
		health_pickup temp;
		temp.position = get_random_inside_unit_sphere() * (active_planet.radius + 0.25f);
		temp.active_timer.start();
		pickups.push_back(temp);
		health_timer.reset();
	}

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
		/*std::cout << follow_enemies[i].position << std::endl;*/
		

		engine::renderer::submit(mesh_shader, obj_transform, follow_object);
	}

	//render grenadier
	grenadier_mat->submit(mesh_shader);
	for (int i = 0; i < grenadier_enemies.size(); i++)
	{

		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, grenadier_enemies[i].position);
		obj_transform = glm::scale(obj_transform, follow_object->scale());
		/*std::cout << grenadier_enemies[i].position << std::endl;*/


		engine::renderer::submit(mesh_shader, obj_transform, follow_object);
	}
	trapper_mat->submit(mesh_shader);
	for (int i = 0; i < trapper_enemies.size(); i++)
	{
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, trapper_enemies[i].position);
		obj_transform = glm::scale(obj_transform, follow_object->scale());
		/*std::cout << follow_enemies[i].position << std::endl;*/


		engine::renderer::submit(mesh_shader, obj_transform, follow_object);
	}

	//render grenades
	grenade_mat->submit(mesh_shader);
	for (int i = 0; i < grenades.size(); i++)
	{

		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, grenades[i].position);
		obj_transform = glm::scale(obj_transform, grenade_object->scale());
		/*std::cout << grenades[i].position << std::endl;*/


		engine::renderer::submit(mesh_shader, obj_transform, grenade_object);
	}
	trapper_mat->submit(mesh_shader);
	for (int i = 0; i < traps.size(); i++)
	{
		trap_object->turn_towards(traps[i].position - active_planet.position);
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, traps[i].position);
		obj_transform = glm::rotate(obj_transform, trap_object->rotation_amount(), trap_object->rotation_axis());
		obj_transform = glm::scale(obj_transform, trap_object->scale());
		/*std::cout << grenades[i].position << std::endl;*/


		engine::renderer::submit(mesh_shader, obj_transform, trap_object);
	}
	follow_mat->submit(mesh_shader);
	for (int i = 0; i < traps.size(); i++)
	{
		trap_trigger->turn_towards(traps[i].position - active_planet.position);
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, traps[i].position-glm::normalize(active_planet.position-traps[i].position)*0.25f);
		obj_transform = glm::rotate(obj_transform, trap_trigger->rotation_amount(), trap_trigger->rotation_axis());
		obj_transform = glm::scale(obj_transform, trap_trigger->scale());
		/*std::cout << grenades[i].position << std::endl;*/


		engine::renderer::submit(mesh_shader, obj_transform, trap_trigger);
	}
	// gun render

	player_mat->submit(mesh_shader);
	for (int i = 0; i < follow_enemies.size(); i++)
	{
		gun_object->turn_towards(follow_enemies[i].velocity);
		glm::mat4 gun_transform(1.f);
		gun_transform = glm::translate(gun_transform, follow_enemies[i].position);
		gun_transform = glm::rotate(gun_transform, gun_object->rotation_amount(), gun_object->rotation_axis());
		gun_transform = glm::scale(gun_transform, gun_object->scale());
		engine::renderer::submit(mesh_shader, gun_transform, gun_object);
	}


	for (int i = 0; i < pickups.size(); i++)
	{
		pickup_object->turn_towards(pickups[i].position - active_planet.position);
		glm::mat4 pickup_transform(1.f);
		pickup_transform = glm::translate(pickup_transform, pickups[i].position);
		pickup_transform = glm::rotate(pickup_transform, pickup_object->rotation_amount(), pickup_object->rotation_axis());
		pickup_transform = glm::scale(pickup_transform, pickup_object->scale());
		engine::renderer::submit(mesh_shader, pickup_transform, pickup_object);
	}
	for (int i = 0; i < grenadier_enemies.size(); i++)
	{
		
		gun_object->turn_towards(grenadier_enemies[i].shoot_vector);
		glm::mat4 gun_transform(1.f);
		gun_transform = glm::translate(gun_transform, grenadier_enemies[i].position);
		gun_transform = glm::rotate(gun_transform, gun_object->rotation_amount(), gun_object->rotation_axis());
		gun_transform = glm::scale(gun_transform, gun_object->scale());
		engine::renderer::submit(mesh_shader, gun_transform, gun_object);
	}
	
	// render menu
	engine::renderer::end_scene();
		engine::renderer::begin_scene(m_2d_cam, mesh_shader);
	damage->on_render(mesh_shader);
	trapped->on_render(mesh_shader);
	engine::renderer::end_scene();





}

void mainscene_layer::on_event(engine::event& event)
{
	
}

void mainscene_layer::update_boid( const engine::timestep& time_step)
{
	float separationRadius = 8.0f;
	float alignmentRadius = 10.0f;
	float cohesionRadius = 20.f;
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
		if (glm::length(boid.position - active_planet.position) > 25.f)
			boid.velocity -= (boid.position - active_planet.position) * 0.1f;
		if (glm::length(boid.position - active_planet.position) <= 25.f)
		{
			boid.velocity += (boid.position - active_planet.position) * 2.f;
		}
		//limiting the max speed of the boid
		if (speed > maxSpeed) {
			boid.velocity = (boid.velocity / speed) * maxSpeed;
		}



	}


	//spawn updater
	
	if (follow_timer.elapsed() >= spawn_follow && follow_enemies.size()<score_timer.elapsed()/10.f)
	{
		int random = rand() % count;
		
		follow_enemy temp_enemy;
		temp_enemy.position = spawn_ships[random].position;
		temp_enemy.velocity = spawn_ships[random].velocity;
		temp_enemy.current_hitp = temp_enemy.max_hitp;
		temp_enemy.active_planet = active_planet;
		follow_enemies.push_back(temp_enemy);
		follow_timer.reset();
	}

	if (grenadier_timer.elapsed() >= spawn_grenadier && grenadier_enemies.size() < score_timer.elapsed() / 10.f)
	{
		int random = rand() % count;

		grenadier_enemy temp_enemy;
		temp_enemy.position = spawn_ships[random].position;
		temp_enemy.velocity = spawn_ships[random].velocity;
		temp_enemy.active_planet = active_planet;
		temp_enemy.current_hitp = temp_enemy.max_hitp;
		temp_enemy.shoot_timer.start();
		grenadier_enemies.push_back(temp_enemy);
		grenadier_timer.reset();
	}

	if (trapper_timer.elapsed() >= spawn_trapper)
	{
		int random = rand() % count;
		glm::vec3 random_vel = get_random_inside_unit_sphere() * 10.f;
		trapper_enemy temp_enemy;
		temp_enemy.position = spawn_ships[random].position;
		temp_enemy.velocity = glm::normalize(random_vel-glm::dot(random_vel,(active_planet.position-spawn_ships[random].position)))*4.f;
		temp_enemy.current_hitp = temp_enemy.max_hitp;
		temp_enemy.active_planet = active_planet;
		temp_enemy.trap_timer.start();
		trapper_enemies.push_back(temp_enemy);
		trapper_timer.reset();
	}
}
void mainscene_layer::update_follow( const engine::timestep& time_step)
{

	for (follow_enemy& follow_instance : follow_enemies)
	{
		
		follow_instance.accel = glm::vec3{ 0.f };
		glm::vec3  player_vec = m_player.position - follow_instance.position;
		glm::vec3 origin_vec = follow_instance.active_planet.position - follow_instance.position;
		follow_instance.accel = (glm::normalize(player_vec)+ glm::normalize(origin_vec)) * (follow_accel+rand()%50);
		follow_instance.accel += origin_vec * 5.f;

		follow_instance.accel += -(follow_instance.velocity - glm::dot(follow_instance.velocity, glm::normalize(origin_vec)) * glm::normalize(origin_vec)) * 5.f;


		//seperate different follow enemies
		for (follow_enemy& other : follow_enemies)
			if (glm::length(follow_instance.position - other.position) <= 1.f && &other!=&follow_instance)
				follow_instance.accel += glm::normalize(follow_instance.position - other.position) * 30.f / glm::length(follow_instance.position - other.position);



		follow_instance.position += follow_instance.velocity * (float)time_step;
		follow_instance.velocity += follow_instance.accel * (float)time_step;

		
		if (glm::length(follow_instance.position - follow_instance.active_planet.position) <= 10.5f)
		{
			follow_instance.accel -= glm::dot(follow_instance.accel, (follow_instance.active_planet.position - follow_instance.position)) * glm::normalize(follow_instance.active_planet.position - follow_instance.position);
			follow_instance.velocity -= 1.5f *glm::dot(follow_instance.velocity, glm::normalize(follow_instance.active_planet.position - follow_instance.position)) * glm::normalize(follow_instance.active_planet.position - follow_instance.position);
		}

		if (glm::length(follow_instance.position - m_player.position) <= 1.5f)
		{
			for (int i = 0; i < follow_enemies.size(); i++)
			{
				if (&follow_enemies[i] == &follow_instance)
				{
					m_audio_manager->play_spatialised_sound("damage", m_3d_cam.position(), m_3d_cam.position());
					follow_enemies.erase(follow_enemies.begin() + i);
					m_player.current_hitp -= 10.f;
					damage->activate();
					break;
				}
			}
		}
	}
	

}

void mainscene_layer::update_grenadier( const engine::timestep& time_step)
{
	
	for (grenadier_enemy& grenadier_instance :grenadier_enemies)
	{
		
		if (glm::length(grenadier_instance.position - m_player.position) <= 1.f)
		{
			for (int i = 0; i < grenadier_enemies.size(); i++)
			{
				if (&grenadier_enemies[i] == &grenadier_instance)
				{
					grenadier_enemies.erase(grenadier_enemies.begin() + i);
					
					break;
				}
			}
			continue;
		}
		grenadier_instance.accel = glm::vec3{ 0.f };
		glm::vec3  player_vec = m_player.position - grenadier_instance.position;
		glm::vec3 origin_vec = grenadier_instance.active_planet.position - grenadier_instance.position;
		grenadier_instance.shoot_vector = -origin_vec * 3.f + glm::normalize(player_vec) * 30.f;
		if (glm::length(player_vec) >= 8.f)
		{
			grenadier_instance.accel = (glm::normalize(player_vec) + glm::normalize(origin_vec)) * (follow_accel + rand() % 50);
		}
		else
		{
			grenadier_instance.accel = -(glm::normalize(player_vec-origin_vec) + glm::normalize(origin_vec)) * (follow_accel + rand() % 50);

			
			
		}

		if (grenadier_instance.shoot_timer.elapsed() >= grenadier_instance.fire_time)
		{
			grenade temp;
			temp.position = grenadier_instance.position;
			temp.velocity = (-origin_vec * 3.f + glm::normalize(player_vec) * 30.f);
			temp.active_planet = grenadier_instance.active_planet;
			grenades.push_back(temp);
			m_audio_manager->play_spatialised_sound("launch_grenade", m_3d_cam.position(), grenadier_instance.position);
			grenadier_instance.shoot_timer.reset();

		}


		grenadier_instance.accel += origin_vec  *5.f;

		grenadier_instance.accel += -(grenadier_instance.velocity - glm::dot(grenadier_instance.velocity, glm::normalize(origin_vec)) * glm::normalize(origin_vec)) * 5.f;


		//seperate different follow enemies
		for (grenadier_enemy& other : grenadier_enemies)
			if (glm::length(grenadier_instance.position - other.position) <= 1.f && &other != &grenadier_instance)
				grenadier_instance.accel += (grenadier_instance.position - other.position) * 30.f / glm::length(grenadier_instance.position - other.position);



		grenadier_instance.position += grenadier_instance.velocity * (float)time_step;
		grenadier_instance.velocity += grenadier_instance.accel * (float)time_step;


		if (glm::length(grenadier_instance.position - grenadier_instance.active_planet.position) <= 10.5f)
		{
			grenadier_instance.accel -= glm::dot(grenadier_instance.accel, (grenadier_instance.active_planet.position - grenadier_instance.position)) * glm::normalize(grenadier_instance.active_planet.position - grenadier_instance.position);
			grenadier_instance.velocity -= 1.5f * glm::dot(grenadier_instance.velocity, glm::normalize(grenadier_instance.active_planet.position - grenadier_instance.position)) * glm::normalize(grenadier_instance.active_planet.position - grenadier_instance.position);
		}
	}
}

void mainscene_layer::update_grenade( const engine::timestep& time_step)
{

	for (grenade& grenade_instance : grenades)
	{
		glm::vec3 origin_vec = grenade_instance.active_planet.position - grenade_instance.position;
		grenade_instance.accel = origin_vec * 5.f;
		grenade_instance.position += grenade_instance.velocity * (float)time_step;
		grenade_instance.velocity += grenade_instance.accel * (float)time_step;


		if (glm::length(grenade_instance.position - grenade_instance.active_planet.position) <= 10.f)
		{
			for (int i = 0; i < grenades.size(); i++)
			{
				m_audio_manager->play_spatialised_sound("grenade_explosion", m_3d_cam.position(), grenade_instance.position);
				if (&grenades[i] == &grenade_instance)
				{
					
					if (glm::length(grenade_instance.position - m_player.position) <= grenade_instance.grenade_radius)
					{
						m_audio_manager->play_spatialised_sound("damage", m_3d_cam.position(), m_3d_cam.position());
						m_player.current_hitp -= 10.f;
						damage->activate();
					}
					
					grenades.erase(grenades.begin() + i);
					break;
				}
					
				
			}
		}
	}


}

void mainscene_layer::update_trapper( const engine::timestep& time_step)
{
	for (trapper_enemy & trapper_instance: trapper_enemies)
	{
		
		glm::vec3 origin_vec = trapper_instance.active_planet.position - trapper_instance.position;
		
		trapper_instance.accel = origin_vec * 10.f;
		trapper_instance.accel += -(trapper_instance.velocity - glm::dot(trapper_instance.velocity, glm::normalize(trapper_instance.active_planet.position - trapper_instance.position)) * glm::normalize(trapper_instance.active_planet.position - trapper_instance.position)) * 5.f;
		trapper_instance.accel += glm::normalize(trapper_instance.velocity) * 40.f;
		if (glm::length(trapper_instance.position - trapper_instance.active_planet.position) < 10.5f)
		{
			trapper_instance.velocity -= 1.2f * glm::dot(trapper_instance.velocity, glm::normalize(trapper_instance.active_planet.position - trapper_instance.position)) * glm::normalize(trapper_instance.active_planet.position - trapper_instance.position);
			trapper_instance.accel -= glm::dot(trapper_instance.accel, glm::normalize(trapper_instance.active_planet.position - trapper_instance.position)) * glm::normalize(trapper_instance.active_planet.position - trapper_instance.position);
			trapper_instance.position = trapper_instance.active_planet.position - (10.5f * glm::normalize(origin_vec));
		}
		trapper_instance.position += trapper_instance.velocity * (float)time_step;
		trapper_instance.velocity += trapper_instance.accel * (float)time_step;

		if (trapper_instance.trap_timer.elapsed() >= trapper_instance.trap_place_time)
		{
			trap temp_trap;
			temp_trap.position = trapper_instance.position+(glm::normalize(origin_vec)*0.5f);
			temp_trap.trap_active_timer.start();
			traps.push_back(temp_trap);
			trapper_instance.trap_timer.reset();
		}


	}
	
}

void mainscene_layer::update_trap( const engine::timestep& time_step)
{
	for (trap& trap_instance : traps)
	{
		if (trap_instance.trap_active_timer.elapsed() >= trap_instance.time_trap)
		{
			for (int i = 0; i < traps.size(); i++)
			{
				if (&traps[i] == &trap_instance)
				{
					
					traps.erase(traps.begin() + i);
					
					break;
				}
			}
		}
		if (glm::length(trap_instance.position - m_player.position) <= 1.f)
		{
			m_audio_manager->play_spatialised_sound("trap", m_3d_cam.position(), trap_instance.position);
			m_player.is_trapped = true;
			m_player.trapped_timer.start();
			trapped->activate();
			for (int i = 0; i < traps.size(); i++)
			{
				if (&traps[i] == &trap_instance)
				{
					traps.erase(traps.begin() + i);

					break;
				}
			}
		}
	}
}

void mainscene_layer::update_player(const engine::timestep& time_step)
{
	if (m_player.is_trapped && m_player.trapped_timer.elapsed() >= m_player.trap_time)
	{
		m_player.is_trapped = false;

	}
	else if (m_player.is_trapped)
		return;
	m_player.accel = glm::vec3{ 0 };
	glm::vec3 input_accel = glm::vec3{ 0 };
	//player acceleration for movement
	// player update
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
		input_accel -= m_3d_cam.right_vector();
	if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
		input_accel += m_3d_cam.right_vector();
	if (engine::input::key_pressed(engine::key_codes::KEY_W)) // Up
		input_accel += m_3d_cam.up_vector();
	if (engine::input::key_pressed(engine::key_codes::KEY_S)) // Down
		input_accel -= m_3d_cam.up_vector();
	if (glm::length(input_accel) != 0.f)
	{
		
		m_player.accel = glm::normalize(input_accel) * 8.f * glm::length(active_planet.position - m_player.position);
		
	}

	if (m_player.dash_timer.elapsed() >= m_player.time_dash)
	{
		m_player.can_dash = true;
	}
	if (engine::input::key_pressed(engine::key_codes::KEY_SPACE) && m_player.can_dash && m_player.accel!= glm::vec3{ 0 })
	{
		/*std::cout << "called jump";*/
		m_player.velocity += glm::normalize(m_player.accel)*30.f;
		m_player.can_dash = false;
		m_player.dash_timer.start();
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
		
		m_player.accel -= glm::dot(m_player.accel, (active_planet.position - m_player.position)) * glm::normalize(active_planet.position - m_player.position);
		m_player.velocity -=1.0f* glm::dot(m_player.velocity, glm::normalize(active_planet.position - m_player.position)) * glm::normalize(active_planet.position- m_player.position);
		m_player.position = active_planet.radius * glm::normalize(m_player.position - active_planet.position);
	}

	
	m_3d_cam.pos_update(active_planet.position + glm::normalize(m_player.position - active_planet.position) * 70.f, m_player.position);

	if(m_player.current_hitp<=0.f)
		engine::application::exit();
}


void mainscene_layer::update_health(const engine::timestep& time_step)
{
	for (health_pickup& health_instance : pickups)
	{
		if (health_instance.active_timer.elapsed() >= health_instance.active_time)
		{
			for (int i = 0; i < pickups.size(); i++)
			{
				if (&pickups[i] == &health_instance)
				{
					pickups.erase(pickups.begin() + i);

					break;
				}
			}
		}

		if (glm::length(health_instance.position - m_player.position) < 1.f)
		{
			for (int i = 0; i < pickups.size(); i++)
			{
				if (&pickups[i] == &health_instance)
				{
					pickups.erase(pickups.begin() + i);

					break;
				}
			}
			m_audio_manager->play_spatialised_sound("health", m_3d_cam.position(), m_player.position);
			m_player.current_hitp += 10.f;
			m_player.current_hitp = std::min(m_player.current_hitp, m_player.max_hitp);
			
		}
	}
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

