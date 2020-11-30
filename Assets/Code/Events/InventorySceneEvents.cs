using System.Collections;
using System.Collections.Generic;
using UAFLib;
using UnityEngine;
using UnityEngine.UI;

public class InventorySceneEvents : MonoBehaviour
{

    bool dirty = true;
    bool menuNeedsRefresh = true;
    List<string> activeMenuButtons = new List<string>();

    // Start is called before the first frame update
    void Start()
    {
        this.dirty = true;
        activeMenuButtons.Add("btnExit");
    }

    // Update is called once per frame
    void Update()
    {
        if (dirty)
        {
            paint();
            dirty = false;
        }

        if (this.menuNeedsRefresh)
        {
            RefreshMenu();
            this.menuNeedsRefresh = false;
        }

        if (Input.GetKeyDown(KeyCode.X))
            Exit();
        if (Input.GetKeyDown(KeyCode.A))
            TakeAll();

    }
    public void paint()
    {
        paintPlayerInventory();
        paintOtherInventory();
    }

    public void paintPlayerInventory()
    {
        Text txtInventoryList = GameObject.Find("txtPlayerInventory").GetComponent<Text>();
        GameState.engine.Execute("consoleResults.payload = makeCharInventoryList(cWarrior);");
        txtInventoryList.text = "Your inventory: \n" + GameState.engineOutput.payload.ToString();
    }

    public void paintOtherInventory()
    {
        Text txtInventoryList = GameObject.Find("txtOtherInventory").GetComponent<Text>();
        GameState.engine.Execute("consoleResults.payload = makeInventoryList(" + Const.ENGINE_MANAGER_OTHER_INV_NAME + ");");
        object engineOutput = GameState.engineOutput.payload;
        if (engineOutput != null && engineOutput.ToString() != "") {
            txtInventoryList.text = "Other: \n" + engineOutput.ToString();
            activeMenuButtons.Add("btnTakeAll");
            this.menuNeedsRefresh = true;
        } else
        {
            txtInventoryList.text = "";
            activeMenuButtons.Remove("btnTakeAll");
        }
    }

    public void Exit()
    {
        UnityEngine.SceneManagement.SceneManager.LoadScene("CombatScene");
    }

    public void TakeAll()
    {
        // Update the engine
        GameState.engine.Execute("transferAllToPlayer(" + Const.ENGINE_MANAGER_OTHER_INV_NAME + ");");

        // Update the menu & the listing on the screen
        this.dirty = true;
        this.menuNeedsRefresh = true;

    }

    public void RefreshMenu()
    {
        Debug.Log("In RefreshMenu");
        HorizontalLayoutGroup menuPanel = GameObject.Find("MenuButtons").GetComponent<HorizontalLayoutGroup>();
        menuPanel.childScaleWidth = false;
        menuPanel.childScaleWidth = true;
        for (int idxMenuItem = 0; idxMenuItem < menuPanel.transform.childCount; idxMenuItem++)
        {
            Debug.Log("In RefreshMenu2");

            Transform transButton = menuPanel.transform.GetChild(idxMenuItem);
            RectTransform rectButton = transButton.GetComponent<RectTransform>();
            if (activeMenuButtons.Contains(transButton.name))
            {
                Debug.Log("In RefreshMenu3");

                rectButton.sizeDelta = new Vector2Int(100, 30);
                rectButton.localScale = new Vector3Int(1, 1, 1);
            }
            else
            {
                Debug.Log("In RefreshMenu4");

                rectButton.sizeDelta = new Vector2Int(0, 0);
                rectButton.localScale = new Vector3Int(0, 0, 0);
            }

        }
    }
}
