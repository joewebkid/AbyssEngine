#ifndef GOF2_AUTOPILOTLIST_H
#define GOF2_AUTOPILOTLIST_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/world/Level.h"

class Level;


class AutoPilotList {
public:
    int selected;
    int x;
    int y;
    int width;
    Array<String *> *entries;
    int count;

    AutoPilotList(Level *level);

    ~AutoPilotList();

    void draw();

    void up();

    void down();

    String getTargetString();

    int touch(int px, int py);

    int fire();
};
#endif
