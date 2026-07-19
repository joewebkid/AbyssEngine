#ifndef GOF2_ENGINE_H
#define GOF2_ENGINE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Vector.h"
#include "engine/math/Matrix.h"

#include "engine/math/AEMath.h"


#include "engine/render/DeviceInfo.h"
#include "engine/render/LightColor.h"
namespace AbyssEngine { 
    class FBOContainer;
    class Mesh;
    class ShaderBaseStruct;
 }


namespace AbyssEngine {
    class Engine;
    class ApplicationManager;
    enum LandscapeMode : int;

    extern bool PostEffectFlag;
}

using ::AbyssEngine::ApplicationManager;

typedef void DestroyCallback(AbyssEngine::Engine *);

typedef void InitializeCallback(AbyssEngine::Engine *);

void glError();



namespace AbyssEngine {
    class Engine {
    public:
        uint32_t deviceWidth;
        uint32_t deviceHeight;
        uint8_t isPad;

        union {
            uint32_t maxTextureSize;
            uint32_t depthBits;
        };

        union {
            uint32_t lastGlError;
            uint32_t field_0x10;
        };

        String lastErrorPath;

        union {
            uint8_t linearFilterFlag;
            uint8_t field_0x20;
        };

        void *fileInterface;

        union {
            int field_0x28;
            float explosionTimeline;
        };

        bool vibrationSupported;
        uint8_t _pad0x2d[3];

        union {
            ApplicationManager *appManager;
            char **field_0x30;
            void *paintCanvas;
        };

        uint32_t field_0x34;
        uint8_t _pad0x38[4];
        String str_0x3c;
        uint8_t _pad0x48[4];
        String str_0x4c;
        uint64_t field_0x58;

        int drawCallCountB;
        int drawCallCountA;
        int triangleCountA;
        int triangleCountB;

        union {
            int textureByteCounter;
            uint32_t field_0x70;
        };

        bool field_0x74;
        uint8_t _pad0x75[3];
        int field_0x78;
        int boundTextures[20];
        float field_0xcc;

        union {
            float glColor[4];

            struct {
                float flCurrentColorR, field_0xd4, field_0xd8, field_0xdc;
            };
        };

        int packedColor;
        uint8_t _pad0xe4[24];
        unsigned char shaderModeFlag;  // 0xfc
        unsigned char statsBucketFlag; // 0xfd
        union {
            uint16_t field_0xfd;       // 0xfe (kept name/offset for cross-file users)
            unsigned char statsEnabled; // 0xfe (low byte)
        };
        uint32_t field_0x100;
        float worldViewProjMatrix[16];
        float modelMatrixGL[16];
        float worldViewMatrixGL[16];
        float uvMatrix[16];

        union {
            float normalMatrix[9];

            struct {
                uint32_t field_0x204, field_0x208, field_0x20c,
                        field_0x210, field_0x214, field_0x218,
                        field_0x21c, field_0x220, field_0x224;
            };
        };

        union {
            LightColor lightDiffuse;

            struct {
                float flLightDiffuseR, flLightDiffuseG, flLightDiffuseB, flLightDiffuseA;
            };
        };
        uint8_t _pad0x238[16];

        union {
            LightColor lightSpecular;

            struct {
                float flLightSpecularR, flLightSpecularG, flLightSpecularB, flLightSpecularA;
            };
        };
        uint8_t _pad0x258[16];

        LightColor lightAmbient;
        uint8_t _pad0x278[16];

        union {
            float sceneAmbient[4];

            struct {
                float field_0x288, field_0x28c, field_0x290;
                uint32_t field_0x294;
            };
        };

        union {
            float materialDiffuse[4];

            struct {
                float field_0x298, field_0x29c, field_0x2a0;
                uint32_t field_0x2a4;
            };
        };

        union {
            float materialAmbient[4];

            struct {
                float field_0x2a8, field_0x2ac, field_0x2b0;
                uint32_t field_0x2b4;
            };
        };

        union {
            float materialSpecular[4];

            struct {
                float field_0x2b8, field_0x2bc, field_0x2c0;
                uint32_t field_0x2c4;
            };
        };

        union {
            float materialShininess;
            float field_0x2c8;
        };

        Vector lightAmbientShaded;
        Vector lightSpecularShaded;
        Vector lightDiffuseShaded;
        Vector particleAmbient;

        Vector field_0x2fc;
        Vector field_0x308;
        Vector field_0x314;

        union {
            Vector rimColor;

            struct {
                float field_0x320, field_0x324, field_0x328;
            };
        };

