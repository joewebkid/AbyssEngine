"""Continue the Pan Level call through production PlayerEgo::setLevel."""

import run_station_binary_asset_gate as runner
import run_level_player_ship_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct PlayerShipBoundBoundary {};",
                        "struct PlayerParticlesBoundBoundary {};")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level, "    throw PlayerShipBoundBoundary{};",
                          "    ego->setLevel(this);\n"
                          "    throw PlayerParticlesBoundBoundary{};")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/ship/PlayerEgo.cpp", [
        "void PlayerEgo::setLevel(Level *level) {"])
    chain += """
struct ParticleCall {
    ParticleSystemManager *manager;
    const Matrix *matrix;
    int set;
    int handle;
};
static std::vector<ParticleCall> particleAdds, particleEmits;
int ParticleSystemManager::addSystem(const Matrix *matrix, ParticleSettings::ParticleSet set, bool flag) {
    assert(matrix && !flag && particleAdds.size() == particleEmits.size());
    int handle = 100 + int(particleAdds.size());
    particleAdds.push_back({this, matrix, int(set), handle});
    return handle;
}
void ParticleSystemManager::enableSystemEmit(int handle, bool enabled) {
    assert(!enabled && particleAdds.size() == particleEmits.size() + 1);
    assert(handle == particleAdds.back().handle && this == particleAdds.back().manager);
    particleEmits.push_back({this, nullptr, 0, handle});
}
"""
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const PlayerShipBoundBoundary &)",
                        "catch (const PlayerParticlesBoundBoundary &)")
    gate = replace_once(gate, "    assert(level->player == observedEgo);",
        "    assert(level->player == observedEgo && observedEgo->level == level);\n"
        "    assert(status->getCurrentCampaignMission() == 50);\n"
        "    assert(particleAdds.size() == 1 && particleEmits.size() == 1);\n"
        "    assert(particleAdds[0].manager == level->field_74);\n"
        "    assert(particleAdds[0].matrix == &observedEgo->geometry->getMatrix());\n"
        "    assert(particleAdds[0].set == 9 && particleAdds[0].handle == observedEgo->currentSystem);\n"
        "    assert(particleEmits[0].manager == level->field_74);\n"
        "    status->currentCampaignMission = 1;\n"
        "    observedEgo->setLevel(level); // Separate early-campaign branch probe.\n"
        "    assert(particleAdds.size() == 4 && particleEmits.size() == 4);\n"
        "    ParticleSystemManager *managers[] = {level->field_74,\n"
        "        level->particleEmitBoolPtr, level->particleRenderBoolPtr};\n"
        "    const int sets[] = {9, 15, 42};\n"
        "    const int handles[] = {observedEgo->currentSystem,\n"
        "        observedEgo->smokeSystem, observedEgo->explosionSmoke};\n"
        "    for (int i = 0; i < 3; ++i) {\n"
        "        assert(managers[i] && particleAdds[i + 1].manager == managers[i]);\n"
        "        assert(particleAdds[i + 1].matrix == &observedEgo->geometry->getMatrix());\n"
        "        assert(particleAdds[i + 1].set == sets[i] && particleAdds[i + 1].handle == handles[i]);\n"
        "        assert(particleEmits[i + 1].manager == managers[i] && particleEmits[i + 1].handle == handles[i]);\n"
        "    }\n"
        "    status->currentCampaignMission = 50;")
    gate = replace_once(gate,
        "Level::createPlayer -> PlayerEgo::setShip -> Pan hull and five AEM meshes: OK",
        "Level::createPlayer -> setShip -> setLevel particle handles and five AEM meshes: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
