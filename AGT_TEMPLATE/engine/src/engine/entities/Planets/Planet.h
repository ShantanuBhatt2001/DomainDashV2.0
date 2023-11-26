#pragma once
#include <engine.h>

class Planet : public engine::layer
{
public:
	Planet(engine::ref<engine::game_object> gameObject, float size, glm::vec3 pos);
	Planet();
	~Planet();

	glm::vec3 const Position() { return position; }
	float const Radius() { return radius; }
	engine::ref<engine::game_object>	m_Planet{};

private:
	glm::vec3 position;
	float radius;
	
};
