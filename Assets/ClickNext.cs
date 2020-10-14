using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ClickNext : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        GameObject.Find("Title").SetActive(true);
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonUp(0) || Input.GetMouseButtonUp(1) || Input.GetMouseButtonUp(2))
        {
            GameObject.Find("Title").SetActive(false);
            GameObject.Find("MenuScreenFrame").SetActive(true);
            Cursor.SetCursor((Texture2D)Resources.Load("cursor_Sword.png"), Vector2.zero, CursorMode.Auto);
        }
    }
}
