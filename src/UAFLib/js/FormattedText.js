function FormattedText() {
    this.textData = new TEXT_DISPLAY_DATA();
    this.errorTextData = new TEXT_DISPLAY_DATA();
    this.pausedTextData = new TEXT_DISPLAY_DATA();
    this.journalTextData = new TEXT_DISPLAY_DATA();
    this.combatTextData = new TEXT_DISPLAY_DATA();
}

FormattedText.prototype.ClearFormattedText = function (data) {
    return "";
}

FormattedText.prototype.ResetFontColorTags = function () {
    //**TODO**/
}

FormattedText.prototype.FormatCombatText = function(data, pText)
{
    this.ResetFontColorTags();
    data = this.ClearFormattedText(data);

    if (pText == null) return;
    if (pText.length == 0) return;

    // PORT NOTE:  Letting the UI deal with this.
    Globals.debug("FormattedText.FormatCombatText: " + pText);
    return pText;
    /*
    data.slowText = false;
    data.highlightAll = false;

    int char_pixels_per_line = COMBAT_STAT_BOX_WIDTH -
        //GraphicsMgr.GetMaxCharacterWidth() - 
        (VertBarSrc.right - VertBarSrc.left) - 16;
    if (char_pixels_per_line <= 0) {
        char_pixels_per_line = 240;
    };
    BOOL done = FALSE;
    const char * start = pText;
    const char * end = pText;
    int currLineWidth = 0;

    while ((!done) && (start != NULL) && (* start != '\0')) {
        currLineWidth = 0;

        while ((!done) && (currLineWidth < char_pixels_per_line) && (* end != '\n') && (* end != '\r')) {
            currLineWidth += GraphicsMgr.GetTextWidth(end, 1);
            end++;
            if (* end == '\0') done = TRUE;
        }

        CString NewString("");

        if (done) {
            NewString += start;
        }
        else {
            CString tmpNewString("");
            const char * temp = end;

            while ((* temp != ' ') && (* temp != '\n') && (* temp != '\r') && (temp != start))
                temp--;

            if (temp != start) {
                tmpNewString.Format("%.*s", temp - start, start);
                end = temp;
            }
            else {
                tmpNewString.Format("%.*s", end - start, start);
            }
            NewString += tmpNewString;
        }

        if (NewString[0] == ' ') NewString.Delete(0);
        //data.text.Add(NewString);
        data.Add(NewString);

        if (!done) {
            while ((* end == '\r') || (* end == '\n'))
                end++;
        }

        data.numLines++;
        start = end;
    }*/
}