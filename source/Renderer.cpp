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
#include "execution"

#define PARALLEL_EXECUTION

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
	//Sphere testSphere{ {0.0f,0.0f,100.0f}, 50.0f, 0 };

	const float fov{ camera.fov };

	camera.CalculateCameraToWorld();
	#if defined(PARALLEL_EXECUTION)
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };
	std::vector<uint32_t> pixelIndices{};

	pixelIndices.reserve(amountOfPixels);
	for (uint32_t index{}; index < amountOfPixels; ++index) pixelIndices.emplace_back(index);

	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int index)
		{
			RenderPixel(pScene, index, fov, aspectRatio, camera.cameraToWorld, camera.origin);
		});
	
	#else
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
			
			pScene->GetClosestHit(hitRay, closestHit);

			if (closestHit.didHit)
			{
				Vector3 rayOrigin{ closestHit.origin };
				rayOrigin += closestHit.normal * 0.0001f;

				for (const auto& light : lights)
				{
					Vector3 directionToLight = LightUtils::GetDirectionToLight(light, rayOrigin);
					const float distanceToLight = directionToLight.Normalize();
					if (m_ShadowEnabled)
					{
						Ray lightRay{ rayOrigin, directionToLight, 0.0001f, distanceToLight };

						if (pScene->DoesHit(lightRay))
						{
							continue;
						}
					}

					float observedArea{ Vector3::Dot(closestHit.normal, directionToLight) };

					switch (m_CurrentLightingMode)
					{
					case dae::Renderer::LightingMode::observedArea:
						if (observedArea > 0.f)
						{
							finalColor += observedArea * ColorRGB(1, 1, 1);
						}
						break;
					case dae::Renderer::LightingMode::radiance:
						finalColor += LightUtils::GetRadiance(light, closestHit.origin);
						break;
					case dae::Renderer::LightingMode::BRDF:
						finalColor += materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, rayDirection);
						break;
					case dae::Renderer::LightingMode::combined:
						if (observedArea > 0.f)
						{
							finalColor +=
								LightUtils::GetRadiance(light, closestHit.origin) *
								materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, rayDirection) *
								observedArea;
						}
						
						break;
					case dae::Renderer::LightingMode::number:
						break;
					default:
						break;
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
	#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld,
	const Vector3 cameraOrigin) const
{
	auto materials{ pScene->GetMaterials() };
	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };

	float rx{ static_cast<float>(px) + 0.5f }, ry{ static_cast<float>(py) + 0.5f };
	float cx{ (2 * (rx / static_cast<float>(m_Width)) - 1) };
	float cy{ (1 - (2 * (ry / static_cast<float>(m_Height)))) * fov };

	auto& lights = pScene->GetLights();
	//Sphere testSphere{ {0.0f,0.0f,100.0f}, 50.0f, 0 };

	ColorRGB finalColor{};

	//calculations for rayDirection to determine the hitRay
	const float screenSpaceX = ((2.0f * ((px + 0.5f) / m_Width)) - 1.0f) * aspectRatio * fov;
	const float screenSpaceY = (1.0f - (2.0f * ((py + 0.5f) / m_Height))) * fov;

	Vector3 rayDirection{ screenSpaceX, screenSpaceY, 1.f };
	rayDirection = cameraToWorld.TransformVector(rayDirection);
	rayDirection.Normalize();

	Ray hitRay{ cameraOrigin, rayDirection };

	//calculate closest hit to visualize the objects in 3D space
	HitRecord closestHit{};

	pScene->GetClosestHit(hitRay, closestHit);

	if (closestHit.didHit)
	{
		Vector3 rayOrigin{ closestHit.origin };
		rayOrigin += closestHit.normal * 0.0001f;

		for (const auto& light : lights)
		{
			Vector3 directionToLight = LightUtils::GetDirectionToLight(light, rayOrigin);
			const float distanceToLight = directionToLight.Normalize();
			if (m_ShadowEnabled)
			{
				Ray lightRay{ rayOrigin, directionToLight, 0.0001f, distanceToLight };

				if (pScene->DoesHit(lightRay))
				{
					continue;
				}
			}

			float observedArea{ Vector3::Dot(closestHit.normal, directionToLight) };

			switch (m_CurrentLightingMode)
			{
			case dae::Renderer::LightingMode::observedArea:
				if (observedArea > 0.f)
				{
					finalColor += observedArea * ColorRGB(1, 1, 1);
				}
				break;
			case dae::Renderer::LightingMode::radiance:
				finalColor += LightUtils::GetRadiance(light, closestHit.origin);
				break;
			case dae::Renderer::LightingMode::BRDF:
				finalColor += materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, rayDirection);
				break;
			case dae::Renderer::LightingMode::combined:
				if (observedArea > 0.f)
				{
					finalColor +=
						LightUtils::GetRadiance(light, closestHit.origin) *
						materials[closestHit.materialIndex]->Shade(closestHit, directionToLight, rayDirection) *
						observedArea;
				}

				break;
			case dae::Renderer::LightingMode::number:
				break;
			default:
				break;
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

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	m_CurrentLightingMode = static_cast<LightingMode>((static_cast<int>(m_CurrentLightingMode) + 1) % static_cast<int>(LightingMode::number));
}
