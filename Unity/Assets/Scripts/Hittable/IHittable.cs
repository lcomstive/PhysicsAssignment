using UnityEngine;

namespace LC.Hittable
{
	public interface IHittable
	{
		public void Hit(Vector3 force);
	}
}