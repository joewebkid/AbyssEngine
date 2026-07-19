#ifndef GOF2_PAINTCANVAS_H
#define GOF2_PAINTCANVAS_H
#include "engine/core/Array.h"
#include "engine/render/PickedTextureRegion.h"
#include "engine/render/RenderEnums.h"
#include "../core/AEString.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include "engine/core/Node.h"
#include "engine/math/Transform.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"

namespace AbyssEngine {
    class Engine;
    class Mesh;
    class Transform;
}

namespace AbyssEngine {
    class Material;
    class Image;
    class Image2D;
    struct Resource;
    class SpriteSystem;
    class Camera;
    class ImageFont;
    class AELoadedTexture;

#ifndef GOF2_ENUM_BlendMode
#define GOF2_ENUM_BlendMode
#endif
}

namespace AbyssEngine {
    using AEMath::Matrix;
    using AEMath::Vector;

    class PaintCanvas {
    public:
        static PaintCanvas *gCanvas;

        // Former union (initialized:1B / selfHandle:4B) collapsed to the 1-byte field + pad.
        // selfHandle (2 uses in PlayerEgo.cpp) reads the full 4-byte slot via
        // reinterpret_cast<unsigned int &>(initialized).
        unsigned char initialized;
        unsigned char _pad_0x1[3];

        int culledCount;
        char *quad2dMesh;
        int field_0xc;
        // Ghidra ground truth (decompiled ChangeCubeTexture): the cube-texture Array lives at 0x10
        // (count=field_0x10, data=field_0x14, capacity=field_0x18). Our decomp had duplicated it as a
        // separate Array at 0x20 (the now-removed field_0x10/0x14/0x18 were unused placeholders),
        // displacing meshCount/meshes/engine by 0xc. Relocated to 0x10 so the named fields hit their
        // real offsets (meshCount 0x24, meshes 0x28, engine 0x34).
        ::Array<AbyssEngine::AELoadedTexture *> cubeTextures;  // 0x10 (count/data/capacity)
        unsigned char field_0x1c;                              // 0x1c
        int field_0x20;                                        // 0x20
        unsigned int meshCount;                                // 0x24
        char **meshes;                                         // 0x28
        char *mask2dImage;                                     // 0x2c (Ghidra gap)
        int gameOrientation;                                   // 0x30 (orientation)
        Engine *engine;                                        // 0x34
        int field_0x38;                                        // 0x38
        int field_0x3c;                                        // 0x3c
        Matrix projMatrix3d;
        Matrix projOrthoMatrix;
        Matrix worldViewMatrix;
        Matrix identityMatrix;
        // AEMath::Matrix is 0x3c here but 0x40 in the original (NEVER change Matrix.h). The four
        // embedded matrices are each 4 bytes short (-0x10); the upstream model is +8, so the net
        // deficit by this point is 8 bytes. Reserve it in the matrix region so lineMesh lands @0x1c8
        // and field_0x1cc/field_0x1f8 land at their named offsets.
        unsigned char _pad_matrix_undersize[4];
        ::Array<AbyssEngine::Resource *> resources;
        ::Array<AbyssEngine::ImageFont *> fonts;
        ::Array<AbyssEngine::Image2D *> images;
        unsigned int transformCount;
        char **transforms;
        unsigned int transforms_cap; // Ghidra: transforms is an inline Array (count/data/capacity@0x160)
        ::Array<AbyssEngine::Camera *> cameras;
        unsigned int currentCamera;
        ::Array<AbyssEngine::Material *> materials;
        ::Array<AbyssEngine::SpriteSystem *> spriteSystems;
        unsigned int glowMeshes_count;
        char *glowMeshes_data;
        unsigned int glowMeshes_cap;
        unsigned int glowMatA_count;
        char *glowMatA_data;
        unsigned int glowMatA_cap;
        unsigned int glowMatB_count;
        char *glowMatB_data;
        unsigned int glowMatB_cap;
        unsigned int glowUints_count;
        char *glowUints_data;
        unsigned int glowUints_cap;
        unsigned int glowMatC_count;
        char *glowMatC_data;
        unsigned int glowMatC_cap;
        char *lineMesh;
        unsigned int field_0x1cc;
        float lineVerts[8];
        unsigned char bgFlagSaved;
        char fogMode;
        int fogEnableFlag;
        unsigned char field_0x1f8;
        float colorR;
        float colorG;
        float colorB;
        float colorA;

        explicit PaintCanvas(Engine *engine);

