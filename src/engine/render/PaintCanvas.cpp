#include "engine/render/PaintCanvas.h"
#include <GLES/gl.h>
#include "engine/core/AbyssEngine.h"
#include "engine/render/Material.h"
#include "engine/render/Mesh.h"
#include "engine/render/Camera.h"
#include "engine/render/ImageFont.h"
#include "engine/core/Array.h"
#include "engine/core/Node.h"
#include "engine/math/Transform.h"
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <new>

extern "C" void glMultMatrixf(const GLfloat *m);

// ARM MeshCloneMaterial calls the specialization defined in Array.cpp.
extern template void ArrayAdd<AbyssEngine::Material *>(AbyssEngine::Material *, Array<AbyssEngine::Material *> &);
extern template void ArrayAdd<AbyssEngine::Camera *>(AbyssEngine::Camera *, Array<AbyssEngine::Camera *> &);
extern template void ArrayAdd<AbyssEngine::Image2D *>(AbyssEngine::Image2D *, Array<AbyssEngine::Image2D *> &);
extern template void ArrayAdd<AbyssEngine::Resource *>(AbyssEngine::Resource * const *, unsigned int,
                                                        Array<AbyssEngine::Resource *> &);

namespace AbyssEngine {
    class SpriteSystem;
    class ImageFont;
    int MeshDraw(Engine *engine, Mesh *mesh);
    void MaterialDraw(PaintCanvas *canvas, Engine *engine, Material *material, bool setTextures);
    void SpriteSystemRelease(Engine *engine, SpriteSystem **slot);
    void SpriteSystemDraw(Engine *engine, const Matrix &view, const Matrix &world, SpriteSystem *sys);
    void SpriteSystemSetAllUv(float u0, float v0, float u1, float v1, SpriteSystem *sys);
    void SpriteSystemSetUv(unsigned short idx, float a, float b, float c, float d, SpriteSystem *sys);
    void SpriteSystemSetRGBA(unsigned short idx, float r, float g, float b, float a, SpriteSystem *sys);
    int CameraIsSphereinViewFrustum(const Vector &center, float radius, Matrix *extra, Camera *cam);
    int CameraIsPointinViewFrustum(const Vector &point, Matrix *extra, Camera *cam);
}

// Byte-faithful "view" structs for the otherwise-untyped engine handles that
// PaintCanvas manipulates by raw pointer offset.  Each struct lays out exactly
// the fields PaintCanvas touches at their real byte offsets; padding members
// fill the gaps so offsets stay correct in the 32-bit MATCH build.
//
// These are pure overlays: PaintCanvas casts an opaque handle (void*/char*)
// to one of these and uses named members instead of pointer arithmetic.
namespace {

// ---------------------------------------------------------------------------
// Image2D handle (entries of PaintCanvas::images).
//   +0x00 void*    backing mesh object (vertex/uv/index buffers)
//   +0x04 uint32   texture id (also reinterpreted as region-record pointer)
//   +0x08 uint16   region: u origin denominator source
//   +0x0a uint16   region: inverse-width source
//   +0x0c uint16   region: u0
//   +0x0e uint16   region: v0
//   +0x10 uint16   width
//   +0x12 uint16   height
//   +0x14 uint8    restore flag
// ---------------------------------------------------------------------------
struct PCImage2DView {
    void *mesh;          // +0x00
    union {
        uint32_t textureId; // +0x04
        uint32_t regionPtr; // +0x04 (legacy pointer interpretation; still a 32-bit word)
    };
    uint16_t regionA;    // +0x08
    uint16_t regionB;    // +0x0a
    uint16_t u0;         // +0x0c
    uint16_t v0;         // +0x0e
    uint16_t width;      // +0x10
    uint16_t height;     // +0x12
    uint8_t restoreFlag; // +0x14
};
static_assert(sizeof(PCImage2DView) == sizeof(AbyssEngine::Image2D), "Image2D overlay size");
static_assert(offsetof(PCImage2DView, width) == offsetof(AbyssEngine::Image2D, sizeX), "Image2D overlay width");
static_assert(offsetof(PCImage2DView, restoreFlag) == offsetof(AbyssEngine::Image2D, restoreFlag), "Image2D overlay restore");

// Region record reached via PCImage2DView::textureId reinterpreted as a pointer
// (i.e. *(char**)(img+4)).  Floats at +0x0c (width) and +0x1c (height).
struct PCRegionView {
    uint8_t pad0[0x0c];
    float width;   // +0x0c
    uint8_t pad1[0x1c - 0x10];
    float height;  // +0x1c
};

// ---------------------------------------------------------------------------
// Mesh handle (entries of PaintCanvas::meshes) for the raw-offset call sites.
//   +0x00 void*    first sub-object (drawable)
//   +0x02 uint16   vertex count
//   +0x04 char*    position buffer (vec3 per vertex)
//   +0x08 char*    uv buffer (vec2 per vertex)
//   +0x0c char*    color buffer (vec4 per vertex)
//   +0x10 char*    normal buffer (vec3 per vertex)
//   +0x14 char*    tangent buffer (vec3 per vertex)
//   +0x18 char*    bitangent buffer (vec3 per vertex)
//   +0x28 uint16   index count
//   +0x2a uint16   triangle capacity
//   +0x2c char*    index/blend buffer
//   +0x30 void*    material
//   +0x34 void*    material resource
//   +0x7c int      tri-count contribution
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct PCMeshView {
    union {
        void *sub0;          // +0x00 (drawable; read as *(void**)mesh)
        struct {
            uint16_t _half0; // +0x00
            uint16_t vertexCount; // +0x02
        };
    };
    char *positions;   // +0x04
    char *uvs;         // +0x08
    char *colors;      // +0x0c
    char *normals;     // +0x10
    char *tangents;    // +0x14
    char *bitangents;  // +0x18
    uint8_t pad1c[0x28 - 0x1c];
    uint16_t indexCount;  // +0x28
    uint16_t triCapacity; // +0x2a
    char *indexBuffer; // +0x2c
    void *material;    // +0x30
    void *materialRes; // +0x34
    uint8_t pad38[0x3c - 0x38];
    uint8_t bounds3c;  // +0x3c (bounding-sphere block; address taken)
    uint8_t pad3d[0x7c - 0x3d];
    int triCountContribution; // +0x7c
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// SpriteSystem handle (entries of PaintCanvas::spriteSystems).
//   +0x00 int16    sprite count (low 16 bits read as uint16/short)
//   +0x04 char*    position buffer (vec3 per sprite)
//   +0x08 char*    size buffer (int16 per sprite)
//   +0x0c uint8    uniform-size flag
// ---------------------------------------------------------------------------
struct PCSpriteSystemView {
    int16_t count;     // +0x00
    uint8_t pad02[0x04 - 0x02];
    char *positions;   // +0x04
    char *sizes;       // +0x08
    uint8_t uniformSize; // +0x0c
};

// SpriteSystem object: the scene node it owns lives at +0x10.
struct PCSpriteSystemView2 {
    uint8_t pad00[0x10];
    ::Node *node;      // +0x10
};

// ---------------------------------------------------------------------------
// Resource record (entries of PaintCanvas::resources).
//   +0x00 uint16   id
//   +0x04 int      type
//   +0x08 int      handle / created index (-1 / 0xffffffff = none)
//   +0x0c void*    payload (info record / created object)
// ---------------------------------------------------------------------------
struct PCResourceView {
    uint16_t id;       // +0x00
    uint8_t pad02[0x04 - 0x02];
    int type;          // +0x04
    int handle;        // +0x08
    void *payload;     // +0x0c
};

// Material-resource info record (payload of a material resource).
//   +0x00 uint16[8] texture resource ids
//   +0x10 uint32    flags 0
//   +0x14 uint32    flags 1
//   +0x18 uint32    flags 2
//   +0x1c float[3]  vector (assigned wholesale)
struct PCMaterialInfoView {
    uint16_t textureIds[8]; // +0x00 .. +0x0e
    uint32_t flags0;        // +0x10
    uint32_t flags1;        // +0x14
    uint32_t flags2;        // +0x18
    float vec[3];           // +0x1c
};

// Mesh-resource info record (payload of a mesh resource).
//   +0x00 char*    file path
//   +0x04 uint16   material resource id
struct PCMeshInfoView {
    char *path;        // +0x00
    uint16_t matResId; // +0x04
};

// Texture-resource info record (payload of a texture resource).
//   +0x00 char*    file path
//   +0x04 float    creation scale
struct PCTexInfoView {
    char *path;        // +0x00
    float value;       // +0x04
};

// ---------------------------------------------------------------------------
// Material handle (entries of PaintCanvas::materials) for raw-offset sites.
//   +0x00 uint32   texture id slot 0 (slots are 4 bytes each, indices 0..7)
//   +0x04 uint16   material id field used by resource material edits
//   +0x08 uint32   blend mode (used by MaterialChange as mat[8])
//   +0x20 uint32   flags 0
//   +0x24 uint32   flags 1
//   +0x28 uint32   flags 2
//   +0x68 ...       vector field assigned in MaterialCreate
// ---------------------------------------------------------------------------
struct PCMaterialView {
    uint32_t textureSlots[8]; // +0x00 .. +0x1c
    uint32_t flags0;          // +0x20
    uint32_t flags1;          // +0x24
    uint32_t flags2;          // +0x28
    uint8_t pad2c[0x68 - 0x2c];
    float vec[3];             // +0x68
};

// Material id lives at +0x04, overlapping textureSlots[1]; expose a helper view.
struct PCMaterialIdView {
    uint8_t pad00[0x04];
    uint16_t materialId; // +0x04
};

// ---------------------------------------------------------------------------
// Camera handle (entries of PaintCanvas::cameras).
//   +0x00 uint32   apply param 0 (cam[0])
//   +0x04 float    near-ish clip / param 1 (cam[1])
//   +0x08 float    param 2 (cam[2])
//   +0x0c float[15] local matrix
//   +0x18 float    eye x
//   +0x28 float    eye y
//   +0x38 float    eye z
//   +0x48 float    factor used in projection (also factor1)
//   +0x4c float    factor used in projection
// ---------------------------------------------------------------------------
struct PCCameraView {
    uint32_t param0;   // +0x00
    float param1;      // +0x04
    float param2;      // +0x08
    float localMatrix[15]; // +0x0c .. +0x44 (15 floats = 0x3c bytes)
    float factor48;    // +0x48
    float factor4c;    // +0x4c
};
// The first three 4-byte camera words are also consumed as raw uint params by
// the camera-apply / set-perspective externs.
struct PCCameraParamsView {
    uint32_t raw[3];   // +0x00 .. +0x08
};
// Eye fields overlap localMatrix; expose via a separate view.
struct PCCameraEyeView {
    uint8_t pad00[0x18];
    float eyeX;        // +0x18
    uint8_t pad1c[0x28 - 0x1c];
    float eyeY;        // +0x28
    uint8_t pad2c[0x38 - 0x2c];
    float eyeZ;        // +0x38
};

// ---------------------------------------------------------------------------
// Transform info record (payload of a transform resource) used by
// TransformCreate(resId,...).
//   +0x3c uint16   child-mesh id count
//   +0x40 char*    child-mesh id array (uint16 each)
//   +0x44 uint16   child-transform id count
//   +0x48 char*    child-transform id array (uint16 each)
// ---------------------------------------------------------------------------
struct PCTransformInfoView {
    uint8_t pad00[0x3c];
    uint16_t childMeshCount;  // +0x3c
    uint8_t pad3e[0x40 - 0x3e];
    char *childMeshIds;       // +0x40
    uint16_t childTfCount;    // +0x44
    uint8_t pad46[0x48 - 0x46];
    char *childTfIds;         // +0x48
};

// ---------------------------------------------------------------------------
// Transform object (entries of PaintCanvas::transforms) for raw-offset sites.
//   +0x3c .. Array<Mesh*>   mesh array (count at +0x3c, data at +0x40)
//   +0x48 uint32   color
//   +0x4c .. Array<...>     child array (count at +0x4c, data at +0x50)
//   +0xd4 ...       bounding-sphere block
//   +0xec uint8    visible flag
//   +0xf8 int64    animation length
//   +0x100 int64   animation start
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct PCTransformView {
    uint8_t pad00[0x3c];
    uint32_t meshCount; // +0x3c
    void **meshData;    // +0x40
    uint32_t meshCap;   // +0x44 (array capacity word)
    uint32_t color;     // +0x48
    uint32_t childCount;// +0x4c
    void **childData;   // +0x50
    uint8_t pad54[0xd4 - 0x54];
    uint8_t bsphere;    // +0xd4 (bounding-sphere block; address taken)
    uint8_t padd5[0xec - 0xd5];
    uint8_t visible;    // +0xec
    uint8_t paded[0xf8 - 0xed];
    int64_t animLength; // +0xf8
    int64_t animStart;  // +0x100
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// Cube-texture record (entries of PaintCanvas::cubeTextures).
//   +0x00 int      gl texture id (-1 = unloaded)
//   +0x04 ...      AEString path field (passed by address)
//   +0x10 float    creation scale
//   +0x14 uint8    restore flag
//   +0x18 int      memory-accounting size
// ---------------------------------------------------------------------------
struct PCCubeTexView {
    int glTexId;       // +0x00
    char pathField[0x10 - 0x04]; // AEString path lives here (+0x04)
    float scale;       // +0x10
    uint8_t restoreFlag; // +0x14
    uint8_t pad15[0x18 - 0x15];
    int memSize;       // +0x18
};

// ---------------------------------------------------------------------------
// Gravity sample returned by the *_getgrav externs.
//   +0x08 double    angle value
// ---------------------------------------------------------------------------
struct PCGravView {
    uint8_t pad00[0x08];
    double angle;      // +0x08
};

// ---------------------------------------------------------------------------
// AEString-like length record used in GetLineArray scratch buffers.
//   +0x08 int       length
// ---------------------------------------------------------------------------
struct PCStrLenView {
    uint8_t pad00[0x08];
    int length;        // +0x08
};

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(PCSpriteSystemView) >= 0x0d, "sprite view");
static_assert(offsetof(PCImage2DView, restoreFlag) == 0x14, "img restore");
static_assert(offsetof(PCImage2DView, width) == 0x10, "img width");
static_assert(offsetof(PCMeshView, vertexCount) == 0x02, "mesh vtxcount");
static_assert(offsetof(PCMeshView, positions) == 0x04, "mesh pos");
static_assert(offsetof(PCMeshView, indexCount) == 0x28, "mesh idxcount");
static_assert(offsetof(PCMeshView, triCapacity) == 0x2a, "mesh tricap");
static_assert(offsetof(PCMeshView, material) == 0x30, "mesh material");
static_assert(offsetof(PCMeshView, materialRes) == 0x34, "mesh materialRes");
static_assert(offsetof(PCMeshView, triCountContribution) == 0x7c, "mesh tri");
static_assert(offsetof(PCSpriteSystemView, positions) == 0x04, "ss pos");
static_assert(offsetof(PCSpriteSystemView, sizes) == 0x08, "ss size");
static_assert(offsetof(PCSpriteSystemView, uniformSize) == 0x0c, "ss flag");
static_assert(offsetof(PCResourceView, type) == 0x04, "res type");
static_assert(offsetof(PCResourceView, handle) == 0x08, "res handle");
static_assert(offsetof(PCResourceView, payload) == 0x0c, "res payload");
static_assert(offsetof(PCMaterialView, flags0) == 0x20, "mat flags0");
static_assert(offsetof(PCMaterialView, vec) == 0x68, "mat vec");
static_assert(offsetof(PCCameraView, localMatrix) == 0x0c, "cam local");
static_assert(offsetof(PCCameraView, factor48) == 0x48, "cam f48");
static_assert(offsetof(PCCameraView, factor4c) == 0x4c, "cam f4c");
static_assert(offsetof(PCCameraEyeView, eyeX) == 0x18, "cam eyeX");
static_assert(offsetof(PCCameraEyeView, eyeZ) == 0x38, "cam eyeZ");
static_assert(offsetof(PCTransformInfoView, childMeshCount) == 0x3c, "tfi mc");
static_assert(offsetof(PCTransformInfoView, childTfIds) == 0x48, "tfi tf");
static_assert(offsetof(PCTransformView, meshCount) == 0x3c, "tf mc");
static_assert(offsetof(PCTransformView, bsphere) == 0xd4, "tf bsphere");
static_assert(offsetof(PCTransformView, childCount) == 0x4c, "tf childcount");
static_assert(offsetof(PCTransformView, visible) == 0xec, "tf vis");
static_assert(offsetof(PCTransformView, animLength) == 0xf8, "tf animlen");
static_assert(offsetof(PCTransformView, animStart) == 0x100, "tf animstart");
static_assert(offsetof(PCCubeTexView, scale) == 0x10, "cube scale");
static_assert(offsetof(PCCubeTexView, restoreFlag) == 0x14, "cube restore");
static_assert(offsetof(PCCubeTexView, memSize) == 0x18, "cube mem");
static_assert(offsetof(PCGravView, angle) == 0x08, "grav angle");
static_assert(offsetof(PCRegionView, height) == 0x1c, "region h");
#endif

} // anonymous namespace

PaintCanvas *PaintCanvas::gCanvas = nullptr;

namespace {
    struct PCArrayHeader {
        uint32_t count;
        void *data;
        uint32_t capacity;
    };

    inline void PCArrayCtor(void *arrayHeader) {
        PCArrayHeader *a = (PCArrayHeader *) arrayHeader;
        a->count = 0;
        a->data = nullptr;
        a->capacity = 0;
    }

    template<class T>
    inline void PCArrayAdd(T item, void *arrayHeader) {
        PCArrayHeader *a = (PCArrayHeader *) arrayHeader;
        if (a->count >= a->capacity) {
            uint32_t newCap = a->count + 1;
            T *grown = (T *) ::operator new((size_t) newCap * sizeof(T));
            for (uint32_t i = 0; i < a->count; ++i)
                grown[i] = ((T *) a->data)[i];
            ::operator delete(a->data);
            a->data = grown;
            a->capacity = newCap;
        }
        ((T *) a->data)[a->count] = item;
        a->count += 1;
    }

