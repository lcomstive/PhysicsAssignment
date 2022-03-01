using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LC.Hittable
{
	[RequireComponent(typeof(Rigidbody))]
	public class RigidbodyHittable : MonoBehaviour, IHittable
	{
		[SerializeField] private bool m_Unfreeze = false;

		private Rigidbody m_RigidBody;

		private void Awake() => m_RigidBody = GetComponent<Rigidbody>();

		public void Hit(Vector3 force)
		{
			if(!m_RigidBody)
				return;

			m_RigidBody.AddForce(force, ForceMode.Impulse);

			if(m_Unfreeze)
			{
				m_RigidBody.freezeRotation = false;
				m_RigidBody.useGravity = true;
				m_RigidBody.isKinematic = false;
			}
		}
	}
}