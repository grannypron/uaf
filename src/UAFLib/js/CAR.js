function CAR(path, version) {
    CArchive.call(this, path, version);
    Object.setPrototypeOf(this, CArchive.prototype);
}