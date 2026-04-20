#pragma once

#include<Editor/Common/EngineState.h>

namespace Pitaya::Editor
{
	class Editor;
	class StateMachine
	{
		friend class Editor;
	private:
		StateMachine() = default;
		~StateMachine() = default;

	public:
		StateMachine(const StateMachine&) = delete;
		StateMachine& operator=(const StateMachine&) = delete;
		StateMachine(StateMachine&&) = delete;
		StateMachine& operator=(StateMachine&&) = delete;

	public:
		inline EngineState GetCurrentState() const noexcept
		{
			return state;
		}
		inline void SwitchState(EngineState state) noexcept
		{
			this->state = state;
		}

	private:
		EngineState state = EngineState::Edit;
	};
}
