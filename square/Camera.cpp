#include "Camera.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#define PI 3.141592

Camera::Camera()
{
}

Camera::Camera(Vector3 pos, Vector3 rot)
{
	transform.SetPosition(pos);
	transform.SetRotation(rot);
}

Matrix Camera::GetViewMatrix()
{
	return transform.WorldMatrix().Invert();
}

void Camera::ImGUI()
{
	ImGui::Text("Camera");
	Vector3 pos = transform.GetPosition();
	ImGui::DragFloat3("Cam Pos",(float*)&pos);
	transform.SetPosition(pos);
}

void Camera::OnInputProcess(const Keyboard::State KeyState, const Keyboard::KeyboardStateTracker& KeyTracker, const Mouse::State& MouseState, const Mouse::ButtonStateTracker& MouseTracker, float deltaT)
{
	static int lastMX, lastMY = 0;

	static float x = transform.GetRotation().x;
	static float y = transform.GetRotation().y;

	float Mspeed = 6.0f * deltaT;

	if (MouseState.rightButton)
	{
		x += (MouseState.x - lastMX) * -Mspeed;
		y += (MouseState.y - lastMY) * Mspeed;
		transform.SetRotation({ y, x, 0 });
	}

	lastMX = MouseState.x;
	lastMY = MouseState.y;

	float speed = 50.0f * deltaT;

	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::W)) {
		transform.AddPosition(transform.Forward() * speed);
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::A)) {
		transform.AddPosition(transform.Right() * -speed);
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::S)) {
		transform.AddPosition(transform.Forward() * -speed);
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::D)) {
		transform.AddPosition(transform.Right() * speed);
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::Space)) {
		transform.AddPosition(transform.Up() * speed);
	}
	if (KeyState.IsKeyDown(DirectX::Keyboard::Keys::LeftShift)) {
		transform.AddPosition(transform.Up() * -speed);
	}
}
