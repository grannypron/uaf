function SoundMgrObj() {
    this.pSndMgr = null;
    this.PlaySoundEffects = true;
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
    Globals.debug("Playing sound " + file);
    return true;
}

SoundMgrObj.prototype.StopSound = function(sound)
{
    if (this.pSndMgr == null) return;
    this.pSndMgr.StopSample(sound);
}