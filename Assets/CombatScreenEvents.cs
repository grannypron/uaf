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
    private const String CONFIG_FILE_URL = "https://raw.githubusercontent.com/grannypron/uaf/unity/config.xml";
    private Engine jintEngine;
    private ConsoleResults setupResults;

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

        jsURLs.Sort();


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

        paintMap();
        Text txtLoading = GameObject.Find("txtLoading").GetComponent<Text>();
        txtLoading.enabled = false;
    }


    void paintMap()
    {
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Tilemap terrainTilemap = GameObject.Find("TerrainTilemap").GetComponent<Tilemap>();
        Tile groundTile = (Tile)terrainTilemap.GetTile(new Vector3Int(-30, 11, 0));
        
        int combatMinX = 0;
        int combatMinY = -10;  // Hacked in
        int combatWidth = 50;
        int combatHeight = 50;
        int combatMaxX = combatMinX + combatWidth;
        int combatMaxY = combatMinY + combatHeight;

        for (int x = combatMinX; x <= combatMaxX; x++)
        {
            for (int y = combatMinY; y <= combatMaxY; y++)
            {
                //terrainTilemap.SetTile(new Vector3Int(x, y, 0), groundTile);// for now, just leave what I have painted on the map
            }
        }
        object[] mapData = (object[])setupResults.payload;

        for (int x = 0; x < mapData.Length; x++)
        {
            object[] column = (object[])mapData[x];
            for (int y = 0; y < column.Length; y++)
            {
                int cellValue = System.Int32.Parse(((object)column[y]).ToString());
                int[] translatedCoords = new int[] { x + combatMinX, y + combatMinY };
                if (cellValue < 0)
                {
                    //terrainTilemap.SetTile(new Vector3Int(translatedCoords[0], translatedCoords[1], zIndex), groundTile);// for now, just leave what I have painted on the map
                } else if (cellValue == 0)
                {
                    //placePlayer(translatedCoords[0], translatedCoords[1]);
                } else if (cellValue > 0)
                {
                    placeMonster(translatedCoords[0], translatedCoords[1], "monster" + cellValue, RandoMonsterID());
                }
            }
        }

    }

    private void placePlayer(int x, int y)
    {
        //Hacking this in:
        float centeringAdjustment = .5f;
        Rigidbody2D player = GameObject.Find("Player").GetComponent<Rigidbody2D>();
        player.MovePosition(new Vector2(40 * (x + centeringAdjustment), 40 * (y + centeringAdjustment)));  // Hacked in 40
    }

    private void placeMonster(int x, int y, string id, string monsterType)
    {
        float centeringAdjustment = 0;
        Canvas canvas = GameObject.Find("Canvas").GetComponent<Canvas>();
        GameObject monsterGO = new GameObject("Monster" + id);
        SpriteRenderer renderer = monsterGO.AddComponent<SpriteRenderer>();
        Tile monsterTile = Resources.Load<Tile>("Sprites/Monsters/" + monsterType);
        renderer.sprite = monsterTile.sprite;
        Rigidbody2D monsterR2D = monsterGO.AddComponent<Rigidbody2D>();
        Transform monsterTransform = monsterGO.GetComponent<Transform>();
        BoxCollider2D boxCollider2D = monsterGO.AddComponent<BoxCollider2D>();
        monsterTransform.parent = canvas.transform;
        monsterR2D.MovePosition(new Vector2(40 * (x + centeringAdjustment), 40 * (y + centeringAdjustment)));  // Hacked in 40
        monsterTransform.localScale = new Vector3Int(PlayerScaleFactor, PlayerScaleFactor, 1);
        monsterR2D.isKinematic = true;
        monsterR2D.mass = 5;
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

        
    }
