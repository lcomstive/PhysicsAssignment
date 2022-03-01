using UnityEngine;
using UnityEngine.InputSystem;

namespace LC.Weapons
{
	public abstract class Weapon : MonoBehaviour
	{
		[SerializeField]
		protected WeaponData Data;

		[SerializeField]
		private InputActionReference m_ShootInput;

		[SerializeField]
		private InputActionReference m_ReloadInput;

		[SerializeField]
		private Transform m_BarrelTip;

		[SerializeField]
		private int m_BulletsLeft;

		[SerializeField]
		private float m_ShootCooldown = 0.0f;

		protected virtual void Start()
		{
			/*
			m_ShootInput.action.performed += ShootInputPerformed;
			m_ReloadInput.action.performed += OnReloadPerformed;
			*/

			m_BulletsLeft = Data.ClipSize;
		}

		protected virtual void OnDestroy()
		{
			/*
			m_ShootInput.action.performed -= ShootInputPerformed;
			m_ReloadInput.action.performed -= OnReloadPerformed;
			*/
		}

		private void Update()
		{
			m_ShootCooldown = Mathf.Clamp(m_ShootCooldown - Time.deltaTime, 0.0f, Data.ShootCooldown);

			if(m_ShootInput.action.IsPressed()) Shoot();
			else if(m_ReloadInput.action.IsPressed()) Reload();
		}

		private void OnReloadPerformed(InputAction.CallbackContext _) => Reload();
		private void ShootInputPerformed(InputAction.CallbackContext _) => Shoot();

		public void Shoot()
		{
			if (m_ShootCooldown > 0.0f || m_BulletsLeft == 0)
				return;

			Shoot(m_BarrelTip);
			WeaponShot(this);

			m_ShootCooldown = Data.ShootCooldown;
			if (m_BulletsLeft > 0) m_BulletsLeft--;

			if (m_BulletsLeft == 0)
				Reload();
		}

		public void Reload()
		{
			// TODO: Play animation?
			m_BulletsLeft = Data.ClipSize;
		}

		public abstract void Shoot(Transform spawnTransform);

		public delegate void OnWeaponShot(Weapon weapon);
		public static event OnWeaponShot WeaponShot;
	}
}