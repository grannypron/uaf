/** TODO **/
function COIN_TYPE() {
    this.Name = "";
    this.rate = 0.0;
    this.IsBase = false; 
    this.Clear();
}

COIN_TYPE.prototype.Clear = function() {
    this.Name = "";
    this.rate = 0.0;
    this.IsBase = false; 
}