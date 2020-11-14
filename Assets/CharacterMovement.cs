using System.Collections;
using System.Collections.Generic;
using UnityEngine.Tilemaps;
using UnityEngine;
using System;

public class CharacterMovement : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.A) || Input.GetKeyDown(KeyCode.LeftArrow))
            movePlayer(-1, 0);
        else if (Input.GetKeyDown(KeyCode.D) || Input.GetKeyDown(KeyCode.RightArrow))
            movePlayer(1, 0);
        else if (Input.GetKeyDown(KeyCode.W) || Input.GetKeyDown(KeyCode.UpArrow))
            movePlayer(0, 1);
        else if (Input.GetKeyDown(KeyCode.S) || Input.GetKeyDown(KeyCode.DownArrow))
            movePlayer(0, -1);
        else if (Input.GetKeyDown(KeyCode.I))
            GameObject.Find("EventSystem").SendMessage("Inventory", null);

    }

    public void movePlayer(int xDir, int yDir)
    {
        GameObject.Find("EventSystem").SendMessage("playerModelMove", new int[] { xDir, yDir });
    }

}
