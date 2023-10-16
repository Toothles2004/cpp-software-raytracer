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

		Vector3 forward{Vector3::UnitZ};
		//Vector3 forward{ 0.266f, -0.453f, 0.860f };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{0.f};
		float totalYaw{0.f};

		Matrix cameraToWorld{};

		float fov{ std::tanf(TO_RADIANS*(fovAngle/2.f)) };
		const float moveSpeed{ 10 };
		const float rotateSpeed{TO_RADIANS*(10.f)};


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
			const float initFovAngle{ fovAngle };
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			//todo: W2
			if((pKeyboardState == nullptr) && (mouseState & SDL_BUTTON_LMASK) && (mouseState & SDL_BUTTON_RMASK))
			{
				return;
			}

			//Calculates the forward and right vector for camera movement with Keyboard input
			origin += moveSpeed * deltaTime *													//multiply forward and right vector with speed
				(
					((pKeyboardState[SDL_SCANCODE_UP] | pKeyboardState[SDL_SCANCODE_W]) -       //calculates the forward movement vector
					(pKeyboardState[SDL_SCANCODE_DOWN] | pKeyboardState[SDL_SCANCODE_S])) *		
					forward +																	
					((pKeyboardState[SDL_SCANCODE_D] | pKeyboardState[SDL_SCANCODE_RIGHT]) -	//calculates the horizontal movement vector
					(pKeyboardState[SDL_SCANCODE_A] | pKeyboardState[SDL_SCANCODE_LEFT])) *		
					right																		
				);

			////Calculates forward and up vector for camera movement when LMB and/or RMB are pressed
			//origin += (moveSpeed / 2) * 
			//	deltaTime * 
			//	(
			//		(static_cast<bool>(mouseState & SDL_BUTTON_LMASK) * 
			//		mouseY * 
			//		(static_cast<bool>(mouseState & SDL_BUTTON_RMASK) - 1)) *					// RMB not pressed gives forward movement, when pressed it stops forward movement
			//		forward +
			//		(static_cast<bool>(mouseState & SDL_BUTTON_RMASK) & static_cast<bool>(mouseState & SDL_BUTTON_LMASK)) *
			//		-mouseY *
			//		up
			//	);

			////Calculates camera's forward vector rotation when LMB and/or RMB are pressed
			//totalPitch += rotateSpeed * 
			//	deltaTime * 
			//	(
			//		static_cast<bool>(mouseState & SDL_BUTTON_RMASK) *
			//		(static_cast<bool>(mouseState & SDL_BUTTON_LMASK) - 1)  *
			//		-mouseY
			//	);

			/*totalYaw += rotateSpeed *
				deltaTime *
				(
					(static_cast<bool>(mouseState & SDL_BUTTON_RMASK) ^ static_cast<bool>(mouseState & SDL_BUTTON_LMASK)) *
					mouseX
				);*/

			if((mouseState & SDL_BUTTON_RMASK) && (mouseState & SDL_BUTTON_LMASK))
			{
				origin += (moveSpeed / 2) * deltaTime * mouseY * Vector3::UnitY;		//moves up and down on Y axis
			}
			else if(mouseState & SDL_BUTTON_RMASK)
			{
				totalYaw += rotateSpeed * deltaTime * mouseX;							//turns around the Y axis (horizontally)
				totalPitch += rotateSpeed * deltaTime * mouseY;							//turns around the X axis (vertically)
			}
			else if(mouseState & SDL_BUTTON_LMASK)
			{
				origin += (moveSpeed / 2) * deltaTime * -mouseY * forward;				//determines forward movement
				totalYaw += rotateSpeed * deltaTime * mouseX;							//turns around the y axis (horizontally)
			}

			const Matrix finalRotation{Matrix::CreateRotation(totalPitch, totalYaw, 0.f)};

			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();

			//checks for angle change
			if(std::abs(initFovAngle - fovAngle) >= 0.00001f)
			{
				fov = std::atanf(fovAngle);
			}

		}
	};
}
