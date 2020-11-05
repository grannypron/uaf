function MonsterFormType() {
    this.FormMammal = 1;
    this.FormAnimal = 2;
    this.FormSnake = 4;
    this.FormGiant = 8;
    this.FormLarge = 16; /*,FormUndead=32*/
}

function MonsterFormTypeObj() {
    MonsterFormType.call(this);
    Object.setPrototypeOf(this, entityType.prototype);
}