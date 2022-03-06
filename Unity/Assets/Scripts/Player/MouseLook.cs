using UnityEngine;
using UnityEngine.InputSystem;

namespace LC.Player
{
	public class MouseLook : MonoBehaviour
	{
		public enum Axis { Horizontal, Vertical, Both }

		[SerializeField] private InputActionReference m_AimInput;
		[SerializeField] private InputActionReference m_PauseInput;
		[SerializeField] private Axis m_Axis = Axis.Horizontal;
		[SerializeField] private float m_Sensitivity = 10.0f;
		[SerializeField] private Vector2 m_Bounds = new Vector2(-180.0f, 180.0f);

		private void Start()
		{
			if (!m_PauseInput)
				return;
			m_PauseInput.action.started += OnPausePerformed;
			ShowCursor(false);
		}

		private void OnDestroy()
		{
			if (m_PauseInput)
				m_PauseInput.action.started -= OnPausePerformed;
		}

		private void OnPausePerformed(InputAction.CallbackContext _)
			=> ShowCursor(Cursor.lockState == CursorLockMode.Locked);

		public void ShowCursor(bool show = true)
		{
			Cursor.visible = show;
			Cursor.lockState = show ? CursorLockMode.None : CursorLockMode.Locked;
		}

		private void Update()
		{
			Vector3 euler = transform.eulerAngles;
			Vector2 aimDelta = m_AimInput.action.ReadValue<Vector2>() * Time.deltaTime * m_Sensitivity;

			if (m_Axis == Axis.Horizontal || m_Axis == Axis.Both)
			{
				if (euler.y > 180) euler.y -= 360.0f;
				if (euler.y < -180) euler.y += 360.0f;
				euler.y = Mathf.Clamp(euler.y + aimDelta.x, m_Bounds.x, m_Bounds.y);
			}
			if (m_Axis == Axis.Vertical || m_Axis == Axis.Both)
			{
				if (euler.x > 180) euler.x -= 360.0f;
				if (euler.x < -180) euler.x += 360.0f;
				euler.x = Mathf.Clamp(euler.x - aimDelta.y, m_Bounds.x, m_Bounds.y);
			}

			transform.eulerAngles = euler;
		}
	}
}