    inline void PCArrayRemoveAll(void *arrayHeader) {
        PCArrayHeader *a = (PCArrayHeader *) arrayHeader;
        a->count = 0;
    }

}

void paintcanvas_ext_has_vibration(void *);

void MatrixIdentity(void *result, void *matrix);

void paintcanvas_ext_sprite_allsize(unsigned int, void *);

char *paintcanvas_ext_find_res(void *, unsigned int);

void paintcanvas_ext_change_mat(void *, void *, void *);

void paintcanvas_ext_setcolor(void *, float, float, float, float);

void paintcanvas_ext_vibrate(void *);

void paintcanvas_ext_drawtextlines6(void *, unsigned int, void *, int, int, bool);

int paintcanvas_ext_strcmp(void *, void *);

unsigned int paintcanvas_ext_strlen(void *);

void paintcanvas_ext_clear(int);

static inline void paintcanvas_ext_gl_disable(unsigned int cap) { glDisable(cap); }

static inline void paintcanvas_ext_gl_enable(unsigned int cap) { glEnable(cap); }

void paintcanvas_ext_setprojmatrix3d(void *, float, float, float);

int paintcanvas_ext_getdisplaywidth(void *);

int paintcanvas_ext_getdisplayheight(void *);

void paintcanvas_ext_mat_intern(AbyssEngine::PaintCanvas *, AbyssEngine::Transform *, AbyssEngine::Material *);

void paintcanvas_ext_camera_apply(void *, unsigned int, unsigned int, unsigned int);


int paintcanvas_ext_is_posteffect(void *);

void paintcanvas_ext_use_refract(void *);

void paintcanvas_ext_set_wvm(void *);

void paintcanvas_ext_font_set_yoff(void *, int);

// SetBlendMode at ARMv7 0x7cb49 calls these GLES entry points directly.
static inline void paintcanvas_ext_sbm_glTexEnvi(unsigned int a, unsigned int b, unsigned int c) {
    glTexEnvi(a, b, c);
}

static inline void paintcanvas_ext_sbm_glEnable(unsigned int cap) { glEnable(cap); }

static inline void paintcanvas_ext_sbm_glDisable(unsigned int cap) { glDisable(cap); }

static inline void paintcanvas_ext_sbm_glBlendFunc(unsigned int a, unsigned int b) { glBlendFunc(a, b); }

static inline void paintcanvas_ext_sbm_glDepthMask(int v) { glDepthMask(v); }

static inline void paintcanvas_ext_sbm_glAlphaFunc(unsigned int a, float ref) { glAlphaFunc(a, ref); }

AbyssEngine::String *paintcanvas_ext_gla_str_new();

void paintcanvas_ext_gla_str_copy(void *out, void *src, bool copy);

void paintcanvas_ext_gla_str_fromchar(void *out, const char *s, bool copy);

void paintcanvas_ext_gla_str_append(void *dst, void *src);

void paintcanvas_ext_gla_str_dtor(void *s);

void paintcanvas_ext_gla_substr(void *out, const void *str, unsigned int begin, unsigned int end);

void paintcanvas_ext_gla_getline(void *self, unsigned int font, void *line, int width, void *out);

void paintcanvas_ext_gla_str_vdtor(void *s);

void paintcanvas_ext_gla_arr_setlen(unsigned int n, ::Array<AbyssEngine::String *> *arr);

unsigned short *paintcanvas_ext_gla_str_index(const void *s, int i);

void paintcanvas_ext_gla_str_assign(void *dst, void *src);

void paintcanvas_ext_add_resource(void *resource, void *resources) {
    ArrayAdd(static_cast<AbyssEngine::Resource *>(resource),
             *static_cast<::Array<AbyssEngine::Resource *> *>(resources));
}

void paintcanvas_ext_remove_meshid(void *, void *);


void paintcanvas_ext_gl_a(unsigned int);

void paintcanvas_ext_gl_bind(unsigned int, unsigned int);

void paintcanvas_ext_gl_c(unsigned int);

void *paintcanvas_ext_cube_restore(void *);

void paintcanvas_ext_cube_tail(void *);

void paintcanvas_ext_set_wvm2(void *self, void *m);

void paintcanvas_ext_meshdraw(void *engine, void *mesh);

void paintcanvas_ext_mesh_changemat(AbyssEngine::PaintCanvas *, void *, void *);

void paintcanvas_ext_transform_changemat(AbyssEngine::PaintCanvas *, void *, void *);

int paintcanvas_ext_font_get_yoff(void *);

unsigned short *paintcanvas_ext_gl_strindex(const AbyssEngine::String *str, unsigned int i);

void paintcanvas_ext_gl_substr(void *out, const AbyssEngine::String *str, unsigned int begin,
                                          unsigned int end);

void paintcanvas_ext_gl_str_fromchar(void *out, const char *s, bool copy);

void paintcanvas_ext_gl_str_assign(AbyssEngine::String *dst, void *src);

void paintcanvas_ext_gl_str_dtor(void *s);

int paintcanvas_ext_get_w(AbyssEngine::PaintCanvas *);

int paintcanvas_ext_get_h(AbyssEngine::PaintCanvas *);

void paintcanvas_ext_cam_persp4(float, float, float, float, float, void *);

void paintcanvas_ext_cam_setcur(AbyssEngine::PaintCanvas *, unsigned int);


void paintcanvas_ext_get_grav(void *);

static inline void paintcanvas_ext_glMatrixMode(unsigned int mode) { glMatrixMode(mode); }

static inline void paintcanvas_ext_gl_depthmask(unsigned int enabled) { glDepthMask(enabled); }

static inline void paintcanvas_ext_gl_color(void *engine, float r, float g, float b, float a) {
    ((Engine *) engine)->SetColor(r, g, b, a);
}

// ARM veneers 0x19b3c8/0x19b3d8 target SetPerspMatrix/glLoadIdentity.
static inline void paintcanvas_ext_matgl_load(void *engine, void *m) {
    ((Engine *) engine)->SetPerspMatrix((float *) m);
}

static inline void paintcanvas_ext_gl_loadidentity() { glLoadIdentity(); }

static inline void paintcanvas_ext_gl_ortho_persp(float x, float y, float z) { glScalef(x, y, z); }

static inline void paintcanvas_ext_gl_loadmatrix(void *m) { glLoadMatrixf((const GLfloat *) m); }

static inline void paintcanvas_ext_gl_done() { glLoadIdentity(); }

void paintcanvas_ext_child_link(void *, void *, void *);

void paintcanvas_ext_transform_dirty(void *);

static inline void tcg_glActiveTexture(unsigned unit) { glActiveTexture(unit); }

static inline void tcg_glBindTexture(unsigned target, unsigned tex) { glBindTexture(target, tex); }

int paintcanvas_ext_ss2_sscreate(void *eng, unsigned short id, bool b, void **out);

static inline int paintcanvas_ext_gsp_getwidth(void *self) { return ((PaintCanvas *) self)->GetWidth(); }

static inline int paintcanvas_ext_gsp_getheight(void *self) { return ((PaintCanvas *) self)->GetHeight(); }

int paintcanvas_ext_sscreate(void *eng, unsigned short id, bool b, void **out);

char *paintcanvas_ext_rs_getAEChar(void *strField);

void paintcanvas_ext_rs_deletearr(char *p);

void paintcanvas_ext_rs_glActiveTexture(unsigned int tex);

void paintcanvas_ext_rs_glBindTexture(unsigned int target, unsigned int tex);

void paintcanvas_ext_tami_bsphere_merge(void *dst, void *src);

void paintcanvas_ext_tami_setanimlen(void *tf, int hi, int lo);

void paintcanvas_ext_tami_setanimstate(void *tf, int a, int b);

void paintcanvas_ext_tami_finalize(void *tf);

void paintcanvas_ext_get_accel(void *);

int paintcanvas_ext_rpm_dispwidth(void *eng);

int paintcanvas_ext_rpm_dispheight(void *eng);

float paintcanvas_ext_rpm_sinf(float v);

float paintcanvas_ext_rpm_cosf(float v);

float paintcanvas_ext_rpm_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_rpm_glMatrixMode(unsigned int mode);

void paintcanvas_ext_rpm_glLoadIdentity();

void paintcanvas_ext_rpm_glScalef(float x, float y, float z);

void paintcanvas_ext_rpm_glLoadMatrixf(void *m);

void paintcanvas_ext_rpm_glFinish();

void paintcanvas_ext_rpm_loadproj(void *eng, void *m);

void paintcanvas_ext_dtor_releaseall(void *self);

void paintcanvas_ext_dtor_op_delete(void *p);

void *paintcanvas_ext_dtor_restex_dtor(void *p);

void *paintcanvas_ext_dtor_resmesh_dtor(void *p);

void *paintcanvas_ext_dtor_restransform_dtor(void *p);

void paintcanvas_ext_dtor_str_dtor(void *p);

void paintcanvas_ext_dtor_meshrelease(void *eng, void *meshptr);


char *paintcanvas_ext_find_mesh(void *, unsigned short);

void paintcanvas_ext_remove_mesh(void *, unsigned int, int);


void paintcanvas_ext_array_remove(void *, void *);

void *paintcanvas_ext_tc_findres(void *self, unsigned short id);

void paintcanvas_ext_start_fbo(void *);

void paintcanvas_ext_gl_deletetextures(int, void *);


void paintcanvas_ext_cam_persp(float, float, float, float, void *);

void paintcanvas_ext_dr2_restore(unsigned int flag, void *img);

void paintcanvas_ext_dr2_settexture(void *self, unsigned int tex);

float paintcanvas_ext_dr2_signedtofloat(int v, unsigned int mode);

float paintcanvas_ext_dr2_sinf(float v);

float paintcanvas_ext_dr2_cosf(float v);

void paintcanvas_ext_dr2_setscaling(void *out, float x, float y, float z);

void paintcanvas_ext_dr2_mtx_mul(void *out, void *a, void *b);

void paintcanvas_ext_dr2_mtx_assign(void *dst, void *src);

void paintcanvas_ext_dr2_setwvm(void *self, void *m);

void paintcanvas_ext_dr2_gldisable(unsigned int cap);

void paintcanvas_ext_dr2_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_dr2_glenable(unsigned int cap);

static inline void paintcanvas_ext_gl_blendfunc(unsigned int src, unsigned int dst) {
    glBlendFunc(src, dst);
}

static inline void paintcanvas_ext_glenable2(void *engine, unsigned int cap, bool enabled) {
    ((Engine *) engine)->GlEnable(cap, enabled);
}

static inline void paintcanvas_ext_setortho(void *engine, void *projection, void *view, bool multiply) {
    ((Engine *) engine)->SetOrthoMatrix((float *) projection, (float *) view, multiply);
}

static inline void paintcanvas_ext_gl_texenvi(unsigned int target, unsigned int pname, unsigned int value) {
    glTexEnvi(target, pname, value);
}

static inline void paintcanvas_ext_gl_scalef(float x, float y, float z) { glScalef(x, y, z); }

static inline void paintcanvas_ext_gl_multmatrix(void *m) { glMultMatrixf((const GLfloat *) m); }

void paintcanvas_ext_dsc_str_copy(void *out, const AbyssEngine::String *src, bool copy);

void paintcanvas_ext_smfg_pushmat(const float *m, void *array);



int paintcanvas_ext_font_get_spacing(void *);

float paintcanvas_ext_fsp_unsignedtofloat(unsigned int v, unsigned int mode);

void paintcanvas_ext_fsp_glFogf(unsigned int pname, float v);

void paintcanvas_ext_fsp_glFogfv(unsigned int pname, void *v);

void paintcanvas_ext_rar_gldeltex(int n, void *ids);

void paintcanvas_ext_rar_str_dtor(void *s);

void paintcanvas_ext_rar_op_delete(void *p);

void paintcanvas_ext_rar_fontrelease(void *eng, void *fontptr);

void paintcanvas_ext_rar_img2drelease(void *eng, void *imgptr);

void paintcanvas_ext_rar_meshrelease(void *eng, void *meshptr);

void *paintcanvas_ext_rar_transform_dtor(void *p);

void *paintcanvas_ext_rar_material_dtor(void *p);

void paintcanvas_ext_rar_ssrelease(void *eng, void *ssptr);

void paintcanvas_ext_tg2d_memcpy(void *dst, void *src, unsigned int n);

float paintcanvas_ext_tg2d_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_tg2d_invtransformvec(void *outMat, void *vec);

void paintcanvas_ext_tg2d_vec_assign(void *dst, void *src);

void paintcanvas_ext_tg2d_inner(void *out, void *self, void *tf, int x, int y);

void paintcanvas_ext_tg2d_errmsg(void *out);

char *paintcanvas_ext_rt_getAEChar(void *strField);

void paintcanvas_ext_rt_deletearr(char *p);

float paintcanvas_ext_tg2di_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_tg2di_meshintersect(void *out, float a, float b, void *mesh);

void paintcanvas_ext_tg2di_memcpy(void *dst, void *src, unsigned int n);

void paintcanvas_ext_tg2di_invtransformvec(void *outMat, void *vec);

void paintcanvas_ext_tg2di_vec_assign(void *dst, void *src);

void paintcanvas_ext_tg2di_inner(void *out, void *self, void *childtf, int x, int y);

void paintcanvas_ext_font_set_spacing(void *, int);


static inline int pc_GetWidth(AbyssEngine::PaintCanvas *self) { return self->GetWidth(); }
static inline int pc_GetHeight(AbyssEngine::PaintCanvas *self) { return self->GetHeight(); }

int paintcanvas_ext_mesh_tricount(AbyssEngine::PaintCanvas *, void *);

int paintcanvas_ext_transform_tricount(AbyssEngine::PaintCanvas *, void *);

int PaintCanvas::GetImage2DWidth(unsigned int index) {
    if (index < this->images.count) {
        PCImage2DView *img = (PCImage2DView *) (this->images.data_)[index];
        return img->width;
    }
    return 0;
}

unsigned int PaintCanvas::CameraGetCurrent() {
    unsigned int cur = this->currentCamera;
    if (cur >= this->cameras.count) {
        cur = 0xffffffff;
    }
    return cur;
}

void PaintCanvas::HasVibration() {
    return paintcanvas_ext_has_vibration(this->engine);
}

void *PaintCanvas::CameraGetLocal(unsigned int index) {
    void *result;
    if (index < this->cameras.count) {
        result = ((PCCameraView *) (this->cameras.data_)[index])->localMatrix;
    } else {
        char tmp[60];
        result = &this->identityMatrix;
        MatrixIdentity(tmp, result);
    }
    return result;
}

void PaintCanvas::DrawTextLines(unsigned int font,
                                Array<AbyssEngine::String *> *arr, int x, int y) {
    this->DrawTextLines(font, arr, x, y, 0u, false);
}

void PaintCanvas::DrawTextLines(unsigned int font,
                                Array<AbyssEngine::String *> *arr, int x, int y, bool center) {
    int xoff = 0;
    for (unsigned int i = 0; i < arr->size(); i++) {
        if (center) {
            int w = this->GetTextWidth(font, *arr->data()[i]);
            xoff = -(w >> 1);
        }
        this->DrawString(font, *arr->data()[i], xoff + x, y, false);
        y += this->GetTextHeight(font);
    }
}

void PaintCanvas::FillRectangle(int x, int y, int w, int h) {
    char abuf[60];
    float fx = (float) x;
    float fy = (float) y;
    float fx2 = (float) (x + w);
    float fy2 = (float) (h + y);

    float *vb = (float *) ((PCMeshView *) this->lineMesh)->positions;
    vb[0] = fx;
    vb[1] = fy;
    vb[3] = fx2;
    vb[4] = fy;
    vb[6] = fx2;
    vb[7] = fy2;
    vb[9] = fx;
    vb[10] = fy2;

    float *m = (float *) abuf;
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 0.0f;
    m[5] = 1.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;
    m[12] = 1.0f;
    m[13] = 1.0f;
    m[14] = 1.0f;

    this->SetWorldViewMatrix(*reinterpret_cast<const Matrix *>(abuf));
    this->engine->GlEnable(0xde1, false);
    AbyssEngine::MeshDraw(this->engine, reinterpret_cast<Mesh *>(this->lineMesh));
    this->engine->GlEnable(0xde1, true);
}

void PaintCanvas::SpriteSystemSetRGBA(unsigned int index, unsigned short sub,
                                      float a, float b, float c, float d) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned int) (unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            return AbyssEngine::SpriteSystemSetRGBA(sub, a, b, c, d,
                                                     reinterpret_cast<SpriteSystem *>(s));
        }
    }
}

void PaintCanvas::TransformCreate(unsigned int &out) {
    Transform *transform = new Transform();
    ArrayAdd<Transform *>(
        transform,
        *reinterpret_cast<Array<Transform *> *>(&this->transformCount));
    out = this->transformCount - 1;
}


void PaintCanvas::DrawRectangle(int x, int y, int w, int h) {
    Matrix worldView(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     1.0f, 1.0f, 1.0f);

    double dw = (double) w;
    double dh = (double) h;
    double dy = (double) y;
    double dx = (double) x;
    float right = (float) (dw - 0.5 + dx);
    float bottom = (float) (dh - 0.5 + dy);

    float *v = this->lineVerts;
    v[0] = (float) (dx + 0.5);
    v[1] = (float) (dy + 0.5);
    v[2] = right;
    v[3] = (float) (dy + 0.5);
    v[4] = right;
    v[5] = bottom;
    v[6] = (float) (dx + 0.5);
    v[7] = bottom;

    this->SetWorldViewMatrix(worldView);
    if (!Engine::enableShader) {
        glLineWidth(1.0f);
        this->engine->GlEnable(0xde1, false);
        glVertexPointer(2, 0x1406, 0, this->lineVerts);
        this->engine->AEClientState(0x8074, true);
        this->engine->AEClientState(0x8078, false);
        this->engine->AEClientState(0x8075, false);
        this->engine->AEClientState(0x8076, false);
        glDrawArrays(2, 0, 4);
        this->engine->GlEnable(0xde1, true);
    } else {
        this->engine->DrawLine2D(this->lineVerts, 4, true);
    }
}

int PaintCanvas::GetImage2DHeight(unsigned int index) {
    if (index < this->images.count) {
        PCImage2DView *img = (PCImage2DView *) (this->images.data_)[index];
        return img->height;
    }
    return 0;
}

static char paintcanvas_g_flipv = 0;

void PaintCanvas::MeshSetUv(unsigned int index, unsigned short sub,
                            float u, float v) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if ((unsigned int) sub < (unsigned int) mesh->vertexCount) {
            float *p = (float *) (mesh->uvs + sub * 8);
            p[0] = u;
            if (paintcanvas_g_flipv != 0) {
                p[1] = 1.0f - v;
                return;
            }
            p[1] = v;
        }
    }
}

void PaintCanvas::StopDraw2FBO() {
    this->engine->DeactivateRender2TextureFBO();
    this->SetBlendMode(static_cast<BlendMode>(0));
    this->engine->DoPostEffect();
    return this->engine->ActivateViewBuffer();
}

void PaintCanvas::SpriteSystemSetAllUv(unsigned int index,
                                       float a, float b, float c, float d) {
    if (this->spriteSystems.count <= index) {
        return;
    }
    void *sprite = (this->spriteSystems.data_)[index];
    if (sprite == 0) {
        return;
    }
    return AbyssEngine::SpriteSystemSetAllUv(a, b, c, d,
                                             static_cast<SpriteSystem *>(sprite));
}

void PaintCanvas::SpriteSystemGetPosition(unsigned int index, unsigned short sub, Vector &out) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned int) (unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            float *p = (float *) (s->positions + sub * 12);
            out.x = p[0];
            out.y = p[1];
            out.z = p[2];
        }
    }
}

void PaintCanvas::SpriteSystemSetAllSize(unsigned int index, short size) {
    if (index < this->spriteSystems.count) {
        void *sprite = (this->spriteSystems.data_)[index];
        if (sprite) {
            return paintcanvas_ext_sprite_allsize(size, sprite);
        }
    }
}

void PaintCanvas::RemoveAllMatsForGlow() {
    ArrayRemoveAll(*reinterpret_cast<Array<Mesh *> *>(&this->glowMeshes_count));
    ArrayRemoveAll(*reinterpret_cast<Array<AEMath::Matrix> *>(&this->glowMatA_count));
    ArrayRemoveAll(*reinterpret_cast<Array<AEMath::Matrix> *>(&this->glowMatB_count));
    ArrayRemoveAll(*reinterpret_cast<Array<unsigned int> *>(&this->glowUints_count));
    ArrayRemoveAll(*reinterpret_cast<Array<AEMath::Matrix> *>(&this->glowMatC_count));
}

void PaintCanvas::MaterialChange(unsigned int index,
                                 AbyssEngine::BlendMode param3, unsigned int param4) {
    if (index < this->materials.count) {
        PCMaterialView *mat = (PCMaterialView *) (this->materials.data_)[index];
        mat->flags0 = param3;
        mat->textureSlots[0] = param4;
    }
}

void PaintCanvas::DrawTextLines(unsigned int font,
                                Array<AbyssEngine::String *> *arr, int x, int y, unsigned int p5,
                                bool flag) {
    int xoff = 0;
    for (unsigned int i = 0; i < arr->size(); i++) {
        if (flag == 0) {
            int w = this->GetTextWidth(font, *arr->data()[i]);
            xoff = (int) p5 - w;
        }
        this->DrawString(font, *arr->data()[i], xoff + x, y, false);
        y += this->GetTextHeight(font);
    }
}

void PaintCanvas::MeshResourceChangeMaterial(unsigned short matId, unsigned short value) {
    unsigned int count = this->resources.count;
    for (unsigned int i = 0; i < count; ++i) {
        PCResourceView *res = (PCResourceView *) (this->resources.data_)[i];
        if (res) {
            if (res->id == matId) {
                PCMaterialIdView *mat = (PCMaterialIdView *) res->payload;
                mat->materialId = value;
            }
        }
    }
}

void *PaintCanvas::TransformGetLocal(unsigned int index) {
    void *result;
    if (index < this->transformCount) {
        result = (this->transforms)[index];
    } else {
        char tmp[60];
        result = &this->identityMatrix;
        MatrixIdentity(tmp, result);
    }
    return result;
}

void PaintCanvas::MeshSetTangent(unsigned int index, unsigned short vtx, const Vector &value) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if (vtx >= mesh->vertexCount) {
            return;
        }
        char *base = mesh->tangents;
        ((Vector *) base)[vtx] = value;
    }
}

int PaintCanvas::GetTextWidth(unsigned int index, const AbyssEngine::String &str,
                              unsigned int begin, unsigned int end) {
    if (index < this->fonts.count) {
        ImageFont *font = this->fonts.data_[index];
        const unsigned short *text = str;
        return ImageFontGetWidth(font, text, begin, end - begin);
    }
    return 0;
}

void *PaintCanvas::MaterialGetMaterial(unsigned int index) {
    if (index < this->materials.count) {
        return (this->materials.data_)[index];
    }
    return 0;
}


char *paintcanvas_g_bg_flag;

void PaintCanvas::EndBG() {
    *paintcanvas_g_bg_flag = this->bgFlagSaved;
}

void *PaintCanvas::FindResource(unsigned short id) {
    unsigned int count = this->resources.count;
    void *found = 0;
    for (unsigned int i = 0; i < count; ++i) {
        char *res = (char *) (this->resources.data_)[i];
        if (res && ((PCResourceView *) res)->id == id) {
            found = res;
            break;
        }
    }
    return found;
}

AbyssEngine::Mesh *PaintCanvas::MeshGetPointer(unsigned int index) {
    if (index < this->meshCount) {
        AbyssEngine::Mesh *mesh;
        char *slot = (this->meshes)[index];
        memcpy(&mesh, &slot, sizeof(mesh));
        return mesh;
    }
    return 0;
}

void PaintCanvas::MeshChangeResourceMaterial(unsigned int meshIndex, unsigned short resId) {
    char *r = paintcanvas_ext_find_res(this, resId);
    if (r) {
        int idx = ((PCResourceView *) r)->handle;
        if (idx + 1 != 0) {
            void *mesh = (this->meshes)[meshIndex];
            void *mat = (this->materials.data_)[idx];
            return paintcanvas_ext_change_mat(this, mesh, mat);
        }
    }
}

void PaintCanvas::SetColor(unsigned int color) {
    float c0 = (float) ((double) (color >> 24) / 255.0);
    float c1 = (float) ((double) ((color >> 16) & 0xff) / 255.0);
    float c2 = (float) ((double) ((color >> 8) & 0xff) / 255.0);
    float c3 = (float) ((double) (color & 0xff) / 255.0);
    this->colorR = c0;
    this->colorG = c1;
    this->colorB = c2;
    this->colorA = c3;
    return paintcanvas_ext_setcolor(this->engine, c0, c1, c2, c3);
}

void PaintCanvas::Vibrate(unsigned short) {
    return paintcanvas_ext_vibrate(this->engine);
}

void PaintCanvas::SpriteSystemAddSize(unsigned int index, unsigned short sub, short delta) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            short *vals = (short *) s->sizes;
            if (s->uniformSize != 0) {
                vals[0] += delta;
                return;
            }
            vals[sub] += delta;
        }
    }
}

void PaintCanvas::TransformSetLocal(unsigned int index, const Matrix &matrix) {
    if (index < this->transformCount) {
        Matrix *t = (Matrix *) (this->transforms)[index];
        *t = matrix;
    }
}

