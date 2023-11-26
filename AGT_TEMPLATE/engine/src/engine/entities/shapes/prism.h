#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a sphere object with a vertex array of a specified size and tesselation
	class prism final
	{
	public:
		// TODO - objects (any object for now, more edge cases will be explained on the way) should be passed as !!! const& !!! [one exception here is the smart pointer "ref"]
		/// \brief Constructor
		prism( uint32_t slices, float radius,float height);

		/// \brief Destructor
		~prism();
		std::vector<glm::vec3> getUnitCircleVertices(float z);
		/// \brief Getter methods
		
		uint32_t slices() const { return m_slices; }
		float radius() const { return m_radius; }
		float height() const { return m_height; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<prism> create( uint32_t slices,float height, float radius);

	private:
		/// \brief Fields
		// horizontal tessellation level of the sphere
		uint32_t m_slices;
		float m_height;
		// Sphere's radius
		float m_radius;
		// prism's mesh
		ref<engine::mesh> m_mesh;
		std::vector<glm::vec3> top_circle;
		std::vector<glm::vec3> bottom_circle;
		glm::vec3 top_center;
		glm::vec3 bottom_center;
	};
}
