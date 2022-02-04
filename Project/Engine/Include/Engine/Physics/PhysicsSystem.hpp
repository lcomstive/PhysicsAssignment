#pragma once
#include <mutex>
#include <chrono>
#include <thread>
#include <glm/glm.hpp>
#include <unordered_map>
#include <Engine/Physics/Octree.hpp>
#include <Engine/Components/Physics/Collider.hpp>

using namespace std::chrono_literals;

// Forward declarations
namespace Engine { class Application; }
namespace Engine::Components { struct Rigidbody; }

namespace Engine::Physics
{
	enum class PhysicsPlayState : int { Stopped = 0, Playing, Paused };
	
	class PhysicsSystem
	{
		std::thread m_Thread;
		std::mutex m_VariableMutex;
		std::mutex m_CollidersMutex;
		std::mutex m_PhysicsStateMutex;
		std::atomic_int m_ThreadState; // Corresponds to ThreadState
		std::condition_variable pauseConditional; // Notifies thread when to unpause

		PhysicsPlayState m_PhysicsState;
		std::chrono::milliseconds m_FixedTimestep;
		glm::vec3 m_Gravity = { 0, -9.81f, 0 };

		OctreeNode* m_Octree;
		std::vector<Engine::Components::Collider*> m_Colliders;
		
		Engine::Application* m_App;

		void PhysicsLoop();

		void AddCollider(Components::Collider* collider);
		void RemoveCollider(Components::Collider* collider);
		
		std::vector<Components::Collider*> Query(OctreeNode* node, AABB& bounds);
		std::vector<Components::Collider*> Query(OctreeNode* node, Sphere& bounds);
		Components::Collider* Raycast(OctreeNode* node, Ray& ray, RaycastHit* outResult);
		bool LineTest(OctreeNode* node, Line& line, Engine::Components::Collider* ignoreCollider);
		Components::Collider* Raycast(OctreeNode* node, Ray& ray, Engine::Components::Collider* ignoreCollider, RaycastHit* outResult);
		Components::Collider* FindClosest(std::vector<Components::Collider*>& set, Ray& ray, RaycastHit* outResult);

		friend struct Engine::Components::Collider;

	protected:
		std::vector<Components::Rigidbody*> m_Colliders1;
		std::vector<Components::Rigidbody*> m_Colliders2;
		std::vector<CollisionManifold> m_CollisionResults;

		/// <summary>
		/// How much positional correction to apply,
		/// smaller values allow objects to penetrate more.
		/// Typically 0.2-0.8
		/// </summary>
		float m_LinearProjectionPercent;

		/// <summary>
		/// How much to allow objects to penetrate.
		/// The larger the number, the less jitter in the system.
		/// Range 0.01-0.1
		/// </summary>
		float m_PenetrationSlack;

		/// <summary>
		/// Physics iterations per update.
		/// Typically 6-8 works well
		/// </summary>
		int m_ImpulseIteration;

	public:
		PhysicsSystem(Engine::Application* app, std::chrono::milliseconds fixedTimeStep = 50ms);
		~PhysicsSystem();

		void Start();
		void Stop();

		void Resume();
		void Pause();
		void TogglePause();

		/// <summary>
		/// Generates a structure internally that speeds up queries & raycasts
		/// </summary>
		/// <returns>Acceleration success</returns>
		bool Accelerate(glm::vec3& position, float size, int maxRecursions = 5);

		std::chrono::milliseconds Timestep();
		void SetTimestep(std::chrono::milliseconds timestep);

		void SetGravity(glm::vec3 gravity);
		glm::vec3 GetGravity();

		PhysicsPlayState GetState();

		bool LineTest(Line line, Engine::Components::Collider* ignoreCollider);
		Engine::Components::Collider* Raycast(Ray& ray, RaycastHit* outResult = nullptr);
		Engine::Components::Collider* Raycast(Ray& ray, Engine::Components::Collider* ignoreCollider, RaycastHit* outResult = nullptr);
		std::vector<Engine::Components::Collider*> Query(Sphere& bounds);
		std::vector<Engine::Components::Collider*> Query(AABB& bounds);
	};
}