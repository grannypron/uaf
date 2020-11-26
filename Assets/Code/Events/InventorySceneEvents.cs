using System.Collections;
using System.Collections.Generic;
using UAFLib;
using UnityEngine;
using UnityEngine.UI;

public class InventorySceneEvents : MonoBehaviour
{

    bool painted = false;
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (!painted)
        {
            paintInventory();
            painted = true;
        }

        if (Input.GetKeyDown(KeyCode.X))
            BackToCombat();

    }

    public void paintInventory()
    {
        Text txtInventoryList = GameObject.Find("txtInventoryList").GetComponent<Text>();
        GameState.engine.Execute("consoleResults.payload = makeInventoryList(cWarrior);");
        txtInventoryList.text = GameState.engineOutput.payload.ToString();
    }

    public void BackToCombat()
    {
        UnityEngine.SceneManagement.SceneManager.LoadScene("CombatScene");
    }
}
