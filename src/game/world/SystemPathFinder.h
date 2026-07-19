#ifndef GOF2_SYSTEMPATHFINDER_H
#define GOF2_SYSTEMPATHFINDER_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/Node.h"
#include "game/world/SolarSystem.h"
#include "game/mission/Status.h"

class Node;
class SolarSystem;
class Status;


class SystemPathFinder {
public:
    SystemPathFinder();

    ~SystemPathFinder();

    int contains(Array<Node *> *nodes, Node *node);

    Array<Node *> *search(Node *start, Node *goal);

    int getJumpDistance(Array<SolarSystem *> *systems, int from, int to);

    Array<Node *> *constructPath(Node *node);

    Array<int> *getSystemPath(Array<SolarSystem *> *systems, int from, int to);
};
#endif
