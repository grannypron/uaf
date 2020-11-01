using Jint;
using System;
using System.Collections;
using System.Collections.Generic;
using UAFLib;
using Unity.Mathematics;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.Tilemaps;

public class CombatScreenEvents : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        StartCoroutine(DoRequest());
    }

    IEnumerator DoRequest()
    {

        //paintMap();
        UnityWebRequest www = UnityWebRequest.Get("https://raw.githubusercontent.com/grannypron/uaf/port/src/UAFLib/UAFLib.csproj");
        yield return www.SendWebRequest();

        if (www.isNetworkError || www.isHttpError)
        {
            Debug.Log(www.error);
        }
        else
        {
            // Show results as text
            Debug.Log(www.downloadHandler.text);
        }
    }


    ConsoleResults loadTest(string testPath)
    {
        Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
        loadJSFiles(engine);
        //string data = getJSFiles();
        //Debug.Log(data);
        //engine.Execute(data);

        TextAsset testString = Resources.Load<TextAsset>(testPath);
        ConsoleResults results = new ConsoleResults();
        engine.SetValue("consoleResults", results).Execute(testString.text);
        return results;


    }
    void paintMap()
    {
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Tilemap terrainTilemap = GameObject.Find("TerrainTilemap").GetComponent<Tilemap>();
        Tile groundTile = (Tile)terrainTilemap.GetTile(new Vector3Int(-30, 11, 0));
        
        int combatMinX = -25;
        int combatMinY = -25;
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
        ConsoleResults results = loadTest("js/Tests/DemoCombat");
        object[] mapData = (object[])results.payload;

        int zIndex = 0;
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
                    placePlayer(translatedCoords[0], translatedCoords[1]);
                } else if (cellValue > 0)
                {
                    placeMonster(translatedCoords[0], translatedCoords[1], "monster" + cellValue, RandoMonsterID());
                }
            }
        }

    }

    private void placePlayer(int x, int y)
    {
        float centeringAdjustment = .5f;
        Rigidbody2D player = GameObject.Find("Player").GetComponent<Rigidbody2D>();
        player.MovePosition(new Vector2(x + centeringAdjustment, y + centeringAdjustment));
    }

    private void placeMonster(int x, int y, string id, string monsterType)
    {
        float centeringAdjustment = .5f;
        GameObject monsterGO = new GameObject("Monster" + id);
        SpriteRenderer renderer = monsterGO.AddComponent<SpriteRenderer>();
        Tile monsterTile = Resources.Load<Tile>("Sprites/Monsters/" + monsterType);
        renderer.sprite = monsterTile.sprite;
        Rigidbody2D monsterR2D = monsterGO.AddComponent<Rigidbody2D>();
        Transform scaleTransform = monsterGO.GetComponent<Transform>();
        BoxCollider2D boxCollider2D = monsterGO.AddComponent<BoxCollider2D>();
        scaleTransform.localScale = new Vector3Int(2, 2, 1);
        monsterR2D.isKinematic = true;
        monsterR2D.mass = 5;
        monsterR2D.MovePosition(new Vector3(x + centeringAdjustment, y + centeringAdjustment));
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

    string getJSFile(string path)
    {
        TextAsset textAsset = Resources.Load<TextAsset>(path);
        return textAsset.text;
    }
    string getJSFiles()
    {
        string str = "";
        str = str + getJSFile("js/aaaaa_intsafe");
        str = str + getJSFile("js/aaaaa_UAFUtil");
        str = str + getJSFile("js/aaaaa_UnorderedQueue");
        str = str + getJSFile("js/A_ASLENTRY_L");
        str = str + getJSFile("js/A_CStringPAIR_L");
        str = str + getJSFile("js/A_SPECABILITY_DEF_L");
        str = str + getJSFile("js/ABILITY_DATA");
        str = str + getJSFile("js/ABILITY_DATA_TYPE");
        str = str + getJSFile("js/ActorInstanceType");
        str = str + getJSFile("js/ActorType");
        str = str + getJSFile("js/alignmentType");
        str = str + getJSFile("js/ASLENTRY");
        str = str + getJSFile("js/ATTACK_DATA");
        str = str + getJSFile("js/ATTACK_DETAILS");
        str = str + getJSFile("js/AURA_ATTACHMENT");
        str = str + getJSFile("js/AURA_SHAPE");
        str = str + getJSFile("js/AURA_WAVELENGTH");
        str = str + getJSFile("js/BASE_CLASS_DATA_TYPE");
        str = str + getJSFile("js/BASECLASS_LIST");
        str = str + getJSFile("js/BASECLASS_STATS");
        str = str + getJSFile("js/BLOCKAGE_STATUS");
        str = str + getJSFile("js/CAR");
        str = str + getJSFile("js/CArchive");
        str = str + getJSFile("js/CBFUNC");
        str = str + getJSFile("js/CBRESULT");
        str = str + getJSFile("js/CDATA");
        str = str + getJSFile("js/chainTriggerType");
        str = str + getJSFile("js/CHARACTER");
        str = str + getJSFile("js/CHARACTER_ID");
        str = str + getJSFile("js/CHARACTER_SPELL");
        str = str + getJSFile("js/charStatusType");
        str = str + getJSFile("js/CLASS_DATA");
        str = str + getJSFile("js/CLASS_DATA_TYPE");
        str = str + getJSFile("js/CList");
        str = str + getJSFile("js/CODES");
        str = str + getJSFile("js/COIN_TYPE");
        str = str + getJSFile("js/COMBAT_DATA");
        str = str + getJSFile("js/COMBAT_EVENT_DATA");
        str = str + getJSFile("js/COMBAT_SUMMARY");
        str = str + getJSFile("js/Combatant");
        str = str + getJSFile("js/combatVictorType");
        str = str + getJSFile("js/CPathFinder");
        str = str + getJSFile("js/creatureSizeType");
        str = str + getJSFile("js/DDATA");
        str = str + getJSFile("js/DEBUG_STRINGS");
        str = str + getJSFile("js/DEFAULT_GLOBAL_SCRIPT");
        str = str + getJSFile("js/DICEPLUS");
        str = str + getJSFile("js/DIFFICULTY_DATA");
        str = str + getJSFile("js/DIFFICULTY_LEVEL_DATA");
        str = str + getJSFile("js/Drawtile");
        str = str + getJSFile("js/entityType");
        str = str + getJSFile("js/EVENT_CONTROL");
        str = str + getJSFile("js/EVENT_TRIGGER_DATA");
        str = str + getJSFile("js/eventDirType");
        str = str + getJSFile("js/eventDistType");
        str = str + getJSFile("js/EventSourceType");
        str = str + getJSFile("js/eventSurpriseType");
        str = str + getJSFile("js/eventTriggerType");
        str = str + getJSFile("js/eventTurnUndeadModType");
        str = str + getJSFile("js/eventType");
        str = str + getJSFile("js/Externs");
        str = str + getJSFile("js/GameEvent");
        str = str + getJSFile("js/GameRules");
        str = str + getJSFile("js/GEM_CONFIG");
        str = str + getJSFile("js/genderType");
        str = str + getJSFile("js/GLOBAL_STATS");
        str = str + getJSFile("js/Globals");
        str = str + getJSFile("js/HIT_DICE_LEVEL_BONUS");
        str = str + getJSFile("js/HOOK_PARAMTERS");
        str = str + getJSFile("js/individualCombatantState");
        str = str + getJSFile("js/ITEM");
        str = str + getJSFile("js/ITEM_DATA");
        str = str + getJSFile("js/ITEM_DATA_TYPE");
        str = str + getJSFile("js/ITEM_ID");
        str = str + getJSFile("js/ITEM_LIST");
        str = str + getJSFile("js/itemClassType");
        str = str + getJSFile("js/itemReadiedLocation");
        str = str + getJSFile("js/Items");
        str = str + getJSFile("js/LASTACTION");
        str = str + getJSFile("js/LITTLE_RAN");
        str = str + getJSFile("js/miscErrorType");
        str = str + getJSFile("js/MONEY_DATA_TYPE");
        str = str + getJSFile("js/MONEY_SACK");
        str = str + getJSFile("js/MONSTER_DATA");
        str = str + getJSFile("js/MONSTER_DATA_TYPE");
        str = str + getJSFile("js/MONSTER_EVENT");
        str = str + getJSFile("js/MONSTER_EVENT_DATA");
        str = str + getJSFile("js/MonsterArrangement");
        str = str + getJSFile("js/MonsterPlacement");
        str = str + getJSFile("js/OBSTICAL_TYPE");
        str = str + getJSFile("js/PARTY");
        str = str + getJSFile("js/PARTY_POSITIONS");
        str = str + getJSFile("js/PATH_DIR");
        str = str + getJSFile("js/PATH_MANAGER");
        str = str + getJSFile("js/PIC_DATA");
        str = str + getJSFile("js/QueuedCombatantData");
        str = str + getJSFile("js/RACE_DATA");
        str = str + getJSFile("js/RACE_DATA_TYPE");
        str = str + getJSFile("js/READY_ITEMS");
        str = str + getJSFile("js/RECT");
        str = str + getJSFile("js/RNG");
        str = str + getJSFile("js/RUNTIME_ENVIRONMENT");
        str = str + getJSFile("js/RunTimeIF");
        str = str + getJSFile("js/SCRIPT_CONTEXT");
        str = str + getJSFile("js/SCRIPT_SOURCE_TYPE");
        str = str + getJSFile("js/SOUND_BYTE");
        str = str + getJSFile("js/SoundMgr");
        str = str + getJSFile("js/SPECAB_STRING_TYPE");
        str = str + getJSFile("js/SPECABILITY_DEF");
        str = str + getJSFile("js/SPECIAL_ABILITIES");
        str = str + getJSFile("js/SPELL_ABILITY");
        str = str + getJSFile("js/SPELL_DATA_TYPE");
        str = str + getJSFile("js/SPELL_EFFECTS_DATA");
        str = str + getJSFile("js/SPELL_LIST");
        str = str + getJSFile("js/spellBookType");
        str = str + getJSFile("js/spellLimitsType");
        str = str + getJSFile("js/surfaceType");
        str = str + getJSFile("js/TAG_LIST");
        str = str + getJSFile("js/TASK_STATE_DATA");
        str = str + getJSFile("js/TERRAIN_CELL");
        str = str + getJSFile("js/TILE_DATA");
        str = str + getJSFile("js/TURNING_SUMMARY");
        str = str + getJSFile("js/weaponClassType");
        str = str + getJSFile("js/zzzGlobalSingletons");
        return str;
    }
    void loadJSFile(Engine engine, string path)
            {
                try {
                    Debug.Log("Loading " + path);
                    //Get-ChildItem *.js | Rename-Item -NewName { $_.name -Replace '\.js$', '.txt' }
                    TextAsset textAsset = Resources.Load<TextAsset>(path);
                    engine.Execute(textAsset.text);
                    Debug.Log("Finished loading " + path);
                } catch(Exception ex)
                {
                    Debug.Log("Error loading " + path);
                }
            }
    void loadJSFiles(Engine engine)
            {
                loadJSFile(engine, "js/aaaaa_intsafe");
                loadJSFile(engine, "js/aaaaa_UAFUtil");
                loadJSFile(engine, "js/aaaaa_UnorderedQueue");
                loadJSFile(engine, "js/A_ASLENTRY_L");
                loadJSFile(engine, "js/A_CStringPAIR_L");
                loadJSFile(engine, "js/A_SPECABILITY_DEF_L");
                loadJSFile(engine, "js/ABILITY_DATA");
                loadJSFile(engine, "js/ABILITY_DATA_TYPE");
                loadJSFile(engine, "js/ActorInstanceType");
                loadJSFile(engine, "js/ActorType");
                loadJSFile(engine, "js/alignmentType");
                loadJSFile(engine, "js/ASLENTRY");
                loadJSFile(engine, "js/ATTACK_DATA");
                loadJSFile(engine, "js/ATTACK_DETAILS");
                loadJSFile(engine, "js/AURA_ATTACHMENT");
                loadJSFile(engine, "js/AURA_SHAPE");
                loadJSFile(engine, "js/AURA_WAVELENGTH");
                loadJSFile(engine, "js/BASE_CLASS_DATA_TYPE");
                loadJSFile(engine, "js/BASECLASS_LIST");
                loadJSFile(engine, "js/BASECLASS_STATS");
                loadJSFile(engine, "js/BLOCKAGE_STATUS");
                loadJSFile(engine, "js/CAR");
                loadJSFile(engine, "js/CArchive");
                loadJSFile(engine, "js/CBFUNC");
                loadJSFile(engine, "js/CBRESULT");
                loadJSFile(engine, "js/CDATA");
                loadJSFile(engine, "js/chainTriggerType");
                loadJSFile(engine, "js/CHARACTER");
                loadJSFile(engine, "js/CHARACTER_ID");
                loadJSFile(engine, "js/CHARACTER_SPELL");
                loadJSFile(engine, "js/charStatusType");
                loadJSFile(engine, "js/CLASS_DATA");
                loadJSFile(engine, "js/CLASS_DATA_TYPE");
                loadJSFile(engine, "js/CList");
                loadJSFile(engine, "js/CODES");
                loadJSFile(engine, "js/COIN_TYPE");
                loadJSFile(engine, "js/COMBAT_DATA");
                loadJSFile(engine, "js/COMBAT_EVENT_DATA");
                loadJSFile(engine, "js/COMBAT_SUMMARY");
                loadJSFile(engine, "js/Combatant");
                loadJSFile(engine, "js/combatVictorType");
                loadJSFile(engine, "js/CPathFinder");
                loadJSFile(engine, "js/creatureSizeType");
                loadJSFile(engine, "js/DDATA");
                loadJSFile(engine, "js/DEBUG_STRINGS");
                loadJSFile(engine, "js/DEFAULT_GLOBAL_SCRIPT");
                loadJSFile(engine, "js/DICEPLUS");
                loadJSFile(engine, "js/DIFFICULTY_DATA");
                loadJSFile(engine, "js/DIFFICULTY_LEVEL_DATA");
                loadJSFile(engine, "js/Drawtile");
                loadJSFile(engine, "js/entityType");
                loadJSFile(engine, "js/EVENT_CONTROL");
                loadJSFile(engine, "js/EVENT_TRIGGER_DATA");
                loadJSFile(engine, "js/eventDirType");
                loadJSFile(engine, "js/eventDistType");
                loadJSFile(engine, "js/EventSourceType");
                loadJSFile(engine, "js/eventSurpriseType");
                loadJSFile(engine, "js/eventTriggerType");
                loadJSFile(engine, "js/eventTurnUndeadModType");
                loadJSFile(engine, "js/eventType");
                loadJSFile(engine, "js/Externs");
                loadJSFile(engine, "js/GameEvent");
                loadJSFile(engine, "js/GameRules");
                loadJSFile(engine, "js/GEM_CONFIG");
                loadJSFile(engine, "js/genderType");
                loadJSFile(engine, "js/GLOBAL_STATS");
                loadJSFile(engine, "js/Globals");
                loadJSFile(engine, "js/HIT_DICE_LEVEL_BONUS");
                loadJSFile(engine, "js/HOOK_PARAMTERS");
                loadJSFile(engine, "js/individualCombatantState");
                loadJSFile(engine, "js/ITEM");
                loadJSFile(engine, "js/ITEM_DATA");
                loadJSFile(engine, "js/ITEM_DATA_TYPE");
                loadJSFile(engine, "js/ITEM_ID");
                loadJSFile(engine, "js/ITEM_LIST");
                loadJSFile(engine, "js/itemClassType");
                loadJSFile(engine, "js/itemReadiedLocation");
                loadJSFile(engine, "js/Items");
                loadJSFile(engine, "js/LASTACTION");
                loadJSFile(engine, "js/LITTLE_RAN");
                loadJSFile(engine, "js/miscErrorType");
                loadJSFile(engine, "js/MONEY_DATA_TYPE");
                loadJSFile(engine, "js/MONEY_SACK");
                loadJSFile(engine, "js/MONSTER_DATA");
                loadJSFile(engine, "js/MONSTER_DATA_TYPE");
                loadJSFile(engine, "js/MONSTER_EVENT");
                loadJSFile(engine, "js/MONSTER_EVENT_DATA");
                loadJSFile(engine, "js/MonsterArrangement");
                loadJSFile(engine, "js/MonsterPlacement");
                loadJSFile(engine, "js/OBSTICAL_TYPE");
                loadJSFile(engine, "js/PARTY");
                loadJSFile(engine, "js/PARTY_POSITIONS");
                loadJSFile(engine, "js/PATH_DIR");
                loadJSFile(engine, "js/PATH_MANAGER");
                loadJSFile(engine, "js/PIC_DATA");
                loadJSFile(engine, "js/QueuedCombatantData");
                loadJSFile(engine, "js/RACE_DATA");
                loadJSFile(engine, "js/RACE_DATA_TYPE");
                loadJSFile(engine, "js/READY_ITEMS");
                loadJSFile(engine, "js/RECT");
                loadJSFile(engine, "js/RNG");
                loadJSFile(engine, "js/RUNTIME_ENVIRONMENT");
                loadJSFile(engine, "js/RunTimeIF");
                loadJSFile(engine, "js/SCRIPT_CONTEXT");
                loadJSFile(engine, "js/SCRIPT_SOURCE_TYPE");
                loadJSFile(engine, "js/SOUND_BYTE");
                loadJSFile(engine, "js/SoundMgr");
                loadJSFile(engine, "js/SPECAB_STRING_TYPE");
                loadJSFile(engine, "js/SPECABILITY_DEF");
                loadJSFile(engine, "js/SPECIAL_ABILITIES");
                loadJSFile(engine, "js/SPELL_ABILITY");
                loadJSFile(engine, "js/SPELL_DATA_TYPE");
                loadJSFile(engine, "js/SPELL_EFFECTS_DATA");
                loadJSFile(engine, "js/SPELL_LIST");
                loadJSFile(engine, "js/spellBookType");
                loadJSFile(engine, "js/spellLimitsType");
                loadJSFile(engine, "js/surfaceType");
                loadJSFile(engine, "js/TAG_LIST");
                loadJSFile(engine, "js/TASK_STATE_DATA");
                loadJSFile(engine, "js/TERRAIN_CELL");
                loadJSFile(engine, "js/TILE_DATA");
                loadJSFile(engine, "js/TURNING_SUMMARY");
                loadJSFile(engine, "js/weaponClassType");
                loadJSFile(engine, "js/zzzGlobalSingletons");
            }
        
    }
