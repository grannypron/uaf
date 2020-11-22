function SoundMgrObj() {
    this.pSndMgr = null;
    this.PlaySoundEffects = true;

    this.DEF_HIT_SOUND = "Hit.wav";       // attack successful - stored in item/moster data
    this.DEF_MISS_SOUND = "Miss.wav";      // attack missed     - stored in item/monster data
    this.DEF_LAUNCH_SOUND = "Launch.wav";  // played as attack is made, before hit or miss sounds
    this.DEF_CAST_SOUND = "Cast.wav";      // spell cast        - stored in spell data
    this.DEF_CHAR_DEATH_SOUND = "CharDeath.wav"; // character dies    - stored in global combat data
    this.DEF_CHAR_MOVE_SOUND = "CharMove.wav";  // character moves   - stored in global combat data
}

SoundMgrObj.prototype.ClearSound = function (sound) {
    if (this.pSndMgr == null) return;
    this.StopSound(sound);
    this.pSndMgr.RemoveSound(sound);
}

SoundMgrObj.prototype.LoadSound = function(file) {
    /**TODO
    if (pSndMgr == NULL) return -1;
    BOOL ismidi = IsMIDIFile(file);
    if (ismidi)
        pSndMgr -> m_MidiType=SOUND_MIDI_STREAM_ID;
    //WriteDebugString("104 Calling AddSound(%s)\n", file);
    long key = pSndMgr -> AddSound(file);
    //WriteDebugString("LoadSound %u (%s)\n", key, file);
    return key;
    **/
}

SoundMgrObj.prototype.ValidSound = function (sound) {
/**TODO** Stub */
    return true;
}

SoundMgrObj.prototype.PlaySound = function (file) {
    /**TODO** Stub */
    UIEventManager.PlaySound(file);
    return true;
}

SoundMgrObj.prototype.StopSound = function(sound)
{
    if (this.pSndMgr == null) return;
    this.pSndMgr.StopSample(sound);
}