using Jint;
using Jint.Runtime;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using UAFLib;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Tilemaps;
using UnityEngine.UI;

public class CombatSceneEvents : MonoBehaviour, IUIListener
{

    // Screen fits about 16 blocks high by 24 wide
    // player is placed in the middle of the battlefield at 25/25

    public static int BlockScaleFactor = 40;          //**TODO** - Idk why
    public const float MONSTER_MOVE_SECONDS = .5f;    // How many seconds do we wait after the monster moves to show their status
    public const float DISTANCE_NOTICE_THRESHOLD = 170.0f;  // Non-inclusive
    private int PlayerScaleFactor = -1;
    private Vector2Int centeringAdjustment = new Vector2Int(-32, -25);
    private const String CONFIG_FILE_URL = "https://raw.githubusercontent.com/grannypron/uaf/unity/config.xml";
    private const String ITEMS_DATA_URL = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/data/items.xml";
    private bool firstMapPainted = false;
    private bool librariesLoaded = false;
    private bool registeredWalls = false;
    private int blockedTileCount = 0;
    private string CombatMessageSuffix = "";
    private GameObject goPlayer;

    // Start is called before the first frame update
    IEnumerator Start()
    {

        this.goPlayer = GameObject.Find("Player");
        PlayerScaleFactor = (int)Math.Floor(this.goPlayer.GetComponent<Transform>().localScale.x);
        togglePanel("pnlDead", false);
        DontDestroyOnLoad(this.gameObject);
        DontDestroyOnLoad(GameObject.Find("Canvas"));
        Text txtCombatantInfo = GameObject.Find("txtCombatantInfo").GetComponent<Text>();
        txtCombatantInfo.text = "";
        if (GameState.engine != null)
        {
            Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
            txtLoading.enabled = false;
            this.firstMapPainted = true;
            this.librariesLoaded = true;
            this.registeredWalls = false;
            playerModelMove(new int[] { 0, 0 }); // little hack 🤷
            yield break;
        }
        else
        {
            GameState.engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly, typeof(UnityEngine.Debug).Assembly));
            
