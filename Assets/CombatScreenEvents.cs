using Jint;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Xml;
using UAFLib;
using Unity.Mathematics;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Tilemaps;
using UnityEngine.UI;

public class CombatScreenEvents : MonoBehaviour
{

    // Screen fits about 16 blocks high by 24 wide
    // player is placed in the middle of the battlefield at 25/25
    // I want draw them map such that the character starts in the center, my leftmost x index would be 25 - half of 24 = 13 and my topmost y index would be 25 - half of 16 = 17
    // Each square on the grid is 40x/y  Not sure why, but it is by eyeball

    private int PlayerScaleFactor = -1;
    private int BlockScaleFactor = 40;          //**TODO** - Idk why
    private int SCREEN_HORIZ_BLOCKS = 24;
    private int SCREEN_VERT_BLOCKS = 16;
    private const String CONFIG_FILE_URL = "https://raw.githubusercontent.com/grannypron/uaf/unity/config.xml";
    private Engine jintEngine;
    private ConsoleResults setupResults;
    private bool mapPainted = false;

    // Start is called before the first frame update
    void Start()
    {
        PlayerScaleFactor = (int)Math.Floor(GameObject.Find("Player").GetComponent<Transform>().localScale.x);
        jintEngine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
        StartCoroutine(InitFromServer());
    }

    IEnumerator InitFromServer()
    {

        UnityWebRequest configHttpReq = UnityWebRequest.Get(CONFIG_FILE_URL);
        yield return configHttpReq.SendWebRequest();

        if (configHttpReq.isNetworkError || configHttpReq.isHttpError) {
            Debug.Log(configHttpReq.error);
        } else {
            // Show results as text
            Debug.Log("Loaded config file from " + CONFIG_FILE_URL + ".  Length:" + configHttpReq.downloadHandler.text.Length);
        }

        XmlDocument configDoc = new XmlDocument();
        configDoc.LoadXml(configHttpReq.downloadHandler.text);

        String jsIndexUrl = configDoc.SelectNodes("//node()[local-name() = 'jsLibraryIndex']")[0].InnerText;
        String setupScriptUrl = configDoc.SelectNodes("//node()[local-name() = 'setupScript']")[0].InnerText;

        UnityWebRequest jsIndexReq = UnityWebRequest.Get(jsIndexUrl);
        yield return jsIndexReq.SendWebRequest();

        XmlDocument indexDoc = new XmlDocument();
        String indexAsStr = jsIndexReq.downloadHandler.text;
        if (indexAsStr[0] > 1000)
        {
            indexAsStr = indexAsStr.Substring(1);
        }
        indexDoc.LoadXml(indexAsStr);
        XmlNodeList nodes = indexDoc.SelectNodes("//node()[local-name() = 'ItemGroup']/node()[local-name() = 'Content']/@Include");
        List<String> jsURLs = new List<string>();
        // Save in list to alphabetize
        foreach (XmlNode node in nodes)
        {
            if (node.InnerText != null && node.InnerText.StartsWith("js\\"))
            {
                string jaUrl = "https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/" + node.InnerText.Replace("\\", "/");
                jsURLs.Add(jaUrl);
            }
        }

        jsURLs.Sort(new CaseInsensitiveStringComparer());


        foreach (string jsUrl in jsURLs)
        {
            Debug.Log("Loading " + jsUrl);
            UnityWebRequest jsReq = UnityWebRequest.Get(jsUrl);
            yield return jsReq.SendWebRequest();
            String fileContents = jsReq.downloadHandler.text;
            int lineCount = fileContents.Split('\n').Length;
            Debug.Log(jsUrl + " loaded.  Length: " + jsReq.downloadHandler.text.Length + ".  Lines: " + lineCount);
            jintEngine.Execute(fileContents);
            Debug.Log(jsUrl + " executed.");
        }

        Debug.Log("Loading setup script " + setupScriptUrl);
        UnityWebRequest setupScriptReq = UnityWebRequest.Get(setupScriptUrl);
        yield return setupScriptReq.SendWebRequest();
        String setupFileContents = setupScriptReq.downloadHandler.text;
        int setupFileLineCount = setupFileContents.Split('\n').Length;
        Debug.Log(setupScriptUrl + " loaded.  Length: " + setupScriptReq.downloadHandler.text.Length + ".  Lines: " + setupFileLineCount);
        setupResults = new ConsoleResults();
        jintEngine.SetValue("consoleResults", setupResults).Execute(setupFileContents);
        Debug.Log(setupScriptUrl + " executed.");

        Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
        txtLoading.enabled = false;
    }

    private void FixedUpdate()
    {
        if (!this.mapPainted) { 
            paintMap();
            this.mapPainted = true;
        }

    }

    void paintMap()
    {
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Tilemap terrainTilemap = GameObject.Find("TerrainTilemap").GetComponent<Tilemap>();
        Tile groundTile = (Tile)terrainTilemap.GetTile(new Vector3Int(-30, 11, 0));

        object[] mapData = (object[])setupResults.payload;

        for (int x = 0; x < mapData.Length; x++)
        {
            object[] column = (object[])mapData[x];
            for (int y = 0; y < column.Length; y++)
            {
                int cellValue = System.Int32.Parse(((object)column[y]).ToString());
                int[] coords = new int[] { x , y };
                if (cellValue < 0)
                {
                    //terrainTilemap.SetTile(new Vector3Int(translatedCoords[0], translatedCoords[1], zIndex), groundTile);// for now, just leave what I have painted on the map
                } else if (cellValue == 0)
                {
                    placePlayer(coords[0], coords[1]);
                } else if (cellValue > 0)
                {
                    placeMonster(coords[0], coords[1], "monster" + cellValue, RandoMonsterID());
                }
            }
        }

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

    private void placeMonster(int x, int y, string id, string monsterType)
    {
        Canvas canvas = GameObject.Find("Canvas").GetComponent<Canvas>();
        GameObject monsterGO = new GameObject("Monster" + id);
        SpriteRenderer renderer = monsterGO.AddComponent<SpriteRenderer>();
        Tile monsterTile = Resources.Load<Tile>("Sprites/Monsters/" + monsterType);
        renderer.sprite = monsterTile.sprite;
        Rigidbody2D monsterR2D = monsterGO.AddComponent<Rigidbody2D>();
        Transform monsterTransform = monsterGO.GetComponent<Transform>();
        BoxCollider2D boxCollider2D = monsterGO.AddComponent<BoxCollider2D>();
        monsterTransform.parent = canvas.transform;
        monsterTransform.localScale = new Vector3Int(PlayerScaleFactor, PlayerScaleFactor, 1);
        Vector2Int pos = gridToScreen(new Vector2Int(x, y));
        monsterTransform.localPosition = new Vector3Int(pos.x, pos.y, -2);
        monsterR2D.isKinematic = true;
        monsterR2D.mass = 5;
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

    public class CaseInsensitiveStringComparer : IComparer<string>
    {
        public int Compare(string x, string y)
        {
            return String.Compare(x.ToLower(), y.ToLower());
        }
    }

}
