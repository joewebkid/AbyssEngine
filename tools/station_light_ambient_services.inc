static unsigned stationSceneAmbientCalls;
void ShaderUpdateMaterialColor() { unexpected("ShaderUpdateMaterialColor outside fixed-function boundary"); }
extern "C" void glLightModelfv(unsigned int name, const float *values) {
    assert(name == 0xb53 && values);
    assert(values[3] == 1.0f);
    ++stationSceneAmbientCalls;
}

static void stationLightAmbientBoundary(StarSystem *star, Engine *engine) {
    assert(stationSceneAmbientCalls == 1);
    assert(engine->sceneAmbient[0] == star->sunLightColor.x * 0.5f);
    assert(engine->sceneAmbient[1] == star->sunLightColor.y * 0.5f);
    assert(engine->sceneAmbient[2] == star->sunLightColor.z * 0.5f);
    assert(engine->sceneAmbient[3] == 1.0f);
    std::puts("Real scene-ambient light setter and GLES1 compatibility call reached");
    throw FlareImageBoundary{};
}