unsigned int PaintCanvas::GetMeshResourceId(AbyssEngine::String &name, unsigned short p2) {
    for (unsigned int i = 0; i < this->resources.count; ++i) {
        PCResourceView *res = (PCResourceView *) (this->resources.data_)[i];
        if (res && res->type == 4) {
            if (paintcanvas_ext_strcmp(&name, *(void **) res->payload) == 0) {
                PCResourceView *res2 = (PCResourceView *) (this->resources.data_)[i];
                if (((PCMaterialIdView *) res2->payload)->materialId == p2) {
                    return res2->id;
                }
            }
        }
    }
    return 0xffff;
}

int PaintCanvas::GetTextWidth(unsigned int index, const AbyssEngine::String &str) {
    if (index < this->fonts.count) {
        ImageFont *font = this->fonts.data_[index];
        const unsigned short *text = str;
        return ImageFontGetWidth(font, text, str.size());
    }
    return 0;
}

void PaintCanvas::SpriteSystemSetSize(unsigned int index,
                                      unsigned short sub, short value) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if (sub < (unsigned short) s->count) {
                unsigned short *p = (unsigned short *) s->sizes;
                if (s->uniformSize) {
                    p[0] = value;
                } else {
                    p[sub] = value;
                }
            }
        }
    }
}

void PaintCanvas::SetColor(unsigned char r, unsigned char g,
                           unsigned char b, unsigned char a) {
    float fr = (float) ((double) (unsigned int) r / 255.0);
    float fg = (float) ((double) (unsigned int) g / 255.0);
    float fb = (float) ((double) (unsigned int) b / 255.0);
    float fa = (float) ((double) (unsigned int) a / 255.0);
    this->colorR = fr;
    this->colorG = fg;
    this->colorB = fb;
    this->colorA = fa;
    return paintcanvas_ext_setcolor(this->engine, fr, fg, fb, fa);
}

float PaintCanvas::CameraGetCurrentFactor1() {
    unsigned int cur = this->currentCamera;
    if (cur >= this->cameras.count) {
        return 1.0f;
    }
    PCCameraView *cam = (PCCameraView *) (this->cameras.data_)[cur];
    return cam->factor48;
}

void PaintCanvas::SpriteSystemAddPosition(unsigned int index, unsigned short sub,
                                          float x, float y, float z) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned int) (unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            float *p = (float *) (s->positions + sub * 12);
            p[0] = p[0] + x;
            p[1] = p[1] + y;
            p[2] = p[2] + z;
        }
    }
}

void PaintCanvas::MeshSetBiTangent(unsigned int index, unsigned short vtx, const Vector &value) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if (vtx >= mesh->vertexCount) {
            return;
        }
        char *base = mesh->bitangents;
        ((Vector *) base)[vtx] = value;
    }
}

int PaintCanvas::GetScreenPosition(const AbyssEngine::AEMath::Vector &a,
                                   AbyssEngine::AEMath::Vector &b) {
    Matrix identity(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f);
    return GetScreenPosition(identity, a, b);
}

static char paintcanvas_g_fog_flag_storage = 0;
static char paintcanvas_g_fog_ptr_storage = 0;
static char *paintcanvas_g_fog_flag = &paintcanvas_g_fog_flag_storage;
static char *paintcanvas_g_fog_ptr = &paintcanvas_g_fog_ptr_storage;

void PaintCanvas::FogEnable(bool mode, AbyssEngine::FogMode enable) {
    this->fogEnableFlag = enable;
    if (enable == 0) {
        if (*paintcanvas_g_fog_flag != 0) {
            *paintcanvas_g_fog_ptr = (char) mode;
        } else if (mode == 0) {
            paintcanvas_ext_gl_disable(0xb60);
        } else {
            paintcanvas_ext_gl_enable(0xb60);
        }
        mode = 0;
    }
    this->fogMode = (char) mode;
}

void PaintCanvas::MeshSetColor(unsigned int index, unsigned short sub,
                               unsigned int color) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if ((unsigned int) sub < (unsigned int) mesh->vertexCount) {
            float *p = (float *) (mesh->colors + sub * 0x10);
            p[0] = (float) ((double) (color >> 24) / 255.0);
            p[1] = (float) ((double) ((color >> 16) & 0xff) / 255.0);
            p[2] = (float) ((double) ((color >> 8) & 0xff) / 255.0);
            p[3] = (float) ((double) (color & 0xff) / 255.0);
        }
    }
}

static float paintcanvas_g_pom_persp_storage = 0.0f;
static float paintcanvas_g_pom_a_storage = 0.0f;
static float paintcanvas_g_pom_b_storage = 0.0f;
static float *paintcanvas_g_pom_persp = &paintcanvas_g_pom_persp_storage;
static float *paintcanvas_g_pom_a = &paintcanvas_g_pom_a_storage;
static float *paintcanvas_g_pom_b = &paintcanvas_g_pom_b_storage;

void PaintCanvas::SetProjOrthoMatrix() {
    float g = *paintcanvas_g_pom_persp;
    if (g != -1.0f) {
        paintcanvas_ext_setprojmatrix3d(this, g, *paintcanvas_g_pom_a, *paintcanvas_g_pom_b);
    }
    void *eng = this->engine;
    float *r;
    r = &this->projOrthoMatrix.m[12];
    r[0] = 0.0f;
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;
    r = &this->projOrthoMatrix.m[8];
    r[0] = 0.0f;
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;
    r = &this->projOrthoMatrix.m[4];
    r[0] = 0.0f;
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;
    r = &this->projOrthoMatrix.m[0];
    r[0] = 0.0f;
    r[1] = 0.0f;
    r[2] = 0.0f;
    r[3] = 0.0f;

    int w = paintcanvas_ext_getdisplaywidth(eng);
    this->projOrthoMatrix.m[0] = (float) (2.0 / (double) w);

    int h = paintcanvas_ext_getdisplayheight(this->engine);
    this->projOrthoMatrix.m[10] = -0.05f;
    this->projOrthoMatrix.m[15] = 1.0f;
    this->projOrthoMatrix.m[12] = -1.0f;
    this->projOrthoMatrix.m[13] = 1.0f;
    this->projOrthoMatrix.m[5] = -(float) (2.0 / (double) h);
}

void PaintCanvas::MeshChangeMaterialIntern(AbyssEngine::Mesh *mesh, AbyssEngine::Material *mat) {
    if (mesh && mat) {
        mesh->field_0x30 = mat;
        return paintcanvas_ext_mat_intern(this, mesh->animation, mat);
    }
}

void PaintCanvas::MeshResourceChangeAllMaterial(unsigned short matId, unsigned short value) {
    unsigned int count = this->resources.count;
    for (unsigned int i = 0; i < count; ++i) {
        PCResourceView *res = (PCResourceView *) (this->resources.data_)[i];
        if (res) {
            PCMaterialIdView *mat = (PCMaterialIdView *) res->payload;
            if (mat->materialId == matId) {
                mat->materialId = value;
            }
        }
    }
}

unsigned int PaintCanvas::GetTextureResourceId(AbyssEngine::String &name) {
    for (unsigned int i = 0; i < this->resources.count; ++i) {
        PCResourceView *res = (PCResourceView *) (this->resources.data_)[i];
        if (res && res->type == 2) {
            void *namePtr = *(void **) res->payload;
            if (paintcanvas_ext_strcmp(&name, namePtr) == 0) {
                return ((PCResourceView *) (this->resources.data_)[i])->id;
            }
        }
    }
    return 0xffff;
}

unsigned int PaintCanvas::GetColor() {
    float a = this->colorR;
    float r = this->colorG;
    float g = this->colorB;
    float b = this->colorA;
    return ((unsigned int) (int) (r * 255.0f) << 16) +
           ((unsigned int) (int) (a * 255.0f) << 24) +
           ((unsigned int) (int) (g * 255.0f) << 8) +
           (unsigned int) (int) (b * 255.0f);
}

void PaintCanvas::CameraSetCurrent(unsigned int index) {
    this->currentCamera = index;
    if (index < this->cameras.count) {
        PCCameraParamsView *cam = (PCCameraParamsView *) (this->cameras.data_)[index];
        return paintcanvas_ext_camera_apply(this, cam->raw[0], cam->raw[1], cam->raw[2]);
    }
}

void PaintCanvas::MeshSetTriangleCount(unsigned int index, unsigned short count) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        unsigned short cap = mesh->triCapacity;
        if (cap < count) {
            count = cap;
        }
        mesh->indexCount = (unsigned short) (count * 3);
    }
}

void PaintCanvas::DisableClip() {
    glDisable(0xc11);
}

void PaintCanvas::TransformSetColor(unsigned int index, unsigned int color) {
    if (index < this->transformCount) {
        PCTransformView *obj = (PCTransformView *) (this->transforms)[index];
        obj->color = color;
    }
}

void PaintCanvas::MeshSetColor(unsigned int index, unsigned short sub,
                               float r, float g, float b, float a) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if ((unsigned int) sub < (unsigned int) mesh->vertexCount) {
            float *p = (float *) (mesh->colors + sub * 0x10);
            p[0] = r;
            p[1] = g;
            p[2] = b;
            p[3] = a;
        }
    }
}

void PaintCanvas::MeshSetNormal(unsigned int index, unsigned short vtx, const Vector &value) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if (vtx >= mesh->vertexCount) {
            return;
        }
        char *base = mesh->normals;
        ((Vector *) base)[vtx] = value;
    }
}

void PaintCanvas::MeshClear2DMask() {
    this->mask2dImage = 0;
}

unsigned int PaintCanvas::GetMeshResourceId(AbyssEngine::String &name) {
    for (unsigned int i = 0; i < this->resources.count; ++i) {
        PCResourceView *res = (PCResourceView *) (this->resources.data_)[i];
        if (res && res->type == 4) {
            void *namePtr = *(void **) res->payload;
            if (paintcanvas_ext_strcmp(&name, namePtr) == 0) {
                return ((PCResourceView *) (this->resources.data_)[i])->id;
            }
        }
    }
    return 0xffff;
}

void PaintCanvas::MeshTranslatePoint(unsigned int index, unsigned short sub,
                                     float x, float y, float z) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if ((unsigned int) mesh->vertexCount <= (unsigned int) sub) {
            return;
        }
        float *p = (float *) (mesh->positions + sub * 12);
        p[0] = p[0] + x;
        p[1] = p[1] + y;
        p[2] = p[2] + z;
    }
}

void PaintCanvas::CheckNUseRefractFBO(bool) {
    if (Engine::enableShader && Engine::EnableRefract &&
        !this->engine->IsPostEffectActivated()) {
        this->engine->ActivateRender2FracFBO();
    }
}

void PaintCanvas::SpriteSystemSetUv(unsigned int index, unsigned short sub,
                                    float a, float b, float c, float d) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned int) (unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            return AbyssEngine::SpriteSystemSetUv(sub, a, b, c, d,
                                                   reinterpret_cast<SpriteSystem *>(s));
        }
    }
}

void PaintCanvas::SetWorldViewMatrix(const AbyssEngine::AEMath::Matrix &matrix) {
    this->engine->SetWorldViewMatrix(matrix);
}

void PaintCanvas::CameraSetLocal(unsigned int index, const Matrix &matrix) {
    if (index < this->cameras.count) {
        PCCameraView *cam = (PCCameraView *) (this->cameras.data_)[index];
        *(Matrix *) cam->localMatrix = matrix;
    }
}

void PaintCanvas::SetShaderMode(int mode) {
    ((Engine *) this->engine)->field_0x4a8 = mode;
}

void PaintCanvas::MeshConvertToVBO(unsigned int index) {
    if (index < this->meshCount) {
        Mesh *mesh = reinterpret_cast<Mesh *>(this->meshes[index]);
        AbyssEngine::MeshConvertToVBO(mesh);
    }
}


static const float g_di2_one_88d90 = 1.0f;

static const float g_di2_def_88d94 = 0.0f;

void PaintCanvas::DrawImage2D(unsigned int index, int x, int y,
                              unsigned char flipFlags) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    if (img->restoreFlag != 0) {
        this->RestoreImage2D(reinterpret_cast<Image2D *>(img));
        img = (PCImage2DView *) ((char **) this->images.data_)[index];
    }
    this->SetTexture(img->textureId, 0xffffffffu);

    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);

    float m[16];
    memset(m, 0, sizeof(m));
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;

    if (flipFlags & 1) {
        m[0] = g_di2_one_88d90;
        float off = g_di2_def_88d94;
        if (index < this->images.count) {
            unsigned short w = ((PCImage2DView *) ((char **) this->images.data_)[index])->width;
            off = static_cast<float>(w);
        }
        fx = off + fx;
    }
    if (flipFlags & 2) {
        m[5] = g_di2_one_88d90;
        float off = g_di2_def_88d94;
        if (index < this->images.count) {
            unsigned short h = ((PCImage2DView *) ((char **) this->images.data_)[index])->height;
            off = static_cast<float>(h);
        }
        fy = off + fy;
    }
    m[3] = fx;
    m[7] = fy;

    this->SetWorldViewMatrix(*reinterpret_cast<const Matrix *>(m));
    glDisable(0xb44);
    AbyssEngine::MeshDraw(this->engine,
                          reinterpret_cast<Mesh *>(((PCImage2DView *) ((char **) this->images.data_)[index])->mesh));
    glEnable(0xb44);
}

void PaintCanvas::FontSetYOffset(unsigned int index, short yoff) {
    if (index < this->fonts.count) {
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_font_set_yoff(font, yoff);
    }
}


void PaintCanvas::SetBlendMode(AbyssEngine::BlendMode mode) {
    this->engine->LightEnable(false);

    if (Engine::enableShader) {
        this->engine->GlEnable(0x1100020, false);
    } else {
        paintcanvas_ext_sbm_glTexEnvi(0x2300, 0x2200, 0x2100);
    }

    switch (mode) {
        case 1:
        case 5:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            glDepthMask(0);
            return;
        case 2:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(1, 1);
            glDepthMask(0);
            return;
        case 3:
            paintcanvas_ext_sbm_glDisable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(1, 1);
            glDepthMask(0);
            return;
        case 4:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0, 0x301);
            glDepthMask(0);
            return;
        case 6:
            this->engine->LightEnable(true);
            this->engine->LightSetLight(0x4000);
        /* fallthrough */
        case 0:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glDisable(0xbe2);
            glDepthMask(1);
            return;
        case 7:
            this->engine->LightEnable(true);
            this->engine->LightSetLight(0x4000);
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(1, 1);
            glDepthMask(0);
            return;
        case 8:
            this->engine->LightEnable(true);
            this->engine->LightSetLight(0x4000);
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            glDepthMask(0);
            return;
        case 9:
            this->engine->LightEnable(true);
            this->engine->LightSetLight(0x4000);
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            glDepthMask(1);
            return;
        case 10:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glDisable(0xbe2);
            paintcanvas_ext_sbm_glDepthMask(1);
            this->engine->GlEnable(0x1000000, true);
            if (!Engine::enableShader) {
                paintcanvas_ext_sbm_glAlphaFunc(0x204, 0.5f);
            }
            return;
        case 0x15:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_glDepthMask(0);
            if (Engine::enableShader) {
                this->engine->GlEnable(0x1100020, true);
                return;
            }
            glTexEnvi(0x2300, 0x2200, 0x8570);
            glTexEnvi(0x2300, 0x8571, 0x104);
            glTexEnvi(0x2300, 0x8572, 0x1e01);
            glTexEnvi(0x2300, 0x8580, 0x1702);
            glTexEnvi(0x2300, 0x8590, 0x300);
            glTexEnvi(0x2300, 0x8581, 0x8577);
            glTexEnvi(0x2300, 0x8591, 0x300);
            return;
        case 0x16:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_glDepthMask(0);
            if (!Engine::enableShader) {
                glTexEnvi(0x2300, 0x2200, 0x8570);
                glTexEnvi(0x2300, 0x8571, 0x8575);
                glTexEnvi(0x2300, 0x8572, 0x1e01);
                glTexEnvi(0x2300, 0x8580, 0x1702);
                glTexEnvi(0x2300, 0x8590, 0x300);
                glTexEnvi(0x2300, 0x8588, 0x1702);
                glTexEnvi(0x2300, 0x8598, 0x302);
                glTexEnvi(0x2300, 0x8581, 0x8577);
                glTexEnvi(0x2300, 0x8591, 0x300);
                glTexEnvi(0x2300, 0x8582, 0x8577);
                glTexEnvi(0x2300, 0x8592, 0x302);
            }
            return;
        case 0x25:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            glDepthMask(1);
            return;
        default:
            return;
    }
}


static const char g_gla_nl_8c4c0[] = "\n";

void PaintCanvas::GetLineArray(unsigned int font, const AbyssEngine::String &str, int width,
                               ::Array<AbyssEngine::String *> *outArray) {
    AbyssEngine::String *acc = paintcanvas_ext_gla_str_new();

    char src[16];
    char nl[16];
    paintcanvas_ext_dsc_str_copy(src, &str, false);
    paintcanvas_ext_gla_str_fromchar(nl, g_gla_nl_8c4c0, false);
    paintcanvas_ext_gla_str_append(src, nl);
    paintcanvas_ext_gla_str_dtor(nl);

    unsigned int count = 0;
    int pos = 0;
    int srcLen = ((PCStrLenView *) src)->length;
    while (pos < srcLen) {
        char sub[16];
        char line[16];
        paintcanvas_ext_gla_substr(sub, src, 0, (unsigned int) srcLen);
        paintcanvas_ext_gla_str_copy(line, sub, false);
        paintcanvas_ext_gla_getline(this, font, line, width, acc);
        paintcanvas_ext_gla_str_dtor(line);
        pos += (int) acc->size();
        paintcanvas_ext_gla_str_dtor(sub);
        count++;
    }
    paintcanvas_ext_gla_str_vdtor(acc);

    paintcanvas_ext_gla_arr_setlen(count, outArray);
    for (unsigned int i = 0; i < count; i++) {
        outArray->data_[i] = paintcanvas_ext_gla_str_new();
    }

    for (unsigned int i = 0; i < count; i++) {
        char sub[16];
        char line[16];
        paintcanvas_ext_gla_substr(sub, src, 0, (unsigned int) ((PCStrLenView *) src)->length);
        paintcanvas_ext_gla_str_copy(line, sub, false);
        AbyssEngine::String *out = outArray->data_[i];
        paintcanvas_ext_gla_getline(this, font, line, width, out);
        paintcanvas_ext_gla_str_dtor(line);

        AbyssEngine::String *cur = outArray->data_[i];
        int len = (int) cur->size();
        int lo = 0;
        while (*paintcanvas_ext_gla_str_index(outArray->data_[i], lo) == 0x20) {
            lo++;
        }
        len++;
        do {
            len--;
        } while (*paintcanvas_ext_gla_str_index(outArray->data_[i], len - 2) == 0x20);

        char trimmed[16];
        paintcanvas_ext_gla_substr(trimmed, outArray->data_[i],
                                   (unsigned int) lo, (unsigned int) len);
        paintcanvas_ext_gla_str_assign(outArray->data_[i], trimmed);
        paintcanvas_ext_gla_str_dtor(trimmed);
        paintcanvas_ext_gla_str_dtor(sub);
    }
    paintcanvas_ext_gla_str_dtor(src);
}

void PaintCanvas::AddResource(AbyssEngine::Resource *resource) {
    return paintcanvas_ext_add_resource(resource, &this->resources);
}

void PaintCanvas::TransformRemoveMeshId(unsigned int transformIndex, unsigned int meshIndex) {
    if (transformIndex < this->transformCount &&
        meshIndex < this->meshCount) {
        void *mesh = (this->meshes)[meshIndex];
        PCTransformView *t = (PCTransformView *) (this->transforms)[transformIndex];
        return paintcanvas_ext_remove_meshid(mesh, &t->meshCount);
    }
}

int PaintCanvas::GetHeight() {
    return this->engine->GetDisplayHeight();
}

void PaintCanvas::ChangeCubeTexture(unsigned int idx) {
    if (Engine::enableShader && idx < this->cubeTextures.count) {
        PCCubeTexView *tex = (PCCubeTexView *) (this->cubeTextures.data_)[idx];
        if (tex->restoreFlag == 0) {
            char *path = reinterpret_cast<String *>(tex->pathField)->GetAEChar();
            delete[] path;
            return;
        }
        AbyssEngine::CubeMapSetIndex = idx;
        glActiveTexture(0x84c7);
        glBindTexture(0x8513, (unsigned int) tex->glTexId);
        glActiveTexture(0x84c0);
    }
}


void PaintCanvas::TransformCreate(unsigned short resId, unsigned int &out) {
    PCResourceView *res = reinterpret_cast<PCResourceView *>(this->FindResource(resId));
    if (res == 0) {
        return;
    }
    if ((unsigned int) res->handle != 0xffffffff) {
        out = (unsigned int) res->handle;
        return;
    }
    PCTransformInfoView *info = (PCTransformInfoView *) res->payload;
    Transform *transform = new Transform();
    ArrayAdd<Transform *>(
        transform,
        *reinterpret_cast<Array<Transform *> *>(&this->transformCount));
    unsigned int idx = this->transformCount - 1;
    res->handle = (int) idx;
    out = idx;
    transform->worldMatrix = *reinterpret_cast<const AEMath::Matrix *>(info);

    for (unsigned int i = 0; i < info->childMeshCount; i++) {
        unsigned short mid = ((unsigned short *) info->childMeshIds)[i];
        unsigned int childMesh = 0xffffffff;
        this->MeshCreate(mid, childMesh, false);
        if (childMesh != 0xffffffff) {
            ArrayAdd<Mesh *>(
                reinterpret_cast<Mesh *>(this->meshes[childMesh]),
                transform->meshes);
        }
    }
    for (unsigned int i = 0; i < info->childTfCount; i++) {
        unsigned short tid = ((unsigned short *) info->childTfIds)[i];
        unsigned int childTf = 0xffffffff;
        this->TransformCreate(tid, childTf);
        if (childTf != 0xffffffff) {
            ArrayAdd<Transform *>(
                reinterpret_cast<Transform *>(this->transforms[childTf]),
                transform->children);
        }
    }
}

