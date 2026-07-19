#include "engine/render/AMeshMerger.h"

AMeshMerger::~AMeshMerger() {
}

void AMeshMerger::render() {
    AMeshMerger_drawMeshes(this->canvas, this->transformId, 0);
}
