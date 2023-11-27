#include "menu_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"

const double goldenRatio = 1.61803398874989484820458683436;
menu_layer::menu_layer():
m_2d_cam(-1.6f, 1.6f, -0.9f, 0.9f),
m_3d_cam((float)engine::application::window().width(), (float)engine::application::window().height())
{
	//world setup
	//engine::application::window().hide_mouse_cursor();// cursor lock
	auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_cross_fade = cross_fade::create("assets/textures/Menu.bmp", 5.0f, 1.6f, 0.9f);
	
	//light setup
	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 1.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("lighting_on", true);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gColorMap", 0);
	
	 m_directionalLight.submit(mesh_shader);
	

	//point light setup
	m_pointLight.Color = glm::vec3(1.0f, 1.f, 1.f);
	m_pointLight.AmbientIntensity = 0.3f;
	m_pointLight.DiffuseIntensity = 3.6f;
	m_pointLight.Position = closest_center;
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("gNumPointLights", (int)num_point_lights);
	m_pointLight.submit(mesh_shader, 0);
	
	//skybox setup
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f,
			(float)engine::application::window().height()));
	m_skybox = engine::skybox::create(100.f,
		{ engine::texture_2d::create("assets/textures/skybox/front.png", true),
		  engine::texture_2d::create("assets/textures/skybox/right.png", true),
		  engine::texture_2d::create("assets/textures/skybox/back.png", true),
		  engine::texture_2d::create("assets/textures/skybox/left.png", true),
		  engine::texture_2d::create("assets/textures/skybox/top.png", true),
		  engine::texture_2d::create("assets/textures/skybox/bottom.png", true)
		});



	//creation of mesh objects
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
	engine::ref <engine::model> ship_model = engine::model::create(ship_loc);
	engine::game_object_properties ship_props;
	ship_props.meshes = ship_model->meshes();
	ship_props.textures = ship_model->textures();
	ship_props.position = { 0.f,0.f, 0.f };
	ship_props.scale = glm::vec3(0.01f);
	enemy = engine::game_object::create(ship_props);

	engine::ref<engine::sphere> player_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties player_props;
	player_props.position = glm::normalize(m_3d_cam.position() - closest_center) * 6.f;
	player_props.meshes = { player_shape->mesh() };
	player_props.type = 1;
	player_props.bounding_shape = glm::vec3(0.5f);
	player_props.restitution = 0.92f;
	player_props.mass = 0.000001f;
	player = engine::game_object::create(player_props);
	player_position = player_props.position;

	//Menu Options
	engine::ref<engine::model> play_model = engine::model::create(play_loc);
	engine::game_object_properties play_props;
	play_props.position = { 0.f,6.f,0.f };
	play_props.meshes = play_model->meshes();
	play_props.scale = glm::vec3{ 0.02f };
	PLAY= engine::game_object::create(play_props);

	engine::ref<engine::model> help_model = engine::model::create(help_loc);
	engine::game_object_properties help_props;
	help_props.position = { -6.f * sin(2 * glm::pi<float>() / 3),6.f*cos(2 * glm::pi<float>() / 3),0.f};
	help_props.meshes = help_model->meshes();
	help_props.rotation = glm::vec3{ 0.f,0.f,2*glm::pi<float>()/3};
	help_props.scale = glm::vec3{ 0.02f };
	HELP = engine::game_object::create(help_props);

	engine::ref<engine::model> exit_model = engine::model::create(exit_loc);
	engine::game_object_properties exit_props;
	exit_props.position = {- 6.f * sin(4 * glm::pi<float>() / 3),6.f * cos(4 * glm::pi<float>() / 3),0.f };
	exit_props.meshes = exit_model->meshes();
	exit_props.rotation = glm::vec3{ 0.f,0.f,4 * glm::pi<float>() / 3 };
	exit_props.scale = glm::vec3{ 0.02f };
	EXIT = engine::game_object::create(exit_props);
	for (int i = 0; i < enemy_count; i++)
	{
		// boid setup

		Boid temp_boid;
		temp_boid.position = get_random_inside_unit_sphere() * 10.f;
		temp_boid.velocity = get_random_inside_unit_sphere() * 1.f;

		boids.push_back(temp_boid);
	}
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.scale = glm::vec3(0.3f);
	protection_sphere = engine::game_object::create(sphere_props);

	engine::ref<engine::prism> cyl_shape = engine::prism::create(3, 0.5f, 0.1f);
	engine::game_object_properties cyl_props;
	cyl_props.meshes = { cyl_shape->mesh() };
	cyl_props.scale = glm::vec3(1.f);
	gun = engine::game_object::create(cyl_props);

	for (int i = 0; i <prot_count; i++)
	{
		/*from numpy import arange, pi, sin, cos, arccos
			n = 50
			goldenRatio = (1 + 5 * *0.5) / 2
			i = arange(0, n)
			theta = 2 * pi * i / goldenRatio
			phi = arccos(1 - 2 * (i + 0.5) / n)
			x, y, z = cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi);*/

		float theta = 2*glm::pi<float>()* i / goldenRatio;
		float phi = acos(1.f - (2.f * (i + 0.5f) / prot_count));
		float r = 6;
		
		sphere_pos.push_back(glm::vec3(r, theta, phi));
	}

	//creation of cross fade(menu screen)
	


	//base mat for spheres
	base_mat = engine::material::create(1.0f, glm::vec3(1.0f,1.f, 1.f),
		glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);


	//gun mat for protection spheres
	gun_mat = engine::material::create(1.0f, glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);

	//mat for light source
	m_lightsource_material = engine::material::create(1.0f, glm::vec3(1.0f, 1.f, 1.f),
		glm::vec3(1.0f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f);
	m_text_manager = engine::text_manager::create();
}
menu_layer::~menu_layer() {}
void menu_layer::on_update(const engine::timestep& time_step)
{

	//rotating sphere barrier
	for (int i = 0; i < prot_count; i++)
	{
		
		sphere_pos[i].y += time_step;

	}

	//update boid behaviour
	updateBoids(boids,time_step);

	
	//animating various elements of the planet by updating their rotations
	glm::vec3 rot_euler;
	//top half rotate
	rot_euler = planet[5]->rotation_euler();
	rot_euler.y += 1.f * time_step;
	planet[5]->set_rotation_euler(rot_euler);
	//bottom half rotate
	rot_euler = planet[6]->rotation_euler();
	rot_euler.y += -1.f * time_step;
	planet[6]->set_rotation_euler(rot_euler);
	
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

	//camera update
	/*player->set_position(glm::normalize(m_3d_cam.position() - closest_center) * 5.f);
	m_3d_cam.on_update(time_step,closest_center,50.f);*/
	
	

	player_accel = glm::vec3{ 0 };
	glm::vec3 input_accel = glm::vec3{ 0 };
	//player acceleration for movement
	// player update
	if (engine::input::key_pressed(engine::key_codes::KEY_A)) // left
		player_accel -= m_3d_cam.right_vector() ;
	if (engine::input::key_pressed(engine::key_codes::KEY_D)) // right
		player_accel += m_3d_cam.right_vector() ;
	if (engine::input::key_pressed(engine::key_codes::KEY_W)) // Up
		player_accel += m_3d_cam.up_vector() ;
	if (engine::input::key_pressed(engine::key_codes::KEY_S)) // Down
		player_accel -= m_3d_cam.up_vector() ;
	if (glm::length(player_accel) != 0.f)
	{
		std::cout << "player accel" << glm::normalize(player_accel);
		player_accel = glm::normalize(player_accel) * 15.f* glm::length(closest_center - player_position);
	}
	
	//player acceleration for gravity
	player_accel += glm::normalize(closest_center - player->position()) * 20.f* glm::length(closest_center - player_position);


	//drag force: not affecting the force towards the center
	player_accel += -(player_velocity- glm::dot(player_velocity, glm::normalize(closest_center - player_position)) * glm::normalize(closest_center - player_position)) * 5.f;

	//updating player position
	player_position += player_velocity * (float)time_step;

	//updating player velocity
	player_velocity += player_accel * (float)time_step;


	//collision detection between player and planet
	if (glm::length(player_position - closest_center) <= 6.f)
	{
		canJump = true;
		player_accel -= glm::dot(player_accel, (closest_center - player_position)) * glm::normalize(closest_center - player_position);
		player_velocity -= glm::dot(player_velocity, glm::normalize(closest_center - player_position)) * glm::normalize(closest_center - player_position);
	}
	else
		canJump = false;

	if (engine::input::key_pressed(engine::key_codes::KEY_SPACE) && canJump)
	{
		std::cout << "called jump";
		player_velocity += m_3d_cam.front_vector() * -40.f;
	}
	player->set_position(player_position);
	
	m_3d_cam.pos_update(closest_center + glm::normalize(player_position - closest_center)*40.f,closest_center);

	m_cross_fade->on_update(time_step);
}
void menu_layer::on_render()
{
	//render setup
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();
	const auto mesh_shader = engine::renderer::shaders_library()->get("mesh");
	engine::renderer::begin_scene(m_3d_cam, mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->set_uniform("gEyeWorldPos", m_3d_cam.position());

	//skybox setup
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_cam.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(mesh_shader, m_skybox, skybox_tranform);
	base_mat->submit(mesh_shader);

	//render of barrier
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("lighting_on", false);


	
	//for (int i = 0; i < prot_count; i++)
	//{
	//	//conversion of r, theta, phi to x,y,z
	//	float r = sphere_pos[i].x;
	//	float theta = sphere_pos[i].y;
	//	float phi = sphere_pos[i].z;
	//	float x = 6 * cos(theta) * sin(phi);
	//	float y = 6 * sin(theta) * sin(phi);
	//	float z = 6 * cos(phi);

	//	glm::mat4 obj_transform(1.f);
	//	obj_transform = glm::translate(obj_transform, glm::vec3(x,y,z));
	//	obj_transform = glm::rotate(obj_transform, protection_sphere->rotation_amount(), protection_sphere->rotation_axis());
	//	obj_transform = glm::scale(obj_transform, protection_sphere->scale());
	//	engine::renderer::submit(mesh_shader, obj_transform, protection_sphere);

	//	//render of guns for protection sphere
	//	gun_mat->submit(mesh_shader);
	//	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
	//		set_uniform("lighting_on", false);
	//	gun->turn_towards(glm::vec3(x,y,z)-closest_center);
	//	obj_transform = glm::mat4(1.f);
	//	obj_transform = glm::translate(obj_transform, glm::vec3(x, y, z));
	//	obj_transform = glm::rotate(obj_transform, gun->rotation_amount(), gun->rotation_axis());
	//	obj_transform = glm::scale(obj_transform, gun->scale());
	//	engine::renderer::submit(mesh_shader, obj_transform, gun);
	//	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
	//		set_uniform("lighting_on", true);
	//	base_mat->submit(mesh_shader);
	//}
	//std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
	//	set_uniform("lighting_on", true);



	// render planet components with their current rotations
	for(int i=0;i<planet.size();i++)
	{
		engine::ref<engine::game_object> obj = planet[i];
		
		
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, obj->position());
		obj_transform = rotateEuler(obj->rotation_euler(),obj_transform);
		obj_transform = glm::scale(obj_transform, obj->scale());
		

		//create the core with unlit material to simulate a light source
		if (i== 0)
		{
			std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
				set_uniform("lighting_on", false);

			m_lightsource_material->submit(mesh_shader);
			engine::renderer::submit(mesh_shader, planet[0]->meshes().at(0),
				obj_transform);
			std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
				set_uniform("lighting_on", true);
		}
		else	
		engine::renderer::submit(mesh_shader,obj_transform, obj);
		
	}



	//render spaceships with their current rotations and positions
	for (int i = 0; i < enemy_count; i++)
	{

		enemy->set_position(boids[i].position);
		enemy->turn_towards(boids[i].velocity);
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, enemy->position());
		obj_transform = glm::rotate(obj_transform, enemy->rotation_amount(), enemy->rotation_axis());
		obj_transform = glm::scale(obj_transform, enemy->scale());
		engine::renderer::submit(mesh_shader, obj_transform, enemy);


		
	}
	base_mat->submit(mesh_shader);
	engine::renderer::submit(mesh_shader, player);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("lighting_on", false);
	glm::mat4 obj_transform(1.f);
	obj_transform = glm::translate(obj_transform,glm::vec3{0});
	obj_transform = glm::rotate(obj_transform, PLAY->rotation_amount(), PLAY->rotation_axis());
	obj_transform = glm::scale(obj_transform, PLAY->scale());
	

	engine::renderer::submit(mesh_shader, obj_transform, PLAY);


	obj_transform = glm::mat4(1.f);
	obj_transform = glm::translate(obj_transform, glm::vec3{ 0 });
	obj_transform = rotateEuler(HELP->rotation_euler(), obj_transform);
	obj_transform = glm::scale(obj_transform, EXIT->scale());
	engine::renderer::submit(mesh_shader, obj_transform, HELP);

	obj_transform = glm::mat4(1.f);
	obj_transform = glm::translate(obj_transform, glm::vec3{ 0 });
	obj_transform = rotateEuler(EXIT->rotation_euler(), obj_transform);
	obj_transform = glm::scale(obj_transform, EXIT->scale());
	engine::renderer::submit(mesh_shader, obj_transform, EXIT);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_shader)->
		set_uniform("lighting_on", true);


	engine::renderer::end_scene();

	// Render text
	const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	m_text_manager->render_text(text_shader, "Press Space to select option", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));

	
	//render menu
	engine::renderer::begin_scene(m_2d_cam, mesh_shader);
	m_cross_fade->on_render(mesh_shader);
	engine::renderer::end_scene();
	
}
void menu_layer::on_event(engine::event& event)
{

	//tab to control wireframe
	//Enter to exit out of menu
	if (event.event_type() == engine::event_type_e::key_pressed)
	{
		auto& e = dynamic_cast<engine::key_pressed_event&>(event);
		if (e.key_code() == engine::key_codes::KEY_TAB)
		{
			engine::render_command::toggle_wireframe();
			
		}
		if (e.key_code() == engine::key_codes::KEY_ENTER && m_cross_fade->is_active())
		{
			m_cross_fade->deactivate();
		}
		if (e.key_code() == engine::key_codes::KEY_SPACE && isPlay())
		{
			std::cout << "PLAYGAME";
			engine::application::instance().pop_layer(this,main_game_layer);
		}
		if(e.key_code() == engine::key_codes::KEY_SPACE && isHelp())
		{
			m_cross_fade->activate();
			std::cout << "Help";
		}
		if (e.key_code() == engine::key_codes::KEY_SPACE && isExit())
		{
			std::cout << "Exit";
			engine::application::exit();
		}
		
		/*if (e.key_code() == engine::key_codes::key_1)
		{
			
			engine::application::instance().pop_layer(this);
		}*/

		
	}

	
}