void PaintCanvas::End2d() {
    char buf[60];
    int v = this->field_0xc;
    if (v >= 1) {
        float *m = (float *) buf;
        ((PCMeshView *) this->quad2dMesh)->indexCount = (unsigned short) (short) (v * 6);
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 0.0f;
        m[5] = 1.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;
        m[12] = 1.0f;
        m[13] = 1.0f;
        m[14] = 1.0f;
        paintcanvas_ext_set_wvm2(this, buf);
        paintcanvas_ext_meshdraw(this->engine, this->quad2dMesh);
    }
}

void PaintCanvas::MeshChangeMaterialIntern(Transform *transform, AbyssEngine::Material *material) {
    if (transform && material) {
        for (unsigned int i = 0; i < transform->meshes.size(); ++i) {
            this->MeshChangeMaterialIntern(transform->meshes[i], material);
        }
        for (unsigned int i = 0; i < transform->children.size(); ++i) {
            this->MeshChangeMaterialIntern(transform->children[i], material);
        }
    }
}

int PaintCanvas::GetTextHeight(unsigned int index) {
    if (index < this->fonts.count) {
        return ImageFontGetHeight(this->fonts.data_[index]);
    }
    return 0;
}


void PaintCanvas::Initialize(bool landscape) {
    this->gameOrientation = landscape ? 2 : 0;
    this->engine->SetScreenOrientation(static_cast<LandscapeMode>(this->gameOrientation));

    memset(&this->projOrthoMatrix.m[0], 0, 0x10);
    memset(&this->projOrthoMatrix.m[4], 0, 0x10);
    memset(&this->projOrthoMatrix.m[8], 0, 0x10);
    memset(&this->projOrthoMatrix.m[12], 0, 0x10);

    int orient = this->gameOrientation;
    float fw = static_cast<float>(this->engine->GetDisplayWidth());
    float fh = static_cast<float>(this->engine->GetDisplayHeight());

    float ymul;
    if (orient == 2) {
        this->projOrthoMatrix.m[0] = 2.0f / fw;
        ymul = fh;
    } else {
        memset(&this->worldViewMatrix.m[0], 0, 0x10);
        memset(&this->worldViewMatrix.m[4], 0, 0x10);
        memset(&this->worldViewMatrix.m[8], 0, 0x10);
        memset(&this->worldViewMatrix.m[11], 0, 0x10);
        this->worldViewMatrix.m[15] = 1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fh;
        this->worldViewMatrix.m[1] = -1.0f;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = 1.0f;
        this->worldViewMatrix.m[13] = fw;
        ymul = fw;
    }
    this->projOrthoMatrix.m[10] = -0.05f;
    this->projOrthoMatrix.m[15] = 1.0f;
    this->projOrthoMatrix.m[12] = -1.0f;
    this->projOrthoMatrix.m[13] = 1.0f;
    this->projOrthoMatrix.m[5] = -2.0f / ymul;

    this->SetProjectionMatrix3d(16384.0f, 512.0f, 65536.0f);
}

void PaintCanvas::DrawMesh(unsigned int index) {
    if (index < this->meshCount) {
        Mesh *mesh = reinterpret_cast<Mesh *>(this->meshes[index]);
        AbyssEngine::MeshDraw(this->engine, mesh);
    }
}

int PaintCanvas::MeshGetTriCount(AbyssEngine::Mesh *mesh) {
    if (mesh) {
        int tri;
        if (mesh->animation == 0) {
            tri = 0;
        } else {
            tri = this->TransformGetTriCount(mesh->animation);
        }
        int q = (int) ((unsigned) (mesh->indexCount) / (unsigned) (3));
        return q + tri;
    }
    return 0;
}

int PaintCanvas::FontGetYOffset(unsigned int index) {
    if (index < this->fonts.count) {
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_font_get_yoff(font);
    }
    return 0;
}


void PaintCanvas::SetProjectionMatrix3d(float fov, float nearPlane,
                                        float farPlane) {
    Engine::farPlane = farPlane;
    Engine::nearPlane = nearPlane;
    i_fov = fov;
    i_zNear = nearPlane;
    i_zFar = farPlane;

    int w = this->engine->GetDisplayWidth();
    int h = this->engine->GetDisplayHeight();
    float half = fov * 0.5f;
    float s = AEMath::Sinf(half);
    float c = AEMath::Cosf(half);
    float fw = static_cast<float>(w);
    float fh = static_cast<float>(h);

    memset(&this->projMatrix3d.m[12], 0, 0x10);
    memset(&this->projMatrix3d.m[8], 0, 0x10);
    memset(&this->projMatrix3d.m[4], 0, 0x10);
    memset(&this->projMatrix3d.m[0], 0, 0x10);

    if (this->gameOrientation <= 3) {
        float aspect = fw / fh;
        float f = 1.0f / (s / c);
        switch (this->gameOrientation) {
            case 0:
                this->projMatrix3d.m[4] = -f;
                this->projMatrix3d.m[1] = f / aspect;
                break;
            case 1:
                this->projMatrix3d.m[4] = f;
                this->projMatrix3d.m[1] = -(f / aspect);
                break;
            case 2:
                this->projMatrix3d.m[5] = f;
                this->projMatrix3d.m[0] = f / aspect;
                break;
            case 3:
                this->projMatrix3d.m[5] = -f;
                this->projMatrix3d.m[0] = -(f / aspect);
                break;
        }
    }

    this->projMatrix3d.m[11] = -1.0f;
    this->projMatrix3d.m[10] = (nearPlane + farPlane) / (nearPlane - farPlane);
    this->projMatrix3d.m[14] = ((farPlane + farPlane) * nearPlane) / (nearPlane - farPlane);
}


void PaintCanvas::DrawSpriteSystem(unsigned int index) {
    if (index >= this->spriteSystems.count) {
        return;
    }
    SpriteSystem *sys = this->spriteSystems.data_[index];
    if (sys == 0) {
        return;
    }

    Matrix world;

    if (this->currentCamera < this->cameras.count) {
        PCCameraView *cam = (PCCameraView *) this->cameras.data_[this->currentCamera];
        if (this->initialized == 0) {
            world = AEMath::MatrixGetInverse(*reinterpret_cast<Matrix *>(cam->localMatrix));
        } else {
            Matrix rotation;
            AEMath::MatrixIdentity(rotation);
            double angle = this->engine->GetGravValue()[1] * 1.57079637;
            float a = (float) angle;
            int orient = this->gameOrientation;
            float rot = (orient == 1) ? a : -a;
            float s = AEMath::Sinf(rot);
            float c = AEMath::Cosf(rot);
            rotation.m[0] = c;
            rotation.m[5] = c;
            rotation.m[1] = -s;
            rotation.m[4] = s;

            Matrix cameraMatrix;
            memcpy(&cameraMatrix, cam->localMatrix, sizeof(cameraMatrix));
            cameraMatrix *= rotation;
            world = AEMath::MatrixGetInverse(cameraMatrix);
        }
    }

    Matrix view;
    SpriteSystemDraw(this->engine, view, world, sys);
}

float PaintCanvas::MeshSetPoint(unsigned int index, unsigned short vtx,
                                float x, float y, float z) {
    if (index < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        if (vtx < mesh->vertexCount) {
            float *p = (float *) (mesh->positions + vtx * 12);
            p[0] = x;
            p[1] = y;
            p[2] = z;
            return z;
        }
    }
    return x;
}


static const char g_getline_empty_7c428[] = "";

int paintcanvas_ext_gl_textwidth(void *self, unsigned int font, const AbyssEngine::String *str,
                                            unsigned int begin, unsigned int end);

void PaintCanvas::GetLine(unsigned int font, AbyssEngine::String str, int maxWidth,
                          AbyssEngine::String *out) {
    char tmp[16];
    unsigned int lastSpace = 0;
    int width = 5;
    unsigned int i = 0;
    unsigned int len = str.size();

    while (i < len) {
        unsigned short *ch = paintcanvas_ext_gl_strindex(&str, i);
        unsigned short c = *ch;
        unsigned int next = i + 1;
        width += paintcanvas_ext_gl_textwidth(this, font, &str, i, next);
        if (c == 0x20) {
            lastSpace = i;
        }
        if (width >= maxWidth) {
            if ((int) lastSpace < 1) {
                paintcanvas_ext_gl_substr(tmp, &str, 0, next);
            } else {
                paintcanvas_ext_gl_substr(tmp, &str, 0, lastSpace + 1);
            }
            paintcanvas_ext_gl_str_assign(out, tmp);
            paintcanvas_ext_gl_str_dtor(tmp);
            return;
        }
        unsigned short *ch2 = paintcanvas_ext_gl_strindex(&str, i);
        if (*ch2 == 0xa) {
            paintcanvas_ext_gl_substr(tmp, &str, 0, next);
            paintcanvas_ext_gl_str_assign(out, tmp);
            paintcanvas_ext_gl_str_dtor(tmp);
            return;
        }
        unsigned short *ch3 = paintcanvas_ext_gl_strindex(&str, i);
        i = next;
        if (*ch3 == 0xd) {
            paintcanvas_ext_gl_substr(tmp, &str, 0, next);
            paintcanvas_ext_gl_str_assign(out, tmp);
            paintcanvas_ext_gl_str_dtor(tmp);
            return;
        }
    }

    if ((int) len < 2) {
        paintcanvas_ext_gl_str_fromchar(tmp, g_getline_empty_7c428, false);
    } else {
        paintcanvas_ext_gl_substr(tmp, &str, 0, len);
    }
    paintcanvas_ext_gl_str_assign(out, tmp);
    paintcanvas_ext_gl_str_dtor(tmp);
}


void PaintCanvas::DrawLine(int x0, int y0, int x1i, int y1i) {
    Matrix worldView(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 1.0f);

    float x1 = static_cast<float>(x0 + 1);
    float y1 = static_cast<float>(y0);
    float x2 = static_cast<float>(x1i + 1);
    float y2 = static_cast<float>(y1i);

    float *v = this->lineVerts;

    if (!Engine::enableShader) {
        glLineWidth(1.0f);
        this->engine->GlEnable(0xde1, false);
        v[0] = x1;
        v[1] = y1;
        v[2] = x2;
        v[3] = y2;
        this->SetWorldViewMatrix(worldView);
        glVertexPointer(2, 0x1406, 0, this->lineVerts);
        this->engine->AEClientState(0x8074, true);
        this->engine->AEClientState(0x8078, false);
        this->engine->AEClientState(0x8075, false);
        this->engine->AEClientState(0x8076, false);
        glDrawArrays(1, 0, 2);
        this->engine->GlEnable(0xde1, true);
    } else {
        this->SetWorldViewMatrix(worldView);
        v[0] = x1;
        v[1] = y1;
        v[2] = x2;
        v[3] = y2;
        this->engine->DrawLine2D(this->lineVerts, 2, false);
    }
}

void PaintCanvas::MeshChangeMaterial(unsigned int meshIndex, unsigned short matIndex) {
    if (matIndex < this->materials.count &&
        meshIndex < this->meshCount) {
        void *mesh = (this->meshes)[meshIndex];
        void *mat = (this->materials.data_)[matIndex];
        return paintcanvas_ext_change_mat(this, mesh, mat);
    }
}

void PaintCanvas::CameraSetPerspective(unsigned int index, float a, float b, float c) {
    if (index < this->cameras.count) {
        float w = (float) paintcanvas_ext_get_w(this);
        float h = (float) paintcanvas_ext_get_h(this);
        void *cam = (this->cameras.data_)[index];
        paintcanvas_ext_cam_persp4(a, b, c, w, h, cam);
        if (this->currentCamera == index) {
            return paintcanvas_ext_cam_setcur(this, index);
        }
    }
}

void PaintCanvas::End3d() {
    Engine *eng = this->engine;

    if (Engine::enableShader) {
        this->RemoveAllMatsForGlow();
    }
    const bool fogPass = this->fogEnableFlag == 1 && this->fogMode != 0;
    if (fogPass) {
        if (Engine::enableShader) {
            Engine::fogEnabled = true;
        } else {
            glEnable(0xb60);
        }
    }

    this->SetBlendMode(static_cast<BlendMode>(0));
    bool glowQueued = false;
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 0) {
            if ((Engine::enableShader && eng->IsPostEffectActivated() && Engine::EnableGlow) && material->addData != nullptr) {
                this->SetMatForGlow(material);
                glowQueued = true;
            }
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
        if ((Engine::enableShader && eng->IsPostEffectActivated() && Engine::EnableGlow) && glowQueued) {
            auto &meshes = *reinterpret_cast<Array<Mesh *> *>(&this->glowMeshes_count);
            auto &matA = *reinterpret_cast<Array<Matrix> *>(&this->glowMatA_count);
            auto &matB = *reinterpret_cast<Array<Matrix> *>(&this->glowMatB_count);
            auto &colors = *reinterpret_cast<Array<unsigned int> *>(&this->glowUints_count);
            auto &matC = *reinterpret_cast<Array<Matrix> *>(&this->glowMatC_count);
            for (unsigned int j = 0; j < meshes.size_; ++j) {
                ArrayAdd(meshes.data_[j], material->meshes);
                ArrayAdd(matA.data_[j], material->arr_2c);
                ArrayAdd(matB.data_[j], material->arr_38);
                PCArrayAdd<unsigned int>(colors.data_[j], &material->arr_50);
                ArrayAdd(matC.data_[j], material->arr_5c);
            }
            this->RemoveAllMatsForGlow();
            glowQueued = false;
        }
    }

    if (fogPass) {
        if (Engine::enableShader) {
            Engine::fogEnabled = false;
        } else {
            glDisable(0xb60);
        }
        for (unsigned int i = 0; i < this->materials.count; ++i) {
            Material *material = this->materials.data_[i];
            if (material->materialMode == 23) {
                AbyssEngine::MaterialDraw(this, eng, material, true);
            }
        }
        if (Engine::enableShader) {
            Engine::fogEnabled = true;
        } else {
            glEnable(0xb60);
        }
    }
    eng->LightEnable(true);
    Matrix camera = *static_cast<Matrix *>(this->CameraGetLocal(this->currentCamera));
    Matrix inverse = AEMath::MatrixGetInverse(camera);
    eng->SetWorldViewMatrix(inverse);
    eng->LightSetLight(0x4000);

    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        const int mode = material->materialMode;
        unsigned int shaderMode = 0;
        bool drawWithShader = false;
        switch (mode) {
            case 6:
                AbyssEngine::MaterialDraw(this, eng, material, true);
                break;
            case 11:
                shaderMode = 0x1100000;
                drawWithShader = true;
                break;
            case 12:
            case 13:
            case 28:
            case 29:
                shaderMode = AbyssEngine::CubeMapSetIndex ? 0x1100014 : 0x1100013;
                drawWithShader = true;
                break;
            case 15:
                shaderMode = 0x1100001;
                drawWithShader = true;
                break;
            case 19:
                shaderMode = 0x1100006;
                drawWithShader = true;
                break;
            case 38:
                shaderMode = 0x1100022;
                drawWithShader = true;
                break;
            default:
                break;
        }
        if (drawWithShader) {
            if ((mode == 12 || mode == 13 || mode == 28 || mode == 29) &&
                (Engine::enableShader && eng->IsPostEffectActivated() && Engine::EnableGlow) && material->addData != nullptr) {
                this->SetMatForGlow(material);
                glowQueued = true;
            }
            eng->GlEnable(shaderMode, true);
            AbyssEngine::MaterialDraw(this, eng, material, true);
            eng->GlEnable(shaderMode, false);
        }
        if ((Engine::enableShader && eng->IsPostEffectActivated() && Engine::EnableGlow) && glowQueued) {
            auto &meshes = *reinterpret_cast<Array<Mesh *> *>(&this->glowMeshes_count);
            auto &matA = *reinterpret_cast<Array<Matrix> *>(&this->glowMatA_count);
            auto &matB = *reinterpret_cast<Array<Matrix> *>(&this->glowMatB_count);
            auto &colors = *reinterpret_cast<Array<unsigned int> *>(&this->glowUints_count);
            auto &matC = *reinterpret_cast<Array<Matrix> *>(&this->glowMatC_count);
            for (unsigned int j = 0; j < meshes.size_; ++j) {
                ArrayAdd(meshes.data_[j], material->meshes);
                ArrayAdd(matA.data_[j], material->arr_2c);
                ArrayAdd(matB.data_[j], material->arr_38);
                PCArrayAdd<unsigned int>(colors.data_[j], &material->arr_50);
                ArrayAdd(matC.data_[j], material->arr_5c);
            }
            this->RemoveAllMatsForGlow();
            glowQueued = false;
        }
    }

    this->SetBlendMode(static_cast<BlendMode>(8));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        unsigned int shaderMode = 0;
        if (material->materialMode == 8) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        } else if (material->materialMode == 16) {
            shaderMode = 0x1100016;
        } else if (material->materialMode == 36) {
            shaderMode = 0x1100021;
        }
        if (shaderMode != 0) {
            eng->GlEnable(shaderMode, true);
            AbyssEngine::MaterialDraw(this, eng, material, true);
            eng->GlEnable(shaderMode, false);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(9));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 9) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(7));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 7) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    eng->LightEnable(false);
    this->SetBlendMode(static_cast<BlendMode>(37));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 37) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(1));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 1) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    if (fogPass) {
        if (Engine::enableShader) {
            Engine::fogEnabled = false;
        } else {
            glDisable(0xb60);
        }
        for (unsigned int i = 0; i < this->materials.count; ++i) {
            Material *material = this->materials.data_[i];
            if (material->materialMode == 24) {
                AbyssEngine::MaterialDraw(this, eng, material, true);
            }
        }
        if (Engine::enableShader) {
            Engine::fogEnabled = true;
        } else {
            glEnable(0xb60);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(21));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 21) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(22));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 22) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(10));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 10) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    eng->GlEnable(0x1000000, false);
    this->SetBlendMode(static_cast<BlendMode>(2));
    if (fogPass) {
        if (Engine::enableShader) {
            Engine::fogEnabled = false;
        } else {
            glDisable(0xb60);
        }
    }
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 2) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(3));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 3) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    if (fogPass) {
        if (Engine::enableShader) {
            Engine::fogEnabled = true;
        } else {
            glEnable(0xb60);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(4));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 4) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(5));
    unsigned int savedColor = static_cast<unsigned int>(eng->packedColor);
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 5) {
            this->SetColor(255, 255, 255, 127);
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }
    this->SetColor(savedColor);
    this->SetBlendMode(static_cast<BlendMode>(1));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        if (material->materialMode == 18) {
            AbyssEngine::MaterialDraw(this, eng, material, true);
        }
    }

    if (Engine::enableShader && Engine::EnableRefract) {
        eng->CopyFBO();
        eng->SetPerspMatrix(reinterpret_cast<float *>(&this->projMatrix3d));
    }
    bool refractQueued = false;
    this->SetBlendMode(static_cast<BlendMode>(0));
    for (unsigned int i = 0; i < this->materials.count; ++i) {
        Material *material = this->materials.data_[i];
        unsigned int shaderMode = 0;
        if (material->materialMode == 34) {
            shaderMode = 0x1100018;
        } else if (material->materialMode == 14) {
            shaderMode = 0x1100015;
        }
        if (shaderMode != 0) {
            const bool hadMeshes = material->meshes.size_ != 0;
            eng->GlEnable(shaderMode, true);
            AbyssEngine::MaterialDraw(this, eng, material, true);
            eng->GlEnable(shaderMode, false);
            refractQueued |= hadMeshes;
        }
    }
    this->SetBlendMode(static_cast<BlendMode>(3));
    if (Engine::DisableRefract) {
        for (unsigned int i = 0; i < this->materials.count; ++i) {
            Material *material = this->materials.data_[i];
            if (material->materialMode == 39) {
                AbyssEngine::MaterialDraw(this, eng, material, true);
            }
        }
    } else {
        for (unsigned int i = 0; i < this->materials.count; ++i) {
            Material *material = this->materials.data_[i];
            if (material->materialMode == 39) {
                const bool hadMeshes = material->meshes.size_ != 0;
                eng->GlEnable(0x1100024, true);
                AbyssEngine::MaterialDraw(this, eng, material, true);
                eng->GlEnable(0x1100024, false);
                refractQueued |= hadMeshes;
            }
        }
    }
    Engine::EnableRefract = refractQueued;
    if (this->fogMode != 0) {
        if (Engine::enableShader) {
            Engine::fogEnabled = false;
        } else {
            glDisable(0xb60);
        }
    }
    if (Engine::enableShader && eng->IsPostEffectActivated() && Engine::EnableGlow) {
        this->SetBlendMode(static_cast<BlendMode>(0));
        eng->GlowBeginGlow(515);
        for (unsigned int i = 0; i < this->materials.count; ++i) {
            Material *material = this->materials.data_[i];
            if (material->addData != nullptr) {
                eng->GlEnable(0x1100017, true);
                AbyssEngine::MaterialDraw(this, eng, material, true);
                eng->GlEnable(0x1100017, false);
            }
        }
        eng->GlowEndGlow();
    }
}

void PaintCanvas::MeshCloneMaterial(unsigned int index, unsigned int &out) {
    if (index >= this->meshCount) {
        out = 0xffffffff;
        return;
    }
    Mesh *mesh = (Mesh *) this->meshes[index];
    Material *copy = new Material((Material *) mesh->material);
    ArrayAdd(copy, this->materials);
    out = this->materials.count - 1;
}

void PaintCanvas::GetGravValue() {
    return paintcanvas_ext_get_grav(this->engine);
}

static char paintcanvas_g_bg_a = 0;
static char paintcanvas_g_bg_b = 0;

