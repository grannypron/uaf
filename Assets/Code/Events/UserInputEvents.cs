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
            if (Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow))
            {
                GameState.allowInput = false;
                movePlayer(-1, 0);
            }
            else if (Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow))
            {
                GameState.allowInput = false;
                movePlayer(1, 0);
            }
            else if (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow))
            {
                GameState.allowInput = false;
                movePlayer(0, 1);
            }
            else if (Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow))
            {
                GameState.allowInput = false;
                movePlayer(0, -1);
            }
            else
            {
                foreach (KeyCode key in menuHotkeys)
                {
                    if (Input.GetKey(key))
                    {
                        GameState.allowInput = false;
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
