#include "pch.h"
#include "InputSystem.h"

InputSystem* InputSystem::instance = nullptr;

InputSystem::InputSystem()
	:m_MouseState(), m_KeyboardState()
{
	assert(instance == nullptr);
	instance = this;
}

void InputSystem::Update(float deltaT)
{
	m_MouseState = m_Mouse->GetState();
	m_MouseStateTracker.Update(m_MouseState);

	m_KeyboardState = m_Keyboard->GetState();
	m_KeyboardStateTracker.Update(m_KeyboardState);

	for (auto iter = m_InputProcedures.begin(); iter != m_InputProcedures.end();	) {
		if (iter->operator bool()) {
			iter->get()->OnInputProcess(m_KeyboardState, m_KeyboardStateTracker, m_MouseState, m_MouseStateTracker, deltaT);
			++iter;
		}
		else {
			iter = m_InputProcedures.erase(iter);
		}
	}
}

bool InputSystem::Initialize(HWND hWnd)
{
	m_Keyboard = std::make_unique<Keyboard>();
	m_Mouse = std::make_unique<Mouse>();
	m_Mouse->SetWindow(hWnd);
	return true;
}

InputProcedure::InputProcedure()
{
	InputSystem::instance->m_InputProcedures.emplace_back(this, [](InputProcedure*) {});
}
