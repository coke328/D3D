#pragma once

class InputSystem;

class InputProcedure {
	friend class InputSystem;
protected:
	virtual void OnInputProcess(const Keyboard::State KeyState,
		const Keyboard::KeyboardStateTracker& KeyTracker,
		const Mouse::State& MouseState, 
		const Mouse::ButtonStateTracker& MouseTracker, float deltaT) = 0;

	InputProcedure();
	virtual ~InputProcedure() {};

};

class InputSystem
{
	friend class InputProcedure;
public:
	
	InputSystem();
	~InputSystem() = default;

	static InputSystem* instance;
	
	void Update(float DeltaT);
	bool Initialize(HWND hWnd);

private:

	std::unique_ptr<DirectX::Keyboard>              m_Keyboard;
	std::unique_ptr<DirectX::Mouse>                 m_Mouse;

	DirectX::Keyboard::KeyboardStateTracker         m_KeyboardStateTracker;
	DirectX::Mouse::ButtonStateTracker              m_MouseStateTracker;

	DirectX::Mouse::State                           m_MouseState;
	DirectX::Keyboard::State                        m_KeyboardState;

	std::vector<std::shared_ptr<InputProcedure>>	m_InputProcedures;

};

