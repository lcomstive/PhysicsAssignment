using UnityEngine;
using UnityEngine.InputSystem;

namespace LC.Weapons
{
	public abstract class Weapon : MonoBehaviour
	{
		[SerializeField]
		private WeaponData Data;

		[SerializeField]
		private InputActionReference m_ShootInput;

		[SerializeField]
		private Transform m_BarrelTip;

		protected virtual void Start() => m_ShootInput.action.performed += ShootInputPerformed;
		protected virtual void OnDestroy() => m_ShootInput.action.performed -= ShootInputPerformed;

		private void ShootInputPerformed(InputAction.CallbackContext obj) => Shoot(m_BarrelTip.position);

		public abstract void Shoot(Vector3 spawnPosition);
	}
}