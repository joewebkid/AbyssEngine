#ifndef GOF2_FBOCONTAINER_H
#define GOF2_FBOCONTAINER_H
#include "../core/AEString.h"

namespace AbyssEngine {
    class Engine;

    class FBOContainer {
    public:
        uint32_t framebuffer;
        uint32_t texture;
        uint32_t renderbuffer;
        int32_t width;
        int32_t height;
        Engine *engine;
        uint8_t created;
        String name;
        uint8_t field_0x28;
        uint32_t extraRenderbuffer0;
        uint32_t extraRenderbuffer1;
        uint32_t extraRenderbuffer2;

        FBOContainer(Engine *engine, String name);

        ~FBOContainer();

        void Create(int width, int height, bool a, bool linear);

        void Release();

        void Activate();

        void BeginCapture();

        void EndCapture();
    };
}

#endif
