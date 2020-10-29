function RunTimeIF() {
    this.charContextStack = [];
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