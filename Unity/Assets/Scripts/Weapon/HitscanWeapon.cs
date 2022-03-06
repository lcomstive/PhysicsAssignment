using UnityEngine;
using LC.Hittable;

namespace LC.Weapons
{
	public class HitscanWeapon : Weapon
	{
		[SerializeField]
		private float m_HitscanRange = 100.0f;

		protected override void Shoot(Transform spawnTransform)
		{
			Transform cam = Camera.main.transform;
			if(!Physics.Raycast(cam.position, cam.forward, out RaycastHit hit, m_HitscanRange, 1, QueryTriggerInteraction.Ignore))
				return;

			IHittable hittable = hit.collider.GetComponent<IHittable>();
			if(hittable != null)
				hittable.Hit(spawnTransform.forward * m_Data.ForceApplied);
		}
	}
}