        union {
            int lightCount;
            int field_0x32c;
        };

        union {
            Vector lightDir;
            Vector field_0x330;
        };

        union {
            Vector field_0x33c;

            struct {
                float field_0x33c_x;
                uint32_t field_0x340;
                float field_0x344;
            };
        };

        union {
            int lineVertexBase;
            int field_0x348;
        };

        union {
            Vector lightColor;

            struct {
                float field_0x34c, field_0x350, field_0x354;
            };
        };

        void *addData;
        int addDataSize;

        union {
            int autoPilotEngaged;
            uint32_t field_0x360;
        };

        uint8_t _pad0x364[4];

        union {
            int framebufferWidth;
            int displayWidth;
        };

        union {
            int framebufferHeight;
            int displayHeight;
        };

        int viewportWidth;
        int viewportHeight;
        float lightDirty[2];

        union {
            Mesh *quadMesh;
            char *field_0x380;
        };

        float projMatrix[16];
        uint8_t field_0x3c4;
        uint32_t field_0x3c8;

        uint32_t field_0x3cc;
        uint32_t field_0x3d0;
        uint32_t field_0x3d4;
        uint32_t field_0x3d8;
        Array<int> *triangleCounts;
        uint32_t field_0x3e0;

        union {
            int currentProgram;
            int field_0x3e4;
        };

        union {
            float fogMinDist;
            uint32_t field_0x3e8;
        };

        union {
            float fogMaxDist;
            uint32_t field_0x3ec;
        };

        union {
            Vector fogColor;
            Vector field_0x3f0;
        };

        union {
            Vector eyePosition;

            struct {
                float eyePosition_x;
                uint32_t field_0x400;
                float eyePosition_z;
            };
        };

        uint8_t _pad0x408[4];

        union {
            uint32_t viewFramebuffer;
            uint32_t field_0x40c;
        };

        uint32_t postEffectFlags;
        AbyssEngine::FBOContainer *postEffectFBO;

        union {
            AbyssEngine::FBOContainer *fboContainer;
            AbyssEngine::FBOContainer *refractFBO;
        };

        uint8_t glowActive;
        uint32_t glEnableFlags;
        uint8_t lightingEnabled;
        float uvMatrixGL[16];
        Vector field_0x468;

        union {
            Vector field_0x474;

            struct {
                float field_0x474_x;
                uint32_t field_0x478;
                float field_0x47c;
            };
        };

        bool hasVibration;
        DestroyCallback *onDestroyCallback;
        float materialAlpha;
        int frameBufferTextures[2];
        uint8_t _pad0x494[16];
        uint32_t clientStateFlagsAE;
        uint32_t field_0x4a8;

        union {
            struct {
                double accelRaw[3];
                volatile double accelValue[3];
            };

            struct {
                double field_0x4b0, field_0x4b8;
                volatile double field_0x4c0, field_0x4c8, field_0x4d0;
                double field_0x4d8;
            };
        };

        union {
            struct {
                double gravRaw[3];
                volatile double gravValue[3];
            };

            struct {
                double field_0x4e0, field_0x4e8;
                volatile double field_0x4f0, field_0x4f8, field_0x500;
                double field_0x508;
            };
        };

        uint32_t shaderCount;
        Array<ShaderBaseStruct *> *shaders;

        Engine();

        ~Engine();

        void AEClientState(unsigned int state, bool enable);

        void ClearBuffer(uint32_t color);

        bool IsExtensionSupported(const char *extension);

        void ReleaseGL();

        void ActivateRefractFBO();

        void ActivateRender2FracFBO();

        void ActivateRender2TextureFBO();

        void ActivateTextureFBO();

        void ActivateViewBuffer();

        void AfterGLInit();

        void CopyFBO();

        void DeactivateRender2FracFBO();

        void DeactivateRender2TextureFBO();

        void DoPostEffect();

        void DrawCloakFBO(AbyssEngine::FBOContainer *fbo);

        void DrawLine2D(float *verts, int count, bool strip);

        void DrawQuad(int x, int y, int width, int height);

        double *GetAccelValue();

        DeviceInfo GetDeviceInfo();

        uint32_t GetDisplayHeight();

        uint32_t GetDisplayWidth();

        double *GetGravValue();

        void GlEnable(unsigned int cap, bool enable);

        void GlowBeginGlow(int depthFunc);

        void GlowEnableGlow();

        void GlowEndGlow();

        bool HasVibration();

        int InitGL(bool shaders, int width, int height);

        void Initialize(InitializeCallback *callback);

        bool IsPostEffectActivated();

        void LightEnable(bool enabled);

