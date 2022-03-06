using LC.Player;
using UnityEngine;
using UnityEngine.InputSystem;

namespace LC.Weapons
{
	public abstract class Weapon : MonoBehaviour
	{
		[SerializeField] protected WeaponData m_Data;
		[SerializeField] private Transform m_BarrelTip;
		[SerializeField] private bool ShowReticle = true;
		[SerializeField] private InputActionReference m_ShootInput;
		[SerializeField] private InputActionReference m_ReloadInput;

		public bool CanShoot = true;

		private int m_BulletsLeft;
		private float m_ShootCooldown = 0.0f;

		public WeaponData Data => m_Data; // Getter

		protected virtual void Start() => m_BulletsLeft = m_Data.ClipSize;

		protected virtual void OnEnable()
		{
			Reticle reticle = FindObjectOfType<Reticle>();
			if (reticle) reticle.enabled = ShowReticle;
		}

		protected virtual void Update()
		{
			m_ShootCooldown = Mathf.Clamp(m_ShootCooldown - Time.deltaTime, 0.0f, m_Data.ShootCooldown);

			if(m_ShootInput.action.IsPressed()) Shoot();
			else if(m_ReloadInput.action.IsPressed()) Reload();
		}

		public void Shoot()
		{
			if (m_ShootCooldown > 0.0f || m_BulletsLeft == 0 || !CanShoot)
				return;

			// Inherited function & events
			Shoot(m_BarrelTip);
			WeaponShot?.Invoke(this);

			// Recoil


			// Setting cooldown & bullets
			m_ShootCooldown = m_Data.ShootCooldown;
			if (m_BulletsLeft > 0) m_BulletsLeft--;

			if (m_BulletsLeft == 0)
				Reload();
		}

		public void Reload()
		{
			// TODO: Play animation?
			m_BulletsLeft = m_Data.ClipSize;
		}

		protected abstract void Shoot(Transform spawnTransform);

		public delegate void OnWeaponShot(Weapon weapon);
		public static event OnWeaponShot WeaponShot;
	}
}