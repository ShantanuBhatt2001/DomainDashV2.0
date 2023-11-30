#include "pch.h"
#include "grid.h"
#include <engine.h>
#include"engine/entities/shapes/heightmap.h"
#include "stb_image/stb_image.h"

grid::grid(float width, float height, glm::vec2 step) : m_width(width), m_height(height), m_step(step)
{

}
grid::~grid() {
}

std::vector<glm::vec3> grid::create_grid()
{
	//engine::ref<engine::heightmap> heightmap_terrain = engine::heightmap::create("game/assets/textures/heightmap.jpg", "game/assets/textures/heightmap.jpg", m_width, m_height, glm::vec3(0.f),1.f);
	std::vector < glm::vec3> return_vector;
	BYTE* bDataPointer;
	std::string sTerrainFilename = "assets/textures/heightmap.jpg";
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(sTerrainFilename.c_str(), &width, &height, &channels, 0);
	CORE_ASSERT(data, "[gl_texture_2d] Texture not loaded correctly");
	m_width = width;
	m_height = height;
	bDataPointer = data; // Retrieve the image data
	bool check;
	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == NULL || width == 0 || height == 0)
		check = false;
	else
		check = true;
	
	if (check)
	{
		glm::vec3 m_origin(0.f);
		

		// Allocate memory and initialize to store the image
		float* m_heightMap = new float[(size_t)m_width * m_height];
		if (m_heightMap != NULL)
		{
			// Clear the heightmap
			memset(m_heightMap, 0, (size_t)m_width * m_height * sizeof(float));

			// Form mesh
			

			uint32_t X = 1;
			uint32_t Z = m_width;
			uint32_t triangleId = 0;
			for (uint32_t z = 0; z <100; z+=1) {
				for (uint32_t x = 0; x < 100; x+=1) {
					uint32_t index = x + z * m_width;

					// Retreive the colour from the terrain image, and set the normalized height in the range [0, 1]
					float grayScale = (bDataPointer[index * 3] + bDataPointer[index * 3 + 1] + bDataPointer[index * 3 + 2]) / 3.0f;
					float height = (grayScale - 128.0f) / 128.0f;

					// Make a point based on this pixel position.  Then, transform so that the mesh has the correct size and origin
					// This transforms a point in image coordinates to world coordinates
					glm::vec3 pos_image = glm::vec3((float)x, height, (float)z);
					glm::vec3 pos = image_to_world_coord(pos_image);
					std::cout << std::endl << pos;
					return_vector.push_back(pos);
				}
			}

		}
		
	}
	return return_vector;
}

glm::vec3 grid::image_to_world_coord(glm::vec3 p)
{
	// Normalize the image point so that it in the range [-1, 1] in x and [-1, 1] in z
	p.x = 2.0f * (p.x / 100) - 1.0f;
	p.z = 2.0f * (p.z / 100) - 1.0f;

	// Now scale the point so that the terrain has the right size in x and z
	p.x *= 100 / 2.0f;
	p.z *= 100 / 2.0f;

	// Now translate the point based on the origin passed into the function
	p += glm::vec3(0.f);

	return p;
}
