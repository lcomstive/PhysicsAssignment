#pragma once
#include <mutex>
#include <chrono>
#include <thread>
#include <glm/glm.hpp>
#include <unordered_map>
#include <Engine/Components/Physics/Collider.hpp>

using namespace std::chrono_literals;

// Forward declarations
namespace Engine { class Application; }
namespace Engine::Components { struct Rigidbody; }

namespace Engine::Physics
{
	class PhysicsSystem
	{
		struct PhysicsState
		{
			glm::vec3 Position;
			glm::vec3 Momentum;

			glm::vec3 Velocity;

			float Mass;
			float InverseMass;

			void Recalculate();
		};

		std::thread m_Thread;
		std::mutex m_VariableMutex;
		std::mutex m_CollidersMutex;
		std::atomic_bool m_KeepThreadAlive;

		std::chrono::milliseconds m_FixedTimestep;
		glm::vec3 m_Gravity = { 0, -9.81f, 0 };

		std::vector<Engine::Components::Collider*> m_Colliders;
		
		Engine::Application* m_App;

		void PhysicsLoop();

		void AddCollider(Engine::Components::Collider* collider);
		void RemoveCollider(Engine::Components::Collider* collider);

		friend struct Engine::Components::Collider;

	public:
		PhysicsSystem(Engine::Application* app, std::chrono::milliseconds fixedTimeStep = 50ms);

		void Start();
		void Stop();

		std::chrono::milliseconds Timestep();
		void SetTimestep(std::chrono::milliseconds timestep);

		void SetGravity(glm::vec3 gravity);
		glm::vec3 GetGravity();

		Engine::Components::Collider* Raycast(Ray& ray, RaycastHit* outResult = nullptr);
		std::vector<Engine::Components::Collider*> Query(Sphere& bounds);
		std::vector<Engine::Components::Collider*> Query(AABB& bounds);
	};
}