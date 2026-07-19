#include "engine/file/FileRead.h"

#include "engine/core/AbyssEngine.h"
#include "engine/core/AERandom.h"
#include "engine/math/AEMath.h"
#include "game/mission/Item.h"
#include "game/ship/Agent.h"
#include "game/ship/Ship.h"
#include "game/world/NewsItem.h"
#include "game/world/SolarSystem.h"
#include "game/world/SpacePoint.h"
#include "game/world/Station.h"
#include "game/world/Wanted.h"
#include "game/mission/Mission.h"
#include "engine/core/Array.h"

using AbyssEngine::Vector;

FileRead::FileRead() {
}

FileRead::~FileRead() {
}

int32_t FileRead::loadStation(int32_t id) {
    int16_t selected = (int16_t) id;
    Array<Station *> *stations = loadStationsBinary(&selected, 1);
    int32_t result = (int32_t)(intptr_t)
    stations->data()[0];
    delete stations;
    return result;
}

int32_t FileRead::loadStationsBinary() {
    String path("stations_binary.bin");
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("stations_binary.bin", &handle);

        int32_t color0;
        int32_t color1;
        int32_t color2;
        int32_t offset = 0;
        String name;
        for (uint32_t i = 0; i < 0x87; i++) {
            AEFile::ReadSwitched(name, handle);
            AEFile::ReadSwitched(offset, handle);
            AEFile::ReadSwitched(color0, handle);
            AEFile::ReadSwitched(color1, handle);
            AEFile::ReadSwitched(color2, handle);
        }
        AEFile::Close(handle);
    }
    return 0;
}

Array<Array<Vector *> *> *FileRead::loadWeaponPositions(int32_t id) {
    String path("weapon_positions.bin");

    Array<Array<Vector *> *> *positions = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("weapon_positions.bin", &handle);

        positions = new Array<Array<Vector *> *>();
        ArraySetLength<Array<Vector *> *>(4, *positions);
        for (uint32_t i = 0; i != positions->size(); i++) {
            positions->data()[i] = 0;
        }

        int16_t ship = 0;
        int16_t count = 0;
        int16_t type = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;

        do {
            AEFile::Read(ship, handle);
            AEFile::Read(count, handle);
            for (int32_t i = 0; i < count; i++) {
                AEFile::ReadSwitched(type, handle);
                AEFile::ReadSwitched(x, handle);
                AEFile::ReadSwitched(y, handle);
                AEFile::ReadSwitched(z, handle);

                float extraX = 0.0f;
                float extraY = 0.0f;
                float extraZ = 0.0f;
                if (type == 3) {
                    AEFile::ReadSwitched(reinterpret_cast<int32_t &>(extraX), handle);
                    AEFile::ReadSwitched(reinterpret_cast<int32_t &>(extraY), handle);
                    AEFile::ReadSwitched(reinterpret_cast<int32_t &>(extraZ), handle);
                }
                if (ship == id) {
                    if (positions->data()[type] == 0) {
                        positions->data()[type] = new Array<Vector *>();
                    }
                    Vector *pos = new Vector;
                    pos->x = (float) x;
                    pos->y = (float) z;
                    pos->z = (float) -y;
                    ArrayAdd<Vector *>(pos, *positions->data()[type]);
                    if (type == 3) {
                        Vector *extra = new Vector;
                        extra->x = extraX;
                        extra->y = extraZ;
                        extra->z = extraY;
                        ArrayAdd<Vector *>(extra, *positions->data()[3]);
                    }
                }
            }
        } while (ship != id);
        AEFile::Close(handle);
    }
    return positions;
}

