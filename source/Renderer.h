#pragma once

#include <cstdint>
#include <vector>

#include "Matrix.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;
	class Material;
	struct Light;
	struct Camera;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		void RenderPixel(
			const Scene* pScene, 
			uint32_t pixelIndex, 
			float fov, 
			float aspectRatio, 
			const Camera& camera, 
			const std::vector<Light>& light, 
			const std::vector<Material*>& material
		) const;
		bool SaveBufferToImage() const;

		void CycleLightingMode();
		void ToggleShadows() { m_ShadowEnabled = !m_ShadowEnabled; }

	private:

		enum class LightingMode
		{
			observedArea,		//lambert cosine law
			radiance,			//incident radians
			BRDF,				//scattering of the light
			combined,			//observedArea*radiance*BRDF
			number
		};

		LightingMode m_CurrentLightingMode{ LightingMode::combined };
		bool m_ShadowEnabled{ true };

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
	};
}
