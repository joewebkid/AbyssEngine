#ifndef GOF2_ABYSSENGINE_H
#define GOF2_ABYSSENGINE_H
#include "engine/core/Array.h"
#include "AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
#include "engine/render/Engine.h"
#include "engine/render/Material.h"
#include "engine/render/ImageFont.h"
#include "engine/render/SpriteSystem.h"
#include "engine/render/Curve.h"
#include "engine/render/Image.h"
#include "engine/render/Image2D.h"
#include "engine/render/AELoadedTexture.h"

#include "engine/math/AEMath.h"


namespace AbyssEngine {
    class Engine;
    class Material;
    class PaintCanvas;
}

using ::AbyssEngine::Engine;
using ::AbyssEngine::Material;
using ::AbyssEngine::PaintCanvas;

unsigned int AELabelObject(unsigned int glIdentifier, unsigned int name, const char *label);

namespace AbyssEngine {
    using AEMath::Vector;
    using AEMath::Matrix;

    struct ESMatrix {
        float m[4][4];
    };

    extern int currentUsedShaderIndex;

    // Namespace-scope globals present in the original binary (defined for symbol parity).
    extern float currentFps;
    extern float debugTouch[4];
    extern int fpsCounter;
    extern int SwapCounter;
    extern unsigned char firstRender;
    extern int loadTexture;
    extern float currentLODBias;
    extern int CubeMapSetIndex;
    extern int debugModusIndex;
    extern unsigned char firstRenderGlow;
    extern unsigned char performanceTest;
    extern long long performanceTime;
    extern float timeBetweenFrames;
    extern unsigned char BiasErrorOutputFlag;
    extern unsigned char performanceTestEnded;
    extern float orientationChangeTimer;
    extern long long performanceFrameCounter;
    extern long long performanceModulSwitches;
    extern long long performanceOverallModulSwitches;
    extern int lauf;
    extern float i_fov;
    extern float i_zFar;
    extern float i_zNear;
    extern float fpsTimer;
    extern int loadMesh;
    extern long long triDrawn;
    extern unsigned char ShaderSet;

    // ImageFont / SpriteSystem / Curve / Image / Image2D / AELoadedTexture
    // are each defined in their own header (included above).
}

#endif
