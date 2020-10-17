/** TODO **/
function GEM_CONFIG() {
    this.MAX_NAME = 10;
    this.minValue = 0;
    this.maxValue = 0;
    this.name = [this.MAX_NAME + 1]; 
    this.Clear();
}

GEM_CONFIG.prototype.Clear = function() {
    this.minValue = 0;
    this.maxValue = 0;
    this.name[0] = '\0'; 
}


GEM_CONFIG.prototype.SetName = function (name) {
    this.name = name + '\0';
}