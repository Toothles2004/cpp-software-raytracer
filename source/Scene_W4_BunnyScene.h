#pragma once
#include "Scene.h"

class Scene_W4_BunnyScene : public dae::Scene
{
public:
	Scene_W4_BunnyScene() = default;
	~Scene_W4_BunnyScene() override = default;

	Scene_W4_BunnyScene(const Scene_W4_BunnyScene&) = delete;
	Scene_W4_BunnyScene(Scene_W4_BunnyScene&&) noexcept = delete;
	Scene_W4_BunnyScene& operator=(const Scene_W4_BunnyScene&) = delete;
	Scene_W4_BunnyScene& operator=(Scene_W4_BunnyScene&&) noexcept = delete;

	void Initialize() override;
	void Update(dae::Timer* pTimer) override;

private:
	dae::TriangleMesh* m_pMesh{ nullptr };
};

