using System.Collections;
using System.Collections.Generic;
using UAFLib;
using UnityEngine;
using UnityEngine.UI;

public class InventorySceneEvents : MonoBehaviour
{
    Jint.Engine engine;
    ConsoleResults engineOutput;

    bool painted = false;
    // Start is called before the first frame update
    void Start()
    {
        this.engine = GameState.engine;
        this.engineOutput = GameState.engineOutput;

    }

    // Update is called once per frame
    void Update()
    {
        if (!painted)
        {
            paintInventory();
            painted = true;
        }
    }

    public void paintInventory()
    {
        Text txtInventoryList = GameObject.Find("txtInventoryList").GetComponent<Text>();
        engine.Execute("consoleResults.payload = makeInventoryList(cWarrior);");
        txtInventoryList.text = engineOutput.payload.ToString();
    }
}
