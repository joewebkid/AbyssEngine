"""Run real StarSystem light setters and return its constructor in the Pan gate."""
import run_station_light_ambient_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


LIGHT_METHODS = (
    "void Engine::LightSetRimColor(",
    "void Engine::LightSetMaterialColorAmbient(",
    "void Engine::LightSetMaterialColorDiffuse(",
    "void Engine::LightSetMaterialColorSpecular(",
    "void Engine::LightSetMaterialColorShininess(",
    "void Engine::LightSetLightDirection(",
    "void Engine::LightSetLightColorAmbient(",
    "void Engine::LightSetLightColorDiffuse(",
    "void Engine::LightSetLightColorSpecular(",
    "void Engine::LightSetParticleAmbient(",
)


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    ambient = (ROOT / "tools/station_light_ambient_services.inc").read_text()
    ambient = replace_once(ambient,
        '    std::puts("Real scene-ambient light setter and GLES1 compatibility call reached");\n'
        '    throw FlareImageBoundary{};',
        '    std::puts("Real scene-ambient light setter reached; continuing");')
    (out / "station_light_ambient_services.inc").write_text(ambient)
    services = (out / "sun_station_services.inc").read_text()
    services += '\n#include "station_light_complete_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + extract("src/engine/render/Engine.cpp", LIGHT_METHODS)
    canvas = (ROOT / "src/engine/render/PaintCanvas.cpp").read_text()
    chain += '\n' + canvas[canvas.index('static char paintcanvas_g_fog_flag_storage'):
                             canvas.index('void PaintCanvas::FogEnable(')]
    chain += '\n' + extract("src/engine/render/PaintCanvas.cpp", [
        "void PaintCanvas::FogEnable("])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    for symbol in (
        '_ZN11AbyssEngine6Engine16LightSetRimColorEfff',
        '_ZN11AbyssEngine6Engine28LightSetMaterialColorAmbientEfff',
        '_ZN11AbyssEngine6Engine28LightSetMaterialColorDiffuseEfff',
        '_ZN11AbyssEngine6Engine29LightSetMaterialColorSpecularEfff',
        '_ZN11AbyssEngine6Engine30LightSetMaterialColorShininessEf',
        '_ZN11AbyssEngine6Engine22LightSetLightDirectionEfffj',
        '_ZN11AbyssEngine6Engine25LightSetLightColorAmbientEfffj',
        '_ZN11AbyssEngine6Engine25LightSetLightColorDiffuseEfffj',
        '_ZN11AbyssEngine6Engine26LightSetLightColorSpecularEfffj',
        '_ZN11AbyssEngine6Engine23LightSetParticleAmbientEfff',
        '_ZN11AbyssEngine11PaintCanvas9FogEnableEbNS_7FogModeE',
    ):
        traps = replace_once(traps, 'TRAP(' + symbol + ')', '')
    (out / "station_binary_traps.inc").write_text(traps)

    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level, function(level, "void Level::createSpace() {"),
                         extract("src/game/world/Level.cpp", ["void Level::createSpace() {"]))
    level = replace_once(level, '    this->starSystem = new StarSystem(mode);',
                         '    this->starSystem = new StarSystem(mode);\n'
                         '    throw StarSystemCtorBoundary{}; // Host boundary after the real constructor returns.')
    (out / "level_init_under_test.inc").write_text(level)
    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, '#include "level_init_under_test.inc"',
                        'struct StarSystemCtorBoundary {};\n#include "level_init_under_test.inc"')
    (out / "sun_host.inc").write_text(host)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, '    Engine engine;\n',
                        '    Engine engine;\n    engine.materialAlpha = 1.0f;\n')
    gate = replace_once(gate,
        '    } catch (const FlareImageBoundary &) {\n        reached = true;\n    }',
        '    } catch (const StarSystemCtorBoundary &) {\n        reached = true;\n    }')
    gate = replace_once(gate,
        '    assert(reached && level->field_134 == 0 && !level->player && !level->starSystem);',
        '    assert(reached && level->field_134 == 0 && !level->player && level->starSystem);\n'
        '    assert(stationMaterialColorCalls == 3 && stationShininessCalls == 1);\n'
        '    assert(stationLightColorCalls == 5 && stationSceneAmbientCalls == 1);\n'
        '    assert(engine.lightCount == 2 && engine.materialShininess == 0.7f);\n'
        '    for (unsigned i = 0; i < 4; ++i) {\n'
        '        assert(engine.materialAmbient[i] == 1.0f);\n'
        '        assert(engine.materialDiffuse[i] == 1.0f);\n'
        '        assert(engine.materialSpecular[i] == 1.0f);\n'
        '    }\n'
        '    auto *star = level->starSystem;\n'
        '    assert(star->planetsArray && star->planetsArray->size() == 6);\n'
        '    assert(star->positionsArray && star->positionsArray->size() == 6);\n'
        '    assert(star->texturesArray && star->texturesArray->size() == 6);\n'
        '    assert(star->stationIdxArray && star->stationIdxArray->size() == 5);\n'
        '    assert(star->lensFlare && star->sunStreak && star->playerTargets);\n'
        '    assert(engine.particleAmbient.x == star->sunLightColor.x);\n'
        '    assert(engine.particleAmbient.y == star->sunLightColor.y);\n'
        '    assert(engine.particleAmbient.z == star->sunLightColor.z);\n'
        '    assert(engine.lightAmbient.r == 0.0f && engine.lightAmbient.g == 0.0f && engine.lightAmbient.b == 0.0f);\n'
        '    assert(engine.lightDiffuse.r == star->sunLightColor.x);\n'
        '    assert(engine.lightDiffuse.g == star->sunLightColor.y);\n'
        '    assert(engine.lightDiffuse.b == star->sunLightColor.z);\n'
        '    assert(engine.lightSpecular.r == 2.0f && engine.lightSpecular.g == 2.0f && engine.lightSpecular.b == 2.0f);\n'
        '    unsigned color = status->getStation()->getTextureIndex() * 3;\n'
        '    assert((&engine.lightDiffuse)[1].r == g_StarSystem_init_stationColors[color] * 1.5f);\n'
        '    assert((&engine.lightDiffuse)[1].g == g_StarSystem_init_stationColors[color + 1] * 1.5f);\n'
        '    assert((&engine.lightDiffuse)[1].b == g_StarSystem_init_stationColors[color + 2] * 1.5f);\n'
        '    assert(star->fogEnabled == 0 && canvas.fogMode == 0 && canvas.fogEnableFlag == 1);')
    gate = replace_once(gate, 'StarSystem real scene ambient boundary',
                        'StarSystem constructor returned after real light setters')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.first.first.first.first.first.sun.station.prepare = prepare
    first.first.first.first.first.first.first.sun.station.main()
