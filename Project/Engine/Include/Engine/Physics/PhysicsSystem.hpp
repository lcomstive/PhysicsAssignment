#pragma once
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <glm/glm.hpp>
#include <unordered_map>
#include <Engine/Log.hpp>
#include <Engine/Physics/Octree.hpp>
#include <Engine/Physics/Solver.hpp>
#include <Engine/Components/Physics/Collider.hpp>
#include <Engine/Physics/Broadphase/Broadphase.hpp>

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

		Solver* m_Solver;
		Broadphase* m_Broadphase;
		PhysicsPlayState m_PhysicsState;
		glm::vec3 m_Gravity = { 0, -9.81f, 0 };
		std::chrono::milliseconds m_LastTimeStep;
		std::chrono::milliseconds m_FixedTimestep;
		std::vector<Components::Collider*> m_Colliders;
		std::vector<Components::Rigidbody*> m_Rigidbodies;

		Engine::Application* m_App;

		void PhysicsLoop();

		void AddCollider(Components::Collider* collider);
		void RemoveCollider(Components::Collider* collider);

		void AddRigidbody(Components::Rigidbody* collider);
		void RemoveRigidbody(Components::Rigidbody* collider);
		
		std::vector<Components::Collider*> Query(OctreeNode* node, AABB& bounds);
		std::vector<Components::Collider*> Query(OctreeNode* node, Sphere& bounds);
		Components::Collider* Raycast(OctreeNode* node, Ray& ray, RaycastHit* outResult);
		bool LineTest(OctreeNode* node, Line& line, Components::Collider* ignoreCollider);
		Components::Collider* Raycast(OctreeNode* node, Ray& ray, Engine::Components::Collider* ignoreCollider, RaycastHit* outResult);
		Components::Collider* FindClosest(std::vector<Components::Collider*>& set, Ray& ray, RaycastHit* outResult);

		void RunSolver(std::vector<CollisionFrame>& collisions);
		void NarrowPhase(std::vector<CollisionFrame>& potentialCollisions);
		
		friend struct Components::Collider;
		friend struct Components::Rigidbody;

	protected:
		/// <summary>
		/// Impulse iterations per update.
		/// Typically 6-8 works well
		/// </summary>
		int m_ImpulseIteration;

		/// <summary>
		/// Physics iterations per update.
		/// Typically 6-8 works well
		/// </summary>
		int m_Substeps;

		/// <summary>
		/// Maximum depth of acceleration octree
		/// </summary>
		unsigned int m_MaxOctreeRecursions = 5;

	public:
		PhysicsSystem(Engine::Application* app, std::chrono::milliseconds fixedTimeStep = 50ms);
		~PhysicsSystem();

		void Start();
		void Stop();
		
		void DrawGizmos();

		void Resume();
		void Pause();
		void TogglePause();

		std::chrono::milliseconds Timestep();
		std::chrono::milliseconds LastTimeStep();
		void SetTimestep(std::chrono::milliseconds timestep);

		void SetGravity(glm::vec3 gravity);
		glm::vec3 GetGravity();

		PhysicsPlayState GetState();

		template<typename T, class... Args>
		T* SetSolver(Args... args)
		{
			Log::Assert(std::is_base_of<Solver, T>(), "Solver needs to have base class Engine::Physics::Solver");

			if (m_Solver)
				delete m_Solver;

			m_Solver = new T(args...);
			return (T*)m_Solver;
		}

		template<typename T, class... Args>
		T* SetBroadphase(Args... args)
		{
			Log::Assert(std::is_base_of<Broadphase, T>(), "Broadphase needs to have base class of Engine::Phyics::Broadphase");

			if (m_Broadphase)
				delete m_Broadphase;

			m_Broadphase = new T(args...);
			return (T*)m_Broadphase;
		}

		bool LineTest(Line line, Components::Collider* ignoreCollider);
		Components::Collider* Raycast(Ray ray, RaycastHit* outResult = nullptr);
		Components::Collider* Raycast(Ray ray, Components::Collider* ignoreCollider, RaycastHit* outResult = nullptr);
		std::vector<Components::Collider*> Query(Sphere& bounds);
		std::vector<Components::Collider*> Query(AABB& bounds);
	};
}
