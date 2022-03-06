using UnityEngine;
using LC.Player;

namespace LC.Weapons
{
	public class ProjectileWeapon : Weapon
	{
		[SerializeField]
		private GameObject m_ProjectilePrefab;

		protected override void Shoot(Transform spawnTransform)
		{
			GameObject spawned = Instantiate(
				m_ProjectilePrefab,
				spawnTransform.position,
				spawnTransform.rotation * m_ProjectilePrefab.transform.rotation
				);
			
			Rigidbody rb = spawned.GetComponent<Rigidbody>();
			if(rb)
				rb.AddForce(spawnTransform.forward * m_Data.ForceApplied, ForceMode.Impulse);
		}
	}
}