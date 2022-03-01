using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LC.Weapons
{
	public class HitscanWeapon : Weapon
	{
		[SerializeField]
		private float m_HitscanRange = 100.0f;

		public override void Shoot(Vector3 spawnPosition)
		{

		}
	}
}