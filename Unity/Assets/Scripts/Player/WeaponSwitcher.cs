using LC.Weapons;
using UnityEngine;
using System.Collections;
using UnityEngine.InputSystem;

namespace LC.Player
{
	public class WeaponSwitcher : MonoBehaviour
	{
		[SerializeField] private Weapon[] m_Weapons;

		[Header("Input")]
		[SerializeField] private InputActionReference m_WeaponChangeInput;

		private int m_WeaponIndex = 0;
		private bool m_CanSwitchWeapon = true;

		private void Start()
		{
			m_CanSwitchWeapon = true;
			foreach(Weapon weapon in m_Weapons)
				weapon.gameObject.SetActive(false);

			m_WeaponChangeInput.action.performed	+= OnWeaponChangePerformed;

			SelectWeapon(0);
		}

		private void OnDestroy() => m_WeaponChangeInput.action.performed += OnWeaponChangePerformed;

		private void OnWeaponChangePerformed(InputAction.CallbackContext obj) => SelectWeapon(m_WeaponIndex + (obj.ReadValue<float>() > 0 ? 1 : -1));

		public void SelectWeapon(int index)
		{
			if(!m_CanSwitchWeapon)
				return;
			StartCoroutine(DoSelectWeapon(index));
		}

		private IEnumerator DoSelectWeapon(int index)
		{
			m_CanSwitchWeapon = false;

			// PullOutWeapon in reverse, to put it away
			yield return PullOutWeapon(true);
			m_Weapons[m_WeaponIndex].gameObject.SetActive(false);

			// Assign new index
			m_WeaponIndex = index % m_Weapons.Length;
			if(m_WeaponIndex < 0)
				m_WeaponIndex += m_Weapons.Length;

			// Pull out new weapon and enable it
			m_Weapons[m_WeaponIndex].gameObject.SetActive(true);
			yield return PullOutWeapon();

			m_CanSwitchWeapon = true;
		}

		private IEnumerator PullOutWeapon(bool reverse = false)
		{
			Weapon weapon = m_Weapons[m_WeaponIndex];
			if(!weapon || !weapon.isActiveAndEnabled)
				yield return null; // Exit

			m_CanSwitchWeapon = false;
			Vector3 weaponEuler = weapon.transform.localRotation.eulerAngles;

			float time = 0.0f;
			float maxTime = Mathf.Min(weapon.Data.PulloutPositionCurve.keys[^1].time, weapon.Data.PulloutRotationCurve.keys[^1].time); // ^1 is last index of
			while (time < maxTime) 
			{
				float yValue = weapon.Data.PulloutPositionCurve.Evaluate(reverse ? (maxTime - time) : time);
				float rotation = weapon.Data.PulloutRotationCurve.Evaluate(reverse ? (maxTime - time) : time);
				weaponEuler.x = rotation;

				weapon.transform.localPosition = new Vector3(
					weapon.transform.localPosition.x,
					yValue,
					weapon.transform.localPosition.z
					);
				weapon.transform.localRotation = Quaternion.Euler(weaponEuler);

				yield return new WaitForEndOfFrame();
				time += Time.deltaTime;
			}

			m_CanSwitchWeapon = true;
		}

		public void NextWeapon() => SelectWeapon(m_WeaponIndex + 1);
		public void PreviousWeapon() => SelectWeapon(m_WeaponIndex - 1);
	}
}
