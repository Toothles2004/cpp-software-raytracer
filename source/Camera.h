#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{45.f};

		//Vector3 forward{Vector3::UnitZ};
		Vector3 forward{ 0.266f, -0.453f, 0.860f };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		float fov{ std::tanf(TO_RADIANS*(fovAngle/2.f)) };


		Matrix CalculateCameraToWorld()
		{
			//todo: W2
			right = Vector3::Cross(Vector3::UnitY, forward);
			right.Normalize();

			up = Vector3::Cross(forward, right);
			up.Normalize();

			cameraToWorld = { right, up, forward, origin };

			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			float initFovAngle{ fovAngle };
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);


			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if(std::abs(initFovAngle - fovAngle) >= 0.00001f)
			{
				fov = std::atanf(fovAngle);
			}
			//todo: W2
			//assert(false && "Not Implemented Yet");
		}
	};
}