        ~PaintCanvas();

        void Initialize(bool landscape);

        void Resume();

        void Suspend();

        void Begin2d();

        void End2d();

        void Begin3d();

        void End3d();

        void BeginBG();

        void EndBG();

        void ClearBuffer(unsigned int mask);

        void SetColor(unsigned int color);

        void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

        unsigned int GetColor();

        void SetBlendMode(BlendMode mode);

        void SetTexture(unsigned int, unsigned int);

        void SetShaderMode(int mode);

        void SetGameOrientation(LandscapeMode orientation);

        void SetWorldViewMatrix(const Matrix &);

        void SetProjOrthoMatrix();

        void SetProjectionMatrix3d(float fov, float nearPlane, float farPlane);

        void ResetPersMatrix();

        void EnableClip(int x, int y, int w, int h);

        void DisableClip();

        void FogEnable(bool mode, FogMode enable);

        void FogSetParameter(FogMode mode, float fogStart, float fogEnd, float fogDensity,
                             unsigned int color);

        void ChangeCubeTexture(unsigned int idx);

        int GetWidth();

        int GetHeight();

        void HasVibration();

        void Vibrate(unsigned short);

        void GetAccelValue();

        void GetGravValue();

        void SwapBuffer();

        void StartDraw2FBO();

        void StopDraw2FBO();

        void CheckNUseRefractFBO(bool);

        void ClearDepth();

        void DrawFrameBufferTexture(int, int, int, int, int, int);

        bool WarmUpTexture();

        void FillRectangle(int x, int y, int w, int h);

        void DrawRectangle(int x, int y, int w, int h);

        void DrawLine(int x0, int y0, int x1, int y1);

        void DrawImage2D(unsigned int index, int x, int y);

        void DrawImage2D(unsigned int index, int x, int y, unsigned char flipFlags);

        void DrawImage2D(unsigned int index, int x, int y, unsigned char regionAlignFlags, unsigned char placeFlags);

        void DrawImage2D(unsigned int index, int x, int y, int w, int h, unsigned char alignFlags,
                         unsigned char anchorFlags, unsigned char flipFlags);

        void DrawRegion2D(unsigned int index, float unused, int rotSteps, int pivotX, int pivotY, int transX,
                          float scaleX, float scaleY);

        void DrawRegion2D(unsigned int index, int srcX, int srcY, int destW, int destH, float unused, int transY,
                          int pivotX, int pivotY, int transX);

        void Image2DCreate(unsigned short resId, unsigned int &out);

        unsigned short GetImage2DWidth(unsigned int index);

        unsigned short GetImage2DHeight(unsigned int index);

        void RestoreImage2D(Image2D *image);

        void DrawString(unsigned int index, const unsigned short *str, int x, int y, bool b);

        void DrawString(unsigned int index, const String &str, int x, int y, bool b);

        void DrawStringColor(unsigned int index, const String &text, int x, int y, bool b);

        void DrawTextLines(unsigned int font, Array<String *> *arr, int x, int y);

        void DrawTextLines(unsigned int font, Array<String *> *arr, int x, int y, bool center);

        void DrawTextLines(unsigned int font, Array<String *> *arr, int x, int y, unsigned int p5,
                           bool flag);

        int GetTextWidth(unsigned int index, const String &str);

        int GetTextWidth(unsigned int index, const String &str, unsigned int begin, unsigned int end);

        String GetReverseString(String in);

        String GetReverseString(String in, bool reverse);

        int GetTextHeight(unsigned int index);

        void GetLine(unsigned int font, String str, int maxWidth, String *out);

        void GetLineArray(unsigned int font, const String &str, int width,
                          Array<String *> *outArray);

        void CheckString(unsigned int index, const String &str);

        void FontCreate(unsigned short resId, unsigned int &out, bool unused);

        int FontGetSpacing(unsigned int index);

        void FontSetSpacing(unsigned int index, short spacing);

        int FontGetYOffset(unsigned int index);

        void FontSetYOffset(unsigned int index, short yoff);

        void CameraCreate(unsigned int &out);

        unsigned int CameraGetCurrent();

        void CameraSetCurrent(unsigned int index);

        void *CameraGetLocal(unsigned int index); // lint: void_ptr (ABI method return; mangling must match lib)

        void CameraSetLocal(unsigned int index, const Matrix &matrix);

        void CameraSetPerspective(unsigned int index, float a, float b, float c);

        void CameraSetPerspective(unsigned int index, float fov, float aspect);

