var WhatSprite = function() {
    this.DeathSprite = 1;
    this.DyingSprite = 2;
}

function WhatSpriteObj() {
    WhatSprite.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}
