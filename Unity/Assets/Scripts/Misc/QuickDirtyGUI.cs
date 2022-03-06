using UnityEngine;
using UnityEngine.SceneManagement;

namespace LC
{
	public class QuickDirtyGUI : MonoBehaviour
	{
		private void CreateControlLabel(string keys, string description)
		{
			GUILayout.BeginHorizontal();
			{
				GUILayout.Label(keys);
				GUILayout.FlexibleSpace();
				GUILayout.Label(description);
			}
			GUILayout.EndHorizontal();
		}

		private void OnGUI()
		{
			GUILayout.BeginArea(new Rect(10, 10, 250, 200), new GUIStyle("Box"));
			{
				CreateControlLabel("WASD", "Movement");
				CreateControlLabel("Space", "Jump");
				CreateControlLabel("ESC / P", "Show/Hide Cursor");
				CreateControlLabel("Left Mouse", "Shoot");
				CreateControlLabel("Mouse Scroll", "Swap Weapon");

				GUILayout.FlexibleSpace();

				if(GUILayout.Button("Reset")) SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
				if(GUILayout.Button("Exit")) Application.Quit();
			}
			GUILayout.EndArea();
		}
	}
}