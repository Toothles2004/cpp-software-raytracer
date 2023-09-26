#include "Scene_W2.h"
#include "Utils.h"
#include "Material.h"

void Scene_W2::Initialize()
{
    m_Camera.origin = { 0.f, 3.f, -9.f };
    m_Camera.fovAngle = 45.f;

    constexpr unsigned char matId_Solid_Red = 0;
    const unsigned char matId_Solid_Blue = AddMaterial(new dae::Material_SolidColor{ dae::colors::Blue });
    const unsigned char matId_Solid_Yellow = AddMaterial(new dae::Material_SolidColor{ dae::colors::Yellow });
    const unsigned char matId_Solid_Green = AddMaterial(new dae::Material_SolidColor{ dae::colors::Green });
    const unsigned char matId_Solid_Cyan = AddMaterial(new dae::Material_SolidColor{ dae::colors::Cyan });
    const unsigned char matId_Solid_Magenta = AddMaterial(new dae::Material_SolidColor{ dae::colors::Magenta });
    const unsigned char matId_Solid_White = AddMaterial(new dae::Material_SolidColor{ dae::colors::White });
    const unsigned char matId_Solid_Gray = AddMaterial(new dae::Material_SolidColor{ dae::colors::Gray });
    const unsigned char matId_Solid_Black = AddMaterial(new dae::Material_SolidColor{ dae::colors::Black });

    //Plane
    AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matId_Solid_Green);
    AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matId_Solid_Green);
    AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matId_Solid_Yellow);
    AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matId_Solid_Yellow);
    AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matId_Solid_Magenta);

    //Spheres
    AddSphere({ -1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
    AddSphere({ 0.f, 1.f, 0.f }, .75f, matId_Solid_Blue);
    AddSphere({ 1.75f, 1.f, 0.f }, .75f, matId_Solid_Red);
    AddSphere({ -1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);
    AddSphere({ 0.f, 3.f, 0.f }, .75f, matId_Solid_Red);
    AddSphere({ 1.75f, 3.f, 0.f }, .75f, matId_Solid_Blue);

    //Light
    AddPointLight({ 0.f, 35.f, -5.f }, 70.f, dae::colors::White);
}
