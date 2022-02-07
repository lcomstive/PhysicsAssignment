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

using namespace std::chrono_literals;

// Forward declarations
namespace Engine { class Application; }
namespace Engine::Components { struct Rigidbody; }

namespace Engine::Physics
{
	enum class PhysicsPlayState : int { Stopped = 0, Playing, Paused };
	
	struct CollisionFrame
	{
		Components::Collider* A;
		Components::Rigidbody* ARigidbody;

		Components::Collider* B;
		Components::Rigidbody* BRigidbody = nullptr;

		CollisionManifold Result;
	};

	class PhysicsSystem
	{
		std::thread m_Thread;
		std::mutex m_VariableMutex;
		std::mutex m_CollidersMutex;
		std::mutex m_PhysicsStateMutex;
		std::atomic_int m_ThreadState; // Corresponds to ThreadState
		std::condition_variable pauseConditional; // Notifies thread when to unpause

		Solver* m_Solver;
		OctreeNode* m_Octree;
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
		bool LineTest(OctreeNode* node, Line& line, Engine::Components::Collider* ignoreCollider);
		Components::Collider* Raycast(OctreeNode* node, Ray& ray, Engine::Components::Collider* ignoreCollider, RaycastHit* outResult);
		Components::Collider* FindClosest(std::vector<Components::Collider*>& set, Ray& ray, RaycastHit* outResult);

		std::vector<CollisionFrame> m_Collisions, m_PreviousCollisions;

		void RunSolver();
		void FindCollisions();
		
		friend struct Engine::Components::Collider;
		friend struct Engine::Components::Rigidbody;

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

	public:
		PhysicsSystem(Engine::Application* app, std::chrono::milliseconds fixedTimeStep = 50ms);
		~PhysicsSystem();

		void Start();
		void Stop();
		
		void DrawGizmos();

		void Resume();
		void Pause();
		void TogglePause();

		/// <summary>
		/// Generates a structure internally that speeds up queries & raycasts
		/// </summary>
		/// <returns>Acceleration success</returns>
		bool Accelerate(glm::vec3& position, float size, int maxRecursions = 5);

		std::chrono::milliseconds LastTimeStep();
		std::chrono::milliseconds Timestep();
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

		bool LineTest(Line line, Engine::Components::Collider* ignoreCollider);
		Engine::Components::Collider* Raycast(Ray ray, RaycastHit* outResult = nullptr);
		Engine::Components::Collider* Raycast(Ray ray, Engine::Components::Collider* ignoreCollider, RaycastHit* outResult = nullptr);
		std::vector<Engine::Components::Collider*> Query(Sphere& bounds);
		std::vector<Engine::Components::Collider*> Query(AABB& bounds);
	};
}