        float CameraGetCurrentFactor1();

        int CameraIsSphereinViewFrustum(const Vector &point, float radius);

        int CameraIsPointinViewFrustum(const Vector &point);

        void TransformCreate(unsigned int &out);

        void TransformCreate(unsigned short resId, unsigned int &out);

        void *TransformGetLocal(unsigned int index); // lint: void_ptr (ABI method return; mangling must match lib)

        void TransformSetLocal(unsigned int index, const Matrix &matrix);

        void TransformSetColor(unsigned int index, unsigned int color);

        void *TransformGetTransform(unsigned int index); // lint: void_ptr (ABI method return; mangling must match lib)

        int TransformGetTriCount(unsigned int index);

        int TransformGetTriCount(Transform *transform);

        void TransformAddChild(unsigned int parent, unsigned int child);

        void TransformRemoveChild(unsigned int parent, unsigned int child);

        void TransformAddMesh(unsigned int transformIndex, unsigned short meshId, bool b);

        void TransformAddMeshId(unsigned int transformIndex, unsigned int meshIndex);

        void TransformRemoveMesh(unsigned int transformIndex, unsigned short meshResId);

        void TransformRemoveMeshId(unsigned int transformIndex, unsigned int meshIndex);

        void DrawTransform(Transform *tf, const Matrix &m2, Matrix &m3);

        void DrawTransform(unsigned int index, const Matrix *viewMatrix);

        PickedTextureRegion TransformGet2DPickedTextureRegion(unsigned int transformIndex, int x, int y, int z,
                                                              int shift);

        PickedTextureRegion TransformGet2DPickedTextureRegion(Transform *transform, int x, int y, int z, int shift);

        void MeshCreate(unsigned short resId, unsigned int &out, bool forceClone);

        void MeshCreate(unsigned short a, unsigned short b, signed char c, unsigned int &out);

        void MeshCreate(unsigned short vertexCount, unsigned short triangleCount, signed char meshType,
                        unsigned short matResId, unsigned int &out);

        Mesh *MeshGetPointer(unsigned int index);

        int MeshGetTriCount(Mesh *mesh);

        void DrawMesh(unsigned int index);

        void DrawMesh(Mesh *mesh, Matrix &worldMatrix,
                      Matrix &viewMatrix, unsigned int color,
                      Matrix &uvMatrix);

        void MeshConvertToVBO(unsigned int index);

        void MeshChangeMaterial(unsigned int meshIndex, unsigned short matIndex);

        void MeshChangeMaterialIntern(Mesh *mesh, Material *mat);

        void MeshChangeMaterialIntern(Transform *transform, Material *material);

        void MeshChangeResourceMaterial(unsigned int meshIndex, unsigned short resId);

        void MeshResourceChangeMaterial(unsigned short matId, unsigned short value);

        void MeshResourceChangeAllMaterial(unsigned short matId, unsigned short value);

        void MeshCloneMaterial(unsigned int index, unsigned int &out);

        void MeshChangeShaderAnimValue(Mesh *mesh, float value, unsigned int mode);

        void MeshChangeShaderAnimValue(Transform *transform, float value, unsigned int mode);

        float MeshSetPoint(unsigned int index, unsigned short vtx, float x, float y, float z);

        void MeshSetUv(unsigned int index, unsigned short sub, float u, float v);

        void MeshSetNormal(unsigned int index, unsigned short vtx, const Vector &value);

        void MeshSetTangent(unsigned int index, unsigned short vtx, const Vector &value);

        void MeshSetBiTangent(unsigned int index, unsigned short vtx, const Vector &value);

        void MeshSetColor(unsigned int index, unsigned short sub, unsigned int color);

        void MeshSetColor(unsigned int index, unsigned short sub, float r, float g, float b, float a);

        void MeshSetTriangle(unsigned int meshIndex, unsigned short tri, unsigned short v0, unsigned short v1,
                             unsigned short v2);

        void MeshSetTriangleCount(unsigned int index, unsigned short count);

        void MeshTranslatePoint(unsigned int index, unsigned short sub, float x, float y, float z);

        void MeshSet2DMask(unsigned int index, int, int);

        void MeshClear2DMask();

        void MaterialCreate(unsigned int &out, BlendMode mode, unsigned int textures, unsigned short p4);

        void MaterialCreate(unsigned short resId, unsigned int &out);

        void *MaterialGetMaterial(unsigned int index); // lint: void_ptr (ABI method return; mangling must match lib)