void PaintCanvas::BeginBG() {
    *(unsigned char *) &((Engine *) this->engine)->field_0xfd = 0;
    paintcanvas_ext_gl_disable(0xb71);
    paintcanvas_ext_gl_depthmask(0);
    paintcanvas_ext_gl_disable(0xbe2);
    paintcanvas_ext_gl_color(this->engine, 1.0f, 1.0f, 1.0f, 1.0f);
    this->bgFlagSaved = paintcanvas_g_bg_a;
    char flag = paintcanvas_g_bg_b;
    paintcanvas_g_bg_a = 0;
    if (flag != 0) {
        return paintcanvas_ext_matgl_load(this->engine, &this->projMatrix3d.m[0]);
    }
    paintcanvas_ext_glMatrixMode(0x1702);
    paintcanvas_ext_gl_loadidentity();
    paintcanvas_ext_gl_ortho_persp(1.0f, -1.0f, 1.0f);
    paintcanvas_ext_glMatrixMode(0x1701);
    paintcanvas_ext_gl_loadmatrix(&this->projMatrix3d.m[0]);
    paintcanvas_ext_glMatrixMode(0x1700);
    return paintcanvas_ext_gl_done();
}


void PaintCanvas::FontCreate(unsigned short resId, unsigned int &out,
                             bool /*unused*/) {
    PCResourceView *res = (PCResourceView *) this->FindResource(resId);
    if (res == 0) {
        return;
    }
    unsigned short *info = (unsigned short *) res->payload;
    PCResourceView *texres = (PCResourceView *) this->FindResource(*info);
    if (texres == 0) {
        return;
    }
    if (texres->handle == -1) {
        unsigned int textureIndex = 0;
        this->TextureCreate(*info, textureIndex, false);
    }
    if (res->handle != -1) {
        out = (unsigned int) res->handle;
        return;
    }
    ImageFont *font = 0;
    PCTexInfoView *texInfo = (PCTexInfoView *) texres->payload;
    int ok = ImageCreateFontFromFile(this->engine, texInfo->path, info[1], &font);
    if (ok != 1) {
        return;
    }
    if (texres->handle != -1) {
        font->field_0x8 = (void *) (uintptr_t) (unsigned int) texres->handle;
    }
    ArrayAdd<AbyssEngine::ImageFont *>(font, this->fonts);
    int idx = this->fonts.count - 1;
    res->handle = idx;
    out = idx;

    Engine *eng = (Engine *) this->engine;
    int cur = eng->field_0x78;
    if (cur == -1) {
        eng->field_0x78 = idx;
    } else {
        ImageFont *curFont = (ImageFont *) (this->fonts.data_)[cur];
        if (curFont->codes[0] <= 0x457f) {
            int curH = ImageFontGetHeight(curFont);
            int newH = ImageFontGetHeight(font);
            if (newH > curH) {
                eng->field_0x78 = out;
            }
        }
    }
}

void PaintCanvas::SetResourceList(AbyssEngine::Resource * const * list, unsigned int count) {
    ArrayAdd<AbyssEngine::Resource *>(list, count, this->resources);
}

void PaintCanvas::MaterialResourceChangeTexture(unsigned int resId,
                                                unsigned int texture, int slot) {
    if ((unsigned int) slot < 8) {
        char *r = paintcanvas_ext_find_res(this, resId);
        if (r) {
            unsigned int matIdx = (unsigned int) ((PCResourceView *) r)->handle;
            if (matIdx + 1 != 0 && matIdx < this->materials.count) {
                PCMaterialView *mat = (PCMaterialView *) (this->materials.data_)[matIdx];
                mat->textureSlots[slot] = texture;
            }
        }
    }
}

void PaintCanvas::TransformAddChild(unsigned int parent, unsigned int child) {
    unsigned int count = this->transformCount;
    if (parent != child && child < count && parent < count) {
        Transform *p = reinterpret_cast<Transform *>(this->transforms[parent]);
        Transform *c = reinterpret_cast<Transform *>(this->transforms[child]);
        ArrayAdd(c, p->children);
        p = reinterpret_cast<Transform *>(this->transforms[parent]);
        c = reinterpret_cast<Transform *>(this->transforms[child]);
        p->bounds().Merge(*c);
        // Android 0x7b348 -> veneer 0x19b2a8 -> CollectAnimationData.
        return reinterpret_cast<Transform *>(this->transforms[parent])->CollectAnimationData();
    }
}

int tcg_TextureCreateFromFile(void *engine, const char *path, void *cb, void *ud,
                                         unsigned *outId, bool b, float f);


void PaintCanvas::TextureCreateGlobal(AbyssEngine::String name, unsigned int unit) {
    unsigned int texture = 0;
    char *path = name.GetAEChar();
    int rc = AbyssEngine::TextureCreateFromFile((AbyssEngine::Engine *) this->engine, path, nullptr,
                                                nullptr, &texture, false, 0.0f);
    if (rc == 1) {
        glActiveTexture(unit + 0x84c0);
        glBindTexture(0xde1, texture);
        glActiveTexture(0x84c0);
    }
    ::operator delete[](path);
}

static char paintcanvas_g_use_matgl = 0;

void PaintCanvas::Begin3d() {
    *(unsigned char *) &((Engine *) this->engine)->field_0xfd = 0;
    paintcanvas_ext_gl_enable(0xb71);
    paintcanvas_ext_gl_depthmask(1);
    paintcanvas_ext_gl_disable(0xbe2);
    paintcanvas_ext_gl_color(this->engine, 1.0f, 1.0f, 1.0f, 1.0f);
    if (paintcanvas_g_use_matgl != 0) {
        return paintcanvas_ext_matgl_load(this->engine, &this->projMatrix3d.m[0]);
    }
    paintcanvas_ext_glMatrixMode(0x1702);
    paintcanvas_ext_gl_loadidentity();
    paintcanvas_ext_gl_ortho_persp(1.0f, -1.0f, 1.0f);
    paintcanvas_ext_glMatrixMode(0x1701);
    paintcanvas_ext_gl_loadmatrix(&this->projMatrix3d.m[0]);
    paintcanvas_ext_glMatrixMode(0x1700);
    return paintcanvas_ext_gl_done();
}

void PaintCanvas::TransformAddMesh(unsigned int transformIndex,
                                   unsigned short meshId, bool b) {
    if (transformIndex < this->transformCount) {
        unsigned int meshIndex = 0xffffffff;
        this->MeshCreate(meshId, meshIndex, b);
        if (meshIndex != 0xffffffff) {
            Transform *transform = reinterpret_cast<Transform *>(this->transforms[transformIndex]);
            Mesh *mesh = reinterpret_cast<Mesh *>(this->meshes[meshIndex]);
            ArrayAdd(mesh, transform->meshes);
            transform->bounds().Merge(*reinterpret_cast<AEMath::BSphere *>(&mesh->boundsCenterX));
            if (mesh->animation != nullptr) {
                if (transform->animationLength < mesh->animation->animationLength) {
                    transform->SetAnimationLength(mesh->animation->animationLength);
                }
                transform->SetAnimationState(static_cast<AnimationMode>(2), nullptr);
            }
            transform->CollectAnimationData();
        }
    }
}

AbyssEngine::String PaintCanvas::GetReverseString(AbyssEngine::String in, bool reverse) {
    if (!reverse) {
        return in;
    }
    String out("");
    for (int i = (int) in.size() - 1; i >= 0; --i) {
        out += in.SubString((unsigned) i, (unsigned) (i + 1));
    }
    return out;
}


void PaintCanvas::SpriteSystemCreate(unsigned short resId, bool flag,
                                     unsigned short matResId, unsigned int &out) {
    void *ss = 0;
    unsigned int result;
    int ok = paintcanvas_ext_ss2_sscreate(this->engine, resId, flag, &ss);
    if (ok == 1) {
        unsigned int mat = 0xffffffff;
        this->MaterialCreate(matResId, mat);
        if (mat <= this->materials.count) {
            ::Node *node = ((PCSpriteSystemView2 *) ss)->node;
            node->field_0x30 =
                    (unsigned int) (uintptr_t) this->materials.data_[mat];
        }
        unsigned int i;
        for (i = 0; i < this->spriteSystems.count; i++) {
            AbyssEngine::SpriteSystem **slot = &this->spriteSystems.data_[i];
            if (*slot == nullptr) {
                *slot = static_cast<AbyssEngine::SpriteSystem *>(ss);
                ss = 0;
                out = i;
                return;
            }
        }
        if (ss == 0) {
            return;
        }
        ArrayAdd<AbyssEngine::SpriteSystem *>(
            static_cast<AbyssEngine::SpriteSystem *>(ss),
            *reinterpret_cast<Array<AbyssEngine::SpriteSystem *> *>(
                &this->spriteSystems));
        result = this->spriteSystems.count - 1;
    } else {
        result = 0xffffffff;
    }
    out = result;
}

bool PaintCanvas::GetScreenPosition(const AbyssEngine::AEMath::Matrix &srcMatrix,
                                    AbyssEngine::AEMath::Vector &outVec) {
    if (this->currentCamera >= this->cameras.count) {
        return false;
    }

    Vector src = {srcMatrix.m[3], srcMatrix.m[7], srcMatrix.m[11]};
    outVec = src;

    PCCameraView *cam = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
    float z = outVec[2];
    float denomX = cam->factor4c * z;
    if (denomX == 0.0f) {
        return false;
    }
    float denomY = z * cam->factor48;

    float px = outVec[0];
    int w0 = paintcanvas_ext_gsp_getwidth(this);
    int w1 = paintcanvas_ext_gsp_getwidth(this);
    double fw = (double) w0;
    double termY = ((double) outVec[1] * 0.5) / (double) denomY;
    double halfW = (double) (w1 >> 1);
    outVec[0] = (float) (halfW - (((double) px * 0.5) / (double) denomX) * fw);

    int h0 = paintcanvas_ext_gsp_getheight(this);
    double fh = (double) h0;
    int h1 = paintcanvas_ext_gsp_getheight(this);
    double halfH = (double) (h1 >> 1);
    outVec[1] = (float) (halfH + termY * fh);

    PCCameraView *cam2 = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
    if (outVec[2] > cam2->param1) {
        return false;
    }
    if (outVec[1] >= 0.0f) {
        int ww = paintcanvas_ext_gsp_getwidth(this);
        float fww = (float) ww;
        if (outVec[0] < fww) {
            int hh = paintcanvas_ext_gsp_getheight(this);
            float fhh = (float) hh;
            return outVec[1] < fhh;
        }
    }
    return false;
}


void PaintCanvas::SpriteSystemCreate(unsigned short resId,
                                     bool flag, unsigned int &out) {
    void *ss = 0;
    unsigned int result;
    int ok = paintcanvas_ext_sscreate(this->engine, resId, flag, &ss);
    if (ok == 1) {
        unsigned int i;
        for (i = 0; i < this->spriteSystems.count; i++) {
            AbyssEngine::SpriteSystem **slot = &this->spriteSystems.data_[i];
            if (*slot == nullptr) {
                *slot = static_cast<AbyssEngine::SpriteSystem *>(ss);
                ss = 0;
                out = i;
                return;
            }
        }
        if (ss == 0) {
            return;
        }
        ArrayAdd<AbyssEngine::SpriteSystem *>(
            static_cast<AbyssEngine::SpriteSystem *>(ss),
            *reinterpret_cast<Array<AbyssEngine::SpriteSystem *> *>(
                &this->spriteSystems));
        result = this->spriteSystems.count - 1;
    } else {
        result = 0xffffffff;
    }
    out = result;
}

void PaintCanvas::MaterialCreate(unsigned int &out, AbyssEngine::BlendMode mode,
                                 unsigned int textures, unsigned short p4) {
    (void) p4;
    Material *mat = new Material();
    mat->textures[0] = textures;
    mat->materialMode = mode;
    ArrayAdd(mat, this->materials);
    out = this->materials.count - 1;
}


int PaintCanvas::CameraIsSphereinViewFrustum(const AbyssEngine::AEMath::Vector &point, float radius) {
    if (radius == 0.0f || this->currentCamera >= this->cameras.count) {
        return 1;
    }

    Camera *cam = this->cameras.data_[this->currentCamera];
    if (this->initialized == 0) {
        return AbyssEngine::CameraIsSphereinViewFrustum(point, radius, nullptr, cam);
    }

    Matrix rotation(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f);
    AEMath::MatrixIdentity(rotation);
    double angle = this->engine->GetGravValue()[1] * 1.57079637;
    float a = (float) angle;
    int orient = this->gameOrientation;
    float rot = (orient == 1) ? a : -a;
    float s = AEMath::Sinf(rot);
    float c = AEMath::Cosf(rot);
    rotation.m[0] = c;
    rotation.m[5] = c;
    rotation.m[1] = -s;
    rotation.m[4] = s;

    return AbyssEngine::CameraIsSphereinViewFrustum(point, radius, &rotation, cam);
}

void PaintCanvas::MeshChangeShaderAnimValue(AbyssEngine::Mesh *mesh, float value, unsigned int mode) {
    if (!mesh) {
        return;
    }
    if (mode == 4) {
        mesh->shaderAnimValue1 = value;
    } else if (mode == 2) {
        mesh->shaderAnimValue0 = value;
    } else if (mode == 1) {
        memcpy(&mesh->materialId, &value, sizeof(value));
    }
    this->MeshChangeShaderAnimValue(mesh->animation, value, mode);
}


static int g_resume_curtex_7e828_storage = 0;
static int *const g_resume_curtex_7e828 = &g_resume_curtex_7e828_storage;

int paintcanvas_ext_rs_texfromfile(void *eng, char *path, void *cb, void *ud,
                                              unsigned int *out, bool b, float f);

void PaintCanvas::Resume() {
    unsigned int out = 0;
    for (unsigned int i = 0; i < this->cubeTextures.count; i++) {
        PCCubeTexView *res = (PCCubeTexView *) (this->cubeTextures.data_)[i];
        char *path = paintcanvas_ext_rs_getAEChar(res->pathField);
        float f = ((PCCubeTexView *) (this->cubeTextures.data_)[i])->scale;
        int ok = paintcanvas_ext_rs_texfromfile(this->engine, path, 0, 0,
                                                &out, false, f);
        if (ok == 1) {
            ((PCCubeTexView *) (this->cubeTextures.data_)[i])->glTexId = 0;
        }
        paintcanvas_ext_rs_deletearr(path);
    }
    int *cur = g_resume_curtex_7e828;
    if (*cur != 0) {
        paintcanvas_ext_rs_glActiveTexture(0x84c7);
        PCCubeTexView *res = (PCCubeTexView *) (this->cubeTextures.data_)[*cur];
        paintcanvas_ext_rs_glBindTexture(0x8513, (unsigned int) res->glTexId);
        paintcanvas_ext_rs_glActiveTexture(0x84c0);
    }
}


void PaintCanvas::TransformAddMeshId(unsigned int transformIndex, unsigned int meshIndex) {
    if (transformIndex >= this->transformCount ||
        meshIndex >= this->meshCount) {
        return;
    }
    Transform *tf = reinterpret_cast<Transform *>(this->transforms[transformIndex]);
    Mesh *mesh = reinterpret_cast<Mesh *>(this->meshes[meshIndex]);
    // Android 0x7b1ee-0x7b1f8 adds the mesh itself, not its vertex stream.
    ArrayAdd(mesh, tf->meshes);
    tf = reinterpret_cast<Transform *>(this->transforms[transformIndex]);
    mesh = reinterpret_cast<Mesh *>(this->meshes[meshIndex]);
    tf->bounds().Merge(*reinterpret_cast<AEMath::BSphere *>(&mesh->boundsCenterX));

    Transform *res = reinterpret_cast<Mesh *>(this->meshes[meshIndex])->animation;
    if (res != 0) {
        long long resLen = res->animationLength;
        long long tfLen = tf->animationLength;
        if (tfLen < resLen) {
            tf->SetAnimationLength(resLen);
            tf = reinterpret_cast<Transform *>(this->transforms[transformIndex]);
        }
        long long tfStart = tf->rangeStart;
        Transform *res2 = reinterpret_cast<Mesh *>(this->meshes[meshIndex])->animation;
        long long resStart = res2->rangeStart;
        if (tfStart == 0 || resStart < tfStart) {
            tf->rangeStart = resStart;
        }
        tf->SetAnimationState(static_cast<AnimationMode>(2), nullptr);
    }
    reinterpret_cast<Transform *>(this->transforms[transformIndex])->CollectAnimationData();
}

AbyssEngine::String PaintCanvas::GetReverseString(AbyssEngine::String in) {
    return this->GetReverseString(in, this->field_0x1c == 0);
}

void PaintCanvas::GetAccelValue() {
    return paintcanvas_ext_get_accel(this->engine);
}


static float g_rpm_fov_8d0dc_storage = 0.0f;
static float *const g_rpm_fov_8d0dc = &g_rpm_fov_8d0dc_storage;

static const unsigned int g_rpm_const_8d234 = 0;

static float g_rpm_near_8d196_storage = 0.0f;
static float *const g_rpm_near_8d196 = &g_rpm_near_8d196_storage;

static float g_rpm_far_8d1a4_storage = 0.0f;
static float *const g_rpm_far_8d1a4 = &g_rpm_far_8d1a4_storage;

static char g_rpm_flag_8d1b8_storage = 0;
static char *const g_rpm_flag_8d1b8 = &g_rpm_flag_8d1b8_storage;

void PaintCanvas::ResetPersMatrix() {
    int w = paintcanvas_ext_rpm_dispwidth(this->engine);
    int h = paintcanvas_ext_rpm_dispheight(this->engine);
    float fov = *g_rpm_fov_8d0dc;
    float s = paintcanvas_ext_rpm_sinf(fov * 0.5f);
    float c = paintcanvas_ext_rpm_cosf(fov * 0.5f);
    float fw = paintcanvas_ext_rpm_signedtofloat(w, 0);
    float fh = paintcanvas_ext_rpm_signedtofloat(h, 0);

    memset(&this->projMatrix3d.m[12], 0, 0x10);
    memset(&this->projMatrix3d.m[0], 0, 0x10);
    memset(&this->projMatrix3d.m[8], 0, 0x10);
    memset(&this->projMatrix3d.m[4], 0, 0x10);

    if (this->gameOrientation <= 3) {
        float aspect = fw / fh;
        float f = 1.0f / (s / c);
        switch (this->gameOrientation) {
            case 0:
                this->projMatrix3d.m[4] = -f;
                this->projMatrix3d.m[1] = f / aspect;
                break;
            case 1:
                this->projMatrix3d.m[4] = f;
                this->projMatrix3d.m[1] = -(f / aspect);
                break;
            case 2:
                this->projMatrix3d.m[5] = f;
                this->projMatrix3d.m[0] = f / aspect;
                break;
            case 3:
                this->projMatrix3d.m[5] = -f;
                this->projMatrix3d.m[0] = -(f / aspect);
                break;
        }
    }

    float n = *g_rpm_near_8d196;
    float fr = *g_rpm_far_8d1a4;
    this->projMatrix3d.m[11] = g_rpm_const_8d234;
    this->projMatrix3d.m[10] = (n + fr) / (fr - n);
    this->projMatrix3d.m[14] = ((n + n) * fr) / (fr - n);

    if (*g_rpm_flag_8d1b8 == 0) {
        paintcanvas_ext_rpm_glMatrixMode(0x1702);
        paintcanvas_ext_rpm_glLoadIdentity();
        float sc;
        *(unsigned int *) &sc = g_rpm_const_8d234;
        paintcanvas_ext_rpm_glScalef(1.0f, sc, 1.0f);
        paintcanvas_ext_rpm_glMatrixMode(0x1701);
        paintcanvas_ext_rpm_glLoadMatrixf(&this->projMatrix3d.m[0]);
        paintcanvas_ext_rpm_glMatrixMode(0x1700);
        paintcanvas_ext_rpm_glFinish();
    } else {
        paintcanvas_ext_rpm_loadproj(this->engine, &this->projMatrix3d.m[0]);
    }
}


int PaintCanvas::CameraIsPointinViewFrustum(const AbyssEngine::AEMath::Vector &point) {
    if (this->currentCamera >= this->cameras.count) {
        return 1;
    }

    if (this->initialized == 0) {
        return AbyssEngine::CameraIsPointinViewFrustum(
            point, nullptr, this->cameras.data_[this->currentCamera]);
    }

    Matrix rotation(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f);
    AEMath::MatrixIdentity(rotation);
    double angle = this->engine->GetGravValue()[1] * 1.57079637;
    float a = (float) angle;
    int orient = this->gameOrientation;
    float rot = (orient == 1) ? a : -a;
    float s = AEMath::Sinf(rot);
    float c = AEMath::Cosf(rot);
    rotation.m[0] = c;
    rotation.m[5] = c;
    rotation.m[1] = -s;
    rotation.m[4] = s;

    return AbyssEngine::CameraIsPointinViewFrustum(
        point, &rotation, this->cameras.data_[this->currentCamera]);
}

void PaintCanvas::SetTexture(unsigned int texture, unsigned int slot) {
    this->engine->SetTextures(texture, slot);
}

