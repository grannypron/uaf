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
        float moveTime = 0.1f;
        float inverseMoveTime = 1f / moveTime;
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Vector2 start = transform.position;
        Vector2 end = start + new Vector2(CombatScreenEvents.BlockScaleFactor * (xDir), CombatScreenEvents.BlockScaleFactor * (yDir));
        Vector3 newPosition = Vector3.MoveTowards(player.position, end, 20);
        player.MovePosition(newPosition);
        // Update the JS model
        int mapX = (int)Math.Round(this.GetComponent<Transform>().localPosition.x / CombatScreenEvents.BlockScaleFactor) + 25;
        int mapY = (int) Math.Round(this.GetComponent<Transform>().localPosition.y / CombatScreenEvents.BlockScaleFactor) + 25;
        GameObject.Find("EventSystem").SendMessage("playerModelMove", new int[] { mapX, mapY });

    }

}
