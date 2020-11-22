function itemRechargeRate() {
    this.irrNever = 0; // fixed qty of charges, once gone no 'Use' is possible
    this.irrDaily = 1;  // full qty of charges restored each day
};

itemRechargeRate.prototype.GetByString = function(s) {
    switch (s.toLowerCase()) {
        case "never":
            return this.irrNever;
            break;
        case "daily":
            return this.irrDaily;
            break;
    }
}