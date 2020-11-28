using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InventoryMenuHandler : MonoBehaviour
{
    public List<string> activeMenuButtons = new List<string>();
    public bool needsRefresh = false;

    void Start()
    {
        this.activeMenuButtons.Add("btnExit");
        this.needsRefresh = true;
    }

    public void ActivateMenuButton(string[] buttonAndKey)
    {
        string buttonName = buttonAndKey[0];
        string key = buttonAndKey[1];
        GameObject.Find("UserInputEvents").SendMessage("EnableHotkey", key);
        if (!activeMenuButtons.Contains(buttonName))
        {
            this.activeMenuButtons.Add(buttonName);
        }
        this.needsRefresh = true;
    }

    public void DeactivateMenuButton(string[] buttonAndKey)
    {
        string buttonName = buttonAndKey[0];
        string key = buttonAndKey[1];
        GameObject.Find("UserInputEvents").SendMessage("DisableHotkey", key);
        if (activeMenuButtons.Contains(buttonName))
        {
            this.activeMenuButtons.Remove(buttonName);
        }
        this.needsRefresh = true;
    }



    // Update is called once per frame
    void Update()
    {

    }
}