Array<SpacePoint *> *FileRead::loadSpacePoints(int32_t id, int32_t group) {
    // Android HD native libgof2hdaa.so opens this file; "space_points.bin" is a stale decomp name.
    String path("data/bin/docks_hd.bin");

    Array<SpacePoint *> *points = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("data/bin/docks_hd.bin", &handle);

        uint16_t current = 0;
        uint16_t count = 0;
        do {
            AEFile::Read(current, handle);
            AEFile::Read(count, handle);
            if ((uint32_t) current != (uint32_t) id) {
                AEFile::Skip((uint32_t) count * 0x26, handle);
            }
        } while ((uint32_t) current != (uint32_t) id);

        points = new Array<SpacePoint *>();

        const float negativeAngleScale = -0.017453292f;
        const float positiveAngleScale = 0.017453292f;

        for (uint32_t i = 0; i < count; i++) {
            uint16_t type;
            AEFile::Read(type, handle);

            float a;
            float b;
            float c;
            float rx;
            float ry;
            float rz;
            float ignored0;
            float ignored1;
            float ignored2;
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(a), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(b), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(c), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(rx), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(ry), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(rz), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(ignored0), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(ignored1), handle);
            AEFile::ReadSwitched(reinterpret_cast<int32_t &>(ignored2), handle);

            Vector position;
            position.x = a;
            position.y = c;
            position.z = -b;

            Matrix matrix;
            MatrixIdentity(matrix);
            Matrix rotation =
                    MatrixSetRotation(matrix, rx * negativeAngleScale, rz * negativeAngleScale, ry * positiveAngleScale, AbyssEngine::AEMath::ROTATION_ORDER_XZY);
            Vector direction = MatrixGetDir(rotation);

            uint32_t selected = 0;
            if (group != -1) {
                selected = i >> 1;
            }
            if (group == -1 || selected == (uint32_t) group) {
                SpacePoint *point = new SpacePoint((int32_t) type, position, direction, (int32_t) i);
                ArrayAdd<SpacePoint *>(point, *points);
            }
        }
        AEFile::Close(handle);
    }
    return points;
}

Array<SolarSystem *> *FileRead::loadSystemsBinary() {
    String path("systems_binary.bin");

    Array<SolarSystem *> *systems = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("systems_binary.bin", &handle);

        systems = new Array<SolarSystem *>();
        ArraySetLength<SolarSystem *>(0x22, *systems);

        String name;
        for (uint32_t i = 0; i < 0x22; i++) {
            int32_t faction;
            int32_t flag;
            int32_t a;
            int32_t b;
            int32_t c;
            int32_t d;
            int32_t e;
            int32_t f;
            uint32_t count;

            AEFile::ReadSwitched(name, handle);
            name.ConvertFromUTF8();
            AEFile::ReadSwitched(faction, handle);
            AEFile::ReadSwitched(flag, handle);
            AEFile::ReadSwitched(a, handle);
            AEFile::ReadSwitched(b, handle);
            AEFile::ReadSwitched(c, handle);
            AEFile::ReadSwitched(d, handle);
            AEFile::ReadSwitched(e, handle);
            AEFile::ReadSwitched(f, handle);

            AEFile::ReadSwitched((int32_t &) count, handle);
            int32_t *routes = new int32_t[count];
            for (int32_t j = 0; j < (int32_t) count; j++) {
                AEFile::ReadSwitched(routes[j], handle);
            }

            AEFile::ReadSwitched((int32_t &) count, handle);
            Array<int32_t> *stations;
            if ((int32_t) count < 1) {
                stations = 0;
            } else {
                stations = new Array<int32_t>();
                ArraySetLength<int32_t>(count, *stations);
                for (uint32_t j = 0; j < stations->size(); j++) {
                    AEFile::ReadSwitched(stations->data()[j], handle);
                }
            }

            AEFile::ReadSwitched((int32_t &) count, handle);
            Array<int32_t> *wrecks;
            if ((int32_t) count < 1) {
                wrecks = 0;
            } else {
                wrecks = new Array<int32_t>();
                ArraySetLength<int32_t>(count, *wrecks);
                for (uint32_t j = 0; j < wrecks->size(); j++) {
                    AEFile::ReadSwitched(wrecks->data()[j], handle);
                }
            }

            AEFile::ReadSwitched((int32_t &) count, handle);
            Array<int32_t> *statics;
            if ((int32_t) count < 1) {
                statics = 0;
            } else {
                statics = new Array<int32_t>();
                ArraySetLength<int32_t>(count, *statics);
                for (uint32_t j = 0; j < statics->size(); j++) {
                    AEFile::ReadSwitched(statics->data()[j], handle);
                }
            }

            SolarSystem *system = new SolarSystem((int32_t) i, name, faction, flag == 1, a, b, c, d, e, f,
                                                  routes, stations, wrecks, statics);
            systems->data()[i] = system;
            delete[] routes;
        }
        AEFile::Close(handle);
    }
    return systems;
}

