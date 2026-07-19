#ifndef GOF2_SHADERBASESTRUCT_H
#define GOF2_SHADERBASESTRUCT_H
#include "Engine.h"
#include "FBOContainer.h"
#include "../core/AEString.h"
#include "Mesh.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"


namespace AbyssEngine { 
    class Engine;
    class FBOContainer;
    class Mesh;
 }


namespace AbyssEngine {
    class ShaderBaseStruct {
    public:
        int program;
        volatile uint16_t flags;
        uint8_t dirty;
        String name;
        const char *vertexPath;
        const char *fragmentPath;

        static int shaderIndexIntern;

        ShaderBaseStruct();

        virtual ~ShaderBaseStruct();

        virtual void Init(Engine *engine) = 0;

        virtual void UpdateMeshData(Mesh *mesh, Engine *engine) = 0;

        virtual void SetInActive() = 0;

        virtual void RenderEffect(FBOContainer *source, Engine *engine);

        virtual void RenderEffect(FBOContainer *source, FBOContainer *&target, AbyssEngine::Engine *engine);

        FBOContainer *RenderEffect(FBOContainer *source, Engine *engine, float strength, AEMath::Vector tint);

        FBOContainer *RenderEffect(FBOContainer *source, FBOContainer *&target, Engine *engine, float strength,
                                   AEMath::Vector tint);

        virtual void DeleteShader();

        virtual void UseShader(bool useExtra);

        String GetShaderName();

        void Update();

        uint32_t ES2LoadProgram(const char *vertexSource, const char *fragmentSource);

        uint32_t ES2LoadShader(uint32_t type, const char *source);

        uint32_t LoadBindShader(const char *vertexPath, const char *fragmentPath);
    };
}

#endif
