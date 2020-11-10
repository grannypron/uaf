function SOUND_BYTE() {

}

SOUND_BYTE.prototype.Clear = function () {

}

SOUND_BYTE.prototype.Play = function () {
    UIEventManager.PlaySound("SOUND_BYTE");
    return true;
/***TODO: Decide what to do with this
    if (hSound == -2) return false; // We already tried and failed!
    if (!ValidSound(hSound)) {
        hSound = LoadSound(this);
    };
    if (PlaySound(hSound, FALSE)) return true;  // It worked
    hSound = -2;
    return false;
    */
}