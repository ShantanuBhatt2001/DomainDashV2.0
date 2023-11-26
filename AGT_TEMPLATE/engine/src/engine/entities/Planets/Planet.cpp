#include "pch.h"
#include "Planet.h"
#include <engine.h>

Planet::Planet(engine::ref<engine::game_object> gameObject, float size, glm::vec3 Pos) :m_Planet{ gameObject }, radius{ size }, position{ Pos } {



}
Planet::Planet()
{
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { 0.f, 0.f, 0.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	Planet::m_Planet = engine::game_object::create(sphere_props);
	Planet::radius = 0.5f;
	Planet::position = { 0.f,0.f,0.f };

}
Planet::~Planet()
{

}
