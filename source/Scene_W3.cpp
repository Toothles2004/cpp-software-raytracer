#include "Scene_W3.h"

void Scene_W3::Initialize()
{
    m_Camera.origin = { 0.f, 1.f, -5.f };
    m_Camera.fovAngle = 45.f;

    //default: Material id0 >> SolidColor Material (RED)
    constexpr unsigned char matId_Solid_Red = 0;
    const unsigned char matId_Solid_Blue = AddMaterial(new dae::Material_SolidColor{ dae::colors::Blue });
    const unsigned char matId_Solid_Yellow = AddMaterial(new dae::Material_SolidColor{ dae::colors::Yellow });

    //Spheres
    AddSphere({ -.75f, 1.f, .0f }, 1.f, matId_Solid_Red);
    AddSphere({ .75f, 1.f, .0f }, 1.f, matId_Solid_Blue);


    //Plane
    AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Solid_Yellow);

    //Light
    AddPointLight({ 0.f, 5.f, 5.f }, 25.f, dae::colors::White);
}
