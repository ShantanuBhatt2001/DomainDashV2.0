#pragma once
#include <engine.h>
class grid
{
private:
	float m_width, m_height;
	glm::vec2 m_step;
	
public:
	grid(float width, float height, glm::vec2 step);


std::vector<glm::vec3> create_grid();
	~grid();
	glm::vec3 grid::image_to_world_coord(glm::vec3 p);
};
