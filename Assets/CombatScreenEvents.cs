using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;

public class CombatScreenEvents : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        paintMap();
    }

    void paintMap()
    {
        Rigidbody2D player = GetComponent<Rigidbody2D>();
        Tilemap terrainTilemap = GameObject.Find("TerrainTilemap").GetComponent<Tilemap>();
        Tilemap monsterTilemap = GameObject.Find("MonsterTilemap").GetComponent<Tilemap>();
        Tile groundTile = (Tile)terrainTilemap.GetTile(new Vector3Int(-30, 11, 0));

        UAFLib.Console console = new UAFLib.Console();
        console.mJSPath = @"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\js";
        UAFLib.ConsoleResults results = console.runTest(@"C:\Users\Shadow\Desktop\uaf.git\uaf-port\src\UAFLib\Tests\TestSimpleCombat.js");
        object[] mapData = (object[])results.payload;

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
}
