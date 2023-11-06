#include "Scene_W4_BunnyScene.h"

#include "Material.h"
#include "Utils.h"

void Scene_W4_BunnyScene::Initialize()
{
	m_Camera.origin = { 0.f,3.f,-9.f };
	m_Camera.fovAngle = 45.f;

	//Materials
	const auto matLambert_GrayBlue = AddMaterial(new dae::Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
	const auto matLambert_White = AddMaterial(new dae::Material_Lambert(dae::colors::White, 1.f));

	//Planes
	AddPlane(dae::Vector3{ 0.f, 0.f, 10.f }, dae::Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
	AddPlane(dae::Vector3{ 0.f, 0.f, 0.f }, dae::Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(dae::Vector3{ 0.f, 10.f, 0.f }, dae::Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
	AddPlane(dae::Vector3{ 5.f, 0.f, 0.f }, dae::Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
	AddPlane(dae::Vector3{ -5.f, 0.f, 0.f }, dae::Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

	////Triangle (Temp)
	////===============
	//auto triangle = Triangle{ {-.75f,.5f,.0f},{-.75f,2.f, .0f}, {.75f,.5f,0.f} };
	//triangle.cullMode = TriangleCullMode::NoCulling;
	//triangle.materialIndex = matLambert_White;

	//m_Triangles.emplace_back(triangle);

	////Triangle Mesh
	////=============
	//m_pMesh = AddTriangleMesh(dae::TriangleCullMode::NoCulling, matLambert_White);
	//m_pMesh->positions = {
	//	{-.75f,-1.f,.0f},  //V0
	//	{-.75f,1.f, .0f},  //V2
	//	{.75f,1.f,1.f},    //V3
	//	{.75f,-1.f,0.f} }; //V4

	//m_pMesh->indices = {
	//	0,1,2, //Triangle 1
	//	0,2,3  //Triangle 2
	//};

	//m_pMesh->CalculateNormals();

	//m_pMesh->Translate({ 0.f,1.5f,0.f });
	//m_pMesh->UpdateTransforms();

	//OBJ
	//===
	m_pMesh = AddTriangleMesh(dae::TriangleCullMode::BackFaceCulling, matLambert_White);
	dae::Utils::ParseOBJ("Resources/lowpoly_bunny2.obj",
	                     //Utils::ParseOBJ("Resources/simple_object.obj",
		m_pMesh->positions,
		m_pMesh->normals,
		m_pMesh->indices);

	//No need to Calculate the normals, these are calculated inside the ParseOBJ function

	m_pMesh->Scale({ 2.f,2.f,2.f });
	m_pMesh->UpdateAABB();
	m_pMesh->UpdateTransforms();


	//Light
	AddPointLight(dae::Vector3{ 0.f, 5.f, 5.f }, 50.f, dae::ColorRGB{ 1.f, .61f, .45f }); //Backlight
	AddPointLight(dae::Vector3{ -2.5f, 5.f, -5.f }, 70.f, dae::ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
	AddPointLight(dae::Vector3{ 2.5f, 2.5f, -5.f }, 50.f, dae::ColorRGB{ .34f, .47f, .68f });
}

void Scene_W4_BunnyScene::Update(dae::Timer* pTimer)
{
	Scene::Update(pTimer);
	m_pMesh->RotateY((cos(pTimer->GetTotal()) + 1.f) / 2.f * dae::PI_2);
	m_pMesh->UpdateTransforms();
}
