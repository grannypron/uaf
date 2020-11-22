function MONSTER_EVENT_DATA()
{
    this.monsters = [];
    this.Clear();
}


MONSTER_EVENT_DATA.prototype.Add = function(data)
{
    this.monsters.push(data);
    return this.monsters.length;
}

MONSTER_EVENT_DATA.prototype.Delete = function (index) {
    if ((index >= 0) && (index < monsters.length))
        this.monsters.splice(index, 1);
}

MONSTER_EVENT_DATA.prototype.Clear = function() {
    this.monsters = [];
}

MONSTER_EVENT_DATA.prototype.GetCount = function() {
    return this.monsters.length;
}