        void LightSetGlobalSceneColorAmbient(float red, float green, float blue);

        void LightSetLight(unsigned int light);

        void LightSetLightColorAmbient(float red, float green, float blue, unsigned int light);

        void LightSetLightColorDiffuse(float red, float green, float blue, unsigned int light);

        void LightSetLightColorSpecular(float red, float green, float blue, unsigned int light);

        void LightSetLightCount(int count);

        void LightSetLightDirection(float x, float y, float z, unsigned int light);

        void LightSetLightPosition(float x, float y, float z, unsigned int light);

        void LightSetMaterialColorAlpha(float alpha);

        void LightSetMaterialColorAmbient(float red, float green, float blue);

        void LightSetMaterialColorDiffuse(float red, float green, float blue);

        void LightSetMaterialColorShininess(float shininess);

        void LightSetMaterialColorSpecular(float red, float green, float blue);

        void LightSetParticleAmbient(float red, float green, float blue);

        void LightSetRimColor(float red, float green, float blue);

        void ReloadShaders();

        void RenderMesh(Mesh *mesh);

        void ResetLightParam();

        void ResetUVMatrix();

        uint32_t Resume();

        void SetAccelValue(double x, double y, double z);

        void SetAddData(void *data, int size);

        void SetColor(float red, float green, float blue, float alpha);

        uint64_t SetEyePosition(float x, float y, float z);

        void SetFrameBufferTexture(int slot0, int slot1);

        void SetGravValue(double x, double y, double z);

        void SetModelMatrix(const AbyssEngine::AEMath::Matrix &matrix);

        void SetOnDestroyApp(DestroyCallback *callback);

        void SetOrthoMatrix(float *projection, float *view, bool multiply);

        void SetPerspMatrix(float *matrix);

        void SetPostEffect(uint32_t effect, bool enable);

        void SetTextureSlot(uint32_t textureIndex, uint32_t slot);

        void SetTextures(uint32_t first, uint32_t second);

        void SetTexturesExt(uint32_t first, ...);

        void PreUpdate();

        void Release();

        void ActivateFrameBuffer(int slot);

        void SetFrameBufferScaleFactor(float factor, int slot);

        void GrabFrameBuffer();

        void *GetJPEGImageData(float quality);

        void SaveImageToPhotosAlbum();

        void SetScreenOrientation(AbyssEngine::LandscapeMode orientation);

        bool IsRefractActivated();

        void Vibrate(unsigned short duration);

        void VibrateSupported();

        void SetUVMatrix(const AbyssEngine::AEMath::Matrix &matrix);

        void SetWorldViewMatrix(const AbyssEngine::AEMath::Matrix &matrix);

        uint32_t ShaderInit();

        void ShaderRegister(AbyssEngine::ShaderBaseStruct *shader);

        void ShaderSetActive(int shaderIndex, Mesh *mesh);

        void ShaderSetInActive();

        void ShaderUpdate();

        uint32_t Suspend();

        void SwapBuffer();

        void initFileInterface();

        static bool vboSupported;
        static bool clampTextures;
        static bool vfc;
        static float lodBiasDiffuse;
        static float lodBiasNormal;
        static unsigned int countryCode;

        static bool EnablePostEffect;

        static bool CheckForOrientationChange;

        static bool fogEnabled;

        static Engine **g_pEngine;

        // Static data members present in the original binary (defined for symbol parity).
        static unsigned char EnableGlow;
        static int ImageCount;
        static int switchGlow;
        static int SwapCounter;
        static int switchBloom;
        static unsigned char enableShader;
        static unsigned char EnableRefract;
        static void *LodDistShader;
        static unsigned char DisableRefract;
        static int AnisotropyValue;
        static unsigned char KeepRawMeshData;
        static unsigned char backupSaveGames;
        static int MultiSampleValue;
        static unsigned char UseAdvancedShader;
        static unsigned char enableReverseFlag;
        static int tv_h;
        static int tv_w;
        static AbyssEngine::AEMath::Vector vendor;
        static unsigned char DrawFBO;
        static int vboSize;
        static unsigned char DEBUGKEY;
        static unsigned char TVEnable;
        static float farPlane;
        static AbyssEngine::AEMath::Vector renderer;
        static AbyssEngine::AEMath::Vector tv_modes;
        static unsigned char WireFrame;
        static float nearPlane;
    };
}

using ::AbyssEngine::Engine;

// The active engine singleton. The original exports this as a plain global
// pointer (symbol `gEngine`), not a class member.
extern AbyssEngine::Engine *gEngine;

#endif
