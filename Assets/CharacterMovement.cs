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
        if (Input.GetKeyDown(KeyCode.A))
            movePlayer(-1, 0);
        if (Input.GetKeyDown(KeyCode.D))
            movePlayer(1, 0);
        if (Input.GetKeyDown(KeyCode.W))
            movePlayer(0, 1);
        else if (Input.GetKeyDown(KeyCode.S))
            movePlayer(0, -1);

    }

    public void movePlayer(int xDir, int yDir)
    {
        GameObject.Find("EventSystem").SendMessage("playerModelMove", new int[] { xDir, yDir });
    }

}
