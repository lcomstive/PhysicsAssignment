using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LC.Weapons
{
	[CreateAssetMenu(fileName = "Weapon", menuName = "Weapon Data")]
	public class WeaponData : ScriptableObject
	{
		public float Damage = 10.0f;

		[Tooltip("Amount of bullets held in gun storage, or < 0 for infinite")]
		public int ClipSize = -1;

		public AnimationClip ReloadAnimation = null;

		[Tooltip("Minimum time, in seconds, between shots being fired")]
		public float ShootCooldown = 0.2f;

		[Tooltip("Amount of force to apply to any shot object, in direction of bullet")]
		public float ForceApplied = 10.0f;
	}
}