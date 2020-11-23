using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UAFLib;

public static class GameState
{
    public const int MAX_MONSTERS = 100;

    public static Jint.Engine engine;
    public static ConsoleResults engineOutput;
    public static Dictionary<int, String> monsters;
    public static int monsterMoveIdx = 0;
    public static bool allowInput = true;
    public static List<int> deadMonsters = new List<int>();
}
