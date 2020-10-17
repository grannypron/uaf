var alignmentType = {};
alignmentType.LawfulGood = 0;    
alignmentType.NeutralGood = 1; 
alignmentType.ChaoticGood = 2; 
alignmentType.LawfulNeutral = 3; 
alignmentType.TrueNeutral = 4; 
alignmentType.ChaoticNeutral = 5;  
alignmentType.LawfulEvil = 6;    
alignmentType.NeutralEvil = 7; 
alignmentType.ChaoticEvil = 8;

function alignmentTypeObj() {
    entityType.call(this, alignmentType);
    Object.setPrototypeOf(this, entityType.prototype);
}

var alignmentType = new alignmentTypeObj(alignmentType);

alignmentType.GetAlignmentName = function (align) {
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
