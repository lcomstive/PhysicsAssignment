using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LC.Weapons
{
	public class ProjectileWeapon : Weapon
	{
		[SerializeField]
		private GameObject m_ProjectilePrefab;

		public override void Shoot(Transform spawnTransform)
		{
			GameObject spawned = Instantiate(
				m_ProjectilePrefab,
				spawnTransform.position,
				spawnTransform.rotation * m_ProjectilePrefab.transform.rotation
				);
			
			Rigidbody rb = spawned.GetComponent<Rigidbody>();
			if(rb)
				rb.AddForce(transform.forward * Data.ForceApplied, ForceMode.Impulse);
		}
	}
}