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

public class CombatSceneEvents : MonoBehaviour, IUIListener
{

    // Screen fits about 16 blocks high by 24 wide
    // player is placed in the middle of the battlefield at 25/25

    public static int BlockScaleFactor = 40;          //**TODO** - Idk why
    private int PlayerScaleFactor = -1;
    private int centeringAdjustment = 25;
    private const String CONFIG_FILE_URL = "https://raw.githubusercontent.com/grannypron/uaf/unity/config.xml";
    private const String ITEMS_DATA_URL = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/data/items.xml";
    private Engine jintEngine;
    private ConsoleResults engineOutput;
    private bool mapPainted = false;
    private bool librariesLoaded = false;
    private bool registeredWalls = false;
    private Dictionary<int, String> monsters = new Dictionary<int, string>();
    private string CombatMessageSuffix = "";
    public const float MONSTER_MOVE_SECONDS = .5f;

    // Start is called before the first frame update
    void Start()
    {
        PlayerScaleFactor = (int)Math.Floor(GameObject.Find("Player").GetComponent<Transform>().localScale.x);
        Canvas canvas = GameObject.Find("Canvas").GetComponent<Canvas>();
        DontDestroyOnLoad(this.gameObject);
        Text txtCombatantInfo = GameObject.Find("txtCombatantInfo").GetComponent<Text>();
        txtCombatantInfo.text = "";
        if (GameState.engine != null)
        {
            this.jintEngine = GameState.engine;
            this.engineOutput = GameState.engineOutput;
            this.monsters = new Dictionary<int, string>();
            Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
            txtLoading.enabled = false;
            this.librariesLoaded = true;
            playerModelMove(new int[] { 0, 0 }); // little hack 🤷
        }
        else {
            jintEngine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly, typeof(UnityEngine.Debug).Assembly));
            /*

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

            UnityWebRequest itemDataHttpReq = UnityWebRequest.Get(ITEMS_DATA_URL);
            yield return itemDataHttpReq.SendWebRequest();
            XmlDocument itemDataDoc = new XmlDocument();
            itemDataDoc.LoadXml(itemDataHttpReq.downloadHandler.text);
            XmlDocument configDoc = new XmlDocument();
            configDoc.LoadXml(configHttpReq.downloadHandler.text);
            IEngineLoader loader = new GitHubEngineLoader();
            */

            XmlDocument configDoc = new XmlDocument();
            configDoc.LoadXml("<?xml version=\"1.0\" encoding=\"utf-8\"?><config><jsLibraryIndex>" + @"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\UAFLib.csproj</jsLibraryIndex><setupScript>C:\Users\Shadow\Desktop\uaf.git\uaf-unity\setup.js</setupScript></config>");
            IEngineLoader loader = new LocalEngineLoader(@"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\");
            XmlDocument itemDataDoc = new XmlDocument();
            itemDataDoc.Load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\items.xml");
            XmlDocument specAbsDataDoc = new XmlDocument();
            specAbsDataDoc.Load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\SpecialAbilities.xml");


            /*
            IEngineLoader loader = new ResourceEngineLoader("js", "setup");
            XmlDocument itemDataDoc = new XmlDocument();
            itemDataDoc.LoadXml(((TextAsset) Resources.Load("data/items")).text);
            XmlDocument specAbsDataDoc = new XmlDocument();
            specAbsDataDoc.LoadXml(((TextAsset)Resources.Load("data/SpecialAbilities")).text);
            XmlDocument configDoc = null;
            */

            UnityUAFEventManager unityUAFEventManager = new UnityUAFEventManager(this);

            this.engineOutput = new ConsoleResults();

            engineOutput.payload = new System.Object[] { new UAFLib.dataLoaders.ItemLoader().load(itemDataDoc), new UAFLib.dataLoaders.SpecabilityLoader().load(specAbsDataDoc) };
            jintEngine.SetValue("consoleResults", engineOutput).SetValue("unityUAFEventManager", unityUAFEventManager);

            StartCoroutine(loader.loadEngine(configDoc, jintEngine, unityUAFEventManager, delegate ()
            {
                Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
                txtLoading.enabled = false;
                this.librariesLoaded = true;
            }));
        }
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

        object[] returnData = (object[])getEngineData("packageMapAndCombatantStatus(combatData.m_aCombatants[0]);");
        object[] characterData = (object[])returnData[0];
        object[] mapData = (object[])returnData[1];

