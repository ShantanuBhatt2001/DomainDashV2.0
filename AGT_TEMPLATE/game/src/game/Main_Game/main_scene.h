#pragma once
#include <engine.h>

class mainscene_layer : public engine::layer
{
public:
	mainscene_layer();
	~mainscene_layer();

	void on_update(const engine::timestep& time_step) override;
	void on_render() override;
	void on_event(engine::event& event) override;

private:
	//mats
	engine::ref<engine::material> planet_mat;
	engine::ref<engine::material> spawn_ship_mat;
	engine::ref<engine::material> follow_mat;
	engine::ref<engine::material> grenadier_mat;
	engine::ref<engine::material> grenade_mat;
	engine::ref<engine::material> trapper_mat;
	engine::ref<engine::material> trap_mat;
	engine::ref<engine::material> player_mat;
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
	engine::ref<engine::game_object> boid_object;
	std::vector<spawn_ship> spawn_ships;
	//count of boid enemies
	int count = 200;
	void update_boid(spawn_ship & spawn_ship_instance);


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
	float follow_accel = 50.f;

	void update_follow(follow_enemy& follow_instance);
	// mortar enemy struct

	struct grenadier_enemy {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		int max_hitp = 10.f;
		int current_hitp;
		float firing_dist=0.f;
		float firing_vel=0.f;
		planet active_planet;
	};
	engine::ref<engine::game_object> grenadier_object;
	std::vector<grenadier_enemy> grenadier_enemies;
	void update_grenadier(grenadier_enemy& grenadier_instance);

	struct grenade {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		float maxTime = 2.f;
		float grenade_radius = 0.5f;
		planet active_planet;
	};
	engine::ref<engine::game_object> grenade_object;
	std::vector<grenade> grenades;
	void update_grenade(grenade& grenade_instance);


	struct trapper_enemy {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		float trap_place_time = 7.f;
		float max_hitp = 10.f;
		float current_hitp;
		planet active_planet;
	};
	engine::ref<engine::game_object> trapper_object;
	std::vector<trapper_enemy> trapper_enemies;
	void update_trapper(trapper_enemy& trapper_instance);

	struct trap {
		glm::vec3 position{ 0.f };
		float time_trap = 2.f;
		float max_hitp = 10.f;
		float trap_radius = 0.5f;
	};
	engine::ref<engine::game_object> trap_object;
	std::vector<trapper_enemy> traps;
	void update_trap(trap& trap_instance);


	struct player {
		glm::vec3 position{ 0.f };
		glm::vec3 velocity{ 0.f };
		glm::vec3 accel{ 0.f };
		float max_hitp = 100.f;
		
		glm::vec3 fire_vector{ 0.f };
		float fireVel;
		bool is_trapped = false;
	};
	engine::ref<engine::game_object> player_object;
	player m_player;
	void update_player();



	


	engine::DirectionalLight            m_directionalLight;



	
	engine::ref<engine::audio_manager>  m_audio_manager{};
	
	engine::ref<engine::text_manager>	m_text_manager{};

	engine::orthographic_camera       m_2d_cam;
	engine::perspective_camera        m_3d_cam;
};
