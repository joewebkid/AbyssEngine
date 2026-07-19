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
    uint32_t reserved_14;
    uint32_t reserved_18;
    int target[6];
    int current[6];
    int working[6];
    int tileImages[48];
    int topImage;
    int mainImage;
    int bottomImage;
    int arrowActive;
    int arrowIdle;
    int markImage;
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
#endif