Array<Wanted *> *FileRead::loadWanted() {
    String path("wanted_binary.bin");

    Array<Wanted *> *wanted = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("wanted_binary.bin", &handle);

        wanted = new Array<Wanted *>();
        ArraySetLength<Wanted *>(0x19, *wanted);

        String name;
        uint32_t index = 0;
        while (index < wanted->size()) {
            int32_t id;
            int32_t a;
            int32_t b;
            int32_t c;
            int32_t d;
            int32_t e;
            int32_t f;
            int32_t g;
            int32_t h;
            int32_t i;
            int32_t j;
            int32_t k;
            int32_t l;
            int32_t imageCount;

            AEFile::ReadSwitched(name, handle);
            name.ConvertFromUTF8();
            AEFile::ReadSwitched(id, handle);
            AEFile::ReadSwitched(a, handle);
            AEFile::ReadSwitched(b, handle);
            AEFile::ReadSwitched(c, handle);
            AEFile::ReadSwitched(d, handle);
            AEFile::ReadSwitched(e, handle);
            AEFile::ReadSwitched(f, handle);
            AEFile::ReadSwitched(g, handle);
            AEFile::ReadSwitched(h, handle);
            AEFile::ReadSwitched(i, handle);
            AEFile::ReadSwitched(j, handle);
            AEFile::ReadSwitched(k, handle);
            AEFile::ReadSwitched(l, handle);

            Wanted *entry = new Wanted(id, name, a, b, c == 1, d, e, f, g, h, i, j, k, l);
            wanted->data()[index] = entry;

            AEFile::ReadSwitched(imageCount, handle);
            uint32_t oldIndex = index++;
            if (imageCount > 0) {
                int32_t *parts = new int32_t[5];
                for (uint32_t n = 0; n < 5; n++) {
                    char part;
                    AEFile::Read(part, handle);
                    parts[n] = part;
                }
                wanted->data()[oldIndex]->setImageParts(parts);
            }
        }
        AEFile::Close(handle);
    }
    return wanted;
}

Array<NewsItem *> *FileRead::loadTicker() {
    String path("ticker_binary.bin");

    Array<NewsItem *> *items = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("ticker_binary.bin", &handle);

        items = new Array<NewsItem *>();
        ArraySetLength<NewsItem *>(0x3b, *items);

        for (uint32_t i = 0; i < 0x3b; i++) {
            int32_t active;
            int32_t flags[4];
            int32_t a;
            int32_t b;
            AEFile::ReadSwitched(active, handle);
            AEFile::ReadSwitched(flags[0], handle);
            AEFile::ReadSwitched(flags[1], handle);
            AEFile::ReadSwitched(flags[2], handle);
            AEFile::ReadSwitched(flags[3], handle);
            AEFile::ReadSwitched(a, handle);
            AEFile::ReadSwitched(b, handle);

            bool *bits = new bool[4];
            for (int32_t j = 0; j != 4; j++) {
                bits[j] = flags[j] != 0;
            }

            NewsItem *item = new NewsItem((int32_t) i, active != 0, bits, 4, a, b);
            items->data()[i] = item;
        }
        AEFile::Close(handle);
    }
    return items;
}

Array<Station *> *FileRead::loadStationsBinary(int16_t *ids, int32_t count) {
    Array<Station *> *stations = new Array<Station *>();
    ArraySetLength<Station *>(count, *stations);

    String path("stations_binary.bin");
    if (AEFile::FileExist(path) == 0) {
        return 0;
    }

    uint32_t handle;
    AEFile::OpenRead("stations_binary.bin", &handle);

    String name;
    int32_t out = 0;
    for (uint32_t stationId = 0; stationId < 0x87; stationId++) {
        int32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
        AEFile::ReadSwitched(name, handle);
        name.ConvertFromUTF8();
        AEFile::ReadSwitched(a, handle);
        AEFile::ReadSwitched(b, handle);
        AEFile::ReadSwitched(c, handle);
        AEFile::ReadSwitched(d, handle);
        for (int32_t i = 0; i < count; i++) {
            if (stationId == (uint32_t) ids[i]) {
                Station *station = new Station(name, a, b, c, d);
                stations->data()[out] = station;
                out++;
            }
        }
    }
    AEFile::Close(handle);
    return stations;
}

static AbyssEngine::AERandom *gNameRandomA = nullptr;
static AbyssEngine::AERandom *gNameRandomB = nullptr;

