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
	int count = 100;
	// smae thing as start in unity
	if (start)
	{
		//do something
		
		for (int i=0;i< count;i++)
		{
			std::vector<glm::vec3> tetrahedron_vertices;
			tetrahedron_vertices.push_back(glm::vec3(0.f, 1.f, 0.f)); //0
			tetrahedron_vertices.push_back(glm::vec3(0.f, 0.f, 2.f)); //1
			tetrahedron_vertices.push_back(glm::vec3(-1.f, 0.f, -1.f)); //2
			tetrahedron_vertices.push_back(glm::vec3(1.f, 0.f, -1.f)); //3
			engine::ref<engine::tetrahedron> tetrahedron_shape =
				engine::tetrahedron::create(tetrahedron_vertices);
			engine::game_object_properties tetrahedron_props;
			// boid setup
			
			Boid temp_boid;
			temp_boid.position = get_random_inside_unit_sphere() * 25.f;
			temp_boid.velocity= get_random_inside_unit_sphere() * 25.f;
			tetrahedron_props.position = map2DToSphere(temp_boid.position.x, temp_boid.position.y, 100, 100, 25);
			tetrahedron_props.meshes = { tetrahedron_shape->mesh() };
			auto temp_object = engine::game_object::create(tetrahedron_props);
			temp_object->turn_towards(closest_center - tetrahedron_props.position);
			enemies.push_back(temp_object);
			boids.push_back(temp_boid);
		}


		start = false;
	}

	updateBoids(boids,time_step);
	for (int i = 0; i < count; i++)
	{
		
		enemies[i]->set_position(boids[i].position);
		enemies[i]->turn_towards(closest_center - enemies[i]->position());
	}

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


	m_cam.on_update(time_step,closest_center,60.f);

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
	for (engine::ref<engine::game_object> obj : enemies)
	{
		glm::mat4 obj_transform(1.f);
		obj_transform = glm::translate(obj_transform, obj->position());
		obj_transform = glm::rotate(obj_transform, obj->rotation_amount(), obj->rotation_axis());
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

glm::vec3 env_layer:: get_random_inside_unit_sphere()
{
	float theta = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2*glm::pi<float>())));
	float z= (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/2.f)))-1.f;
	glm::vec3 random_vec(1.f);
	random_vec.x = sqrt(1 - (z * z)) * glm::cos(theta);
	random_vec.y = sqrt(1 - (z * z)) * glm::sin(theta);
	random_vec.z = z;
	return random_vec;
}

void env_layer:: updateBoids(std::vector<Boid>& boids, float deltaTime) {
	// Define behavior parameters
	float separationRadius = 5.0f;
	float alignmentRadius = 5.0f;
	float cohesionRadius = 7.5f;
	float maxSpeed = 20.0f;

	for (Boid& boid : boids) {
		// Update the boid's position based on its velocity and time step
		boid.position += boid.velocity * deltaTime;
		
		// Implement boid behavior rules (simplified here)
		// You can customize these rules for more complex behavior
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
		boid.velocity += 0.1f * separation + 0.05f * alignment + 0.002f * cohesion;
		float speed = glm::length(boid.velocity);
		if (glm::length(boid.position - closest_center) > 20.f)
			boid.velocity -= glm::normalize(boid.position - closest_center) *0.5f;
		 if (glm::length(boid.position - closest_center) < 20.f)
		{
			boid.velocity += glm::normalize(boid.position - closest_center) * 0.5f;
		}
		if (speed > maxSpeed) {
			boid.velocity = (boid.velocity / speed) * maxSpeed;
		}
		
		
		
	}
}

glm::vec3 env_layer::map2DToSphere(double x, double y, double screenWidth, double screenHeight, double sphereRadius) {
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
