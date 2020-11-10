using Jint;
using Jint.Runtime;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using UAFLib;
using Unity.Mathematics;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Tilemaps;
using UnityEngine.UI;

public class CombatScreenEvents : MonoBehaviour, IUIListener
{

    // Screen fits about 16 blocks high by 24 wide
    // player is placed in the middle of the battlefield at 25/25

    public static int BlockScaleFactor = 40;          //**TODO** - Idk why
    private int PlayerScaleFactor = -1;
    private const String CONFIG_FILE_URL = "https://raw.githubusercontent.com/grannypron/uaf/unity/config.xml";
    private Engine jintEngine;
    private ConsoleResults setupResults;
    private bool mapPainted = false;
    private bool librariesLoaded = false;
    private Dictionary<int, String> monsters = new Dictionary<int, string>();
    private string CombatMessageSuffix = "";

    // Start is called before the first frame update
    IEnumerator Start()
    {
        PlayerScaleFactor = (int)Math.Floor(GameObject.Find("Player").GetComponent<Transform>().localScale.x);
        jintEngine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly, typeof(UnityEngine.Debug).Assembly));

        
        UnityWebRequest configHttpReq = UnityWebRequest.Get(CONFIG_FILE_URL);
        yield return configHttpReq.SendWebRequest();

        if (configHttpReq.isNetworkError || configHttpReq.isHttpError)
        {
            Debug.Log(configHttpReq.error);
        }
        else
        {
            // Show results as text
            Debug.Log("Loaded config file from " + CONFIG_FILE_URL + ".  Length:" + configHttpReq.downloadHandler.text.Length);
        }
        
        XmlDocument configDoc = new XmlDocument();
        configDoc.LoadXml(configHttpReq.downloadHandler.text);
        IEngineLoader loader = new GitHubEngineLoader();
        //XmlDocument configDoc = new XmlDocument();
        //configDoc.LoadXml("<?xml version=\"1.0\" encoding=\"utf-8\"?><config><jsLibraryIndex>" + @"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\UAFLib.csproj</jsLibraryIndex><setupScript>C:\Users\Shadow\Desktop\uaf.git\uaf-unity\setup.js</setupScript></config>");
        //IEngineLoader loader = new LocalEngineLoader(@"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\");

        UnityUAFEventManager unityUAFEventManager = new UnityUAFEventManager(this);

        StartCoroutine(loader.loadEngine(configDoc, jintEngine, unityUAFEventManager, delegate (ConsoleResults setupResults)
        {
            Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
            txtLoading.enabled = false;
            this.setupResults = setupResults;
            this.librariesLoaded = true;
        }));

    }

    private void FixedUpdate()
    {
        if (!this.mapPainted & this.librariesLoaded) { 
            paintMap();
            this.mapPainted = true;
        }

    }

    void paintMap()
    {
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Tilemap terrainTilemap = GameObject.Find("TerrainTilemap").GetComponent<Tilemap>();
        Tile groundTile = (Tile)terrainTilemap.GetTile(new Vector3Int(-30, 11, 0));

        object[] returnData = (object[])setupResults.payload;
        object[] characterData = (object[])returnData[0];
        object[] mapData = (object[])returnData[1];

        for (int y = 0; y < mapData.Length; y++)
        {
            object[] column = (object[])mapData[y];
            for (int x = 0; x < column.Length; x++)
            {
                int cellValue = System.Int32.Parse(((object)column[x]).ToString());
                int[] coords = new int[] { x , y };
                if (cellValue < 0)
                {
                    //terrainTilemap.SetTile(new Vector3Int(translatedCoords[0], translatedCoords[1], zIndex), groundTile);// for now, just leave what I have painted on the map
                } else if (cellValue == 0)
                {
                    placePlayer(coords[0], coords[1]);
                } else if (cellValue > 0)
                {
                    if (!monsters.ContainsKey(cellValue))
                    {
                        string monsterID = RandoMonsterID();
                        newMonster(cellValue, monsterID);
                        monsters.Add(cellValue, monsterID);
                    }
                    placeMonster(coords[0], coords[1], cellValue);
                }
            }
        }

        paintCharStatus(Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), characterData[2].ToString(), Int32.Parse(characterData[3].ToString()), Int32.Parse(characterData[4].ToString()), Int32.Parse(characterData[5].ToString()), Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), Int32.Parse(characterData[6].ToString()));
    }

    private void placePlayer(int x, int y)
    {
        Canvas canvas = GameObject.Find("Canvas").GetComponent<Canvas>();
        Rigidbody2D player = GameObject.Find("Player").GetComponent<Rigidbody2D>();
        Transform playerTransform = player.GetComponent<Transform>();
        playerTransform.parent = canvas.transform;
        playerTransform.localScale = new Vector3Int(PlayerScaleFactor, PlayerScaleFactor, 1);
        Vector2Int pos = gridToScreen(new Vector2Int(x, y));
        playerTransform.localPosition = new Vector3Int(pos.x, pos.y, -2);
    }

    private void newMonster(int id, string monsterType)
    {
        Canvas canvas = GameObject.Find("Canvas").GetComponent<Canvas>();
        GameObject monsterGO = new GameObject("Monster" + id);
        SpriteRenderer renderer = monsterGO.AddComponent<SpriteRenderer>();
        Tile monsterTile = Resources.Load<Tile>("Sprites/Monsters/" + monsterType);
        Transform monsterTransform = monsterGO.GetComponent<Transform>();
        monsterTransform.parent = canvas.transform;
        monsterTransform.localScale = new Vector3Int(PlayerScaleFactor, PlayerScaleFactor, 1);
        renderer.sprite = monsterTile.sprite;
        Rigidbody2D monsterR2D = monsterGO.AddComponent<Rigidbody2D>();
        BoxCollider2D boxCollider2D = monsterGO.AddComponent<BoxCollider2D>();
        monsterR2D.isKinematic = true;
        monsterR2D.mass = 5;
    }

    void placeMonster(int x, int y, int id) {
        Canvas canvas = GameObject.Find("Canvas").GetComponent<Canvas>();
        GameObject monsterGO = GameObject.Find("Monster" + id);
        Vector2Int pos = gridToScreen(new Vector2Int(x, y));
        Transform monsterTransform = monsterGO.GetComponent<Transform>();
        monsterTransform.localPosition = new Vector3Int(pos.x, pos.y, -2);
    }

    private Vector2Int gridToScreen(Vector2Int v)
    {
        // 25x25 is the center of the combat - Anyone at position 25/25 should have x/y of 0/0
        int xOffset = -25;
        int yOffset = -25;
        return new Vector2Int(BlockScaleFactor * (v.x + xOffset), BlockScaleFactor * (v.y + yOffset));

    }

    // Update is called once per frame
    void Update()
    {

    }

    public void playerModelMove(int[] xy)
    {
        jintEngine.Execute("cWarrior.MoveCombatant(cWarrior.x + " + xy[0] + ", cWarrior.y + " + xy[1] + ", false); consoleResults.payload = packageMapAndCombatantStatus(combatData.m_aCombatants[0]);");
        object[] data = (object[])this.setupResults.payload;
        object[] characterData = (object[])data[0];
        paintMap();
    }


    string RandoMonsterID()
    {
        string[] monsters = getMonsters();
        return monsters[UnityEngine.Random.Range(0, monsters.Length)];
    }

    string[] getMonsters()
    {
        return new string[] { "FireGiant", "GnomeWarrior", "Kobold", "Lich", "Mummy", "Myconid", "PC_ThiefMale", "PurpleWorm" };
        //"CopperDragon", "GiantCrocodile", 
    }

    void paintCharStatus(int x, int y, string name, int hp, int ac, int attacks, int cursorX, int cursorY, int movesLeft)
    {
        Text txtCombatantInfo = GameObject.Find("txtCombatantInfo").GetComponent<Text>();
        txtCombatantInfo.enabled = true;
        txtCombatantInfo.text = name + " (0:" + x + "," + y + ")\n";
        txtCombatantInfo.text += "HITPOINTS  " + hp + "\n";
        txtCombatantInfo.text += "AC   " + ac + "\n";
        txtCombatantInfo.text += "Attacks: " + attacks + "\n";
        txtCombatantInfo.text += "Cursor: " + x + ", " + y + "\n";
        txtCombatantInfo.text += "Moves Left: " + movesLeft + "\n";
    }

    public void playerAttack(int attacker, int attacked)
    {
        //***TODO***:  Not the best plan to use the UI events to trigger model events.  This is done to bridge the gap between StartAttack and makeAttack since there is no sequencing of these events with a general event queue
        try
        {
            jintEngine.Execute("var deathIndex = []; combatData.m_aCombatants[" + attacker + "].makeAttack(" + attacked + ", 0, deathIndex);");
        } catch (JavaScriptException ex)
        {
            Debug.LogException(ex);
            Debug.Log("Line:" + ex.LineNumber);
        }
    }

    public void combatantDying(int id, int x, int y)
    {
        this.CombatMessageSuffix = " and kills!";   // To be added on when the CombatMsg is printed ¯\(ツ)/¯
        this.RemoveCombatant(id);
    }

    public void RemoveCombatant(int id)
    {
        Debug.Log("Destroying Monster" + id);
        GameObject monsterGO = GameObject.Find("Monster" + id);
        monsterGO.GetComponent<SpriteRenderer>().sprite = ((Tile)Resources.Load<Tile>("Sprites/CombatDeath")).sprite;
        GameObject.Destroy(monsterGO, 2);
    }

    public void OnEvent(string eventName, object data)
    {
        ;
        switch (eventName) {
            case "UpdateCombatMessage":
                GameObject.Find("txtCombatMessage").GetComponent<Text>().text = (string)data + this.CombatMessageSuffix;
                this.CombatMessageSuffix = "";
                break;
            case "CombatantMoved":
                object[] aMove = (object[])data;
                paintCharStatus((int)aMove[0], (int)aMove[1], (string)aMove[2], (int)aMove[3], (int)aMove[4], (int)aMove[5], (int)aMove[0], (int)aMove[1], (int)aMove[6]);
                break;
            case "StartAttack":
                int[] aAttack = (int[])data;
                GameObject.Find("txtCombatMessage").GetComponent<Text>().text = "Combatant " + aAttack[0] + " attacks Combatant " + aAttack[1];
                playerAttack(aAttack[0], aAttack[1]);
                break;
            case "CombatantDying":
                int[] aDying = (int[])data;
                combatantDying(aDying[0], aDying[1], aDying[2]);
                break;
            case "CombatantDead":
                int[] aDead = (int[])data;
                combatantDying(aDead[0], aDead[1], aDead[2]);
                break;
            default:
                break;
        }
    }

}
