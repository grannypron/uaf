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
    Dictionary<string, IUIListener> listeners = new Dictionary<string, IUIListener>();

    public UnityUAFEventManager() {
    }

    public void UpdateCombatMessage(string message)
    {
        this.notifyAll("UpdateCombatMessage", message);
    }

    public void AddListener(IUIListener listener, string tag)
    {
        // Does not allow multiple listeners with the same tag
        if (listeners.ContainsKey(tag))
        {
            listeners.Remove(tag);
        }
        this.listeners.Add(tag, listener);
    }

    public void CombatantMoved(int x, int y, string name, int hp, int ac, int attacks, int moves, int xp)
    {
        object[] data = new object[8];
        data[0] = x;
        data[1] = y;
        data[2] = name;
        data[3] = hp;
        data[4] = ac;
        data[5] = attacks;
        data[6] = moves;
        data[7] = xp;
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
        foreach (IUIListener listener in listeners.Values)
        {
            listener.OnEvent(eventName, data);
        }

    }
    public void clearAllListeners()
    {
        this.listeners = new Dictionary<string, IUIListener>();

    }
}

