#pragma once
#include <engine.h>
#include "engine/utils/timer.h"
#include "cross_fade.h"

class mainscene_layer : public engine::layer
{
public:
	mainscene_layer();
	~mainscene_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;

private:
	
	// bool for start
	bool isStart=true;


	//timer for score and difficulty
	engine::timer score_timer;
	//mats
	engine::ref<engine::material> planet_mat;
	engine::ref<engine::material> spawn_ship_mat;
	engine::ref<engine::material> follow_mat;
	engine::ref<engine::material> grenadier_mat;
	engine::ref<engine::material> grenade_mat;
	engine::ref<engine::material> trapper_mat;
	engine::ref<engine::material> trap_mat;
	engine::ref<engine::material> player_mat;

	//miscellaneous

	engine::ref<engine::game_object> gun_object;
	//skybox
	engine::ref<engine::skybox>			m_skybox{};

	//skybox Locations
	std::string front;
	std::string back;
	std::string left;
	std::string right;
	std::string up;
	std::string down;

	//planet structure

	struct planet {
		glm::vec3 position;
		float radius;
		};


	//list of planets
	std::vector<planet> world_planets;
	std::vector< engine::ref<engine::game_object>> planet_gameObjects;
	planet active_planet;
	


	//boid enemy  struct

	struct spawn_ship {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 acccel{ 0.f };
		
		};
	engine::ref<engine::game_object> spawn_object;
	std::vector<spawn_ship> spawn_ships;
	//count of boid enemies
	int count = 250;
	void update_boid( const engine::timestep& time_step);
	std::string ship_loc = "assets/models/static/ship.fbx";


	//spawn times
	float spawn_follow= 5.f;
	float spawn_grenadier = 7.f;
	float spawn_trapper = 10.f;

	engine::timer follow_timer;
	engine::timer grenadier_timer;
	engine::timer trapper_timer;


	//follow enemy struct
	struct follow_enemy {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		int max_hitp = 30.f;
		int current_hitp;
		planet active_planet;
		
	};
	//follow enemy acceleration
	engine::ref<engine::game_object> follow_object;
	std::vector<follow_enemy> follow_enemies;
	float follow_accel = 70.f;

	void update_follow( const engine::timestep& time_step);
	// mortar enemy struct

	struct grenadier_enemy {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		int max_hitp = 10.f;
		int current_hitp;
		float firing_dist=0.f;
		float firing_vel=0.f;
		float fire_time = 4.f;
		planet active_planet;
		engine::timer shoot_timer;
		glm::vec3 shoot_vector;
	};
	engine::ref<engine::game_object> grenadier_object;
	std::vector<grenadier_enemy> grenadier_enemies;
	void update_grenadier(const engine::timestep& time_step);

	struct grenade {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		float maxTime = 2.f;
		float grenade_radius = 2.f;
		planet active_planet;
	};
	engine::ref<engine::game_object> grenade_object;
	std::vector<grenade> grenades;
	void update_grenade(const engine::timestep& time_step);


	struct trapper_enemy {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		float trap_place_time = 7.f;
		float max_hitp = 10.f;
		float current_hitp;
		planet active_planet;
		engine::timer trap_timer;
	};
	engine::ref<engine::game_object> trapper_object;
	std::vector<trapper_enemy> trapper_enemies;
	void update_trapper( const engine::timestep& time_step);

	struct trap {
		glm::vec3 position{ 0.f };
		float time_trap = 5.f;
		float max_hitp = 10.f;
		float trap_radius = 0.5f;
		engine::timer trap_active_timer;
	};
	engine::ref<engine::game_object> trap_object;
	std::vector<trap> traps;
	void update_trap( const engine::timestep& time_step);


	struct player {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		float max_hitp = 100.f;
		float current_hitp;
		glm::vec3 fire_vector{ 0.f };
		float fireVel;
		bool is_trapped = false;
		bool can_dash = true;
		float trap_time = 0.5f;
		engine::timer trapped_timer;
		engine::timer dash_timer;
		float time_dash = 2.f;
	};

	//health pickup

	struct health_pickup
	{
		float amount;
		float active_time=7.f;
		engine::timer active_timer;
		glm::vec3 position;
		
	};

	engine::timer health_timer;
	float spawn_health=10.f;
	engine::ref < engine::game_object> pickup_object;
	std::vector<health_pickup> pickups;
	void update_health(const engine::timestep& time_step);


	engine::ref<cross_fade>							damage{};
	engine::ref<cross_fade>							trapped{};
	engine::ref<engine::game_object> player_object;
	player m_player;
	void update_player(const engine::timestep& time_step);



	


	engine::DirectionalLight            m_directionalLight;



	
	engine::ref<engine::audio_manager>  m_audio_manager{};
	
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::orthographic_camera       m_2d_cam;
	engine::perspective_camera        m_3d_cam;


	glm::vec3 mainscene_layer::get_random_inside_unit_sphere();
	
};
