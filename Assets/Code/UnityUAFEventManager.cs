using Esprima.Ast;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.UI;

public class UnityUAFEventManager
{
    List<IUIListener> listeners = new List<IUIListener>();

    public UnityUAFEventManager(IUIListener listener) {
        this.listeners.Add(listener);
    }

    public void UpdateCombatMessage(string message)
    {
        this.notifyAll("UpdateCombatMessage", message);
    }

    public void CombatantMoved(int x, int y, string name, int hp, int ac, int attacks, int moves)
    {
        object[] data = new object[7];
        data[0] = x;
        data[1] = y;
        data[2] = name;
        data[3] = hp;
        data[4] = ac;
        data[5] = attacks;
        data[6] = moves;
        this.notifyAll("CombatantMoved", data);
    }

    public void StartAttack(int attacker, int attacked)
    {
        this.notifyAll("StartAttack", new int[] { attacker, attacked });
    }

    public void CombatantDying(int id, int x, int y)
    {
        this.notifyAll("CombatantDying", new int[] { id, x, y});
    }

    public void CombatantDead(int id, int x, int y)
    {
        this.notifyAll("CombatantDead", new int[] { id, x, y });
    }

    public void PlaySound(string soundName)
    {
        this.notifyAll("PlaySound", soundName);
    }

    private void notifyAll(string eventName, object data)
    {
        foreach (IUIListener listener in listeners)
        {
            listener.OnEvent(eventName, data);
        }

    }

}

