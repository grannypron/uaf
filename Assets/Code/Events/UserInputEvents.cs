using System.Collections;
using System.Collections.Generic;
using UnityEngine.Tilemaps;
using UnityEngine;
using System;

public class UserInputEvents : MonoBehaviour
{
    public List<KeyCode> menuHotkeys = new List<KeyCode>(new KeyCode[] { KeyCode.I });  // Inventory is on by default
    private Dictionary<KeyCode, string> MENU_MESSAGES = new Dictionary<KeyCode, string>();

    // Start is called before the first frame update
    void Start()
    {
        MENU_MESSAGES.Add(KeyCode.I, "Inventory");
        MENU_MESSAGES.Add(KeyCode.O, "OpenChest");
    }

    // Update is called once per frame
    void Update()
    {
        if (GameState.allowInput)
        {
            if (Input.GetKeyDown(KeyCode.A) || Input.GetKeyDown(KeyCode.LeftArrow))
                movePlayer(-1, 0);
            else if (Input.GetKeyDown(KeyCode.D) || Input.GetKeyDown(KeyCode.RightArrow))
                movePlayer(1, 0);
            else if (Input.GetKeyDown(KeyCode.W) || Input.GetKeyDown(KeyCode.UpArrow))
                movePlayer(0, 1);
            else if (Input.GetKeyDown(KeyCode.S) || Input.GetKeyDown(KeyCode.DownArrow))
                movePlayer(0, -1);
            else
            {
                foreach (KeyCode key in menuHotkeys)
                {
                    if (Input.GetKeyDown(key))
                    {
                        GameObject.Find("EventSystem").SendMessage(MENU_MESSAGES[key], null);
                    }
                }

            }
        }
    }

    public void EnableHotkey(string key)
    {
        KeyCode keyCode = (KeyCode)System.Enum.Parse(typeof(KeyCode), key);
        if (!menuHotkeys.Contains(keyCode)) {
            this.menuHotkeys.Add(keyCode);
        }
    }

    public void DisableHotkey(string key)
    {
        KeyCode keyCode = (KeyCode)System.Enum.Parse(typeof(KeyCode), key);
        if (!menuHotkeys.Contains(keyCode))
        {
            this.menuHotkeys.Remove(keyCode);
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
