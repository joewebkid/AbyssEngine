#include "engine/render/AMeshMerger.h"
#include "engine/render/PaintCanvas.h"

AMeshMerger::~AMeshMerger() {
}

void AMeshMerger::render() {
    this->canvas->DrawTransform(this->transformId, nullptr);
}