PaintCanvas::~PaintCanvas() {
    paintcanvas_ext_dtor_releaseall(this);

    for (unsigned int i = 0; i < this->resources.count; i++) {
        PCResourceView *res = (PCResourceView *) (this->resources.data_)[i];
        if (res != 0) {
            void *payload = res->payload;
            int type = res->type;
            switch (type) {
                case 1:
                case 3:
                case 6:
                    paintcanvas_ext_dtor_op_delete(payload);
                    break;
                case 2:
                    if (payload) {
                        paintcanvas_ext_dtor_op_delete(paintcanvas_ext_dtor_restex_dtor(payload));
                    }
                    break;
                case 4:
                    if (payload) {
                        paintcanvas_ext_dtor_op_delete(paintcanvas_ext_dtor_resmesh_dtor(payload));
                    }
                    break;
                case 5:
                    if (payload) {
                        paintcanvas_ext_dtor_op_delete(paintcanvas_ext_dtor_restransform_dtor(payload));
                    }
                    break;
                default:
                    break;
            }
            void *cell = (this->resources.data_)[i];
            if (cell != 0) {
                paintcanvas_ext_dtor_op_delete(cell);
            }
            (this->resources.data_)[i] = 0;

            {
                PCArrayHeader *a = (PCArrayHeader *) &this->glowMeshes_count;
                for (uint32_t j = 0; j < a->count; ++j) {
                    void *e = ((void **) a->data)[j];
                    if (e != nullptr)
                        ::operator delete(e);
                }
                a->count = 0;
            }
            {
                PCArrayHeader *a = (PCArrayHeader *) &this->glowMatA_count;
                ::operator delete(a->data);
                a->data = nullptr;
                a->count = 0;
                a->capacity = 0;
            }
            {
                PCArrayHeader *a = (PCArrayHeader *) &this->glowMatB_count;
                ::operator delete(a->data);
                a->data = nullptr;
                a->count = 0;
                a->capacity = 0;
            }
            {
                PCArrayHeader *a = (PCArrayHeader *) &this->glowUints_count;
                for (uint32_t j = 0; j < a->count; ++j) {
                    void *e = ((void **) a->data)[j];
                    if (e != nullptr)
                        ::operator delete(e);
                }
                a->count = 0;
            }
            {
                PCArrayHeader *a = (PCArrayHeader *) &this->glowMatC_count;
                ::operator delete(a->data);
                a->data = nullptr;
                a->count = 0;
                a->capacity = 0;
            }
        }
    }

    paintcanvas_ext_dtor_meshrelease(this->engine, &this->quad2dMesh);
    paintcanvas_ext_dtor_meshrelease(this->engine, &this->lineMesh);

    for (unsigned int i = 0; i < this->cubeTextures.count; i++) {
        PCCubeTexView *tex = (PCCubeTexView *) (this->cubeTextures.data_)[i];
        if (tex != 0) {
            paintcanvas_ext_dtor_str_dtor(tex->pathField);
            paintcanvas_ext_dtor_op_delete(tex);
        }
        (this->cubeTextures.data_)[i] = 0;
    }

    {
        PCArrayHeader *a = (PCArrayHeader *) &this->glowMatC_count;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->glowUints_count;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->glowMatB_count;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->glowMatA_count;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->glowMeshes_count;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->spriteSystems;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->materials;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->cameras;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->transformCount;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->images;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->fonts;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->resources;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->meshCount;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
    {
        PCArrayHeader *a = (PCArrayHeader *) &this->cubeTextures;
        ::operator delete(a->data);
        a->data = nullptr;
        a->count = 0;
        a->capacity = 0;
    }
}


void PaintCanvas::Image2DCreate(unsigned short resId, unsigned int &out) {
    PCResourceView *res = (PCResourceView *) this->FindResource(resId);
    if (res == 0) {
        return;
    }
    unsigned short *info = (unsigned short *) res->payload;
    PCResourceView *texres = (PCResourceView *) this->FindResource(*info);
    if (texres == 0) {
        return;
    }
    if (texres->handle == -1) {
        unsigned int texture = 0;
        this->TextureCreate(*info, texture, false);
    }
    unsigned int idx = (unsigned int) res->handle;
    if (idx == 0xffffffff) {
        Image2D *img = new Image2D;
        // ARM 0x79cde-0x79ce8 clears fields through +0x14, not tail padding.
        memset(img, 0, offsetof(Image2D, restoreFlag) + sizeof(img->restoreFlag));
        char *texpath = *(char **) texres->payload;
        int ok = AbyssEngine::ImageCreateRegionFromFile(this->engine, texpath, info[1], img);
        if (ok != 1) {
            return;
        }
        if (texres->handle != -1) {
            img->textureId = (uint32_t) texres->handle;
        }
        ArrayAdd<Image2D *>(img, this->images);
        idx = this->images.count - 1;
        res->handle = (int) idx;
    }
    out = idx;
}


bool PaintCanvas::GetScreenPosition(AbyssEngine::AEMath::Matrix &m,
                                    const AbyssEngine::AEMath::Vector &worldPos,
                                    AbyssEngine::AEMath::Vector &outVec) {
    Vector transformed = AEMath::MatrixTransformVector(m, worldPos);

    if (this->currentCamera >= this->cameras.count) {
        return false;
    }

    PCCameraView *cam = (PCCameraView *) ((void **) this->cameras.data_)[this->currentCamera];
    if (this->initialized == 0) {
        transformed = AEMath::MatrixInverseTransformVector(
            *reinterpret_cast<Matrix *>(cam->localMatrix), transformed);
    } else {
        Matrix rotation;
        AEMath::MatrixIdentity(rotation);
        double angle = this->engine->GetGravValue()[1] * 1.57079637;
        float a = (float) angle;
        int orient = this->gameOrientation;
        float rot = (orient == 1) ? a : -a;
        float s = AEMath::Sinf(rot);
        float c = AEMath::Cosf(rot);
        rotation.m[0] = c;
        rotation.m[5] = c;
        rotation.m[1] = -s;
        rotation.m[4] = s;

        Matrix cameraMatrix;
        memcpy(&cameraMatrix, cam->localMatrix, sizeof(cameraMatrix));
        cameraMatrix *= rotation;
        transformed = AEMath::MatrixInverseTransformVector(cameraMatrix, transformed);
    }
    outVec = transformed;

    float z = outVec[2];
    PCCameraView *cam2 = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
    if (z > cam2->param1) {
        return false;
    }
    float denomX = z * cam2->factor4c;
    if (denomX == 0.0f) {
        return false;
    }
    float denomY = z * cam2->factor48;

    float px = outVec[0];
    int w0 = GetWidth();
    int w1 = GetWidth();
    float py = outVec[1];
    double fw = (double) w0;
    double halfW = (double) (w1 >> 1);
    outVec[0] = (float) (halfW - (((double) px * 0.5) / (double) denomX) * fw);

    int h0 = GetHeight();
    double fh = (double) h0;
    int h1 = GetHeight();
    double halfH = (double) (h1 >> 1);
    outVec[1] = (float) (halfH + (((double) py * 0.5) / (double) denomY) * fh);

    float nx = outVec[0];
    if (nx >= 0.0f) {
        int ww = GetWidth();
        if (nx < (float) ww) {
            int hh = GetHeight();
            return outVec[1] < (float) hh;
        }
    }
    return false;
}

int PaintCanvas::ResourceLoaded(unsigned int index, AbyssEngine::ResourceType type) {
    unsigned int count;
    switch (type) {
        case 1: {
            PCResourceView *res = (PCResourceView *) (this->resources.data_)[index];
            if (res->type == 2) {
                int handle = res->handle;
                return handle + 1 != 0 ? 1 : 0;
            }
            return 0;
        }
        case 2:
            count = this->fonts.count;
            break;
        case 3:
            count = this->images.count;
            break;
        case 4:
            count = this->meshCount;
            break;
        case 5:
            count = this->transformCount;
            break;
        case 6:
            count = this->materials.count;
            break;
        default:
            return 0;
    }
    return index < count ? 1 : 0;
}

int PaintCanvas::TransformGetTriCount(unsigned int index) {
    if (index < this->transformCount) {
        Transform *transform = reinterpret_cast<Transform *>(this->transforms[index]);
        return paintcanvas_ext_transform_tricount(this, transform);
    }
    return 0;
}

float PaintCanvas::SpriteSystemSetPosition(unsigned int index, unsigned short sub,
                                           float x, float y, float z) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if (sub < (unsigned short) s->count) {
                float *p = (float *) (s->positions + sub * 12);
                p[0] = x;
                p[1] = y;
                p[2] = z;
                return z;
            }
        }
    }
    return x;
}

void PaintCanvas::DrawImage2D(unsigned int index, int x, int y) {
    char abuf[60];
    if (index < this->images.count) {
        PCImage2DView *img = (PCImage2DView *) (this->images.data_)[index];
        if (img->restoreFlag) {
            this->RestoreImage2D(reinterpret_cast<Image2D *>(img));
            img = (PCImage2DView *) (this->images.data_)[index];
        }
        this->SetTexture(img->textureId, 0xffffffffu);

        float fx = (float) x;
        float fy = (float) y;
        float *m = (float *) abuf;
        m[0] = 1.0f;
        m[1] = 0.0f;
        m[2] = 0.0f;
        m[3] = 0.0f;
        m[4] = 0.0f;
        m[5] = 1.0f;
        m[6] = 0.0f;
        m[7] = 0.0f;
        m[8] = 0.0f;
        m[9] = 0.0f;
        m[10] = 1.0f;
        m[11] = 0.0f;
        m[12] = 1.0f;
        m[13] = 1.0f;
        m[14] = 1.0f;
        m[3] = fx;
        m[7] = fy;

        this->SetWorldViewMatrix(*reinterpret_cast<const Matrix *>(abuf));
        glDisable(0xb44);
        AbyssEngine::MeshDraw(this->engine,
                              reinterpret_cast<Mesh *>(((PCImage2DView *) (this->images.data_)[index])->mesh));
        glEnable(0xb44);
    }
}

void paintcanvas_ctor_matrix(void *);

void paintcanvas_ext_meshcreate5(void *, unsigned short, unsigned short,
                                            signed char, void *);

PaintCanvas::PaintCanvas(AbyssEngine::Engine *engine) {
    PCArrayCtor(&this->meshCount);
    paintcanvas_ctor_matrix(&this->identityMatrix);
    PCArrayCtor(&this->transformCount);
    PCArrayCtor(&this->glowMeshes_count);
    PCArrayCtor(&this->glowMatA_count);
    PCArrayCtor(&this->glowMatB_count);
    PCArrayCtor(&this->glowUints_count);
    PCArrayCtor(&this->glowMatC_count);

    this->fogMode = 0;
    this->mask2dImage = 0;
    this->fogEnableFlag = 1;
    this->culledCount = 0;
    this->initialized = 0;
    this->engine = engine;
    this->currentCamera = 0xffffffff;

    paintcanvas_ext_meshcreate5(engine, 4, 2, 0x11, &this->lineMesh);

    int *p = (int *) ((PCMeshView *) this->lineMesh)->indexBuffer;
    p[0] = 0x20000;
    p[1] = 1;
    p[2] = *(int *) 0x87878;
    this->colorR = 1.0f;
    this->colorG = 1.0f;
    this->colorB = 1.0f;
    this->colorA = 1.0f;
    this->field_0x1c = 1;
    engine->shaderModeFlag = 1;

    paintcanvas_ext_meshcreate5(engine, 400, 200, 0x1b, &this->quad2dMesh);

    short *buf = (short *) ((PCMeshView *) this->quad2dMesh)->indexBuffer;
    int j = 0;
    for (int i = 0; i != 0x4b0; i += 0xc) {
        short *e = buf + i / 2;
        short a = (short) (j << 2);
        e[0] = a;
        e[1] = (short) ((j << 2) | 2);
        e[2] = (short) ((j << 2) | 1);
        e[3] = a;
        e[4] = (short) ((j << 2) | 3);
        e[5] = (short) ((j << 2) | 2);
        j++;
    }
    this->field_0x1f8 = 1;
    this->field_0x1cc = 0;
}

void PaintCanvas::TransformRemoveMesh(unsigned int transformIndex, unsigned short meshResId) {
    if (this->transformCount <= transformIndex) {
        return;
    }
    char *x = paintcanvas_ext_find_mesh(this, meshResId);
    if (x) {
        return paintcanvas_ext_remove_mesh(this, transformIndex, ((PCResourceView *) x)->handle);
    }
}

void PaintCanvas::ClearBuffer(unsigned int color) {
    glEnable(0xb71);
    glDepthMask(1);
    this->engine->ClearBuffer(color);
}

void PaintCanvas::ClearDepth() {
    glClear(0x100);
}

void PaintCanvas::DrawFrameBufferTexture(int, int, int, int, int, int) {
}

bool PaintCanvas::WarmUpTexture() {
    return true;
}

void PaintCanvas::TransformRemoveChild(unsigned int parent, unsigned int child) {
    unsigned int count = this->transformCount;
    if (parent != child && child < count && parent < count) {
        char **arr = this->transforms;
        char *p = arr[parent];
        char *c = arr[child];
        paintcanvas_ext_array_remove(c, p + 0x4c);
        char **arr2 = this->transforms;
        return paintcanvas_ext_transform_dirty(arr2[parent]);
    }
}

void PaintCanvas::TextureCreate(unsigned short resId, void (*callback)(AbyssEngine::Image *, void *),
                                void *userData, unsigned int &out, bool useCallbackLoader) {
    ((Engine *) this->engine)->boundTextures[0] = -1;
    ((Engine *) this->engine)->boundTextures[1] = -1;

    PCResourceView *res = (PCResourceView *) this->FindResource(resId);
    if (res != 0) {
        unsigned int idx = (unsigned int) res->handle;
        if (idx != 0xffffffff) {
            out = idx;
            return;
        }

        {
            PCTexInfoView *info = (PCTexInfoView *) res->payload;
            unsigned int texture = 0;
            float scale = info->value;
            char *path = info->path;
            int result;
            if (useCallbackLoader)
                result = AbyssEngine::TextureCreateFromFile((Engine *) this->engine, path,
                                                            callback, userData, &texture, true, scale);
            else
                result = AbyssEngine::TextureCreateFromFileIntern((Engine *) this->engine, path,
                                                                  callback, userData, &texture, scale,
                                                                  nullptr, true);
            if (result == 1) {
                idx = texture;
                res->handle = (int) texture;
                out = idx;
            }
        }
    }
}

void PaintCanvas::TextureCreate(unsigned short id, unsigned int &out, bool flag) {
    TextureCreate(id, nullptr, (void *) 0, out, flag);
}

void PaintCanvas::SwapBuffer() {
}

void PaintCanvas::StartDraw2FBO() {
    return paintcanvas_ext_start_fbo(this->engine);
}

void PaintCanvas::Suspend() {
    char texId[4];
    for (unsigned int i = 0; i < this->cubeTextures.count; i++) {
        int *p = (int *) (this->cubeTextures.data_)[i];
        *(int *) texId = *p;
        if (*p != -1) {
            paintcanvas_ext_gl_deletetextures(1, texId);
            p = (int *) (this->cubeTextures.data_)[i];
        }
        *p = -1;
    }
}

void PaintCanvas::DrawTransform(Transform *tf, const AbyssEngine::AEMath::Matrix &m2,
                                AbyssEngine::AEMath::Matrix &m3) {
    if (tf && tf->visible) {
        Matrix combined = m2 * tf->worldMatrix;
        if (tf->keyFrames.size() != 0) {
            combined *= tf->rotationMatrix;
        }
        for (unsigned int i = 0; i < tf->meshes.size(); i++) {
            this->DrawMesh(tf->meshes[i], combined, m3, tf->color, tf->localMatrix);
        }
        for (unsigned int i = 0; i < tf->children.size(); i++) {
            if (this->currentCamera < this->cameras.count &&
                tf->children[i]->InCameraVF(&combined,
                                            this->cameras.data_[this->currentCamera])) {
                this->DrawTransform(tf->children[i], combined, m3);
            } else {
                this->culledCount += 1;
            }
        }
    }
}

void PaintCanvas::DrawImage2D(unsigned int index, int x, int y,
                              int w, int h, unsigned char alignFlags, unsigned char anchorFlags,
                              unsigned char flipFlags) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    if (img->restoreFlag != 0) {
        this->RestoreImage2D(reinterpret_cast<Image2D *>(img));
        img = (PCImage2DView *) ((char **) this->images.data_)[index];
    }
    this->SetTexture(img->textureId, 0xffffffffu);

    int baseX = x;
    int hOff;
    if ((alignFlags & 7) == 4) {
        hOff = this->GetWidth() >> 1;
    } else if ((alignFlags & 7) == 2) {
        baseX = -x;
        hOff = this->GetWidth();
    } else {
        hOff = 0;
    }

    int spanW = w;
    if (alignFlags & 8) {
        spanW = this->GetWidth() - (w + x);
    }
    float fSpanW = static_cast<float>(spanW);
    PCRegionView *region = (PCRegionView *)
        ((PCMeshView *) ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh)->positions;
    float regW = region->width;

    int spanH = h;
    if (alignFlags & 0x80) {
        spanH = this->GetHeight() - (h + y);
        region = (PCRegionView *)
            ((PCMeshView *) ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh)->positions;
    }
    float fSpanH = static_cast<float>(spanH);
    float regH = region->height;

    int baseY = y;
    int vOff;
    if ((alignFlags & 0x70) == 0x40) {
        vOff = this->GetHeight() >> 1;
    } else if ((alignFlags & 0x70) == 0x20) {
        vOff = this->GetHeight();
        baseY = -y;
    } else {
        vOff = 0;
    }

    int anchorX;
    if ((anchorFlags & 7) == 4) {
        if ((alignFlags & 8) == 0) {
            anchorX = -(w >> 1);
        } else {
            anchorX = ((w + x) - this->GetWidth()) >> 1;
        }
    } else if ((anchorFlags & 7) == 2) {
        if ((alignFlags & 8) == 0) {
            anchorX = -w;
        } else {
            anchorX = (w + x) - this->GetWidth();
        }
    } else {
        anchorX = 0;
    }

    float scaleX = fSpanW / regW;
    float scaleY = fSpanH / regH;

    int anchorY;
    if ((anchorFlags & 0x70) == 0x40) {
        if ((alignFlags & 0x80) == 0) {
            anchorY = -(h >> 1);
        } else {
            anchorY = ((h + y) - this->GetHeight()) >> 1;
        }
    } else if ((anchorFlags & 0x70) == 0x20) {
        if ((alignFlags & 0x80) == 0) {
            anchorY = -h;
        } else {
            anchorY = (h + y) - this->GetHeight();
        }
    } else {
        anchorY = 0;
    }

    float m[16];
    memset(m, 0, sizeof(m));
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
    m[3] = static_cast<float>(baseX + hOff + anchorX);
    m[7] = static_cast<float>(anchorY + vOff + baseY);
    m[0] = scaleX;

    if (flipFlags & 1) {
        m[0] = -scaleX;
        m[3] = static_cast<float>(w) + m[3];
    }
    m[5] = scaleY;
    if (flipFlags & 2) {
        m[5] = -scaleY;
        m[7] = static_cast<float>(h) + m[7];
    }

    this->SetWorldViewMatrix(*reinterpret_cast<const Matrix *>(m));
    glDisable(0xb44);
    AbyssEngine::MeshDraw(this->engine,
                          reinterpret_cast<Mesh *>(((PCImage2DView *) ((char **) this->images.data_)[index])->mesh));
    glEnable(0xb44);
}

void PaintCanvas::CameraSetPerspective(unsigned int index, float fov, float aspect) {
    if (index < this->cameras.count) {
        float w = (float) paintcanvas_ext_get_w(this);
        float h = (float) paintcanvas_ext_get_h(this);
        void *cam = (this->cameras.data_)[index];
        paintcanvas_ext_cam_persp(fov, aspect, w, h, cam);
        if (this->currentCamera == index) {
            return paintcanvas_ext_cam_setcur(this, index);
        }
    }
}

void PaintCanvas::DrawRegion2D(unsigned int index, float /*unused*/, int rotSteps,
                               int pivotX, int pivotY, int transX, float scaleX, float scaleY) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    if (img->restoreFlag != 0) {
        paintcanvas_ext_dr2_restore(img->restoreFlag, img);
        img = (PCImage2DView *) ((char **) this->images.data_)[index];
    }
    paintcanvas_ext_dr2_settexture(this, img->textureId);

    float rot = paintcanvas_ext_dr2_signedtofloat(rotSteps, 0);
    float px = paintcanvas_ext_dr2_signedtofloat(pivotX, 0);
    float py = paintcanvas_ext_dr2_signedtofloat(pivotY, 0);
    float tx = paintcanvas_ext_dr2_signedtofloat(transX, 0);
    float negpx = paintcanvas_ext_dr2_signedtofloat(-pivotX, 0);
    float negpy = paintcanvas_ext_dr2_signedtofloat(-pivotY, 0);

    float transM[16];
    memset(transM, 0, sizeof(transM));
    transM[0] = 1.0f;
    transM[5] = 1.0f;
    transM[10] = 1.0f;
    transM[15] = 1.0f;
    transM[3] = py + rot * scaleX;
    transM[7] = tx + px * scaleY;

    float pivotM[16];
    memset(pivotM, 0, sizeof(pivotM));
    pivotM[0] = 1.0f;
    pivotM[5] = 1.0f;
    pivotM[10] = 1.0f;
    pivotM[15] = 1.0f;
    pivotM[3] = negpx * scaleX;
    pivotM[7] = negpy * scaleY;

    float s = paintcanvas_ext_dr2_sinf(negpx * scaleX);
    float c = paintcanvas_ext_dr2_cosf(negpx * scaleX);
    float rotM[16];
    memset(rotM, 0, sizeof(rotM));
    rotM[0] = c;
    rotM[5] = c;
    rotM[10] = 1.0f;
    rotM[15] = 1.0f;
    *(unsigned int *) &rotM[1] = *(unsigned int *) &s ^ 0x80000000;
    rotM[4] = s;

    float scaleBuf[16];
    paintcanvas_ext_dr2_setscaling(scaleBuf, scaleX, scaleY, 1.0f);

    float composed[16];
    paintcanvas_ext_dr2_mtx_mul(scaleBuf, rotM, transM);
    paintcanvas_ext_dr2_mtx_assign(composed, scaleBuf);
    float scratch[16];
    paintcanvas_ext_dr2_mtx_mul(scratch, pivotM, composed);
    paintcanvas_ext_dr2_mtx_mul(scaleBuf, scratch, pivotM);
    paintcanvas_ext_dr2_mtx_assign(composed, scaleBuf);

    paintcanvas_ext_dr2_setwvm(this, composed);
    paintcanvas_ext_dr2_gldisable(0xb44);
    paintcanvas_ext_dr2_meshdraw(this->engine,
                                 ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh);
    paintcanvas_ext_dr2_glenable(0xb44);
}

