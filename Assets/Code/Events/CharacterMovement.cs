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
        if (GameState.allowInput) { 
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
    }

    public void movePlayer(int xDir, int yDir)
    {
        if (xDir < 0 || yDir > 0) { 
            GameObject.Find("Player").GetComponent<SpriteRenderer>().flipX = true;
        } else { 
            GameObject.Find("Player").GetComponent<SpriteRenderer>().flipX = false;
        }

        GameObject.Find("EventSystem").SendMessage("playerModelMove", new int[] { xDir, yDir });
    }

}
