using System;
using System.Collections.Generic;
using UnityEngine;

public class TreasureChestEvents : MonoBehaviour
{
    public List<ChestItem> items = new List<ChestItem>();
    
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        GameObject.Find("CombatMenuHandler").SendMessage("ActivateMenuButton", new string[] { "btnOpenChest", "O" });
        try
        {
            // This check is in a try because we cannot test against gameObjects that are actually null vs those that have been
            // destroyed because the == operator is overloaded for GameObject.  If the GameObject is actually null, then it will
            // throw an NPE here.  If it is a destroyed GameObject, then that is the collider we were on before we left this scene,
            // so we need to refresh it
            // 
            if (GameState.activeItemCollider.GetInstanceID().GetType() != null)
            {
                // This is because we have returned from an interaction screen.  The original GameObject is now destroyed,
                // so we must refresh this object from the engine
                refreshThisObjectFromEngine();
            }
        } catch (System.Exception ex) { }
        GameState.activeItemCollider = this.gameObject;
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        GameObject.Find("CombatMenuHandler").SendMessage("DeactivateMenuButton", new string[] { "btnOpenChest", "O" });
        GameState.activeItemCollider = null;
    }

    private void refreshThisObjectFromEngine()
    {
        GameState.engineExecute("consoleResults.payload = itemListAsArray(" + Const.ENGINE_MANAGER_OTHER_INV_NAME + ");");
        this.items.Clear();
        object oPayload = GameState.engineOutput.payload;
        if (oPayload.GetType() != typeof(object[]))
        {
            Debug.LogWarning("Engine did not return expected object[] during treasure chest refresh");
        }
        foreach (object oItem in (object[])oPayload)
        {
            string id = ((object[])oItem)[0].ToString();
            int qty = Int32.Parse(((object[])oItem)[1].ToString());
            ChestItem chestItem = new ChestItem();
            chestItem.itemID = id;
            chestItem.qty = qty;
            this.items.Add(chestItem);
        }
    }

    [System.Serializable]
    public class ChestItem
    {
        public string itemID;
        public int qty;
    }
}
