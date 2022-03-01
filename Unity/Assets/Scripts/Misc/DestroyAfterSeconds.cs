using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LC
{
	public class DestroyAfterSeconds : MonoBehaviour
	{
		[SerializeField] private float m_Seconds;

		private void Start() => Destroy(gameObject, m_Seconds);
	}
}