//used to rotate and object using euler angles
glm::mat4 menu_layer::rotateEuler(glm::vec3 rot_angles, glm::mat4 matrix)
{
	matrix = glm::rotate(matrix, rot_angles.x, glm::vec3(1.f, 0.f, 0.f));
	matrix = glm::rotate(matrix, rot_angles.y, glm::vec3(0.f, 1.f, 0.f));
	matrix = glm::rotate(matrix, rot_angles.z, glm::vec3(0.f, 0.f, 1.f));
	return matrix;
}


//used to get a radom vector in 3d space of unit magnitude using equal area projection
glm::vec3 menu_layer:: get_random_inside_unit_sphere()
{
	float theta = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2*glm::pi<float>())));
	float z= (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.f)))-1.f;
	glm::vec3 random_vec(1.f);
	random_vec.x = sqrt(1 - (z * z)) * glm::cos(theta);
	random_vec.y = sqrt(1 - (z * z)) * glm::sin(theta);
	random_vec.z = z;
	return random_vec;
}

void menu_layer:: updateBoids(std::vector<Boid>& boids, float deltaTime) {
	// Define behavior parameters
	float separationRadius = 10.0f;
	float alignmentRadius = 5.0f;
	float cohesionRadius = 10.f;
	float maxSpeed = 50.0f;

	for (Boid& boid : boids) {
		// Update the boid's position based on its velocity and time step
		boid.position += boid.velocity * deltaTime;
		
		// simple boid behaviour implemented here
		
		glm::vec3 separation(0.0f);
		glm::vec3 alignment(0.0f);
		glm::vec3 cohesion(0.0f);

		for (const Boid& other : boids) {
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
		if (glm::length(boid.position - closest_center) > 10.f)
			boid.velocity -= (boid.position - closest_center) *0.1f;
		 if (glm::length(boid.position - closest_center) <= 10.f)
		{
			 boid.velocity += (boid.position - closest_center) * 2.f;
		}
		 //limiting the max speed of the boid
		if (speed > maxSpeed) {
			boid.velocity = (boid.velocity / speed) * maxSpeed;
		}
		
		
		
	}
}

//not really using this right now, it was an experiment
glm::vec3 menu_layer::map2DToSphere(double x, double y, double screenWidth, double screenHeight, double sphereRadius) {
	// Convert 2D coordinates to spherical coordinates
	double phi = glm::pi<double>() * (y / screenHeight);  // Latitude
	double theta = 2 * glm::pi<double>() * (x / screenWidth);  // Longitude

	// Convert spherical coordinates to 3D Cartesian coordinates on the sphere's surface
	double sinPhi = sin(phi);
	double cosPhi = cos(phi);
	double sinTheta = sin(theta);
	double cosTheta = cos(theta);

	double sphereX = sphereRadius * sinPhi * cosTheta;
	double sphereY = sphereRadius * sinPhi * sinTheta;
	double sphereZ = sphereRadius * cosPhi;

	return glm::vec3(sphereX, sphereY, sphereZ);
}

bool menu_layer::isPlay()
{
	bool is_play = false;
	if (glm::length(player_position - PLAY->position()) <= 3.f)
		is_play = true;
	return is_play;
}

bool menu_layer::isHelp()
{
	bool is_play = false;
	if (glm::length(player_position - HELP->position()) <= 3.f)
		is_play = true;
	return is_play;
}

bool menu_layer::isExit()
{
	bool is_play = false;
	if (glm::length(player_position - EXIT->position()) <= 3.f)
		is_play = true;
	return is_play;
}
