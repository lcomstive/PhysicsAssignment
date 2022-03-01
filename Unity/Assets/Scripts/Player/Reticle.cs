using System;
using UnityEngine;
using LC.Weapons;

namespace LC.Player
{
	public class Reticle : MonoBehaviour
	{
		[Serializable]
		public struct ReticleItem
		{
			public Transform Transform;
			public Vector3 MoveDirection;
		}

		[SerializeField] private float m_MoveSpeed = 1.0f;
		[SerializeField] private AnimationCurve m_SmoothingCurve;

		[SerializeField] private ReticleItem[] m_Reticles;

		private float m_TimeRemaining = 0.0f;

		private void Start() => Weapon.WeaponShot += OnWeaponShot;

		private void OnDestroy() => Weapon.WeaponShot -= OnWeaponShot;

		private void OnWeaponShot(Weapon weapon) => m_TimeRemaining = 1.0f;

		private void Update()
		{
			m_TimeRemaining = Mathf.Clamp(m_TimeRemaining - Time.deltaTime * m_MoveSpeed, 0.0f, 1.0f);
			float distance = m_SmoothingCurve.Evaluate(1.0f - m_TimeRemaining);

			foreach(ReticleItem reticle in m_Reticles)
			{
				reticle.MoveDirection.Normalize();
				reticle.Transform.position = reticle.Transform.parent.position + reticle.MoveDirection * distance;
			}
		}
	}
}