Array<String *> *FileRead::loadNamesBinary(int32_t type, bool first, bool second) {
    Array<String *> *names = 0;
    String path;

    switch (type) {
        case 0: {
            const char *text = "names_0_ab";
            if (!first) {
                text = "names_0_b";
            }
            if (!second) {
                text = "names_0_a";
            }
            path = text;
            break;
        }
        case 1: {
            path = second ? "names_1_b" : "names_1_a";
            break;
        }
        case 2: {
            path = second ? "names_2_b" : "names_2_a";
            break;
        }
        case 3: {
            if (second) {
                path = gNameRandomA->nextInt(2) != 0 ? "names_3_b" : "names_3_a";
            } else {
                path = gNameRandomA->nextInt(2) != 0 ? "names_3_d" : "names_3_c";
            }
            break;
        }
        case 4: {
            path = second ? "names_4_b" : "names_4_a";
            break;
        }
        case 5: {
            if (!second) {
                return names;
            }
            path = "names_5";
            break;
        }
        case 6: {
            path = second ? "names_6_b" : "names_6_a";
            break;
        }
        case 7: {
            if (!second) {
                return names;
            }
            path = "names_7";
            break;
        }
        case 8: {
            if (second) {
                path = gNameRandomB->nextInt(2) != 0 ? "names_8_b" : "names_8_a";
            } else {
                path = gNameRandomB->nextInt(2) != 0 ? "names_8_d" : "names_8_c";
            }
            break;
        }
        default:
            return names;
    }

    path = String(".bin") + path;

    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        uint32_t count;
        AEFile::OpenRead(path, &handle);
        AEFile::ReadSwitched((int32_t &) count, handle);

        names = new Array<String *>();
        ArraySetLength<String *>(count, *names);

        String tmp;
        for (int32_t i = 0; i < (int32_t) count; i++) {
            AEFile::ReadSwitched(tmp, handle);
            names->data()[i] = new String(tmp);
        }
        AEFile::Close(handle);
    }

    return names;
}

Array<Station *> *FileRead::loadStationsBinary(SolarSystem *system) {
    Array<Station *> *stations = new Array<Station *>();

    String path("stations_binary.bin");
    if (AEFile::FileExist(path) == 0) {
        return 0;
    }

    uint32_t handle;
    AEFile::OpenRead("stations_binary.bin", &handle);

    Array<int32_t> *ids = system->stationIds;
    ArraySetLength<Station *>(ids->size(), *stations);

    String name;
    uint32_t out = 0;
    for (uint32_t stationId = 0; stationId < 0x87; stationId++) {
        int32_t a;
        int32_t b;
        int32_t c;
        int32_t d;
        AEFile::ReadSwitched(name, handle);
        name.ConvertFromUTF8();
        AEFile::ReadSwitched(a, handle);
        AEFile::ReadSwitched(b, handle);
        AEFile::ReadSwitched(c, handle);
        AEFile::ReadSwitched(d, handle);
        for (uint32_t i = 0; i < ids->size(); i++) {
            if ((uint32_t) ids->data()[i] == stationId) {
                Station *station = new Station(name, a, b, c, d);
                stations->data()[out++] = station;
            }
            if (out == ids->size()) {
                AEFile::Close(handle);
                return stations;
            }
        }
    }
    AEFile::Close(handle);
    return stations;
}

Array<Agent *> *FileRead::loadAgents() {
    String path("agents_binary.bin");

    Array<Agent *> *agents = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("agents_binary.bin", &handle);

        agents = new Array<Agent *>();
        ArraySetLength<Agent *>(0x1b, *agents);

        String name;
        uint32_t index = 0;
        while (index < agents->size()) {
            int32_t id;
            int32_t a;
            int32_t b;
            int32_t c;
            int32_t flag;
            int32_t d;
            int32_t e;
            int32_t f;
            int32_t g;
            int32_t imageCount;

            AEFile::ReadSwitched(name, handle);
            name.ConvertFromUTF8();
            AEFile::ReadSwitched(id, handle);
            AEFile::ReadSwitched(a, handle);
            AEFile::ReadSwitched(b, handle);
            AEFile::ReadSwitched(c, handle);
            AEFile::ReadSwitched(flag, handle);
            AEFile::ReadSwitched(d, handle);
            AEFile::ReadSwitched(e, handle);
            AEFile::ReadSwitched(f, handle);
            AEFile::ReadSwitched(g, handle);

            Agent *agent = new Agent(id, name, a, b, c, flag == 1, d, e, f, g);
            agents->data()[index] = agent;

            AEFile::ReadSwitched(imageCount, handle);
            uint32_t oldIndex = index++;
            if (imageCount > 0) {
                int32_t *parts = new int32_t[5];
                for (uint32_t i = 0; i < 5; i++) {
                    char part;
                    AEFile::Read(part, handle);
                    parts[i] = part;
                }
                agents->data()[oldIndex]->setImageParts(parts);
            }
        }
        AEFile::Close(handle);
    }
    return agents;
}