        for (int y = 0; y < mapData.Length; y++)
        {
            object[] column = (object[])mapData[y];
            for (int x = 0; x < column.Length; x++)
            {
                int cellValue = System.Int32.Parse(((object)column[x]).ToString());
                int[] coords = new int[] { x - centeringAdjustment, y - centeringAdjustment };
                if (cellValue < 0)
                {
                    if (!registeredWalls) {
                        // This is just temporary - I am populating the engine's grid from the tiles I drew in the Unity tilemap. It should go the other way around
                        if (x <= terrainTilemap.size.x && y <= terrainTilemap.size.y) {
                            // 25, 25 is the center of the tilemap
                            Tile thisTile = (Tile)terrainTilemap.GetTile(new Vector3Int(x - centeringAdjustment, y - centeringAdjustment, 0));
                            if (thisTile == null || thisTile.name != "OpenGroundTile") {
                                this.jintEngine.Execute("Drawtile.terrain[" + y + "][" + x + "].cell = -1;");
                            }
                        }
                    }
                    //terrainTilemap.SetTile(new Vector3Int(translatedCoords[0], translatedCoords[1], zIndex), groundTile);// for now, just leave what I have painted on the map
                }
                else if (cellValue == 0)
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
        registeredWalls = true;
        paintCharStatus(Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), characterData[2].ToString(), Int32.Parse(characterData[3].ToString()), Int32.Parse(characterData[4].ToString()), Int32.Parse(characterData[5].ToString()), Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), Int32.Parse(characterData[6].ToString()));

    }

    private void placePlayer(int x, int y)
    {
        Grid combatGrid = GameObject.Find("CombatGrid").GetComponent<Grid>();
        Rigidbody2D player = GameObject.Find("Player").GetComponent<Rigidbody2D>();
        Transform playerTransform = player.GetComponent<Transform>();
        playerTransform.parent = combatGrid.transform;
        playerTransform.localScale = new Vector3Int(PlayerScaleFactor, PlayerScaleFactor, 1);
        playerTransform.localPosition = new Vector3Int(x, y, -2);
    }

    private void newMonster(int id, string monsterType)
    {
        Grid combatGrid = GameObject.Find("CombatGrid").GetComponent<Grid>();
        GameObject monsterGO = new GameObject("Monster" + id);
        SpriteRenderer renderer = monsterGO.AddComponent<SpriteRenderer>();
        Tile monsterTile = Resources.Load<Tile>("Sprites/Monsters/" + monsterType);
        Transform monsterTransform = monsterGO.GetComponent<Transform>();
        monsterTransform.parent = combatGrid.transform;
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
        Transform monsterTransform = monsterGO.GetComponent<Transform>();
        monsterTransform.localPosition = new Vector3Int(x, y, -2);
    }

    // Update is called once per frame
    void Update()
    {
    }

    public void playerModelMove(int[] xy)
    {
        jintEngine.Execute("startRound();");
        jintEngine.Execute("cWarrior.MoveCombatant(cWarrior.x + " + xy[0] + ", cWarrior.y + " + xy[1] + ", false);");
        jintEngine.Execute("cWarrior.EndTurn();");
        GameState.allowInput = false;
        paintMap();
        int numCombatants = Int32.Parse(getEngineData("combatData.NumCombatants()").ToString());
        Debug.Assert(numCombatants == monsters.Count + 1); // +1 to count the player
        StartCoroutine(moveMonster());
    }

    public IEnumerator moveMonster()
    {
        if (GameState.monsterMoveIdx >= monsters.Count)
        {
            GameState.monsterMoveIdx = 0;
            GameState.allowInput = true;
            yield break;
        }
        else {
            GameState.monsterMoveIdx++;
            jintEngine.Execute("moveMonster(" + GameState.monsterMoveIdx + ");");
            paintMap();
            yield return new WaitForSeconds(MONSTER_MOVE_SECONDS);
            StartCoroutine(moveMonster());
        }
    }

    public object getEngineData(string expression)
    {
        jintEngine.Execute("consoleResults.payload = " + expression + ";");
        return (object)this.engineOutput.payload;
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
        txtCombatantInfo.text += "Cursor: " + y + ", " + x + "\n";   // x/y are flipped in combat engine position array
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

    public void Inventory()
    {
        UnityEngine.SceneManagement.SceneManager.LoadScene("ViewInventoryScene");
        GameState.engine = this.jintEngine;
        GameState.engineOutput = this.engineOutput;
    }
}
