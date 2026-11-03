#ifndef GOF2_HACKINGGAME_H
#define GOF2_HACKINGGAME_H
#include <cstdint>

class HackingGame {
public:
    HackingGame(int type, int canvas, int rewardItem, int rewardAmount, int dockingIndex);

    ~HackingGame();

    int getDockingIndex();

    int getRewardAmount();

    int getRewardItem();

    bool isRotating();

    int gameWon();

    int gameWon(int *state);

    void rotateLeftCW(bool sound);

    void rotateLeftCW(int *state);

    void rotateRightCW(bool sound);

    void rotateRightCW(int *state);

    void render2D();

    void reInit();

    int solvableInNSteps(int steps, int depth, int leftCount, int rightCount, int *state);

    int update(int dt);

    int difficulty;
    int target[6];
    int current[6];
    int working[6];
    int tileImages[48];
    unsigned int topImage;
    unsigned int mainImage;
    unsigned int bottomImage;
    unsigned int arrowActive;
    unsigned int arrowIdle;
    unsigned int markImage;
    int type;
    bool rotatingLeft;
    bool rotatingRight;
    uint16_t pad_12a;
    int rotateTimer;
    int wonTimer;
    int rewardItem;
    int rewardAmount;
    int dockingIndex;
    uint8_t trailingState[0x320 - 0x140];
};

static_assert(__builtin_offsetof(HackingGame, target) == 0x04, "HackingGame target offset");
static_assert(__builtin_offsetof(HackingGame, current) == 0x1c, "HackingGame current offset");
static_assert(__builtin_offsetof(HackingGame, tileImages) == 0x4c, "HackingGame tile offset");
static_assert(__builtin_offsetof(HackingGame, type) == 0x124, "HackingGame type offset");
static_assert(__builtin_offsetof(HackingGame, wonTimer) == 0x130, "HackingGame wonTimer offset");
static_assert(__builtin_offsetof(HackingGame, dockingIndex) == 0x13c, "HackingGame docking offset");
static_assert(sizeof(HackingGame) == 0x320, "HackingGame size");
#endif