Array<int32_t> *FileRead::loadWreckCollision(int32_t id) {
    String path("wreck_collision.bin");

    Array<int32_t> *result = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("wreck_collision.bin", &handle);

        int32_t key = 0;
        uint32_t count = 0;
        for (uint32_t i = 0; i < 6; i++) {
            AEFile::Read(key, handle);
            AEFile::Read((int32_t &) count, handle);
            count++;
            if (key == id) {
                result = new Array<int32_t>();
                int32_t *buffer = new int32_t[count];
                AEFile::Read(count << 2, buffer, handle);
                ArraySetLength<int32_t>(count, *result);
                for (int32_t j = 0; j < (int32_t) count; j++) {
                    result->data()[j] = buffer[j];
                }
                delete[] buffer;
                AEFile::Close(handle);
                return result;
            }
            AEFile::Skip(count << 2, handle);
        }
        AEFile::Close(handle);
    }
    return result;
}

Array<int32_t> *FileRead::loadStationCollision(int32_t id) {
    String path("station_collision.bin");

    Array<int32_t> *result = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("station_collision.bin", &handle);

        int32_t key = 0;
        uint32_t count = 0;
        for (uint32_t i = 0; i < 0x88; i++) {
            AEFile::Read(key, handle);
            AEFile::Read((int32_t &) count, handle);
            count++;
            if (key == id) {
                result = new Array<int32_t>();
                int32_t *buffer = new int32_t[count];
                AEFile::Read(count << 2, buffer, handle);
                ArraySetLength<int32_t>(count, *result);
                for (int32_t j = 0; j < (int32_t) count; j++) {
                    result->data()[j] = buffer[j];
                }
                delete[] buffer;
                AEFile::Close(handle);
                return result;
            }
            AEFile::Skip(count << 2, handle);
        }
        AEFile::Close(handle);
    }
    return result;
}

Array<int32_t> *FileRead::loadStaticCollision(int32_t id) {
    String path("static_collision.bin");

    Array<int32_t> *result = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("static_collision.bin", &handle);

        int32_t key = 0;
        uint32_t count = 0;
        for (uint32_t i = 0; i < 7; i++) {
            AEFile::Read(key, handle);
            AEFile::Read((int32_t &) count, handle);
            count++;
            if (key == id) {
                result = new Array<int32_t>();
                int32_t *buffer = new int32_t[count];
                AEFile::Read(count << 2, buffer, handle);
                ArraySetLength<int32_t>(count, *result);
                for (int32_t j = 0; j < (int32_t) count; j++) {
                    result->data()[j] = buffer[j];
                }
                delete[] buffer;
                AEFile::Close(handle);
                return result;
            }
            AEFile::Skip(count << 2, handle);
        }
        AEFile::Close(handle);
    }
    return result;
}

Array<int32_t> *FileRead::loadStationParts(int32_t id, int32_t special) {
    String path("station_parts.bin");

    Array<int32_t> *parts = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("station_parts.bin", &handle);

        int32_t wanted = id + 1;
        if (special == 1) {
            wanted = 0x65;
        }

        for (uint32_t i = 0; i < 0x88; i++) {
            char group;
            char count;
            int16_t station;
            AEFile::Read(group, handle);
            AEFile::ReadSwitched(station, handle);
            AEFile::Read(count, handle);

            parts = new Array<int32_t>();
            ArraySetLength<int32_t>((int8_t) count * 7 + 7, *parts);
            int32_t *data = parts->data();
            data[0] = station;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            data[5] = 0x800;
            data[6] = 0;

            for (uint32_t off = 7; off < parts->size(); off += 7) {
                int16_t value;
                AEFile::ReadSwitched(value, handle);
                data[off] = value;
                AEFile::ReadSwitched(data[off + 1], handle);
                AEFile::ReadSwitched(data[off + 2], handle);
                AEFile::ReadSwitched(data[off + 3], handle);
                AEFile::ReadSwitched(value, handle);
                data[off + 4] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 5] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 6] = value;
            }

            if (wanted == (int8_t) group) {
                return parts;
            }
            delete parts;
            parts = 0;
        }
        AEFile::Close(handle);
    }
    return parts;
}

