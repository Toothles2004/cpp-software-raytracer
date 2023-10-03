#pragma once
#include "Material.h"
#include "Scene.h"

class Scene_W3 : public dae::Scene
{
public:
	Scene_W3() = default;
	~Scene_W3() override = default;

	Scene_W3(const Scene_W3&) = delete;
	Scene_W3(Scene_W3&&) noexcept = delete;
	Scene_W3& operator=(const Scene_W3&) = delete;
	Scene_W3& operator=(Scene_W3&&) noexcept = delete;

	void Initialize() override;
};

