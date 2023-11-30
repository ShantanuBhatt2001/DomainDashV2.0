#include "pch.h"
#include "cylinder.h"
#include <engine.h>

engine::cylinder::cylinder(uint32_t slices, float height, float radius) : m_slices(slices), m_height(height), m_radius(radius)
{
    std::cout << "called constructor";
    std::vector<engine::mesh::vertex> cylinder_vertices;
    std::vector<uint32_t> cylinder_indices;
    if (m_slices < 3)
        m_slices = 3;
    const float sliceStep = 2 * PI / m_slices;
    const float lengthInv = 1.0f / m_radius;
    bottom_circle = getUnitCircleVertices(0.f);
    top_circle = getUnitCircleVertices(m_height);



    std::cout << "called unit circles";

    std::vector<glm::vec3> normals;
    normals.push_back(glm::vec3{ 0.f,0.f,1.f });
    for (int i = 0; i < m_slices; i++)
    {
        glm::vec3 temp = glm::cross(top_circle.at(i % m_slices) - top_circle.at((i + 1) % m_slices), bottom_circle.at((i + 1) % m_slices) - top_circle.at((i + 1) % m_slices));
        normals.push_back(temp);
    }
    normals.push_back(glm::vec3{ 0.f,0.f,-1.f });
    /*glm::vec3 temp = glm::cross(top_circle.at(0) - top_circle.at(1), bottom_circle.at(1) - top_circle.at(1));
    normals.push_back(temp);
    temp = glm::cross(top_circle.at(1) - top_circle.at(2), bottom_circle.at(2) - top_circle.at(2));
    normals.push_back(temp);
    temp = glm::cross(top_circle.at(2) - top_circle.at(0), bottom_circle.at(0) - top_circle.at(0));
    normals.push_back(temp);*/
    std::cout << "called normals";
    //prism_vertices = {
    //    //top face
    //    {top_circle.at(0),glm::vec3(0.f,0.f,1.f),glm::vec2(1.f / m_slices,1.f)},//0
    //    {top_circle.at(1),glm::vec3(0.f,0.f,1.f),glm::vec2(2.f / m_slices,1.f)},//1
    //    {top_circle.at(2),glm::vec3(0.f,0.f,1.f),glm::vec2(3.f / m_slices,1.f)},//2
    //    //bottom face
    //    {bottom_circle.at(0),glm::vec3(0.f,0.f,-1.f),glm::vec2(1.f / m_slices,0.f)},//3
    //    {bottom_circle.at(1),glm::vec3(0.f,0.f,-1.f),glm::vec2(2.f / m_slices,0.f)},//4
    //    {bottom_circle.at(2),glm::vec3(0.f,0.f,-1.f),glm::vec2(3.f / m_slices,0.f)},//5
    //    //rectangular face
    //    {bottom_circle.at(0),normals.at(0),glm::vec2(1.f / m_slices,0.f)},//6
    //    {bottom_circle.at(1),normals.at(0),glm::vec2(2.f / m_slices,0.f)},//7
    //    {top_circle.at(0),normals.at(0),glm::vec2(1.f / m_slices,1.f)},//8
    //    {top_circle.at(1),normals.at(0),glm::vec2(2.f / m_slices,1.f)},//9

    //    {bottom_circle.at(1),normals.at(1),glm::vec2(2.f / m_slices,0.f)},//10
    //    {bottom_circle.at(2),normals.at(1),glm::vec2(3.f / m_slices,0.f)},//11
    //    {top_circle.at(1),normals.at(1),glm::vec2(2.f / m_slices,1.f)},//12
    //    {top_circle.at(2),normals.at(1),glm::vec2(3.f / m_slices,1.f)},//13

    //    {bottom_circle.at(2),normals.at(2),glm::vec2(3.f / m_slices,0.f)},//14
    //    {bottom_circle.at(0),normals.at(2),glm::vec2(1.f / m_slices,0.f)},//15
    //    {top_circle.at(2),normals.at(2),glm::vec2(3.f / m_slices,1.f)},//16
    //    {top_circle.at(0),normals.at(2),glm::vec2(1.f / m_slices,1.f)},//17


    //};

    // bottom circle vertices
    engine::mesh::vertex bottom_center = { glm::vec3(0.f) ,normals.at(m_slices + 1),glm::vec2(0.5f) };

    for (int i = 0; i < m_slices; i++)
    {
        cylinder_vertices.push_back({ top_circle[i % m_slices],normals.at(i + 1),glm::vec2(top_circle[i] / m_radius) });
        cylinder_vertices.push_back({ bottom_circle[i % m_slices],normals.at(i + 1),glm::vec2(bottom_circle[i] / m_radius) });
        cylinder_vertices.push_back({ bottom_circle[(i + 1) % m_slices],normals.at(i + 1),glm::vec2(bottom_circle[(i + 1) % m_slices] / m_radius) });

        cylinder_vertices.push_back({ bottom_circle[(i+1) % m_slices],normals.at(i + 1),glm::vec2(top_circle[i] / m_radius) });
        cylinder_vertices.push_back({ top_circle[(i+1) % m_slices],normals.at(i + 1),glm::vec2(bottom_circle[i] / m_radius) });
        cylinder_vertices.push_back({ top_circle[(i ) % m_slices],normals.at(i + 1),glm::vec2(bottom_circle[(i + 1) % m_slices] / m_radius) });
    }
    int j = 0;
    for (j; j < cylinder_vertices.size(); ++j)
    {
        cylinder_indices.push_back(j);
    }
    for (int i = 0; i < m_slices; i++)
    {
        cylinder_vertices.push_back({ bottom_circle[i],normals.at(m_slices+1),glm::vec2(bottom_circle[i] / m_radius) });
    }

    engine::mesh::vertex top_center = { glm::vec3({0.f,0.f,m_height}) ,normals.at(0),glm::vec2(0.5f) };
   
    for (int i = 0; i < m_slices; i++)
    {
        cylinder_vertices.push_back({ top_circle[i],normals.at(0),glm::vec2(top_circle[i] / m_radius) });
    }
    
    cylinder_vertices.push_back(bottom_center);
    cylinder_vertices.push_back(top_center);
    std::cout << std::endl << "i after sides" << j << std::endl;
    for (int i=6*m_slices; i <(7*m_slices); ++i)
    {
        cylinder_indices.push_back(cylinder_vertices.size() - 2);
        if((i+1)>=7*m_slices)
            cylinder_indices.push_back(((i + 1)-m_slices));
        else
        cylinder_indices.push_back(((i + 1) % (7*m_slices)));
        cylinder_indices.push_back(i);
        
    }

    for (int i = 7 * m_slices; i < (8 * m_slices); ++i)
    {
        cylinder_indices.push_back(i);
        if ((i + 1) >= 8 * m_slices)
            cylinder_indices.push_back(((i + 1) - m_slices));
        else
            cylinder_indices.push_back(((i + 1) % (8 * m_slices)));
        
        cylinder_indices.push_back(cylinder_vertices.size() - 1);
    }
   /* j += i;
    for (int i=0;; i < 8*m_slices; i++)
    {
        cylinder_indices.push_back(cylinder_vertices.size() - 1);
        cylinder_indices.push_back(i% (8 * m_slices));
        cylinder_indices.push_back((i + 1) % (8 * m_slices));
    }
    std::cout << "called vertices";*/
    
    std::cout << "called indices";
    m_mesh = engine::mesh::create(cylinder_vertices, cylinder_indices);
    std::cout << "called mesh";

}
std::vector<glm::vec3> engine::cylinder::getUnitCircleVertices(float z)
{
    const float PI = 3.1415926f;
    float sectorStep = 2 * PI / m_slices;
    float sectorAngle;  // radian

    std::vector<glm::vec3> unitCircleVertices;
    for (int i = 0; i < m_slices; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back(glm::vec3(m_radius * cos(sectorAngle), m_radius * sin(sectorAngle), z)); // x
        std::cout << "\nx=" << m_radius * cos(sectorAngle) << ",y=" << m_radius * sin(sectorAngle) << ",z=" << z;
    }
    return unitCircleVertices;
}
engine::cylinder::~cylinder() {}

engine::ref<engine::cylinder> engine::cylinder::create(uint32_t slices, float height, float radius)
{
    std::cout << "called create";
    return std::make_shared < engine::cylinder > (slices, height, radius);
}
