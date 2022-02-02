#pragma once

namespace Engine
{
	class GameObject;

	namespace Components
	{
		struct Transform;
		struct Rigidbody;

		class Component
		{
			GameObject* m_GameObject;

			friend class Engine::GameObject;

		protected:
			/// <summary>
			/// Component has been added and initialised
			/// </summary>
			virtual void Added() { }

			/// <summary>
			/// Component is about to be removed from object
			/// </summary>
			virtual void Removed() { }

			virtual void Draw() { }
			virtual void DrawGizmos() { }
			virtual void Update() { }
			virtual void FixedUpdate(float timestep) { }

		public:
			Rigidbody* GetRigidbody() const;
			Transform* GetTransform() const;
			GameObject* GetGameObject() const;
		};
	}
}