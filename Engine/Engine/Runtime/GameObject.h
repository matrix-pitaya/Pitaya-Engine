#pragma once

#include"Enum/Enum.h"
#include"Transfom.h"

#include<memory>
#include<vector>

namespace Pitaya::Engine
{
	class GameObject : public Object
	{
		DELETE_COPY_AND_MOVE(GameObject)

	public:
		GameObject(const std::string& name = "GameObject")
			:name(name) 
		{
			components[static_cast<size_t>(Pitaya::Engine::ComponentType::Transform)] = &transform;
		}
		~GameObject()
		{
			for (auto& component : components)
			{
				if (component == nullptr || typeid(*component) == typeid(Transform))
				{
					continue;
				}

				//Engine::DestoryComponent(component);
				component = nullptr;
			}
			components.clear();
		}

		template<typename T>
		T* GetComponent() 
		{
			return static_cast<T*>(components[static_cast<size_t>(T::Type)]);
		}

		template<typename T, typename... Args>
		T* AddComponent(Args&&... args) 
		{
			size_t index = static_cast<size_t>(T::Type);
			if (components[index])
			{
				return static_cast<T*>(components[index]);
			}
				
			T* component = Engine::CreateComponent<T>(this, std::forward<Args>(args)...);
			components[index] = component;
			return component;
		}

	private:
		std::string name = "GameObject";
		Pitaya::Engine::Transform transform = Pitaya::Engine::Transform(this);
		std::vector<Component*> components = std::vector<Component*>(static_cast<size_t>(Pitaya::Engine::ComponentType::Count), nullptr);
	};
}

/*
* TODO
if constexpr (T::Type == itaya::Engine::ComponentType::RigidBody)
{
	//component = Engine::CreateRigidBody(this, std::forward<Args>(args)...);
}
else if constexpr (T::Type == itaya::Engine::ComponentType::MeshRenderer)
{
	//component = Engine::CreateMeshRenderer(this, std::forward<Args>(args)...)
}
*/