Array<int32_t> *FileRead::loadShipParts(int32_t id) {
    String path("ship_parts.bin");

    Array<int32_t> *parts = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("ship_parts.bin", &handle);

        int32_t wanted = id + 1;
        for (uint32_t i = 0; i <= 0x40; i++) {
            if (i > 0x3f) {
                AEFile::Close(handle);
                break;
            }
            char group;
            char count;
            AEFile::Read(group, handle);
            AEFile::Read(count, handle);

            parts = new Array<int32_t>();
            ArraySetLength<int32_t>((int8_t) count * 10, *parts);
            int32_t *data = parts->data();
            for (uint32_t off = 0; off < parts->size(); off += 10) {
                int16_t value;
                AEFile::ReadSwitched(value, handle);
                data[off] = value;
                AEFile::ReadSwitched(data[off + 1], handle);
                AEFile::ReadSwitched(data[off + 2], handle);
                AEFile::ReadSwitched(data[off + 3], handle);
                AEFile::ReadSwitched(value, handle);
                data[off + 4] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 5] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 6] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 7] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 8] = value;
                AEFile::ReadSwitched(value, handle);
                data[off + 9] = value;
            }
            if (wanted == (int8_t) group) {
                break;
            }
        }
    }
    return parts;
}

Array<Item *> *FileRead::loadItemsBinary() {
    String path("items_binary.bin");

    Array<Item *> *items = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("items_binary.bin", &handle);

        items = new Array<Item *>();
        ArraySetLength<Item *>(0xe9, *items);

        for (uint32_t i = 0; i < 0xe9; i++) {
            uint32_t count0 = 0;
            uint32_t count1 = 0;
            uint32_t count2 = 0;
            Array<int> *a0;
            Array<int> *a1;
            Array<int> *a2;

            AEFile::ReadSwitched((int32_t &) count0, handle);
            if ((int32_t) count0 < 1) {
                a0 = 0;
            } else {
                a0 = new Array<int>();
                ArraySetLength<int32_t>(count0, *a0);
                for (int32_t j = 0; j < (int32_t) count0; j++) {
                    AEFile::ReadSwitched(a0->data()[j], handle);
                }
            }

            AEFile::ReadSwitched((int32_t &) count1, handle);
            if ((int32_t) count1 < 1) {
                a1 = 0;
            } else {
                a1 = new Array<int>();
                ArraySetLength<int32_t>(count1, *a1);
                for (int32_t j = 0; j < (int32_t) count1; j++) {
                    AEFile::ReadSwitched(a1->data()[j], handle);
                }
            }

            AEFile::ReadSwitched((int32_t &) count2, handle);
            if ((int32_t) count2 < 1) {
                a2 = 0;
            } else {
                a2 = new Array<int>();
                ArraySetLength<int32_t>(count2, *a2);
                for (int32_t j = 0; j < (int32_t) count2; j++) {
                    AEFile::ReadSwitched(a2->data()[j], handle);
                }
            }

            Item *item = new Item(a0, a1, a2);
            items->data()[i] = item;
        }
        AEFile::Close(handle);
    }
    return items;
}

Array<Ship *> *FileRead::loadShipsBinary() {
    String path("ships_binary.bin");

    Array<Ship *> *ships = 0;
    if (AEFile::FileExist(path) != 0) {
        uint32_t handle;
        AEFile::OpenRead("ships_binary.bin", &handle);

        ships = new Array<Ship *>();
        ArraySetLength<Ship *>(0x40, *ships);

        for (uint32_t i = 0; i < 0x40; i++) {
            int32_t a;
            int32_t b;
            int32_t c;
            int32_t d;
            int32_t e;
            int32_t f;
            int32_t g;
            int32_t h;
            int32_t speed;
            AEFile::ReadSwitched(a, handle);
            AEFile::ReadSwitched(b, handle);
            AEFile::ReadSwitched(c, handle);
            AEFile::ReadSwitched(d, handle);
            AEFile::ReadSwitched(e, handle);
            AEFile::ReadSwitched(f, handle);
            AEFile::ReadSwitched(g, handle);
            AEFile::ReadSwitched(h, handle);
            AEFile::ReadSwitched(speed, handle);
            Ship *ship = new Ship(a, b, c, d, e, f, g, h, (float) speed);
            ships->data()[i] = ship;
        }
        AEFile::Close(handle);
    }
    return ships;
}
