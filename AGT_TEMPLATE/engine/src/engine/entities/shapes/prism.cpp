#include "pch.h"
#include "prism.h"
#include <engine.h>

engine::prism::prism( uint32_t slices,float height, float radius) : m_slices(slices),m_height(height), m_radius(radius)
{
    std::cout << "called constructor";
	std::vector<engine::mesh::vertex> prism_vertices;
	std::vector<uint32_t> prism_indices;
    m_slices = 3;
	const float sliceStep = 2 * PI / m_slices;
	const float lengthInv = 1.0f / m_radius;
    bottom_circle = getUnitCircleVertices(0.f);
    top_circle = getUnitCircleVertices(m_height);
    
    

    std::cout << "called unit circles";
   
        std::vector<glm::vec3> normals;
        glm::vec3 temp = glm::cross(top_circle.at(0) - top_circle.at(1), bottom_circle.at(1) - top_circle.at(1));
        normals.push_back(temp);
        temp = glm::cross(top_circle.at(1) - top_circle.at(2), bottom_circle.at(2) - top_circle.at(2));
        normals.push_back(temp);
        temp = glm::cross(top_circle.at(2) - top_circle.at(0), bottom_circle.at(0) - top_circle.at(0));
        normals.push_back(temp);
        std::cout << "called normals";
        prism_vertices={
            //top face
            {top_circle.at(0),glm::vec3(0.f,0.f,1.f),glm::vec2(1.f / m_slices,1.f)},//0
            {top_circle.at(1),glm::vec3(0.f,0.f,1.f),glm::vec2(2.f / m_slices,1.f)},//1
            {top_circle.at(2),glm::vec3(0.f,0.f,1.f),glm::vec2(3.f / m_slices,1.f)},//2
            //bottom face
            {bottom_circle.at(0),glm::vec3(0.f,0.f,-1.f),glm::vec2(1.f / m_slices,0.f)},//3
            {bottom_circle.at(1),glm::vec3(0.f,0.f,-1.f),glm::vec2(2.f / m_slices,0.f)},//4
            {bottom_circle.at(2),glm::vec3(0.f,0.f,-1.f),glm::vec2(3.f / m_slices,0.f)},//5
            //rectangular face
            {bottom_circle.at(0),normals.at(0),glm::vec2(1.f / m_slices,0.f)},//6
            {bottom_circle.at(1),normals.at(0),glm::vec2(2.f / m_slices,0.f)},//7
            {top_circle.at(0),normals.at(0),glm::vec2(1.f / m_slices,1.f)},//8
            {top_circle.at(1),normals.at(0),glm::vec2(2.f / m_slices,1.f)},//9

            {bottom_circle.at(1),normals.at(1),glm::vec2(2.f / m_slices,0.f)},//10
            {bottom_circle.at(2),normals.at(1),glm::vec2(3.f / m_slices,0.f)},//11
            {top_circle.at(1),normals.at(1),glm::vec2(2.f / m_slices,1.f)},//12
            {top_circle.at(2),normals.at(1),glm::vec2(3.f / m_slices,1.f)},//13

            {bottom_circle.at(2),normals.at(2),glm::vec2(3.f / m_slices,0.f)},//14
            {bottom_circle.at(0),normals.at(2),glm::vec2(1.f / m_slices,0.f)},//15
            {top_circle.at(2),normals.at(2),glm::vec2(3.f / m_slices,1.f)},//16
            {top_circle.at(0),normals.at(2),glm::vec2(1.f / m_slices,1.f)},//17

           
        };
        std::cout << "called vertices";
        prism_indices=
        {
         0, 1, 2, //top

         3, 5, 4, //bottom
         8, 6, 7, //1-1
         9,8,7, // 1-2
         12, 10, 11, //2-1
         11,13,12,//2-2
         16,14,15,//3-1
         15,17,16//3-2
        };
        std::cout << "called indices";
        m_mesh = engine::mesh::create(prism_vertices, prism_indices);
        std::cout << "called mesh";
    
}
std::vector<glm::vec3> engine::prism::getUnitCircleVertices(float z)
{
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / m_slices;
    float sectorAngle;  // radian

    std::vector<glm::vec3> unitCircleVertices;
    for (int i = 0; i < m_slices; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back(glm::vec3(m_radius* cos(sectorAngle), m_radius*sin(sectorAngle),z)); // x
        std::cout << "\nx=" << m_radius * cos(sectorAngle) << ",y=" << m_radius * sin(sectorAngle) << ",z=" << z;
    }
    return unitCircleVertices;
}
engine::prism::~prism() {}

engine::ref<engine::prism> engine::prism::create( uint32_t slices,float height, float radius)
{
    std::cout << "called create";
    return std::make_shared<engine::prism>( slices,height, radius);
}
