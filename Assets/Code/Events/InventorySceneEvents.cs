using System.Collections;
using System.Collections.Generic;
using UAFLib;
using UnityEngine;
using UnityEngine.UI;

public class InventorySceneEvents : MonoBehaviour
{

    bool dirty = true;
    // Start is called before the first frame update
    void Start()
    {
        this.dirty = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (dirty)
        {
            paint();
            dirty = false;
        }

        if (Input.GetKeyDown(KeyCode.X))
            Exit();

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
        txtInventoryList.text = "Other: \n" + GameState.engineOutput.payload.ToString();
    }

    public void Exit()
    {
        UnityEngine.SceneManagement.SceneManager.LoadScene("CombatScene");
    }
}
