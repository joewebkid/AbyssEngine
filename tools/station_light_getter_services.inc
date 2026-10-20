static void stationLightGetterBoundary(StarSystem *star, Engine *engine, float ambientScale) {
    assert(star && engine && engine == ApplicationManager::gAppManager->engine);
    assert(engine->field_0x32c == 2 && ambientScale == 0.5f);
    assert(star->sunLightColor.x == 0.239999995f);
    assert(star->sunLightColor.y == 0.400000006f);
    assert(star->sunLightColor.z == 0.370000005f);
    std::puts("All planets complete; real StarSystem::initLight reached its GetEngine result");
    throw FlareImageBoundary{};
}
