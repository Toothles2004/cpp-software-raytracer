//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float aspectRatio = static_cast<float>(m_Width) / m_Height;
	Sphere testSphere{ {0.0f,0.0f,100.0f}, 50.0f, 0 };

	const float fov{ camera.fov };

	camera.CalculateCameraToWorld();

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{};

			//calculations for rayDirection to determine the hitRay
			const float screenSpaceX = ((2.0f * ((px + 0.5f) / m_Width)) - 1.0f) * aspectRatio * fov;
			const float screenSpaceY = (1.0f - (2.0f * ((py + 0.5f) / m_Height))) * fov;
			
			Vector3 rayDirection{ screenSpaceX, screenSpaceY, 1.f};
			rayDirection = camera.cameraToWorld.TransformVector(rayDirection);
			rayDirection.Normalize();

			Ray hitRay{ camera.origin, rayDirection };

			//calculate closest hit to visualize the objects in 3D space
			HitRecord closestHit{};

			Plane testPlane{ {0.f, -50.f, 0.f}, {0.f, 1.f, 0.f}, 0 };
			
			pScene->GetClosestHit(hitRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
			}

			Vector3 rayOrigin{ closestHit.origin };
			rayOrigin += closestHit.normal * 0.0001f;

			if(m_ShadowEnabled)
			{
				for (const auto& light : lights)
				{
					Vector3 directionToLight = LightUtils::GetDirectionToLight(light, rayOrigin);
					const float distanceToLight = directionToLight.Normalize();

					Ray lightRay{ rayOrigin, directionToLight, 0.0001f, distanceToLight };

					if (pScene->DoesHit(lightRay))
					{
						finalColor *= 0.5f;
					}
				}
			}
			

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	m_CurrentLightingMode = static_cast<LightingMode>((static_cast<int>(m_CurrentLightingMode) + 1) % static_cast<int>(LightingMode::number));
}
