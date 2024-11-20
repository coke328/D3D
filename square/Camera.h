#pragma once
#include "../D3D/D3D_app.h"
#include "../D3D/Transform.h"

class Camera : public InputProcedure
{
public:
	Camera();
	Camera(Vector3 pos,Vector3 rot);

	Transform transform;

	Matrix GetViewMatrix();
	
	void ImGUI();

	void OnInputProcess(const Keyboard::State KeyState,
		const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState,
		const Mouse::ButtonStateTracker& MouseTracker, float deltaT) override;
};

