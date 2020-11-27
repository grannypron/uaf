using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class CombatMenuHandler : MonoBehaviour
{
    public List<string> activeMenuButtons = new List<string>();
    public bool needsRefresh = false;

    void Start()
    {
        this.activeMenuButtons.Add("btnInventory");
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


    public void RefreshMenu()
    {
        HorizontalLayoutGroup menuPanel = GameObject.Find("MenuButtons").GetComponent<HorizontalLayoutGroup>();
        menuPanel.childScaleWidth = false;
        menuPanel.childScaleWidth = true;
        for (int idxMenuItem = 0; idxMenuItem < menuPanel.transform.childCount; idxMenuItem++)
        {
            Transform transButton = menuPanel.transform.GetChild(idxMenuItem);
            RectTransform rectButton = transButton.GetComponent<RectTransform>();
            if (activeMenuButtons.Contains(transButton.name)) {
                rectButton.sizeDelta = new Vector2Int(120, 30);
                rectButton.localScale = new Vector3Int(1, 1, 1);
            }
            else {
                rectButton.sizeDelta = new Vector2Int(0, 0);
                rectButton.localScale = new Vector3Int(0, 0, 0);
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (this.needsRefresh)
        {
            RefreshMenu();
            this.needsRefresh = false;
        }

    }
}
