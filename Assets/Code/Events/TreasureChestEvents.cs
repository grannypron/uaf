using System.Collections;
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
        GameState.activeItemCollider = this.gameObject;
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        GameObject.Find("CombatMenuHandler").SendMessage("DeactivateMenuButton", new string[] { "btnOpenChest", "O" });
        GameState.activeItemCollider = null;
    }

    [System.Serializable]
    public class ChestItem
    {
        public string itemID;
        public int qty;
    }
}
