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

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{};
			float screenSpaceX = ((2.0f * ((static_cast<float>(px) + 0.5f) / static_cast<float>(m_Width))) - 1.0f) * aspectRatio;
			//float screenSpaceY = ((1.0f - 2.0f * (static_cast<float>(py) + 0.5f)) / (static_cast<float>(m_Height)));
			float screenSpaceY = (1.0f - (2.0f * ((static_cast<float>(py) + 0.5f) / static_cast<float>(m_Height))));
			Vector3 rayDirection{ screenSpaceX, screenSpaceY, 1.0f};
			rayDirection.Normalize();

			Ray hitRay{ camera.origin, rayDirection };

			HitRecord closestHit{};

			pScene->GetClosestHit(hitRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();
				//const float scaled_t = (closestHit.t - 50.0f) / 40.0f;
				//finalColor = { scaled_t, scaled_t , scaled_t };
			}

			//Update Color in Buffer
			//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };
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
