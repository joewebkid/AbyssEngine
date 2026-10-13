#include <cassert>
#include <cstdio>
#include <vector>

struct ImagePart {};
template <typename T> struct Array {};

struct ImageFactory {
    Array<ImagePart *> parts;
    int loads = 0;
    int *lastDescriptor = nullptr;
    Array<ImagePart *> *loadChar(int *descriptor) {
        ++loads;
        lastDescriptor = descriptor;
        for (int i = 0; i < 5; ++i) assert(descriptor[i] == 0);
        return &parts;
    }
};

struct Standing {
    bool enemies[4] = {};
    std::vector<int> queries;
    int isEnemy(int index) {
        queries.push_back(index);
        return enemies[index];
    }
};

struct Status {
    Standing standing;
    int standingReads = 0;
    Standing *getStanding() { ++standingReads; return &standing; }
};

struct PaintCanvas {
    std::vector<unsigned short> imageIds;
    unsigned measuredWidth = 0;
    unsigned measuredHeight = 0;
    void Image2DCreate(unsigned short id, unsigned &out) {
        imageIds.push_back(id);
        out = static_cast<unsigned>(imageIds.size());
    }
    int GetImage2DWidth(unsigned handle) { measuredWidth = handle; return 64; }
    int GetImage2DHeight(unsigned handle) { measuredHeight = handle; return 32; }
};

struct Globals {
    static void *imageFactory;
    static Status *status;
    static PaintCanvas *Canvas;
};
void *Globals::imageFactory = nullptr;
Status *Globals::status = nullptr;
PaintCanvas *Globals::Canvas = nullptr;

struct StatusWindow {
    Array<ImagePart *> *imageParts = nullptr;
    unsigned rankImage0 = 0, rankImage1 = 0, rankImage2 = 0, rankImage3 = 0;
    int charImageWidth = 0, charImageHeight = 0;
    void reInit();
};

static int g_sw_charDef[5] = {};
// PRODUCTION_REINIT

static void checkCase(StatusWindow &window, Status &status, PaintCanvas &canvas,
                      const std::vector<unsigned short> &expected) {
    status.standing.queries.clear();
    status.standingReads = 0;
    canvas.imageIds.clear();
    window.reInit();
    assert(status.standingReads == 4);
    assert((status.standing.queries == std::vector<int>{0, 1, 2, 3}));
    assert(canvas.imageIds == expected);
    assert(window.rankImage0 == 1 && window.rankImage1 == 2 &&
           window.rankImage2 == 3 && window.rankImage3 == 4);
    assert(canvas.measuredWidth == window.rankImage0);
    assert(canvas.measuredHeight == window.rankImage0);
    assert(window.charImageWidth == 64 && window.charImageHeight == 32);
}

int main() {
    ImageFactory factory;
    Status status;
    PaintCanvas canvas;
    StatusWindow window;
    Globals::imageFactory = &factory;
    Globals::status = &status;
    Globals::Canvas = &canvas;

    checkCase(window, status, canvas, {0x493, 0x492, 0x490, 0x491});
    status.standing.enemies[1] = true;
    status.standing.enemies[2] = true;
    checkCase(window, status, canvas, {0x494, 0x497, 0x499, 0x49a});
    status.standing.enemies[0] = true;
    status.standing.enemies[3] = true;
    checkCase(window, status, canvas, {0x495, 0x497, 0x499, 0x49b});
    assert(factory.loads == 3 && factory.lastDescriptor == g_sw_charDef);
    assert(window.imageParts == &factory.parts);
    std::puts("StatusWindow reInit host smoke: OK");
}
