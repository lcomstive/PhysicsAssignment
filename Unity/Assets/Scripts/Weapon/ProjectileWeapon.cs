using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LC.Weapons
{
	public class ProjectileWeapon : Weapon
	{
		[SerializeField]
		private GameObject m_ProjectilePrefab;

		[SerializeField]
		private float m_ProjectileSpeed = 25.0f;

		public override void Shoot(Vector3 spawnPosition)
		{
			GameObject spawned = Instantiate(m_ProjectilePrefab, spawnPosition, Quaternion.identity);
			
			Rigidbody rb = spawned.GetComponent<Rigidbody>();
			if(rb)
				rb.AddForce(transform.forward * m_ProjectileSpeed, ForceMode.Impulse);
		}
	}
}