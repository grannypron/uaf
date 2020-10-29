using Jint;
using System.Collections;
using System.Collections.Generic;
using UAFLib;
using UnityEngine;
using UnityEngine.Tilemaps;

public class CombatScreenEvents : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        paintMap();
    }
    ConsoleResults loadTest(string testPath)
    {
        Engine engine = new Engine(cfg => cfg.AllowClr(typeof(MFCSerializer).Assembly));
        loadJSFiles(engine);

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
                terrainTilemap.SetTile(new Vector3Int(x, y, 0), groundTile);
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
                    terrainTilemap.SetTile(new Vector3Int(translatedCoords[0], translatedCoords[1], zIndex), groundTile);
                } else if (cellValue == 0)
                {
                    placePlayer(translatedCoords[0], translatedCoords[1]);
                } else if (cellValue > 0)
                {
                    placeMonster(translatedCoords[0], translatedCoords[1], "monster" + cellValue, "Kobold");
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

    void loadJSFile(Engine engine, string path)
    {
        //Get-ChildItem *.js | Rename-Item -NewName { $_.name -Replace '\.js$', '.txt' }
        TextAsset textAsset = Resources.Load<TextAsset>(path);
        engine.Execute(textAsset.text);
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
