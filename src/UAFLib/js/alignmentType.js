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