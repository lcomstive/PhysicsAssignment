using System;
using System.Linq;
using UnityEngine;
using System.Collections;

namespace LC
{
	public class MoveOnTrigger : MonoBehaviour
	{
		[SerializeField] private Vector3 m_Movement;
		[SerializeField] private Transform m_Target;
		[SerializeField] private string[] m_TriggerTags = { "Player" };
		[SerializeField] private AnimationCurve m_SpeedCurve;

		[SerializeField, Tooltip("How many times the movement can be triggered before this component is destroyed. <0 for infinite")]
		private int m_TimesBeforeDestroy = -1;

		private Vector3 m_InitialPosition;

		[Flags]
		public enum TriggerType
		{
			Enter = 1 << 1,
			Exit = 1 << 2,
			Stay = 1 << 3
		}

		public TriggerType Trigger;
		private Coroutine m_Coroutine = null;

		private void OnTriggerEnter(Collider other)
		{
			if (m_Coroutine == null &&
				m_TriggerTags.Contains(other.tag) &&
				Trigger.HasFlag(TriggerType.Enter))
				m_Coroutine = StartCoroutine(Execute());
		}

		private void OnTriggerExit(Collider other)
		{
			if (m_Coroutine == null &&
				m_TriggerTags.Contains(other.tag) &&
				Trigger.HasFlag(TriggerType.Exit))
				m_Coroutine = StartCoroutine(Execute());
		}

		private void OnTriggerStay(Collider other)
		{
			if (m_Coroutine == null &&
				m_TriggerTags.Contains(other.tag) &&
				Trigger.HasFlag(TriggerType.Stay))
				m_Coroutine = StartCoroutine(Execute());
		}

		private IEnumerator Execute()
		{
			m_InitialPosition = m_Target.localPosition;
			Vector3 finalPos = m_InitialPosition + m_Movement;

			float time = 0.0f;
			float maxTime = m_SpeedCurve.keys[^1].value; // ^1 is last index
			while (time <= maxTime)
			{
				m_Target.localPosition = Vector3.Lerp(m_InitialPosition, finalPos, m_SpeedCurve.Evaluate(time));

				yield return new WaitForEndOfFrame();
				time += Time.deltaTime;
			}

			m_Target.localPosition = finalPos;
			if(--m_TimesBeforeDestroy == 0)
				Destroy(this);
			m_Coroutine = null;
		}
	}
}