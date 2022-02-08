using UnityEngine;
using UnityEngine.InputSystem;

namespace LC
{
	[RequireComponent(typeof(CharacterController))]
	public class PlayerMovement : MonoBehaviour
	{
		[SerializeField] private float m_MoveSpeed = 12.5f;
		[SerializeField] private float m_JumpHeight = 15.0f;

		[Header("Input Actions")]
		[SerializeField] private InputActionReference m_MovementMap;
		[SerializeField] private InputActionReference m_JumpMap;

		private bool m_JumpPressed = false;
		private Vector3 m_Velocity = Vector3.zero;
		private CharacterController m_Controller;

		private void Start()
		{
			m_Controller = GetComponent<CharacterController>();

			m_JumpMap.action.started += (e) => m_JumpPressed = true;
			m_JumpMap.action.canceled += (e) => m_JumpPressed = false;
		}

		private void FixedUpdate()
		{
			// Input
			Vector2 currentInput = m_MovementMap.action.ReadValue<Vector2>();
			Vector3 force = new Vector3(currentInput.x, 0, currentInput.y);
			force *= m_MoveSpeed;

			// Velocity
			m_Velocity += Physics.gravity * Time.fixedDeltaTime;

			if (m_Controller.isGrounded)
				m_Velocity.y = m_JumpPressed ? m_JumpHeight : 0;

			m_Controller.Move((m_Velocity + force) * Time.fixedDeltaTime);
		}
	}
}