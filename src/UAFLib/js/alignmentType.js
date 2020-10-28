function alignmentType() {
    this.LawfulGood = 0;    
    this.NeutralGood = 1; 
    this.ChaoticGood = 2; 
    this.LawfulNeutral = 3; 
    this.TrueNeutral = 4; 
    this.ChaoticNeutral = 5;  
    this.LawfulEvil = 6;    
    this.NeutralEvil = 7; 
    this.ChaoticEvil = 8;

    // PORT NOTE:  Wasn't able to move this to alignmentTypeObj.prototype like I wanted to - probably a Jint quirk
    this.GetAlignmentName = function (align) {
        var temp = "??";
        switch (align) {
            case alignmentType.LawfulGood: temp = "Lawful Good"; break;
            case alignmentType.NeutralGood: temp = "Neutral Good"; break;
            case alignmentType.ChaoticGood: temp = "Chaotic Good"; break;
            case alignmentType.LawfulNeutral: temp = "Lawful Neutral"; break;
            case alignmentType.TrueNeutral: temp = "True Neutral"; break;
            case alignmentType.ChaoticNeutral: temp = "Chaotic Neutral"; break;
            case alignmentType.LawfulEvil: temp = "Lawful Evil"; break;
            case alignmentType.NeutralEvil: temp = "Neutral Evil"; break;
            case alignmentType.ChaoticEvil: temp = "Chaotic Evil"; break;
        }
        return temp;
    }
}
function alignmentTypeObj() {
    alignmentType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