            StartCoroutine(getLoader("resource", delegate ()
            {
                Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
                txtLoading.enabled = false;
                this.librariesLoaded = true;
            }));
            yield break;

            
            
        }
    }

    private void FixedUpdate()
    {
        if (!this.firstMapPainted & this.librariesLoaded)
        {
            scanAndDrawCombatants();
            randomizeAndPlaceMonsters();
            // This is done because the monsters' moves paint their status on the screen, so let's replace it with the character's
            paintCharStats();
            this.firstMapPainted = true;
            GameState.soundOn = true;
        }

    }

    private void paintCharStats()
    {
        object[] characterData = (object[])getEngineData("packageCombatantStatus(combatData.m_aCombatants[0])");
        paintStatus(Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), characterData[2].ToString(), Int32.Parse(characterData[3].ToString()), Int32.Parse(characterData[4].ToString()), Int32.Parse(characterData[5].ToString()), Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), Int32.Parse(characterData[6].ToString()), Int32.Parse(characterData[8].ToString()));

    }

    /** 
     * Scans the map and records blocked spaces from drawn map.  Also places players and monsters RigidBodys on map, 
     * creating the monsters if necessary 
     **/
    void scanAndDrawCombatants()
    {
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Tilemap terrainTilemap = GameObject.Find("TerrainTilemap").GetComponent<Tilemap>();
        object[] returnData = (object[])getEngineData("packageMapAndCombatantStatus(combatData.m_aCombatants[" + GameState.monsterMoveIdx + "]);");
        object[] characterData = (object[])returnData[0];
        object[] mapData = (object[])returnData[1];
        int tileIdx = 0; // Enumerates all tiles

        // Assume a square map
        GameState.mapDataWidth = mapData.Length;
        GameState.mapDataHeight = mapData.Length;

        int totalNumMapTiles = GameState.mapDataWidth * GameState.mapDataHeight;
        GameState.blockedSquares = new BitArray(totalNumMapTiles, false);

        for (int y = 0; y < GameState.mapDataWidth; y++)
        {
            object[] column = (object[])mapData[y];
            Debug.Assert(column.Length == GameState.mapDataHeight);
            Debug.Assert(column.Length == GameState.mapDataWidth);
            for (int x = 0; x < GameState.mapDataHeight; x++)
            {
                int cellValue = System.Int32.Parse(((object)column[x]).ToString());
                int[] coords = new int[] { x + centeringAdjustment.x, y + centeringAdjustment.y };
                if (cellValue == 0)
                {
                    placePlayer(coords[0], coords[1]);
                }
                else
                {
                    if (cellValue > 0)
                    {
                        if (!GameState.monsters.ContainsKey(cellValue))
                        {
                            string monsterID = RandoMonsterID();
                            newMonster(cellValue, monsterID);
                            GameState.monsters.Add(cellValue, monsterID);
                        }
                        placeMonster(coords[0], coords[1], cellValue);
                    }
                    if (!registeredWalls)
                    {
                        // This is just temporary - I am populating the engine's grid from the tiles I drew in the Unity tilemap. It should go the other way around
                        if (x <= terrainTilemap.size.x && y <= terrainTilemap.size.y)
                        {
                            // adjust for the center of the tilemap
                            Tile thisTile = (Tile)terrainTilemap.GetTile(new Vector3Int(x + centeringAdjustment.x, y + centeringAdjustment.y, 0));
                            if (thisTile == null || (thisTile.name != "OpenGroundTile" && thisTile.name != "stairs" && thisTile.name != "stonefloor"))
                            {
                                GameState.engineExecute("Drawtile.terrain[" + y + "][" + x + "].cell = -1;");
                                GameState.blockedSquares.Set(tileIdx, true);
                                blockedTileCount++;
                            }
                        }
                    }
                    // At this point, there may be a monster in a tile that is listed as blocked.  That is ok, they will
                    // be moved by randomizeAndPlaceMonsters()
                }
                tileIdx++;
            }
        }
        registeredWalls = true;
        paintStatus(Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), characterData[2].ToString(), Int32.Parse(characterData[3].ToString()), Int32.Parse(characterData[4].ToString()), Int32.Parse(characterData[5].ToString()), Int32.Parse(characterData[0].ToString()), Int32.Parse(characterData[1].ToString()), Int32.Parse(characterData[6].ToString()), Int32.Parse(characterData[8].ToString()));

    }

    /** Generates random positions for all monsters and places them on the map **/
    private void randomizeAndPlaceMonsters()
    {
        // Generate a number from 1 to the number of available cells.  One for each monster
        int totalNumMapTiles = GameState.mapDataWidth * GameState.mapDataHeight;  // Assume a square map
        BitArray monsterPositions = new BitArray(totalNumMapTiles - blockedTileCount, false);
        for (int idxMonster = 1; idxMonster < GameState.monsters.Count; idxMonster++)
        {
            monsterPositions.Set(UnityEngine.Random.Range(0, totalNumMapTiles - blockedTileCount), true);
        }

        List<int[]> positions = new List<int[]>();
        int monsterIdx = 0;       // will enumerate each monster id (+1 will be added to match the monster's id)
        int availableCellIdx = 0; // will enumerate each cell that is not closed
        int allCellIdx = 0;       // will enumerate all cells 
        for (int y = 0; y < GameState.mapDataWidth && monsterIdx < GameState.monsters.Count; y++)
        {
            for (int x = 0; x < GameState.mapDataHeight && monsterIdx < GameState.monsters.Count; x++)
            {
                if (!GameState.blockedSquares.Get(allCellIdx))
                {
                    // If there is a monster that is supposed to be here, place him
                    if (monsterPositions.Get(availableCellIdx))
                    {
                        ++monsterIdx;
                        GameState.engineExecute("combatData.m_aCombatants[" + monsterIdx + "].MoveCombatant(" + x + ", " + y + ", false);");
                        int[] coords = new int[] { x + centeringAdjustment.x, y + centeringAdjustment.y };
                        placeMonster(coords[0], coords[1], monsterIdx);
                    }
                    availableCellIdx++;
                }
                allCellIdx++;
            }
        }
    }

    /** Draw the player on the map **/
    private void placePlayer(int x, int y)
    {
        Grid combatGrid = GameObject.Find("CombatGrid").GetComponent<Grid>();
        Rigidbody2D player = this.goPlayer.GetComponent<Rigidbody2D>();
        Transform playerTransform = player.GetComponent<Transform>();
        playerTransform.parent = combatGrid.transform;
        playerTransform.localScale = new Vector3Int(PlayerScaleFactor, PlayerScaleFactor, 1);
        playerTransform.localPosition = new Vector3Int(x, y, -2);
    }

    /** Create a monster of a specific type and records their new random name / id in the engine **/
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
        GameState.engineExecute("combatData.m_aCombatants[" + id + "].m_pCharacter.name = '" + monsterType + "';combatData.m_aCombatants[" + id + "].m_pCharacter.monsterID = '" + monsterType + "';");
    }

    /** Places a monster on the map **/
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

    /** Move the player in the engine **/
    public void playerModelMove(int[] xy)
    {
        GameState.engineExecute("cWarrior.m_isCombatReady = 1");
        GameState.engineExecute("cWarrior.MoveCombatant(cWarrior.x + " + xy[0] + ", cWarrior.y + " + xy[1] + ", false);");
        GameState.engineExecute("cWarrior.EndTurn();");
        GameState.allowInput = false;
        scanAndDrawCombatants();
        int numCombatants = Int32.Parse(getEngineData("combatData.NumCombatants()").ToString());
        Debug.Assert(numCombatants == GameState.monsters.Count + 1); // +1 to count the player
        StartCoroutine(moveMonster());
    }

    /** Called after all monsters have moved to start a new round in the engine **/
    private void endMonsterMoves()
    {
        GameState.engineExecute("startRound();");
        paintCharStats();
        GameState.allowInput = true;
    }

    /** Moves a monster in the engine - recursive to call all monsters **/
    public IEnumerator moveMonster()
    {
        if (GameState.monsterMoveIdx >= GameState.monsters.Count)
        {
            yield return new WaitForSeconds(MONSTER_MOVE_SECONDS);
            GameState.monsterMoveIdx = 0;
            endMonsterMoves();
            yield break;
        }
        else {
            //SpriteRenderer player = this.goPlayer.GetComponent<SpriteRenderer>();
            //if (player.sprite.name != "icon_PC_FighterMale_0") { 
            //    player.sprite = ((Tile)Resources.Load<Tile>("Sprites/icon_PC_FighterMale_0")).sprite;
            //}
            GameState.monsterMoveIdx++;
            if (GameState.deadMonsters.IndexOf(GameState.monsterMoveIdx) < 0)
            {
                if (monsterNoticesPlayer(GameState.monsterMoveIdx))
                {
                    yield return new WaitForSeconds(MONSTER_MOVE_SECONDS);
                    GameState.engineExecute("moveMonster(" + GameState.monsterMoveIdx + ");");
                    scanAndDrawCombatants();   // Don't wait/paint for dead monsters
                }
            }
            StartCoroutine(moveMonster());
        }
    }

    // Determines if a monster should get a turn to move or not
    private bool monsterNoticesPlayer(int id)
    {
        GameObject goMonster = GameObject.Find("Monster" + id);
        Debug.Assert(goMonster != null, "monster with id " + id + " is null!  Already dead?");
        float dist = Vector2.Distance(goMonster.GetComponent<Rigidbody2D>().position, this.goPlayer.GetComponent<Rigidbody2D>().position);
        return dist < DISTANCE_NOTICE_THRESHOLD;
    }

    public object getEngineData(string expression)
    {
        GameState.engineExecute("consoleResults.payload = " + expression + ";");
        return (object)GameState.engineOutput.payload;
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

    void paintStatus(int x, int y, string name, int hp, int ac, int attacks, int cursorX, int cursorY, int movesLeft, int xp)
    {
        Text txtCombatantInfo = GameObject.Find("txtCombatantInfo").GetComponent<Text>();
        txtCombatantInfo.enabled = true;
        txtCombatantInfo.text = name + " (0:" + x + "," + y + ")\n";
        txtCombatantInfo.text += "HITPOINTS  " + hp + "\n";
        txtCombatantInfo.text += "AC   " + ac + "\n";
        txtCombatantInfo.text += "XP   " + xp + "\n";
        //txtCombatantInfo.text += "Attacks: " + attacks + "\n";
        //txtCombatantInfo.text += "Cursor: " + y + ", " + x + "\n";   // x/y are flipped in combat engine position array
        //txtCombatantInfo.text += "Moves Left: " + movesLeft + "\n";
    }

    public void playerAttack(int attacker, int attacked)
    {
        //***TODO***:  Not the best plan to use the UI events to trigger model events.  This is done to bridge the gap between StartAttack and makeAttack since there is no sequencing of these events with a general event queue
        try
        {
            //SpriteRenderer player = this.goPlayer.GetComponent<SpriteRenderer>();
            //player.sprite = ((Sprite)Resources.Load<Sprite>("Sprites/PlayerAttack")).sprite;
            GameState.engineExecute("combatData.m_aCombatants[" + attacker + "].makeAttack(" + attacked + ", 0, -1);");
        } catch (JavaScriptException ex)
        {
            Debug.LogException(ex);
            Debug.Log("Line:" + ex.LineNumber);
        }
    }

    public void combatantDying(int id, int x, int y)
    {
        if (id == 0) {
            gameOver();
        } else { 
            this.CombatMessageSuffix = " and kills!";   // To be added on when the CombatMsg is printed ¯\(ツ)/¯
            GameState.deadMonsters.Add(id);
            this.RemoveCombatant(id);
        }
    }

    private void gameOver()
    {
        GameState.monsterMoveIdx = int.MaxValue;
        togglePanel("pnlDead", true);
        GameState.allowInput = false;
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
        switch (eventName) {
            case "UpdateCombatMessage":
                GameObject.Find("txtCombatMessage").GetComponent<Text>().text = (string)data + this.CombatMessageSuffix;
                this.CombatMessageSuffix = "";
                break;
            case "CombatantMoved":
                object[] aMove = (object[])data;
                paintStatus((int)aMove[0], (int)aMove[1], (string)aMove[2], (int)aMove[3], (int)aMove[4], (int)aMove[5], (int)aMove[0], (int)aMove[1], (int)aMove[6], (int)aMove[7]);
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
            case "PlaySound":
                string soundName = data.ToString();
                playSound(soundName);
                break;
            default:
                break;
        }
    }

    public void Inventory()
    {
        UnityEngine.SceneManagement.SceneManager.LoadScene("ViewInventoryScene");
    }

    private void togglePanel(string panelName, bool on)
    {
        // Just make it disappear by shrinking the y scale to 0
        if (on) { 
            GameObject.Find(panelName).GetComponent<RectTransform>().localScale = new Vector3Int(1, 1, 0);
        } else {
            GameObject.Find(panelName).GetComponent<RectTransform>().localScale = new Vector3Int(1, 0, 0);
        }

    }

    private void playSound(string soundName)
    {
        if (!GameState.soundOn)
        {
            return;
        }
        AudioClip clip = Resources.Load<AudioClip>("Sounds/" + soundName);
        if (clip == null)
        {
            Debug.Log("Sound could not be loaded: " + soundName);
        }
        GameObject.Find("Canvas").GetComponent<AudioSource>().PlayOneShot(clip);
    }

    private IEnumerator getLoader(string type, InitComplete complete)
    {
        XmlDocument itemDataDoc;
        XmlDocument configDoc;
        XmlDocument specAbsDataDoc;
        IEngineLoader loader;
        if (type == "github")
        {
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
            itemDataDoc = new XmlDocument();
            itemDataDoc.LoadXml(itemDataHttpReq.downloadHandler.text);
            configDoc = new XmlDocument();
            configDoc.LoadXml(configHttpReq.downloadHandler.text);
            loader = new GitHubEngineLoader();
            specAbsDataDoc = new XmlDocument();
        }
        else if (type == "local") {

            configDoc = new XmlDocument();
            configDoc.LoadXml("<?xml version=\"1.0\" encoding=\"utf-8\"?><config><jsLibraryIndex>" + @"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\UAFLib.csproj</jsLibraryIndex><setupScript>C:\Users\Shadow\Desktop\uaf.git\uaf-unity\engineManager.js</setupScript></config>");
            loader = new LocalEngineLoader(@"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\");
            itemDataDoc = new XmlDocument();
            itemDataDoc.Load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\items.xml");
            specAbsDataDoc = new XmlDocument();
            specAbsDataDoc.Load("C:\\Users\\Shadow\\Desktop\\uaf.git\\uaf-port\\src\\UAFLib\\data\\SpecialAbilities.xml");
        } else
        {

            loader = new ResourceEngineLoader("js", "engineManager");
            itemDataDoc = new XmlDocument();
            itemDataDoc.LoadXml(((TextAsset)Resources.Load("data/items")).text);
            specAbsDataDoc = new XmlDocument();
            specAbsDataDoc.LoadXml(((TextAsset)Resources.Load("data/SpecialAbilities")).text);
            configDoc = null;
        }

        UnityUAFEventManager unityUAFEventManager = new UnityUAFEventManager(this);

        GameState.engineOutput.payload = new System.Object[] { new UAFLib.dataLoaders.ItemLoader().load(itemDataDoc), new UAFLib.dataLoaders.SpecabilityLoader().load(specAbsDataDoc) };
        GameState.engine.SetValue("consoleResults", GameState.engineOutput).SetValue("unityUAFEventManager", unityUAFEventManager);
        loader.loadEngine(configDoc, GameState.engine, unityUAFEventManager, complete);
    }
}