void PaintCanvas::MeshSetTriangle(unsigned int meshIndex, unsigned short tri,
                                  unsigned short v0, unsigned short v1, unsigned short v2) {
    if (meshIndex < this->meshCount) {
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[meshIndex];
        unsigned int t3 = tri * 3;
        if (t3 >= mesh->indexCount) {
            return;
        }
        unsigned short *buf = (unsigned short *) mesh->indexBuffer;
        buf[t3] = v0;
        buf[t3 + 1] = v1;
        buf[t3 + 2] = v2;
    }
}


static void zero16(void *p) {
    memset(p, 0, 0x10);
}

void PaintCanvas::SetGameOrientation(AbyssEngine::LandscapeMode orientation) {
    if (this->gameOrientation == orientation) {
        return;
    }
    this->gameOrientation = orientation;
    this->engine->SetScreenOrientation(orientation);

    this->projMatrix3d.m[1] = -this->projMatrix3d.m[1];
    this->projMatrix3d.m[4] = -this->projMatrix3d.m[4];

    float fw = static_cast<float>(this->engine->GetDisplayWidth());
    float fh = static_cast<float>(this->engine->GetDisplayHeight());

    if (orientation == 3) {
        zero16(&this->projOrthoMatrix.m[9]);
        zero16(&this->worldViewMatrix.m[8]);
        zero16(&this->worldViewMatrix.m[4]);
        zero16(&this->projOrthoMatrix.m[5]);
        zero16(&this->projOrthoMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[11]);
        zero16(&this->worldViewMatrix.m[0]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->worldViewMatrix.m[13] = 0;
        this->worldViewMatrix.m[14] = 0;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[0] = -1.0f;
        this->worldViewMatrix.m[1] = 0.0f;
        this->projOrthoMatrix.m[10] = -0.05f;
        this->projOrthoMatrix.m[12] = -1.0f;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = 0.0f;
        this->worldViewMatrix.m[5] = -1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fw;
        this->projOrthoMatrix.m[5] = -(2.0f / fh);
        this->worldViewMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[12] = static_cast<float>(this->engine->GetDisplayWidth());
        this->worldViewMatrix.m[13] = static_cast<float>(this->engine->GetDisplayHeight());
    } else if (orientation == 1) {
        zero16(&this->projOrthoMatrix.m[9]);
        zero16(&this->worldViewMatrix.m[8]);
        zero16(&this->worldViewMatrix.m[4]);
        zero16(&this->projOrthoMatrix.m[5]);
        zero16(&this->projOrthoMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[11]);
        zero16(&this->worldViewMatrix.m[0]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[1] = 1.0f;
        this->projOrthoMatrix.m[10] = -0.05f;
        this->projOrthoMatrix.m[12] = -1.0f;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = -1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fh;
        this->projOrthoMatrix.m[5] = -(2.0f / fw);
        this->worldViewMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[12] = static_cast<float>(this->engine->GetDisplayHeight());
    } else if (orientation != 0) {
        zero16(&this->projOrthoMatrix.m[9]);
        zero16(&this->worldViewMatrix.m[8]);
        zero16(&this->worldViewMatrix.m[4]);
        zero16(&this->projOrthoMatrix.m[5]);
        zero16(&this->projOrthoMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[11]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->worldViewMatrix.m[13] = 0;
        this->worldViewMatrix.m[14] = 0;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[0] = 1.0f;
        this->projOrthoMatrix.m[10] = -0.05f;
        this->projOrthoMatrix.m[12] = -1.0f;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[5] = 1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fw;
        this->projOrthoMatrix.m[5] = -(2.0f / fh);
        this->worldViewMatrix.m[15] = 1.0f;
    } else {
        zero16(&this->projOrthoMatrix.m[9]);
        zero16(&this->worldViewMatrix.m[8]);
        zero16(&this->worldViewMatrix.m[4]);
        zero16(&this->projOrthoMatrix.m[5]);
        zero16(&this->projOrthoMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[11]);
        zero16(&this->worldViewMatrix.m[0]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[1] = -1.0f;
        this->projOrthoMatrix.m[10] = -0.05f;
        this->projOrthoMatrix.m[12] = -1.0f;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = 1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fh;
        this->projOrthoMatrix.m[5] = -(2.0f / fw);
        this->worldViewMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[13] = static_cast<float>(this->engine->GetDisplayWidth());
    }

    if (this->currentCamera == -1) {
        return;
    }
    float *cam = ((float **) this->cameras.data_)[this->currentCamera];
    this->SetProjectionMatrix3d(cam[0], cam[1], cam[2]);
}

int paintcanvas_ext_mc_meshcreate(void *eng, unsigned short a, unsigned short b,
                                             signed char c, void **out);


void PaintCanvas::MeshCreate(unsigned short vertexCount, unsigned short triangleCount,
                             signed char meshType, unsigned short matResId, unsigned int &out) {
    int result = -1;
    unsigned int mat = 0xffffffff;
    void *mesh = 0;
    this->MaterialCreate(matResId, mat);
    int ok = paintcanvas_ext_mc_meshcreate(this->engine, vertexCount, triangleCount,
                                           meshType, &mesh);
    if (ok == 1) {
        if (mat <= this->materials.count) {
            void *m = this->materials.data_[mat];
            if (mesh) {
                ((AbyssEngine::Mesh *) mesh)->field_0x30 = m;
            }
        }
        ArrayAdd<AbyssEngine::Mesh *>((AbyssEngine::Mesh *) mesh,
                                      *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->meshCount));
        result = this->meshCount - 1;
    }
    out = (unsigned int) result;
}


static unsigned int mulColors(unsigned int a, unsigned int b) {
    unsigned int r = (((a >> 16) & 0xff) * ((b >> 16) & 0xff)) & 0xffffff00;
    unsigned int g = (((a >> 8) & 0xff) * ((b >> 8) & 0xff)) & 0xffffff00;
    unsigned int bl = ((a & 0xff) * (b & 0xff)) >> 8;
    unsigned int al = (((a >> 24) & 0xff) * ((b >> 24) & 0xff)) & 0xffffff00;
    return (bl) | (g << 8) | (r << 8) | (al << 16);
}

void PaintCanvas::DrawMesh(AbyssEngine::Mesh *mesh, AbyssEngine::AEMath::Matrix &worldMatrix,
                           AbyssEngine::AEMath::Matrix &viewMatrix, unsigned int color,
                           AbyssEngine::AEMath::Matrix &uvMatrix) {
    Matrix worldM = worldMatrix;
    Matrix uvM = uvMatrix;
    unsigned int meshColor = color;

    if (mesh->animation != 0) {
        Transform *resTf = mesh->animation;
        Matrix pivotNegative;
        Matrix pivotPositive;
        AEMath::MatrixSetTranslation(pivotNegative,
                                     -mesh->pivotX, -mesh->pivotZ, mesh->pivotY);
        AEMath::MatrixSetTranslation(pivotPositive,
                                     mesh->pivotX, mesh->pivotZ, -mesh->pivotY);
        Matrix position;
        AEMath::MatrixSetTranslation(position,
                                     AEMath::MatrixGetPosition(resTf->rotationMatrix));
        Matrix rotation = resTf->rotationMatrix;
        rotation.m[3] = 0.0f;
        rotation.m[7] = 0.0f;
        rotation.m[11] = 0.0f;
        worldM = (((worldMatrix * position) * pivotPositive) * rotation) * pivotNegative;
        uvM = resTf->localMatrix;
        meshColor = mulColors(resTf->color, color);
    }

    Vector center{mesh->boundsCenterX, mesh->boundsCenterY, mesh->boundsCenterZ};
    center = AEMath::MatrixTransformVector(worldM, center);
    Vector extent{mesh->boundsRadius, mesh->boundsRadius, mesh->boundsRadius};
    extent = AEMath::MatrixRotateVector(worldM, extent);

    float ax = extent.x < 0.0f ? -extent.x : extent.x;
    float ay = extent.y < 0.0f ? -extent.y : extent.y;
    float az = extent.z < 0.0f ? -extent.z : extent.z;
    float maxxy = ax > ay ? ax : ay;

    if (mesh->vertexCount != 0) {
        float r2 = maxxy > az ? maxxy : az;
        int vis = this->CameraIsSphereinViewFrustum(center, r2 * mesh->boundsRadiusSq);
        if (vis == 0) {
            this->culledCount += 1;
            return;
        }
        if (mesh->vertexCount != 0) {
            if (mesh->material == 0) {
                this->engine->SetColor(
                    this->colorR * static_cast<float>(meshColor >> 24) / 255.0f,
                    this->colorG * static_cast<float>((meshColor >> 16) & 0xff) / 255.0f,
                    this->colorB * static_cast<float>((meshColor >> 8) & 0xff) / 255.0f,
                    this->colorA * static_cast<float>(meshColor & 0xff) / 255.0f);

                Matrix worldView = viewMatrix * worldM;
                this->SetWorldViewMatrix(worldView);
                this->engine->SetModelMatrix(worldM);
                this->engine->SetUVMatrix(uvM);
                AbyssEngine::MeshDraw(this->engine, mesh);
                this->engine->ResetUVMatrix();
            } else {
                Material *res = static_cast<Material *>(mesh->material);
                ArrayAddCached(mesh, res->meshes);
                ArrayAddCached(worldM, res->arr_2c);
                ArrayAddCached(uvM, res->arr_38);
                ArrayAddCached(viewMatrix, res->arr_5c);
                ArrayAddCached(meshColor, res->arr_50);
            }
        }
    }

    if (mesh->animation != 0) {
        Transform *res = mesh->animation;
        for (int n = static_cast<int>(res->meshes.size()) - 1; n >= 0; --n) {
            this->DrawMesh(res->meshes[n], worldM, viewMatrix, color, uvM);
        }
        for (unsigned int i = 0; i < res->children.size(); i++) {
            if (this->currentCamera < this->cameras.count) {
                Transform *tf = res->children[i];
                if (tf->InCameraVF(&worldMatrix,
                                   this->cameras.data_[this->currentCamera])) {
                    this->DrawTransform(res->children[i], worldM, viewMatrix);
                }
            }
        }
    }
}

static char paintcanvas_g_b2d_flag_storage = 0;
static char *paintcanvas_g_b2d_flag = &paintcanvas_g_b2d_flag_storage;

void PaintCanvas::Begin2d() {
    *(unsigned char *) &((Engine *) this->engine)->field_0xfd = 1;
    paintcanvas_ext_gl_disable(0xb71);
    paintcanvas_ext_gl_depthmask(0);
    paintcanvas_ext_gl_enable(0xbe2);
    paintcanvas_ext_gl_blendfunc(0x302, 0x303);
    paintcanvas_ext_setcolor(this->engine, 1.0f, 1.0f, 1.0f, 1.0f);
    paintcanvas_ext_glenable2(this->engine, 0xde1, true);
    if (*paintcanvas_g_b2d_flag == 0) {
        paintcanvas_ext_gl_texenvi(0x2300, 0x2200, 0x2100);
        paintcanvas_ext_glMatrixMode(0x1702);
        paintcanvas_ext_gl_loadidentity();
        paintcanvas_ext_gl_scalef(1.0f, 1.0f, 1.0f);
        paintcanvas_ext_glMatrixMode(0x1701);
        paintcanvas_ext_gl_loadmatrix(&this->projOrthoMatrix.m[0]);
        if (this->gameOrientation != 2) {
            paintcanvas_ext_gl_multmatrix(&this->worldViewMatrix.m[0]);
        }
        paintcanvas_ext_glMatrixMode(0x1700);
        paintcanvas_ext_gl_loadidentity();
    } else {
        paintcanvas_ext_setortho(this->engine, &this->projOrthoMatrix.m[0],
                                 &this->worldViewMatrix.m[0], this->gameOrientation != 2);
    }
    this->field_0xc = 0;
}

void PaintCanvas::DrawString(unsigned int index, const unsigned short *str,
                             int x, int y, bool b) {
    if (index < this->fonts.count) {
        ImageFont *font = (ImageFont *) (this->fonts.data_)[index];
        this->SetTexture((unsigned int) (uintptr_t) font->field_0x8, 0xffffffff);
        ImageFontDrawString(font, str, x, y, this, this->engine, b);
    }
}


static const char g_dsc_fmt_882ee[] = "%x";

void PaintCanvas::DrawStringColor(unsigned int index, const AbyssEngine::String &text,
                                  int x, int y, bool b) {
    if (index < this->fonts.count) {
        this->SetTexture((unsigned int) (uintptr_t) this->fonts.data_[index]->field_0x8,
                         0xffffffff);
        unsigned int savedColor = this->GetColor();
        String source(text, false);
        Array<String *> *parts = source.SplitTags(String("c", false));

        if (parts != 0) {
            bool drawText = true;
            for (unsigned int i = 0; i < parts->count; ++i) {
                if (drawText) {
                    ImageFontDrawString(this->fonts.data_[index],
                                        (unsigned short *) *parts->data_[i],
                                        parts->data_[i]->size(), x, y, this, this->engine, b);
                    x += this->GetTextWidth(index, *parts->data_[i]);
                } else if (parts->data_[i]->size() != 0) {
                    unsigned int color = 0;
                    char *encodedColor = parts->data_[i]->GetAEChar();
                    std::sscanf(encodedColor, g_dsc_fmt_882ee, &color);
                    this->SetColor(color);
                } else {
                    this->SetColor(savedColor);
                }
                drawText = !drawText;
            }
            this->SetColor(savedColor);
            ArrayReleaseClasses<String *>(*parts);
            delete parts;
        }
    }
}


void PaintCanvas::SetMatForGlow(AbyssEngine::Material *glowSource) {
    for (unsigned int i = 0; i < glowSource->meshes.size_; i++) {
        ArrayAdd<AbyssEngine::Mesh *>(glowSource->meshes.data_[i],
                                      *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->glowMeshes_count));

        ArrayAdd(glowSource->arr_2c.data_[i],
                 *reinterpret_cast<Array<AEMath::Matrix> *>(&this->glowMatA_count));
        ArrayAdd(glowSource->arr_38.data_[i],
                 *reinterpret_cast<Array<AEMath::Matrix> *>(&this->glowMatB_count));

        PCArrayAdd<unsigned int>(glowSource->arr_50.data_[i], &this->glowUints_count);

        ArrayAdd(glowSource->arr_5c.data_[i],
                 *reinterpret_cast<Array<AEMath::Matrix> *>(&this->glowMatC_count));
    }
}

void PaintCanvas::EnableClip(int x, int y, int w, int h) {
    glEnable(0xc11);
    int sx, sy, sw, sh;
    switch (this->gameOrientation) {
        case 0:
            sx = y;
            sy = x;
            sw = h;
            sh = w;
            break;
        case 1: {
            int dispH = this->GetHeight();
            int dispW = this->GetWidth();
            sx = dispH - (y + h);
            sy = dispW - (x + w);
            sw = h;
            sh = w;
            break;
        }
        case 2: {
            int dispH = this->GetHeight();
            sx = x;
            sy = dispH - (y + h);
            sw = w;
            sh = h;
            break;
        }
        case 3: {
            int dispW = this->GetWidth();
            sx = dispW - (x + w);
            sy = y;
            sw = w;
            sh = h;
            break;
        }
        default:
            sx = x;
            sy = y;
            sw = w;
            sh = h;
            break;
    }
    glScissor(sx, sy, sw, sh);
}

void PaintCanvas::DrawImage2D(unsigned int index, int x, int y,
                              unsigned char regionAlignFlags, unsigned char placeFlags) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    if (img->restoreFlag != 0) {
        this->RestoreImage2D(reinterpret_cast<Image2D *>(img));
    }

    int hOff;
    if ((placeFlags & 7) == 4) {
        hOff = this->GetWidth() >> 1;
    } else if ((placeFlags & 7) == 2) {
        hOff = this->GetWidth();
        y = -y;
    } else {
        hOff = 0;
    }

    int vOff;
    unsigned int yShift = regionAlignFlags;
    if ((placeFlags & 0x70) == 0x40) {
        vOff = this->GetHeight() >> 1;
    } else if ((placeFlags & 0x70) == 0x20) {
        vOff = this->GetHeight();
        yShift = (unsigned int) (-(int) yShift);
    } else {
        vOff = 0;
    }

    int rx;
    PCRegionView *region = (PCRegionView *) img->regionPtr;
    if ((regionAlignFlags & 7) == 4) {
        double w = (double) static_cast<float>((int) region->width);
        rx = (int) (long long) (w * -0.5);
    } else if ((regionAlignFlags & 7) == 2) {
        rx = -(int) region->width;
    } else {
        rx = 0;
    }

    int ry;
    PCRegionView *region2 = (PCRegionView *)
        ((PCImage2DView *) ((char **) this->images.data_)[index])->regionPtr;
    if ((regionAlignFlags & 0x70) == 0x20) {
        ry = -(int) region2->height;
    } else if ((regionAlignFlags & 0x70) == 0x40) {
        double h = (double) static_cast<float>((int) region2->height);
        ry = (int) (long long) (h * -0.5);
    } else {
        ry = 0;
    }

    PCImage2DView *img2 = (PCImage2DView *) ((char **) this->images.data_)[index];
    this->SetTexture(img2->textureId, 0xffffffffu);

    float fx = static_cast<float>(hOff + y + rx);
    float fy = static_cast<float>(vOff + (int) yShift + ry);

    float m[16];
    memset(m, 0, sizeof(m));
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
    m[3] = fx;
    m[7] = fy;

    this->SetWorldViewMatrix(*reinterpret_cast<const Matrix *>(m));
    AbyssEngine::MeshDraw(this->engine,
                          reinterpret_cast<Mesh *>(((PCImage2DView *) ((char **) this->images.data_)[index])->mesh));
}

void PaintCanvas::MeshCreate(unsigned short a, unsigned short b,
                             signed char c, unsigned int &out) {
    void *mesh = 0;
    int result = paintcanvas_ext_mc_meshcreate(this->engine, a, b, c, &mesh);
    if (result == 1) {
        ArrayAdd<AbyssEngine::Mesh *>(static_cast<AbyssEngine::Mesh *>(mesh),
                                      *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->meshCount));
        result = (int) this->meshCount - 1;
    } else {
        result = -1;
    }
    out = result;
}

int PaintCanvas::FontGetSpacing(unsigned int index) {
    if (index < this->fonts.count) {
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_font_get_spacing(font);
    }
    return 0;
}

void *PaintCanvas::TransformGetTransform(unsigned int index) {
    if (index < this->transformCount) {
        return (this->transforms)[index];
    }
    return 0;
}


void PaintCanvas::MeshCreate(unsigned short resId, unsigned int &out,
                             bool forceClone) {
    PCResourceView *res = reinterpret_cast<PCResourceView *>(this->FindResource(resId));
    if (res != 0) {
        unsigned int idx = (unsigned int) res->handle;
        if (idx != 0xffffffff) {
            AbyssEngine::Mesh **meshes = reinterpret_cast<AbyssEngine::Mesh **>(this->meshes);
            AbyssEngine::Mesh *existing = meshes[idx];
            if (existing->animation != 0 || forceClone) {
                AbyssEngine::Mesh *clone = new AbyssEngine::Mesh(
                    meshes[(unsigned int) res->handle]);
                ArrayAdd<AbyssEngine::Mesh *>(clone,
                                              *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->meshCount));
                idx = this->meshCount - 1;
            }
            out = idx;
            return;
        }

        PCMeshInfoView *info = (PCMeshInfoView *) res->payload;
        unsigned int mat = 0xffffffff;
        this->MaterialCreate(info->matResId, mat);
        AbyssEngine::Material *matptr = 0;
        if (mat <= this->materials.count) {
            matptr = this->materials.data_[mat];
        }
        AbyssEngine::Mesh *mesh = 0;
        if (AbyssEngine::MeshCreateFromFile(this->engine, info->path, &mesh, matptr) == 1) {
            if (Engine::vboSupported) {
                mesh->vboEligible = 1;
                AbyssEngine::MeshConvertToVBO(mesh);
            }
            ArrayAdd<AbyssEngine::Mesh *>(mesh,
                                          *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->meshCount));
            idx = this->meshCount - 1;
            res->handle = (int) idx;
            out = idx;
        }
    }
}


void PaintCanvas::FogSetParameter(AbyssEngine::FogMode mode, float fogStart, float fogEnd,
                                  float fogDensity, unsigned int color) {
    if (!Engine::enableShader) {
        float modeF = paintcanvas_ext_fsp_unsignedtofloat((unsigned int) mode, 0);
        paintcanvas_ext_fsp_glFogf(0xb65, modeF);
        paintcanvas_ext_fsp_glFogf(0xb62, fogDensity);
        paintcanvas_ext_fsp_glFogf(0xb63, fogStart);
        paintcanvas_ext_fsp_glFogf(0xb64, fogEnd);

        float col[4];
        col[0] = float(color >> 24) / 255.0f;
        col[1] = float((color >> 16) & 0xff) / 255.0f;
        col[2] = float((color >> 8) & 0xff) / 255.0f;
        col[3] = float(color & 0xff) / 255.0f;
        paintcanvas_ext_fsp_glFogfv(0xb66, col);
    } else {
        Engine *eng = (Engine *) this->engine;
        eng->fogMinDist = fogStart;
        eng->fogMaxDist = fogEnd;
        Vector col = {
            float(double(color >> 24) / 255.0),
            float(double((color >> 16) & 0xff) / 255.0),
            float(double((color >> 8) & 0xff) / 255.0),
        };
        eng->fogColor = col;
    }
}


