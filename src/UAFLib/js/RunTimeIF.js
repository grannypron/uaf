function RunTimeIF() {
    // PORT NOTE:  These two were global variables, but I put them here
    this.NULL_ACTOR = null;
    this.INVALID_ACTOR = -1;

    this.charContextStack = [];
    this.targetContext = this.NULL_ACTOR;
}

RunTimeIF.prototype.HaveCharContext = function () {
    return (this.charContextStack.length > 0);
}
RunTimeIF.prototype.ClearCharContext = function() {
    Globals.ASSERT(this.HaveCharContext());
    if (this.charContextStack.length > 0) this.charContextStack.pop();
}

RunTimeIF.prototype.SetCharContext = function(data)
{ 
    this.charContextStack.push(data);
}

RunTimeIF.prototype.SetTargetContext = function (data) {
    targetContext = data;
}