        void MaterialChange(unsigned int index, BlendMode param3, unsigned int param4);

        void MaterialResourceChangeTexture(unsigned int resId, unsigned int texture, int slot);

        void TextureCreate(unsigned short resId, void (*callback)(AbyssEngine::Image *, void *), void *userData,
                           // lint: void_ptr (ABI method params: callback userData; mangling must match lib)
                           unsigned int &out, bool useCallbackLoader);

        void TextureCreate(unsigned short id, unsigned int &out, bool flag);

        void TextureCreateGlobal(String name, unsigned int unit);

        void AddResource(Resource *resource);

        void SetResourceList(AbyssEngine::Resource *const *list, unsigned int count);

        void *FindResource(unsigned short id); // lint: void_ptr (ABI method return; mangling must match lib)

        int ResourceLoaded(unsigned int index, ResourceType type);

        unsigned int GetMeshResourceId(String &name);

        unsigned int GetMeshResourceId(String &name, unsigned short p2);

        unsigned int GetTextureResourceId(String &name);

        void ReleaseAllResources();

        void ReloadTextures();

        void RemoveAllMatsForGlow();

        void SetMatForGlow(Material *glowSource);

        void SpriteSystemCreate(unsigned short resId, bool flag, unsigned int &out);

        void SpriteSystemCreate(unsigned short resId, bool flag, unsigned short matResId, unsigned int &out);

        void DrawSpriteSystem(unsigned int index);

        void DrawSpriteSystem(unsigned int index, Matrix mat);

        void DrawSpriteSystem(unsigned int index, Matrix matA, Matrix matB);

        void ReleaseSpriteSystemResource(unsigned int index);

        float SpriteSystemSetPosition(unsigned int index, unsigned short sub, float x, float y, float z);

        void SpriteSystemAddPosition(unsigned int index, unsigned short sub, float x, float y, float z);

        void SpriteSystemGetPosition(unsigned int index, unsigned short sub, Vector &out);

        void SpriteSystemGetPosition(unsigned int index, unsigned short sub, const Matrix &m, Vector &out);

        void SpriteSystemSetSize(unsigned int index, unsigned short sub, short value);

        void SpriteSystemAddSize(unsigned int index, unsigned short sub, short delta);

        void SpriteSystemSetAllSize(unsigned int index, short size);

        void SpriteSystemSetUv(unsigned int index, unsigned short sub, float a, float b, float c, float d);

        void SpriteSystemSetAllUv(unsigned int index, float a, float b, float c, float d);

        void SpriteSystemSetRGBA(unsigned int index, unsigned short sub, float a, float b, float c, float d);

        int GetScreenPosition(const Vector &a, Vector &b);

        void GetScreenPosition(const Matrix &srcMatrix, Vector &outVec);

        void GetScreenPosition(Matrix &m, const Vector &worldPos,
                               Vector &outVec);
    };

#if __SIZEOF_POINTER__ == 4
    static_assert(__builtin_offsetof(PaintCanvas, initialized) == 0x0, "PaintCanvas::initialized offset");
    static_assert(__builtin_offsetof(PaintCanvas, culledCount) == 0x4, "PaintCanvas::culledCount offset");
    static_assert(__builtin_offsetof(PaintCanvas, cubeTextures) == 0x10, "PaintCanvas::cubeTextures @0x10");
    static_assert(__builtin_offsetof(PaintCanvas, meshCount) == 0x24, "PaintCanvas::meshCount @0x24");
    static_assert(__builtin_offsetof(PaintCanvas, meshes) == 0x28, "PaintCanvas::meshes @0x28");
    static_assert(__builtin_offsetof(PaintCanvas, engine) == 0x34, "PaintCanvas::engine @0x34");
    static_assert(__builtin_offsetof(PaintCanvas, projMatrix3d) == 0x40, "PaintCanvas::projMatrix3d @0x40");
    static_assert(__builtin_offsetof(PaintCanvas, lineMesh) == 0x1c8, "PaintCanvas::lineMesh offset");
    static_assert(__builtin_offsetof(PaintCanvas, field_0x1cc) == 0x1cc, "PaintCanvas::field_0x1cc offset");
    static_assert(__builtin_offsetof(PaintCanvas, field_0x1f8) == 0x1f8, "PaintCanvas::field_0x1f8 offset");
    static_assert(__builtin_offsetof(PaintCanvas, colorR) == 0x1fc, "PaintCanvas::colorR offset");
#endif
}

using ::AbyssEngine::PaintCanvas;

#endif