void PaintCanvas::DrawRegion2D(unsigned int index, int srcX, int srcY,
                               int destW, int destH, float angle, int pivotX, int pivotY, int x, int y) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    this->SetTexture(img->textureId, 0xffffffff);
    PCImage2DView *mesh = (PCImage2DView *) ((char **) this->images.data_)[index];
    PCMeshView *meshObj = (PCMeshView *) mesh->mesh;

    mesh->restoreFlag = 1;

    float fw = (float) destW;
    float fh = (float) destH;
    float *verts = (float *) meshObj->positions;
    verts[0] = 0.0f;
    verts[1] = 0.0f;
    verts[2] = 0.0f;
    verts[3] = fw;
    verts[4] = 0.0f;
    verts[5] = 0.0f;
    verts[6] = fw;
    verts[7] = fh;
    verts[8] = 0.0f;
    verts[9] = 0.0f;
    verts[10] = fh;

    float u0 = (float) mesh->regionB;
    float w = (float) mesh->regionA;
    float h = (float) mesh->u0;
    float v0 = (float) mesh->v0;
    float a2 = (float) srcX;
    float a3 = (float) srcY;
    float a2s = (float) (short) srcX;

    float *uv = (float *) meshObj->uvs;
    float invW = 1.0f / w;
    float invH = 1.0f / u0;
    float ulo = (a2 + h) * invW;
    float vlo = (a3 + v0) * invH;
    float uhi = (fw + h + a2s) * invW;
    float vhi = (fh + v0 + a3) * invH;
    uv[0] = ulo;
    uv[1] = vlo;
    uv[2] = uhi;
    uv[3] = vlo;
    uv[4] = uhi;
    uv[5] = vhi;
    uv[6] = ulo;
    uv[7] = vhi;

    unsigned int *blend = (unsigned int *) meshObj->indexBuffer;
    blend[0] = 0x20000;
    blend[1] = 1;
    blend[2] = 0x20003;

    const float px = (float) pivotX;
    const float py = (float) pivotY;
    const float tx = (float) (x + pivotX);
    const float ty = (float) (y + pivotY);
    const float s = AEMath::Sinf(angle);
    const float c = AEMath::Cosf(angle);
    // Android 0x78538 composes T(x+pivot) * R(angle) * T(-pivot).
    const Matrix composed(
        c, -s, 0.0f, tx - c * px + s * py,
        s, c, 0.0f, ty - s * px - c * py,
        0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f);

    this->SetWorldViewMatrix(composed);
    glDisable(0xb44);
    MeshDraw(this->engine, (Mesh *) mesh->mesh);
    glEnable(0xb44);
}

void PaintCanvas::RestoreImage2D(AbyssEngine::Image2D *image) {
    PCImage2DView *img = (PCImage2DView *) image;
    PCMeshView *m = (PCMeshView *) img->mesh;
    img->restoreFlag = 0;
    float *s = (float *) m->positions;

    float w = (float) (unsigned int) img->width;
    float h = (float) (unsigned int) img->height;

    s[0] = 0.0f;
    s[1] = 0.0f;
    s[2] = 0.0f;
    s[3] = w;
    s[4] = 0.0f;
    s[5] = 0.0f;
    s[6] = w;
    s[7] = h;
    s[8] = 0.0f;
    s[9] = 0.0f;
    s[10] = h;
    s[11] = 0.0f;

    float invW = (float) (1.0 / (double) (unsigned int) img->regionB);
    float invH = (float) (1.0 / (double) (unsigned int) img->regionA);
    float u0 = (float) (unsigned int) img->u0;
    float v0 = (float) (unsigned int) img->v0;

    float *buf = (float *) m->uvs;
    float a = u0 * invH;
    float b = v0 * invW;
    float c = (h + v0) * invW;
    float d = (w + u0) * invH;
    buf[0] = a;
    buf[1] = b;
    buf[2] = d;
    buf[3] = b;
    buf[4] = d;
    buf[5] = c;
    buf[6] = a;
    buf[7] = c;

    int *ipd = (int *) m->indexBuffer;
    ipd[0] = 0x20000;
    ipd[1] = 1;
    ipd[2] = 0x20003;
}

void PaintCanvas::SpriteSystemGetPosition(unsigned int index, unsigned short sub,
                                          const Matrix &m, Vector &out) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned int) (unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            const float *mm = (const float *) &m;
            float *p = (float *) (s->positions + sub * 12);
            float p0 = p[0];
            float p1 = p[1];
            float p2 = p[2];
            float half = (float) (s->count >> 1);
            float x = mm[0] * p0 + mm[1] * p1 + mm[2] * p2 + mm[3];
            float y = mm[4] * p0 + mm[5] * p1 + mm[6] * p2 + mm[7];
            float z = mm[8] * p0 + mm[9] * p1 + mm[10] * p2 + mm[11];
            out.x = x - half;
            out.y = y + half;
            out.z = z;
        }
    }
}

void PaintCanvas::MeshSet2DMask(unsigned int index, int, int) {
    unsigned int i = index;
    if (this->images.count <= index) {
        return;
    }
    char **arr = (char **) this->images.data_;
    char *img = arr[i];
    if (((PCImage2DView *) img)->restoreFlag != 0) {
        RestoreImage2D(reinterpret_cast<AbyssEngine::Image2D *>(img));
        arr = (char **) this->images.data_;
    }
    this->mask2dImage = arr[i];
}


static int g_rar_curtex_87c98_storage = 0;
static int *const g_rar_curtex_87c98 = &g_rar_curtex_87c98_storage;

static int g_rar_texcount_87cce_storage = 0;
static int *const g_rar_texcount_87cce = &g_rar_texcount_87cce_storage;

static int g_rar_tricount_87d96_storage = 0;
static int *const g_rar_tricount_87d96 = &g_rar_tricount_87d96_storage;

void PaintCanvas::ReleaseAllResources() {
    *g_rar_curtex_87c98 = 0;

    for (int i = 0; i < this->resources.count; i++) {
        PCResourceView *res = (PCResourceView *) ((char **) this->resources.data_)[i];
        res->handle = -1;
    }

    for (unsigned int i = 0; i < this->cubeTextures.count; i++) {
        PCCubeTexView *tex = (PCCubeTexView *) ((char **) this->cubeTextures.data_)[i];
        if (tex->glTexId != -1) {
            unsigned int id = (unsigned int) tex->glTexId;
            paintcanvas_ext_rar_gldeltex(1, &id);
            *g_rar_texcount_87cce = *g_rar_texcount_87cce - 1;
            Engine *eng = (Engine *) this->engine;
            PCCubeTexView *texEntry = (PCCubeTexView *) ((char **) this->cubeTextures.data_)[i];
            eng->field_0x70 = eng->field_0x70 - texEntry->memSize;
            tex = (PCCubeTexView *) ((char **) this->cubeTextures.data_)[i];
        }
        if (tex != 0) {
            paintcanvas_ext_rar_str_dtor(tex->pathField);
            paintcanvas_ext_rar_op_delete(tex);
        }
        ((int **) this->cubeTextures.data_)[i] = 0;
    }
    this->cubeTextures.count = 0;

    for (unsigned int i = 0; i < this->fonts.count; i++) {
        if (((void **) this->fonts.data_)[i] != 0) {
            paintcanvas_ext_rar_fontrelease(this->engine,
                                            &((void **) this->fonts.data_)[i]);
        }
    }
    PCArrayRemoveAll(&this->fonts);

    for (unsigned int i = 0; i < this->images.count; i++) {
        if (((void **) this->images.data_)[i] != 0) {
            paintcanvas_ext_rar_img2drelease(this->engine,
                                             &((void **) this->images.data_)[i]);
        }
    }
    PCArrayRemoveAll(&this->images);

    for (unsigned int i = 0; i < this->meshCount; i++) {
        PCMeshView *mesh = (PCMeshView *) ((char **) this->meshes)[i];
        if (mesh != 0) {
            *g_rar_tricount_87d96 = *g_rar_tricount_87d96 - mesh->triCountContribution;
            paintcanvas_ext_rar_meshrelease(this->engine,
                                            &((void **) this->meshes)[i]);
        }
    }
    PCArrayRemoveAll(&this->meshCount);

    for (unsigned int i = 0; i < this->transformCount; i++) {
        void *tf = ((void **) this->transforms)[i];
        if (tf != 0) {
            paintcanvas_ext_rar_op_delete(paintcanvas_ext_rar_transform_dtor(tf));
            ((void **) this->transforms)[i] = 0;
        }
    }
    PCArrayRemoveAll(&this->transformCount);

    for (unsigned int i = 0; i < this->cameras.count; i++) {
        void *cam = ((void **) this->cameras.data_)[i];
        if (cam != 0) {
            paintcanvas_ext_rar_op_delete(cam);
            ((void **) this->cameras.data_)[i] = 0;
        }
    }
    PCArrayRemoveAll(&this->cameras);
    this->currentCamera = -1;

    for (unsigned int i = 0; i < this->materials.count; i++) {
        void *mat = ((void **) this->materials.data_)[i];
        if (mat != 0) {
            paintcanvas_ext_rar_op_delete(paintcanvas_ext_rar_material_dtor(mat));
            ((void **) this->materials.data_)[i] = 0;
        }
    }
    PCArrayRemoveAll(&this->materials);

    for (unsigned int i = 0; i < this->spriteSystems.count; i++) {
        if (((void **) this->spriteSystems.data_)[i] != 0) {
            paintcanvas_ext_rar_ssrelease(this->engine,
                                          &((void **) this->spriteSystems.data_)[i]);
        }
    }
    PCArrayRemoveAll(&this->spriteSystems);
    this->field_0x1cc = 0;
}


static const unsigned int g_tg2d_defval_7b590 = 0;

AbyssEngine::PickedTextureRegion
PaintCanvas::TransformGet2DPickedTextureRegion(unsigned int transformIndex, int x, int y, int z,
                                               int w) {
    PickedTextureRegion result;
    char matbuf[60];
    char vecbuf[64];

    if (transformIndex < this->transformCount) {
        char *tf = ((char **) this->transforms)[transformIndex];
        paintcanvas_ext_tg2d_memcpy(vecbuf, tf, 0x3c);
        float fy = paintcanvas_ext_tg2d_signedtofloat(y, 0);
        float fz = paintcanvas_ext_tg2d_signedtofloat(z, 0);
        float fx = paintcanvas_ext_tg2d_signedtofloat(w, 0);
        float vin[3];
        vin[0] = fy;
        vin[1] = fz;
        vin[2] = fx;
        paintcanvas_ext_tg2d_invtransformvec(matbuf, vecbuf);
        paintcanvas_ext_tg2d_vec_assign(vin, matbuf);
        result = this->TransformGet2DPickedTextureRegion(
            reinterpret_cast<Transform *>(tf), x, (int) vin[0], (int) vin[1], 0);
    } else {
        result.u = *(const float *) &g_tg2d_defval_7b590;
        result.v = *(const float *) &g_tg2d_defval_7b590;
        paintcanvas_ext_tg2d_errmsg(&result);
    }
    return result;
}

void PaintCanvas::CheckString(unsigned int index, const AbyssEngine::String &str) {
    if (index < this->fonts.count) {
        ImageFont *font = this->fonts.data_[index];
        const unsigned short *text = str;
        return ImageFontCheckString(font, text, str.size());
    }
}

int paintcanvas_ext_rt_texfromfile(void *eng, char *path, void *cb, void *ud,
                                              unsigned int *out, bool b, float f);

void PaintCanvas::ReloadTextures() {
    unsigned int out = 0;
    for (unsigned int i = 0; i < this->cubeTextures.count; i++) {
        PCCubeTexView *res = (PCCubeTexView *) (this->cubeTextures.data_)[i];
        if (res->glTexId == -1) {
            char *path = paintcanvas_ext_rt_getAEChar(res->pathField);
            float f = ((PCCubeTexView *) (this->cubeTextures.data_)[i])->scale;
            int ok = paintcanvas_ext_rt_texfromfile(this->engine, path, 0, 0,
                                                    &out, false, f);
            if (ok == 1) {
                ((PCCubeTexView *) (this->cubeTextures.data_)[i])->glTexId = 0;
            }
            paintcanvas_ext_rt_deletearr(path);
        }
    }
}


void PaintCanvas::MaterialCreate(unsigned short resId, unsigned int &out) {
    PCResourceView *res = (PCResourceView *) this->FindResource(resId);
    if (res == 0) {
        return;
    }
    unsigned int idx = (unsigned int) res->handle;
    if (idx == 0xffffffff) {
        PCMaterialInfoView *info = (PCMaterialInfoView *) res->payload;
        Material *mat = new Material();
        for (unsigned int i = 0; i < 8; i++) {
            unsigned short tid = info->textureIds[i];
            if (tid != 0xffff) {
                PCResourceView *tres = (PCResourceView *) this->FindResource(tid);
                if (tres == 0) {
                    break;
                }
                int t = tres->handle;
                if (t == -1) {
                    unsigned int textureOut = 0;
                    this->TextureCreate(tid, textureOut, true);
                    t = tres->handle;
                }
                mat->textures[i] = t;
            }
        }
        mat->materialMode = info->flags0;
        mat->addData = (void *) (uintptr_t) info->flags1;
        mat->addDataSize = info->flags2;
        mat->ambientColor = *(Vector *) info->vec;
        ArrayAdd(mat, this->materials);
        idx = this->materials.count - 1;
        res->handle = (int) idx;
    }
    out = idx;
}


static const unsigned int g_tg2di_neg1_8b588 = 0;

AbyssEngine::PickedTextureRegion
PaintCanvas::TransformGet2DPickedTextureRegion(Transform *transform, int x, int y, int z, int w) {
    PickedTextureRegion result;
    PCTransformView *tf = (PCTransformView *) transform;
    int shift = w;
    float vy = paintcanvas_ext_tg2di_signedtofloat((y >> shift) >> shift, 0);
    float vx = paintcanvas_ext_tg2di_signedtofloat((z >> shift) >> shift, 0);

    unsigned int i = 0;
    bool found = false;
    while (i < tf->meshCount) {
        void *mesh = tf->meshData[i];
        paintcanvas_ext_tg2di_meshintersect(&result, vx, vy, mesh);
        i++;
        if (result.u != -1.0f && result.v != -1.0f) {
            found = true;
            break;
        }
    }
    if (found) {
        return result;
    }

    float fy = paintcanvas_ext_tg2di_signedtofloat(z, 0);
    float fx = paintcanvas_ext_tg2di_signedtofloat(y, 0);
    float fz = paintcanvas_ext_tg2di_signedtofloat(w, 0);

    i = 0;
    while (i < tf->childCount) {
        char vecbuf[64];
        char matbuf[60];
        char *child = (char *) tf->childData[i];
        paintcanvas_ext_tg2di_memcpy(vecbuf, child, 0x3c);
        float vin[3];
        vin[0] = fx;
        vin[1] = fy;
        vin[2] = fz;
        paintcanvas_ext_tg2di_invtransformvec(matbuf, vecbuf);
        paintcanvas_ext_tg2di_vec_assign(vin, matbuf);
        result = this->TransformGet2DPickedTextureRegion(
            reinterpret_cast<Transform *>(child), x, (int) vin[0], (int) vin[1], 0);
        i++;
        if (result.u != -1.0f && result.v != -1.0f) {
            return result;
        }
    }
    result.u = *(const float *) &g_tg2di_neg1_8b588;
    result.v = *(const float *) &g_tg2di_neg1_8b588;
    return result;
}


void PaintCanvas::DrawTransform(unsigned int index, const AbyssEngine::AEMath::Matrix *viewMatrix) {
    if (index >= this->transformCount) {
        return;
    }
    Transform *tf = reinterpret_cast<Transform *>(this->transforms[index]);
    if (!tf->visible) {
        return;
    }

    Matrix world(1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                 1.0f, 1.0f, 1.0f);

    if (this->currentCamera < this->cameras.count) {
        if (this->initialized == 0) {
            int visible = reinterpret_cast<Transform *>(this->transforms[index])->InCameraVF(
                nullptr, this->cameras.data_[this->currentCamera]);
            if (visible == 0) {
                this->culledCount += 1;
                return;
            }
            const Matrix &source = viewMatrix ? *viewMatrix
                                              : this->cameras.data_[this->currentCamera]->projection;
            world = AEMath::MatrixGetInverse(source);
        } else {
            Matrix rotation(1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            1.0f, 1.0f, 1.0f);
            AEMath::MatrixIdentity(rotation);
            double angle = this->engine->GetGravValue()[1] * 1.57079637;
            float a = (float) angle;
            int orient = this->gameOrientation;
            float ang = (orient == 1) ? a : -a;
            float s = AEMath::Sinf(ang);
            float c = AEMath::Cosf(ang);
            rotation.m[0] = c;
            rotation.m[5] = c;
            rotation.m[1] = -s;
            rotation.m[4] = s;

            int visible = reinterpret_cast<Transform *>(this->transforms[index])->InCameraVF(
                &rotation, this->cameras.data_[this->currentCamera]);
            if (visible == 0) {
                this->culledCount += 1;
                return;
            }

            Matrix view(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        1.0f, 1.0f, 1.0f);
            view = viewMatrix ? *viewMatrix
                              : this->cameras.data_[this->currentCamera]->projection;
            view *= rotation;
            world = AEMath::MatrixGetInverse(view);
        }
        Camera *cam = this->cameras.data_[this->currentCamera];
        this->engine->SetEyePosition(cam->projection.m[3],
                                     cam->projection.m[7],
                                     cam->projection.m[11]);
    }

    Matrix identity(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f);
    this->DrawTransform(reinterpret_cast<Transform *>(this->transforms[index]),
                        identity, world);
}

void PaintCanvas::FontSetSpacing(unsigned int index, short spacing) {
    if (index < this->fonts.count) {
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_font_set_spacing(font, spacing);
    }
}

int PaintCanvas::GetWidth() {
    return this->engine->GetDisplayWidth();
}

void PaintCanvas::CameraCreate(unsigned int &out) {
    Camera *cam = static_cast<Camera *>(::operator new(sizeof(Camera)));
    int w = pc_GetWidth(this);
    int h = pc_GetHeight(this);
    new (cam) Camera(8000.0f, 256.0f, 16384.0f, (float) w, (float) h);
    ArrayAdd(cam, this->cameras);
    out = this->cameras.count - 1;
}


void PaintCanvas::DrawSpriteSystem(unsigned int index, AbyssEngine::AEMath::Matrix mat) {
    if (index >= this->spriteSystems.count) {
        return;
    }
    SpriteSystem *ss = this->spriteSystems.data_[index];
    if (ss == 0) {
        return;
    }

    if (this->initialized == 0) {
        mat = AEMath::MatrixGetInverse(mat);
    } else {
        Matrix rotation(1.0f, 0.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        1.0f, 1.0f, 1.0f);
        AEMath::MatrixIdentity(rotation);
        double angle = this->engine->GetGravValue()[1] * 1.57079637;
        float a = (float) angle;
        int orient = this->gameOrientation;
        float rot = (orient == 1) ? a : -a;
        float s = AEMath::Sinf(rot);
        float c = AEMath::Cosf(rot);
        rotation.m[0] = c;
        rotation.m[5] = c;
        rotation.m[1] = -s;
        rotation.m[4] = s;

        mat *= rotation;
        mat = AEMath::MatrixGetInverse(mat);
    }

    Matrix view(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 1.0f);
    SpriteSystemDraw(this->engine, view, mat, ss);
}

void PaintCanvas::DrawString(unsigned int index, const AbyssEngine::String &str,
                             int x, int y, bool b) {
    if (index < this->fonts.count) {
        ImageFont *font = (ImageFont *) (this->fonts.data_)[index];
        this->SetTexture((unsigned int) (uintptr_t) font->field_0x8, 0xffffffff);
        ImageFontDrawString(font, str.GetAEWChar(), str.size(), x, y, this, this->engine, b);
    }
}

int PaintCanvas::TransformGetTriCount(Transform *transform) {
    if (!transform) {
        return 0;
    }
    int total = 0;
    unsigned int n1 = transform->meshes.size();
    for (unsigned int i = 0; i != n1; ++i) {
        total += this->MeshGetTriCount(transform->meshes[i]);
    }
    unsigned int n2 = transform->children.size();
    for (unsigned int i = 0; i != n2; ++i) {
        total += this->TransformGetTriCount(transform->children[i]);
    }
    return total;
}

void PaintCanvas::MeshChangeShaderAnimValue(Transform *transform, float value, unsigned int mode) {
    if (transform) {
        for (unsigned int i = 0; i < transform->meshes.size(); ++i) {
            this->MeshChangeShaderAnimValue(transform->meshes[i], value, mode);
        }
        for (unsigned int i = 0; i < transform->children.size(); ++i) {
            this->MeshChangeShaderAnimValue(transform->children[i], value, mode);
        }
    }
}

void PaintCanvas::ReleaseSpriteSystemResource(unsigned int index) {
    if (index < this->spriteSystems.count) {
        AbyssEngine::SpriteSystemRelease(this->engine, &this->spriteSystems.data_[index]);
    }
}


void PaintCanvas::DrawSpriteSystem(unsigned int index,
                                   AbyssEngine::AEMath::Matrix matA, AbyssEngine::AEMath::Matrix matB) {
    if (index >= this->spriteSystems.count) {
        return;
    }
    SpriteSystem *sys = this->spriteSystems.data_[index];
    if (sys == 0) {
        return;
    }

    if (this->initialized == 0) {
        matB = AEMath::MatrixGetInverse(matB);
    } else {
        Matrix rotation;
        AEMath::MatrixIdentity(rotation);
        int ia = (int) (this->engine->GetGravValue()[1] * -1.57079637);
        if (this->gameOrientation == 1) {
            ia = -ia;
        }
        float a = (float) ia;
        float s = AEMath::Sinf(a);
        float c = AEMath::Cosf(a);
        rotation.m[0] = c;
        rotation.m[5] = c;
        rotation.m[1] = -s;
        rotation.m[4] = s;

        matB *= rotation;
        matB = AEMath::MatrixGetInverse(matB);
        matA *= rotation;
    }

    SpriteSystemDraw(this->engine, matA, matB, sys);
}
