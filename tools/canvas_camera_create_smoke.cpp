#include <cassert>
#include <cstdio>
#include <new>

namespace AbyssEngine {
struct Camera {
    float args[5];
    Camera(float a, float b, float c, float d, float e)
        : args{a, b, c, d, e} {}
};

template<class T> struct Array {
    unsigned int count = 0;
    T data[2]{};
};
template<class T> void ArrayAdd(T value, Array<T> &array) {
    array.data[array.count++] = value;
}

struct PaintCanvas {
    Array<Camera *> cameras;
    int width;
    int height;
    int GetWidth() { return width; }
    int GetHeight() { return height; }
    void CameraCreate(unsigned int &out);
};

static inline int pc_GetWidth(PaintCanvas *canvas) { return canvas->GetWidth(); }
static inline int pc_GetHeight(PaintCanvas *canvas) { return canvas->GetHeight(); }

// PRODUCTION_METHOD
}

int main() {
    AbyssEngine::PaintCanvas canvas{{}, 1024, 768};
    unsigned int id = 99;
    canvas.CameraCreate(id);
    assert(id == 0 && canvas.cameras.count == 1);
    const auto *first = canvas.cameras.data[0];
    assert(first->args[0] == 8000.0f && first->args[1] == 256.0f);
    assert(first->args[2] == 16384.0f);
    assert(first->args[3] == 1024.0f && first->args[4] == 768.0f);

    canvas.width = 640;
    canvas.height = 480;
    canvas.CameraCreate(id);
    assert(id == 1 && canvas.cameras.count == 2);
    const auto *second = canvas.cameras.data[1];
    assert(second != first);
    assert(second->args[3] == 640.0f && second->args[4] == 480.0f);

    delete canvas.cameras.data[0];
    delete canvas.cameras.data[1];
    std::puts("PaintCanvas camera creation production smoke: OK");
}
