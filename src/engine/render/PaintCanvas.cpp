#include "engine/render/PaintCanvas.h"
#include <GLES/gl.h>
#include "engine/core/AbyssEngine.h"
#include "engine/render/Material.h"
#include "engine/core/Array.h"
#include "engine/core/Node.h"
#include "engine/math/Transform.h"
#include <cstdint>
#include <cstddef>

namespace AbyssEngine {
    class SpriteSystem;
    class ImageFont;
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
        void *regionPtr;    // +0x04 (same word reinterpreted as a region record)
    };
    uint16_t regionA;    // +0x08
    uint16_t regionB;    // +0x0a
    uint16_t u0;         // +0x0c
    uint16_t v0;         // +0x0e
    uint16_t width;      // +0x10
    uint16_t height;     // +0x12
    uint8_t restoreFlag; // +0x14
};

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
//   +0x04 uint32   loader parameter
struct PCTexInfoView {
    char *path;        // +0x00
    uint32_t param;    // +0x04
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
// Font handle (entries of PaintCanvas::fonts) for raw-offset sites.
//   +0x00 uint16   ascent/height key
//   +0x08 void*    texture/atlas pointer (also read as uint texture id)
// ---------------------------------------------------------------------------
struct PCFontView {
    uint16_t key;      // +0x00
    uint8_t pad02[0x08 - 0x02];
    void *atlas;       // +0x08
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

// ---------------------------------------------------------------------------
// Split-tags array record returned by paintcanvas_ext_dsc_splittags.
//   +0x00 uint32   count
//   +0x04 char**   data
// ---------------------------------------------------------------------------
struct PCSplitArrayView {
    uint32_t count;    // +0x00
    char **data;       // +0x04
};

// ---------------------------------------------------------------------------
// String part record (entries of PCSplitArrayView::data).
//   +0x08 int       length
// ---------------------------------------------------------------------------
struct PCStrPartView {
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
static_assert(offsetof(PCFontView, atlas) == 0x08, "font atlas");
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

int paintcanvas_ext_dtl_textwidth(void *, unsigned int, void *);

void paintcanvas_ext_dtl_drawstring(void *, unsigned int, void *, int, int, bool);

int paintcanvas_ext_dtl_textheight(void *, unsigned int);

void paintcanvas_ext_fr_setwvm(void *self, void *m);

void paintcanvas_ext_fr_glenable(void *eng, unsigned int cap, bool on);

void paintcanvas_ext_fr_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_sprite_rgba(unsigned int, float, float, float, float, void *);

void *paintcanvas_ext_alloc(unsigned int);

void *paintcanvas_ext_transform_ctor(void *);

void paintcanvas_ext_add_child(void *, void *);

void paintcanvas_ext_dr_setwvm(void *self, void *m);

void paintcanvas_ext_dr_glLineWidth(float w);

void paintcanvas_ext_dr_glcap(void *eng, unsigned int cap, int on);

void paintcanvas_ext_dr_glVertexPointer(int a, int b, int c, void *p);

void paintcanvas_ext_dr_glColorMask(void *eng, unsigned int cap, int on);

void paintcanvas_ext_dr_glDrawArrays(int a, int b, int c);

void paintcanvas_ext_dr_drawline2d(void *eng, void *p, int n, bool b);

void paintcanvas_ext_fbo_a(void *);

void paintcanvas_ext_fbo_b(void *, int);

void paintcanvas_ext_fbo_c(void *);

void paintcanvas_ext_fbo_d(void *);

void paintcanvas_ext_sprite_alluv(float, float, float, float, void *);

void paintcanvas_ext_sprite_allsize(unsigned int, void *);

void *paintcanvas_ext_str_text(const AbyssEngine::String *);

int paintcanvas_ext_text_width_range(void *, void *, unsigned int, unsigned int);

char *paintcanvas_ext_find_res(void *, unsigned int);

void paintcanvas_ext_change_mat(void *, void *, void *);

void paintcanvas_ext_setcolor(void *, float, float, float, float);

void paintcanvas_ext_vibrate(void *);

void paintcanvas_ext_drawtextlines6(void *, unsigned int, void *, int, int, bool);

int paintcanvas_ext_strcmp(void *, void *);

unsigned int paintcanvas_ext_strlen(void *);

int paintcanvas_ext_text_width(void *, unsigned int, unsigned int);

void paintcanvas_ext_clear(int);

int paintcanvas_ext_getscreenpos_m(void *self, void *m, const Vector *a, Vector *b);

void paintcanvas_ext_gl_disable(unsigned int);

void paintcanvas_ext_gl_enable(unsigned int);

void paintcanvas_ext_setprojmatrix3d(void *, float, float, float);

int paintcanvas_ext_getdisplaywidth(void *);

int paintcanvas_ext_getdisplayheight(void *);

void paintcanvas_ext_mat_intern(void *, void *);

void paintcanvas_ext_camera_apply(void *, unsigned int, unsigned int, unsigned int);

void paintcanvas_ext_disable(int);

int paintcanvas_ext_is_posteffect(void *);

void paintcanvas_ext_use_refract(void *);

void paintcanvas_ext_sprite_uv(unsigned int, float, float, float, float, void *);

void paintcanvas_ext_set_wvm(void *);

void paintcanvas_ext_convert_vbo(void *);

void paintcanvas_ext_di2_restore(unsigned int flag, void *img);

void paintcanvas_ext_di2_settexture(void *self, unsigned int tex, int slot);

float paintcanvas_ext_di2_signedtofloat(int v, unsigned int mode);

float paintcanvas_ext_di2_unsignedtofloat(unsigned int v, unsigned int mode);

void paintcanvas_ext_di2_setwvm(void *self, void *m);

void paintcanvas_ext_di2_gldisable(unsigned int cap);

void paintcanvas_ext_di2_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_di2_glenable(unsigned int cap);

void paintcanvas_ext_font_set_yoff(void *, int);

void paintcanvas_ext_sbm_lightenable(void *eng, int on);

void paintcanvas_ext_sbm_lightsetlight(void *eng, int v);

void paintcanvas_ext_sbm_glenablecap(void *eng, unsigned int v, int on);

void paintcanvas_ext_sbm_glTexEnvi(unsigned int a, unsigned int b, unsigned int c);

void paintcanvas_ext_sbm_glEnable(unsigned int cap);

void paintcanvas_ext_sbm_glDisable(unsigned int cap);

void paintcanvas_ext_sbm_glBlendFunc(unsigned int a, unsigned int b);

void paintcanvas_ext_sbm_glDepthMask(int v);

void paintcanvas_ext_sbm_glAlphaFunc(unsigned int a, float ref);

void paintcanvas_ext_sbm_setalpha(void *eng, unsigned int v, int on);

void paintcanvas_ext_sbm_setlight(int on);

void paintcanvas_ext_sbm_texcombine(unsigned int a, unsigned int b, unsigned int c);

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

void paintcanvas_ext_add_resource(void *, void *);

void paintcanvas_ext_remove_meshid(void *, void *);

int paintcanvas_ext_get_height(void *);

void paintcanvas_ext_gl_a(unsigned int);

void paintcanvas_ext_gl_bind(unsigned int, unsigned int);

void paintcanvas_ext_gl_c(unsigned int);

void *paintcanvas_ext_cube_restore(void *);

void paintcanvas_ext_cube_tail(void *);

void *paintcanvas_ext_tfc_findres(void *self, unsigned short id);

void *paintcanvas_ext_tfc_new_transform();

void paintcanvas_ext_tfc_mtx_assign(void *dst, void *src);

void paintcanvas_ext_tfc_meshcreate(void *self, unsigned short id, unsigned int *out, bool b);

void paintcanvas_ext_set_wvm2(void *self, void *m);

void paintcanvas_ext_meshdraw(void *engine, void *mesh);

void paintcanvas_ext_mesh_changemat(AbyssEngine::PaintCanvas *, void *, void *);

void paintcanvas_ext_transform_changemat(AbyssEngine::PaintCanvas *, void *, void *);

int paintcanvas_ext_text_height(void *);

void paintcanvas_ext_init_setorientation(void *eng);

int paintcanvas_ext_init_dispwidth(void *eng);

int paintcanvas_ext_init_dispheight(void *eng);

float paintcanvas_ext_init_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_init_setpersp(void *self, float a, float b, float c);

void paintcanvas_ext_draw_mesh(void *, void *);


int paintcanvas_ext_font_get_yoff(void *);

int paintcanvas_ext_spm_dispwidth(void *eng);

int paintcanvas_ext_spm_dispheight(void *eng);

float paintcanvas_ext_spm_sinf(float v);

float paintcanvas_ext_spm_cosf(float v);

float paintcanvas_ext_spm_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_dss1_matidentity(void *out, void *m);

void *paintcanvas_ext_dss1_getgrav(void *eng);

float paintcanvas_ext_dss1_sinf(float v);

float paintcanvas_ext_dss1_cosf(float v);

void paintcanvas_ext_dss1_memcpy(void *dst, void *src, unsigned int n);

void paintcanvas_ext_dss1_mtx_muleq(void *m, void *rhs);

void paintcanvas_ext_dss1_mtx_getinv(void *out, void *m);

void paintcanvas_ext_dss1_mtx_assign(void *dst, void *src);

void paintcanvas_ext_dss1_ssdraw(void *eng, void *ident, void *m, void *ss);

unsigned short *paintcanvas_ext_gl_strindex(const AbyssEngine::String *str, unsigned int i);

void paintcanvas_ext_gl_substr(void *out, const AbyssEngine::String *str, unsigned int begin,
                                          unsigned int end);

void paintcanvas_ext_gl_str_fromchar(void *out, const char *s, bool copy);

void paintcanvas_ext_gl_str_assign(AbyssEngine::String *dst, void *src);

void paintcanvas_ext_gl_str_dtor(void *s);

void paintcanvas_ext_dl_glLineWidth(float w);

void paintcanvas_ext_dl_glEnable(void *eng, bool on);

float paintcanvas_ext_dl_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_dl_setwvm(void *self, void *m);

void paintcanvas_ext_dl_glVertexPointer(int a, int b, int c, void *p);

void paintcanvas_ext_dl_glColorMask(void *eng, unsigned int cap, int on);

void paintcanvas_ext_dl_glDrawArrays(int a, int b, int c);

void paintcanvas_ext_dl_drawline2d(void *eng, void *p, bool b);

int paintcanvas_ext_get_w(AbyssEngine::PaintCanvas *);

int paintcanvas_ext_get_h(AbyssEngine::PaintCanvas *);

void paintcanvas_ext_cam_persp4(float, float, float, float, float, void *);

void paintcanvas_ext_cam_setcur(AbyssEngine::PaintCanvas *, unsigned int);

void paintcanvas_ext_end3d(AbyssEngine::PaintCanvas *);

void paintcanvas_ext_material_clone(void *, void *);

void paintcanvas_ext_material_add(void *, void *);

void paintcanvas_ext_get_grav(void *);

void paintcanvas_ext_glMatrixMode(unsigned int);

void paintcanvas_ext_gl_depthmask(unsigned int);

void paintcanvas_ext_gl_color(void *, float, float, float, float);

void paintcanvas_ext_matgl_load(void *, void *);

void paintcanvas_ext_gl_loadidentity();

void paintcanvas_ext_gl_ortho_persp(float, float, float);

void paintcanvas_ext_gl_loadmatrix(void *);

void paintcanvas_ext_gl_done();

void *paintcanvas_ext_fc_findres(void *self, unsigned short id);

void paintcanvas_ext_fc_texcreate(void *self, unsigned short id, bool b);

int paintcanvas_ext_fc_fontfromfile(void *eng, char *path, unsigned short region, void **out);

int paintcanvas_ext_fc_fontheight(void *font);

void paintcanvas_ext_set_reslist(AbyssEngine::Resource * const *, unsigned int, void *);

void paintcanvas_ext_child_link(void *, void *, void *);

void paintcanvas_ext_transform_dirty(void *);

static inline void tcg_glActiveTexture(unsigned unit) { glActiveTexture(unit); }

static inline void tcg_glBindTexture(unsigned target, unsigned tex) { glBindTexture(target, tex); }

int paintcanvas_ext_ss2_sscreate(void *eng, unsigned short id, bool b, void **out);

void paintcanvas_ext_ss2_matcreate(void *self, unsigned short id, unsigned int *out);

void paintcanvas_ext_gsp_vec_assign(void *dst, void *src);

static inline int paintcanvas_ext_gsp_getwidth(void *self) { return ((PaintCanvas *) self)->GetWidth(); }

static inline int paintcanvas_ext_gsp_getheight(void *self) { return ((PaintCanvas *) self)->GetHeight(); }

float paintcanvas_ext_gsp_signedtofloat(int v, unsigned int mode);

int paintcanvas_ext_sscreate(void *eng, unsigned short id, bool b, void **out);

void *paintcanvas_ext_material_ctor(void *);

void paintcanvas_ext_cisvf_matidentity(void *out, void *m);

void *paintcanvas_ext_cisvf_getgrav(void *eng);

float paintcanvas_ext_cisvf_sinf(float v);

float paintcanvas_ext_cisvf_cosf(float v);

int paintcanvas_ext_cisvf_inner(const float *pt, float radius, void *m, void *cam);

void paintcanvas_ext_shader_anim(void *, void *);

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

void paintcanvas_ext_cipvf_matidentity(void *out, void *m);

void *paintcanvas_ext_cipvf_getgrav(void *eng);

float paintcanvas_ext_cipvf_sinf(float v);

float paintcanvas_ext_cipvf_cosf(float v);

int paintcanvas_ext_cipvf_inner(const float *pt, void *m, void *cam);

void paintcanvas_ext_set_texture(void *);

void paintcanvas_ext_dtor_releaseall(void *self);

void paintcanvas_ext_dtor_op_delete(void *p);

void *paintcanvas_ext_dtor_restex_dtor(void *p);

void *paintcanvas_ext_dtor_resmesh_dtor(void *p);

void *paintcanvas_ext_dtor_restransform_dtor(void *p);

void paintcanvas_ext_dtor_str_dtor(void *p);

void paintcanvas_ext_dtor_meshrelease(void *eng, void *meshptr);

void *paintcanvas_ext_i2d_findres(void *self, unsigned short id);

void paintcanvas_ext_i2d_texcreate(void *self, unsigned short id, bool b);

int paintcanvas_ext_i2d_imgregion(void *eng, char *path, unsigned short region, void *img);

void paintcanvas_ext_gsp2_transformvec(void *out, const void *vec);

void paintcanvas_ext_gsp2_matidentity(void *out, void *m);

void *paintcanvas_ext_gsp2_getgrav(void *eng);

float paintcanvas_ext_gsp2_sinf(float v);

float paintcanvas_ext_gsp2_cosf(float v);

void paintcanvas_ext_gsp2_memcpy(void *dst, void *src, unsigned int n);

void paintcanvas_ext_gsp2_mtx_muleq(void *m, void *rhs);

void paintcanvas_ext_gsp2_invtransformvec(void *outMat, void *vec);

void paintcanvas_ext_gsp2_vec_assign(void *dst, void *src);

int paintcanvas_ext_gsp2_getwidth(void *self);

int paintcanvas_ext_gsp2_getheight(void *self);

float paintcanvas_ext_gsp2_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_di_restore(unsigned int flag, void *img);

void paintcanvas_ext_di_settexture(void *self, unsigned int tex, int slot);

void paintcanvas_ext_di_setwvm(void *self, void *m);

void paintcanvas_ext_di_gldisable(unsigned int cap);

void paintcanvas_ext_di_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_di_glenable(unsigned int cap);

char *paintcanvas_ext_find_mesh(void *, unsigned short);

void paintcanvas_ext_remove_mesh(void *, unsigned int, int);

void paintcanvas_ext_enable(int);

void paintcanvas_ext_depthmask(int);

void paintcanvas_ext_clear2(void *, unsigned int);

void paintcanvas_ext_array_remove(void *, void *);

void *paintcanvas_ext_tc_findres(void *self, unsigned short id);

void paintcanvas_ext_start_fbo(void *);

void paintcanvas_ext_gl_deletetextures(int, void *);

void paintcanvas_ext_mtx_mul(void *out, const void *a, void *b);

void paintcanvas_ext_mtx_muleq(void *m, void *rhs);

int paintcanvas_ext_dt_incamvf(void *tf, void *m, void *cam);

void paintcanvas_ext_dt_drawtransform_rec(void *self, void *tf, void *m, void *m3);

void paintcanvas_ext_di4_restore(unsigned int flag, void *img);

void paintcanvas_ext_di4_settexture(void *self, unsigned int tex);

int paintcanvas_ext_di4_getwidth(void *self);

int paintcanvas_ext_di4_getheight(void *self);

float paintcanvas_ext_di4_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_di4_setwvm(void *self, void *m);

void paintcanvas_ext_di4_gldisable(unsigned int cap);

void paintcanvas_ext_di4_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_di4_glenable(unsigned int cap);

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

void paintcanvas_ext_sgo_setorientation(void *eng, int mode);

int paintcanvas_ext_sgo_dispwidth(void *eng);

int paintcanvas_ext_sgo_dispheight(void *eng);

float paintcanvas_ext_sgo_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_sgo_setpersp(void *self, float a, float b, float c);

void paintcanvas_ext_mc_matcreate(void *self, unsigned short id, unsigned int *out);

void paintcanvas_ext_dm_memcpy(void *dst, const void *src, unsigned int n);

void paintcanvas_ext_dm_settrans(void *out, float v);

void paintcanvas_ext_dm_getpos(void *out);

void paintcanvas_ext_dm_settrans_vec(void *out, void *vec);

void paintcanvas_ext_dm_mtx_mul(void *out, const void *a, const void *b);

void paintcanvas_ext_dm_mtx_assign(void *dst, const void *src);

void paintcanvas_ext_dm_transformvec(void *m, void *vec);

void paintcanvas_ext_dm_rotatevec(void *m, void *vec);

void paintcanvas_ext_dm_vec_assign(void *dst, const void *src);

int paintcanvas_ext_dm_spherefrustum(void *self, void *pt, float radius);

float paintcanvas_ext_dm_unsignedtofloat(unsigned int v, unsigned int mode);

void paintcanvas_ext_dm_setcolor(void *eng, float r, float g, float b, float a);

void paintcanvas_ext_dm_mtx_muleq(void *m, const void *rhs);

void paintcanvas_ext_dm_setwvm(void *self, void *m);

void paintcanvas_ext_dm_setmodelmatrix(void *eng);

void paintcanvas_ext_dm_setuvmatrix(void *eng, void *m);

void paintcanvas_ext_dm_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_dm_resetuvmatrix(void *eng);

void paintcanvas_ext_dm_addcached_mesh(void *mesh, void *arr);

void paintcanvas_ext_dm_addcached_uint(unsigned int v, void *arr);

void paintcanvas_ext_dm_pushmat(const float *m, void *arr);

int paintcanvas_ext_dm_incamvf(void *tf, void *m, void *cam);

void paintcanvas_ext_dm_drawtransform(void *self, void *tf, void *m, void *m2);

void paintcanvas_ext_gl_blendfunc(unsigned int, unsigned int);

void paintcanvas_ext_glenable2(void *, unsigned int, bool);

void paintcanvas_ext_setortho(void *, void *, void *, bool);

void paintcanvas_ext_gl_texenvi(unsigned int, unsigned int, unsigned int);

void paintcanvas_ext_gl_scalef(float, float, float);

void paintcanvas_ext_gl_multmatrix(void *);

void paintcanvas_ext_string_prep(AbyssEngine::PaintCanvas *, void *, int);

void paintcanvas_ext_dsc_settexture(void *self, unsigned int tex);

void paintcanvas_ext_dsc_getcolor(void *self);

void paintcanvas_ext_dsc_str_copy(void *out, const AbyssEngine::String *src, bool copy);

void paintcanvas_ext_dsc_str_fromchar(void *out, const char *s, bool copy);

void *paintcanvas_ext_dsc_splittags(void *str, void *sep);

void paintcanvas_ext_dsc_str_dtor(void *s);

unsigned short *paintcanvas_ext_dsc_str_cast(void *str);

int paintcanvas_ext_dsc_textwidth(void *self, unsigned int font, void *str);

void paintcanvas_ext_dsc_setcolor(void *self);

char *paintcanvas_ext_dsc_getAEChar(void *str);

int paintcanvas_ext_dsc_sscanf(const char *s, const char *fmt, void *out);

void paintcanvas_ext_dsc_releaseclasses(void *arr);

void *paintcanvas_ext_dsc_arr_dtor(void *arr);

void paintcanvas_ext_dsc_op_delete(void *p);

void paintcanvas_ext_smfg_pushmat(const float *m, void *array);

void paintcanvas_ext_ec_glEnable(unsigned int cap);

int paintcanvas_ext_ec_getHeight(void *self);

int paintcanvas_ext_ec_getWidth(void *self);

void paintcanvas_ext_ec_glScissor(int x, int y, int w, int h);

void paintcanvas_ext_di3_restore(unsigned int flag, void *img);

int paintcanvas_ext_di3_getwidth(void *self);

int paintcanvas_ext_di3_getheight(void *self);

float paintcanvas_ext_di3_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_di3_settexture(void *self, unsigned int tex);

void paintcanvas_ext_di3_setwvm(void *self, void *m);

void paintcanvas_ext_di3_meshdraw(void *eng, void *mesh);

int paintcanvas_ext_meshcreate(void *, void *);

int paintcanvas_ext_font_get_spacing(void *);

void *paintcanvas_ext_mc2_findres(void *self, unsigned short id);

void paintcanvas_ext_mc2_matcreate(void *self, unsigned short id, unsigned int *out);

int paintcanvas_ext_mc2_meshfromfile(void *eng, char *path, void **out, void *mat);

void *paintcanvas_ext_mc2_new_mesh_copy(void *src);

void paintcanvas_ext_mc2_converttovbo(void *mesh);

float paintcanvas_ext_fsp_unsignedtofloat(unsigned int v, unsigned int mode);

void paintcanvas_ext_fsp_glFogf(unsigned int pname, float v);

void paintcanvas_ext_fsp_glFogfv(unsigned int pname, void *v);

void paintcanvas_ext_fsp_vec_assign(void *dst, void *src);

void paintcanvas_ext_dr3_settexture(void *self, unsigned int tex);

float paintcanvas_ext_dr3_signedtofloat(int v, unsigned int mode);

float paintcanvas_ext_dr3_unsignedtofloat(unsigned int v, unsigned int mode);

float paintcanvas_ext_dr3_sinf(float v);

float paintcanvas_ext_dr3_cosf(float v);

void paintcanvas_ext_dr3_mtx_mul(void *out, void *m);

void paintcanvas_ext_dr3_mtx_assign(void *dst, void *src);

void paintcanvas_ext_dr3_setwvm(void *self, void *m);

void paintcanvas_ext_dr3_gldisable(unsigned int cap);

void paintcanvas_ext_dr3_meshdraw(void *eng, void *mesh);

void paintcanvas_ext_dr3_glenable(unsigned int cap);

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

void paintcanvas_ext_check_string(void *, unsigned int, unsigned int);

char *paintcanvas_ext_rt_getAEChar(void *strField);

void paintcanvas_ext_rt_deletearr(char *p);

void *paintcanvas_ext_matc_findres(void *self, unsigned short id);

void *paintcanvas_ext_matc_new_material();

void paintcanvas_ext_matc_texcreate(void *self, unsigned short id, bool b);

void paintcanvas_ext_matc_vec_assign(void *dst, void *src);

float paintcanvas_ext_tg2di_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_tg2di_meshintersect(void *out, float a, float b, void *mesh);

void paintcanvas_ext_tg2di_memcpy(void *dst, void *src, unsigned int n);

void paintcanvas_ext_tg2di_invtransformvec(void *outMat, void *vec);

void paintcanvas_ext_tg2di_vec_assign(void *dst, void *src);

void paintcanvas_ext_tg2di_inner(void *out, void *self, void *childtf, int x, int y);

void paintcanvas_ext_dt2_matidentity(void *out, void *m);

void *paintcanvas_ext_dt2_getgrav(void *eng);

float paintcanvas_ext_dt2_sinf(float v);

float paintcanvas_ext_dt2_cosf(float v);

int paintcanvas_ext_dt2_incamvf(void *tf, void *m, void *cam);

void paintcanvas_ext_dt2_mtx_assign(void *dst, const void *src);

void paintcanvas_ext_dt2_mtx_muleq(void *m, void *rhs);

void paintcanvas_ext_dt2_mtx_getinv(void *out, void *m);

void paintcanvas_ext_dt2_seteye(void *eng, float a, float b, float c);

void paintcanvas_ext_dt2_drawrec(void *self, void *tf, void *m, void *m2);

void paintcanvas_ext_font_set_spacing(void *, int);

int paintcanvas_ext_get_width(void *);

static inline int pc_GetWidth(AbyssEngine::PaintCanvas *self) { return self->GetWidth(); }
static inline int pc_GetHeight(AbyssEngine::PaintCanvas *self) { return self->GetHeight(); }

void pc_Camera_ctor(void *cam, float h, float w);

void pc_ArrayAdd_Camera(void *cam, void *arr);

void paintcanvas_ext_dss_matidentity(void *out, void *m);

void *paintcanvas_ext_dss_getgrav(void *eng);

float paintcanvas_ext_dss_sinf(float v);

float paintcanvas_ext_dss_cosf(float v);

void paintcanvas_ext_dss_mtx_muleq(void *m, void *rhs);

void paintcanvas_ext_dss_mtx_getinv(void *out, void *m);

void paintcanvas_ext_dss_mtx_assign(void *dst, void *src);

void paintcanvas_ext_dss_ssdraw(void *eng, void *ident, void *m, void *ss);

int paintcanvas_ext_mesh_tricount(AbyssEngine::PaintCanvas *, void *);

int paintcanvas_ext_transform_tricount(AbyssEngine::PaintCanvas *, void *);

void paintcanvas_ext_mesh_shaderanim(AbyssEngine::PaintCanvas *, void *, float, unsigned int);

void paintcanvas_ext_transform_shaderanim(AbyssEngine::PaintCanvas *, void *, float, unsigned int);

void paintcanvas_ext_release_sprite_res(void *, void *);

void paintcanvas_ext_dss2_matidentity(void *out, void *m);

void *paintcanvas_ext_dss2_getgrav(void *eng);

float paintcanvas_ext_dss2_sinf(float v);

float paintcanvas_ext_dss2_cosf(float v);

float paintcanvas_ext_dss2_signedtofloat(int v, unsigned int mode);

void paintcanvas_ext_dss2_mtx_muleq(void *m, void *rhs);

void paintcanvas_ext_dss2_mtx_getinv(void *out, void *m);

void paintcanvas_ext_dss2_mtx_assign(void *dst, void *src);

void paintcanvas_ext_dss2_ssdraw(void *eng, void *worldM, void *viewM, void *ss);

unsigned short PaintCanvas::GetImage2DWidth(unsigned int index) {
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
            int w = paintcanvas_ext_dtl_textwidth(this, font, arr->data()[i]);
            xoff = -(w >> 1);
        }
        paintcanvas_ext_dtl_drawstring(this, font, arr->data()[i], xoff + x, y, false);
        y += paintcanvas_ext_dtl_textheight(this, font);
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

    paintcanvas_ext_fr_setwvm(this, abuf);
    paintcanvas_ext_fr_glenable(this->engine, 0xde1, false);
    paintcanvas_ext_fr_meshdraw(this->engine, this->lineMesh);
    paintcanvas_ext_fr_glenable(this->engine, 0xde1, true);
}

void PaintCanvas::SpriteSystemSetRGBA(unsigned int index, unsigned short sub,
                                      float a, float b, float c, float d) {
    if (index < this->spriteSystems.count) {
        PCSpriteSystemView *s = (PCSpriteSystemView *) (this->spriteSystems.data_)[index];
        if (s) {
            if ((unsigned int) (unsigned short) s->count <= (unsigned int) sub) {
                return;
            }
            return paintcanvas_ext_sprite_rgba(sub, a, b, c, d, s);
        }
    }
}

void PaintCanvas::TransformCreate(unsigned int &out) {
    void *obj = paintcanvas_ext_alloc(0x180);
    paintcanvas_ext_transform_ctor(obj);
    paintcanvas_ext_add_child(obj, &this->transformCount);
    out = this->transformCount - 1;
}


static char g_dr_flag_79368_storage = 0;
static char *const g_dr_flag_79368 = &g_dr_flag_79368_storage;

void PaintCanvas::DrawRectangle(int x, int y, int w, int h) {
    char abuf[60];

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

    paintcanvas_ext_dr_setwvm(this, abuf);
    if (*g_dr_flag_79368 == 0) {
        paintcanvas_ext_dr_glLineWidth(1.0f);
        paintcanvas_ext_dr_glcap(this->engine, 0xde1, 0);
        paintcanvas_ext_dr_glVertexPointer(2, 0x1406, 0, this->lineVerts);
        paintcanvas_ext_dr_glColorMask(this->engine, 0x8074, 1);
        paintcanvas_ext_dr_glColorMask(this->engine, 0x8078, 0);
        paintcanvas_ext_dr_glColorMask(this->engine, 0x8075, 0);
        paintcanvas_ext_dr_glColorMask(this->engine, 0x8076, 0);
        paintcanvas_ext_dr_glDrawArrays(2, 0, 4);
        paintcanvas_ext_dr_glcap(this->engine, 0xde1, 1);
    } else {
        paintcanvas_ext_dr_drawline2d(this->engine, this->lineVerts, 4, true);
    }
}

unsigned short PaintCanvas::GetImage2DHeight(unsigned int index) {
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
    paintcanvas_ext_fbo_a(this->engine);
    paintcanvas_ext_fbo_b(this, 0);
    paintcanvas_ext_fbo_c(this->engine);
    return paintcanvas_ext_fbo_d(this->engine);
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
    return paintcanvas_ext_sprite_alluv(a, b, c, d, sprite);
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
    PCArrayRemoveAll(&this->glowMeshes_count);
    PCArrayRemoveAll(&this->glowMatA_count);
    PCArrayRemoveAll(&this->glowMatB_count);
    PCArrayRemoveAll(&this->glowUints_count);
    return PCArrayRemoveAll(&this->glowMatC_count);
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
            int w = paintcanvas_ext_dtl_textwidth(this, font, arr->data()[i]);
            xoff = (int) p5 - w;
        }
        paintcanvas_ext_dtl_drawstring(this, font, arr->data()[i], xoff + x, y, false);
        y += paintcanvas_ext_dtl_textheight(this, font);
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
        void *font = (this->fonts.data_)[index];
        void *text = paintcanvas_ext_str_text(&str);
        return paintcanvas_ext_text_width_range(font, text, begin, end - begin);
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
        void *font = (this->fonts.data_)[index];
        void *data = paintcanvas_ext_str_text(&str);
        return paintcanvas_ext_text_width(font, (unsigned int) (uintptr_t) data, str.size());
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
    char buf[60];
    float *m = (float *) buf;
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

    return paintcanvas_ext_getscreenpos_m(this, buf, &a, &b);
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
        return paintcanvas_ext_mat_intern(this, mesh->field_0x34);
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
    paintcanvas_ext_disable(0xc11);
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

static char paintcanvas_g_refract1 = 0;
static char paintcanvas_g_refract2 = 0;

void PaintCanvas::CheckNUseRefractFBO(bool) {
    if (paintcanvas_g_refract1 != 0 && paintcanvas_g_refract2 != 0 &&
        paintcanvas_ext_is_posteffect(this->engine) == 0) {
        return paintcanvas_ext_use_refract(this->engine);
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
            return paintcanvas_ext_sprite_uv(sub, a, b, c, d, s);
        }
    }
}

void PaintCanvas::SetWorldViewMatrix(const AbyssEngine::AEMath::Matrix &) {
    return paintcanvas_ext_set_wvm(this->engine);
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
        return paintcanvas_ext_convert_vbo(
            (this->meshes)[index]);
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
        paintcanvas_ext_di2_restore(img->restoreFlag, img);
        img = (PCImage2DView *) ((char **) this->images.data_)[index];
    }
    paintcanvas_ext_di2_settexture(this, img->textureId, -1);

    float fx = paintcanvas_ext_di2_signedtofloat(x, 0);
    float fy = paintcanvas_ext_di2_signedtofloat(y, 0);

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
            off = paintcanvas_ext_di2_unsignedtofloat(w, 0);
        }
        fx = off + fx;
    }
    if (flipFlags & 2) {
        m[5] = g_di2_one_88d90;
        float off = g_di2_def_88d94;
        if (index < this->images.count) {
            unsigned short h = ((PCImage2DView *) ((char **) this->images.data_)[index])->height;
            off = paintcanvas_ext_di2_unsignedtofloat(h, 0);
        }
        fy = off + fy;
    }
    m[3] = fx;
    m[7] = fy;

    paintcanvas_ext_di2_setwvm(this, m);
    paintcanvas_ext_di2_gldisable(0xb44);
    paintcanvas_ext_di2_meshdraw(this->engine,
                                 ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh);
    paintcanvas_ext_di2_glenable(0xb44);
}

void PaintCanvas::FontSetYOffset(unsigned int index, short yoff) {
    if (index < this->fonts.count) {
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_font_set_yoff(font, yoff);
    }
}


static char g_sbm_flag_8cb62_storage = 0;
static char *const g_sbm_flag_8cb62 = &g_sbm_flag_8cb62_storage;

static const unsigned int g_sbm_const_8ce34 = 0;

void PaintCanvas::SetBlendMode(AbyssEngine::BlendMode mode) {
    paintcanvas_ext_sbm_lightenable(this->engine, 0);

    char *flag = g_sbm_flag_8cb62;
    if (*flag != 0) {
        paintcanvas_ext_sbm_glenablecap(this->engine, g_sbm_const_8ce34, 0);
    } else {
        paintcanvas_ext_sbm_glTexEnvi(0x2300, 0x2200, 0x2100);
    }

    switch (mode) {
        case 1:
        case 5:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_setlight(0);
            return;
        case 2:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(1, 1);
            paintcanvas_ext_sbm_setlight(0);
            return;
        case 3:
            paintcanvas_ext_sbm_glDisable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(1, 1);
            paintcanvas_ext_sbm_setlight(0);
            return;
        case 4:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0, 0x301);
            paintcanvas_ext_sbm_setlight(0);
            return;
        case 6:
            paintcanvas_ext_sbm_lightenable(this->engine, 1);
            paintcanvas_ext_sbm_lightsetlight(this->engine, 0x4000);
        /* fallthrough */
        case 0:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glDisable(0xbe2);
            paintcanvas_ext_sbm_setlight(1);
            return;
        case 7:
            paintcanvas_ext_sbm_lightenable(this->engine, 1);
            paintcanvas_ext_sbm_lightsetlight(this->engine, 0x4000);
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(1, 1);
            paintcanvas_ext_sbm_setlight(0);
            return;
        case 8:
            paintcanvas_ext_sbm_lightenable(this->engine, 1);
            paintcanvas_ext_sbm_lightsetlight(this->engine, 0x4000);
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_setlight(0);
            return;
        case 9:
            paintcanvas_ext_sbm_lightenable(this->engine, 1);
            paintcanvas_ext_sbm_lightsetlight(this->engine, 0x4000);
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_setlight(1);
            return;
        case 10:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glDisable(0xbe2);
            paintcanvas_ext_sbm_glDepthMask(1);
            paintcanvas_ext_sbm_setalpha(this->engine, 0x1000000, 1);
            if (*flag == 0) {
                paintcanvas_ext_sbm_glAlphaFunc(0x204, 0.5f);
            }
            return;
        case 0x15:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_glDepthMask(0);
            if (*flag != 0) {
                paintcanvas_ext_sbm_setalpha(this->engine, g_sbm_const_8ce34, 1);
                return;
            }
            paintcanvas_ext_sbm_texcombine(0x2300, 0x2200, 0x8570);
            paintcanvas_ext_sbm_texcombine(0x2300, 0x8571, 0x104);
            paintcanvas_ext_sbm_texcombine(0x2300, 0x8572, 0x1e01);
            paintcanvas_ext_sbm_texcombine(0x2300, 0x8580, 0x1702);
            paintcanvas_ext_sbm_texcombine(0x2300, 0x8590, 0x300);
            paintcanvas_ext_sbm_texcombine(0x2300, 0x8581, 0x8577);
            paintcanvas_ext_sbm_texcombine(0x2300, 0x8591, 0x300);
            return;
        case 0x16:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_glDepthMask(0);
            if (*flag == 0) {
                paintcanvas_ext_sbm_texcombine(0x2300, 0x2200, 0x8570);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8571, 0x8575);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8572, 0x1e01);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8580, 0x1702);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8590, 0x300);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8588, 0x1702);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8598, 0x302);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8581, 0x8577);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8591, 0x300);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8582, 0x8577);
                paintcanvas_ext_sbm_texcombine(0x2300, 0x8592, 0x302);
            }
            return;
        case 0x25:
            paintcanvas_ext_sbm_glEnable(0xb44);
            paintcanvas_ext_sbm_glEnable(0xbe2);
            paintcanvas_ext_sbm_glBlendFunc(0x302, 0x303);
            paintcanvas_ext_sbm_setlight(1);
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
    return paintcanvas_ext_get_height(this->engine);
}

static char paintcanvas_g_cube_enabled = 0;
static int paintcanvas_g_cube_slot = 0;

void PaintCanvas::ChangeCubeTexture(unsigned int idx) {
    if (paintcanvas_g_cube_enabled != 0 && idx < this->cubeTextures.count) {
        PCCubeTexView *tex = (PCCubeTexView *) (this->cubeTextures.data_)[idx];
        if (tex->restoreFlag == 0) {
            return paintcanvas_ext_cube_tail(paintcanvas_ext_cube_restore(tex->pathField));
        }
        paintcanvas_g_cube_slot = idx;
        paintcanvas_ext_gl_a(0x84c7);
        PCCubeTexView *tex2 = (PCCubeTexView *) (this->cubeTextures.data_)[idx];
        paintcanvas_ext_gl_bind(0x8513, (unsigned int) tex2->glTexId);
        return paintcanvas_ext_gl_c(0x84c0);
    }
}


void PaintCanvas::TransformCreate(unsigned short resId, unsigned int &out) {
    PCResourceView *res = (PCResourceView *) paintcanvas_ext_tfc_findres(this, resId);
    if (res == 0) {
        return;
    }
    if ((unsigned int) res->handle != 0xffffffff) {
        out = (unsigned int) res->handle;
        return;
    }
    PCTransformInfoView *info = (PCTransformInfoView *) res->payload;
    char *tf = (char *) paintcanvas_ext_tfc_new_transform();
    ArrayAdd<Transform *>((Transform *) tf, *reinterpret_cast<Array<Transform *> *>(&this->transformCount));
    unsigned int idx = this->transformCount - 1;
    res->handle = (int) idx;
    out = idx;
    paintcanvas_ext_tfc_mtx_assign(tf, info);

    unsigned int childMesh = 0xffffffff;
    for (unsigned int i = 0; i < info->childMeshCount; i++) {
        unsigned short mid = ((unsigned short *) info->childMeshIds)[i];
        paintcanvas_ext_tfc_meshcreate(this, mid, &childMesh, false);
    }
    unsigned int childTf = 0xffffffff;
    for (unsigned int i = 0; i < info->childTfCount; i++) {
        unsigned short tid = ((unsigned short *) info->childTfIds)[i];
        this->TransformCreate(tid, childTf);
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
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_text_height(font);
    }
    return 0;
}


static const unsigned int g_init_const_7e7b4 = 0;

static const unsigned int g_init_const_7e7b8 = 0;

void PaintCanvas::Initialize(bool landscape) {
    this->gameOrientation = landscape ? 2 : 0;
    paintcanvas_ext_init_setorientation(this->engine);

    memset(&this->projOrthoMatrix.m[0], 0, 0x10);
    memset(&this->projOrthoMatrix.m[4], 0, 0x10);
    memset(&this->projOrthoMatrix.m[8], 0, 0x10);
    memset(&this->projOrthoMatrix.m[12], 0, 0x10);

    int orient = this->gameOrientation;
    int w = paintcanvas_ext_init_dispwidth(this->engine);
    float fw = paintcanvas_ext_init_signedtofloat(w, 0);
    int h = paintcanvas_ext_init_dispheight(this->engine);
    float fh = paintcanvas_ext_init_signedtofloat(h, 0);

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
        this->worldViewMatrix.m[1] = g_init_const_7e7b4;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = 1.0f;
        this->worldViewMatrix.m[13] = fw;
        ymul = fw;
    }
    this->projOrthoMatrix.m[10] = g_init_const_7e7b8;
    this->projOrthoMatrix.m[15] = 1.0f;
    this->projOrthoMatrix.m[12] = g_init_const_7e7b4;
    this->projOrthoMatrix.m[13] = 1.0f;
    this->projOrthoMatrix.m[5] = -2.0f / ymul;

    paintcanvas_ext_init_setpersp(this, 16384.0f, 512.0f, 65536.0f);
}

void PaintCanvas::DrawMesh(unsigned int index) {
    if (index < this->meshCount) {
        void *mesh = (this->meshes)[index];
        return paintcanvas_ext_draw_mesh(this->engine, mesh);
    }
}

int PaintCanvas::MeshGetTriCount(AbyssEngine::Mesh *mesh) {
    if (mesh) {
        int tri;
        if (mesh->animation == 0) {
            tri = 0;
        } else {
            tri = paintcanvas_ext_transform_tricount(this, mesh->animation);
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


static float g_spm_p0_7b826_storage = 0.0f;
static float *const g_spm_p0_7b826 = &g_spm_p0_7b826_storage;

static float g_spm_p1_7b82a_storage = 0.0f;
static float *const g_spm_p1_7b82a = &g_spm_p1_7b82a_storage;

static float g_spm_p2_7b82c_storage = 0.0f;
static float *const g_spm_p2_7b82c = &g_spm_p2_7b82c_storage;

static float g_spm_p3_7b82e_storage = 0.0f;
static float *const g_spm_p3_7b82e = &g_spm_p3_7b82e_storage;

static float g_spm_p4_7b830_storage = 0.0f;
static float *const g_spm_p4_7b830 = &g_spm_p4_7b830_storage;

static const unsigned int g_spm_const_7b950 = 0;

void PaintCanvas::SetProjectionMatrix3d(float fov, float nearPlane,
                                        float farPlane) {
    *g_spm_p0_7b826 = farPlane;
    *g_spm_p1_7b82a = nearPlane;
    *g_spm_p4_7b830 = fov;
    *g_spm_p2_7b82c = nearPlane;
    *g_spm_p3_7b82e = farPlane;

    int w = paintcanvas_ext_spm_dispwidth(this->engine);
    int h = paintcanvas_ext_spm_dispheight(this->engine);
    float half = fov * 0.5f;
    float s = paintcanvas_ext_spm_sinf(half);
    float c = paintcanvas_ext_spm_cosf(half);
    float fw = paintcanvas_ext_spm_signedtofloat(w, 0);
    float fh = paintcanvas_ext_spm_signedtofloat(h, 0);

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

    this->projMatrix3d.m[11] = g_spm_const_7b950;
    this->projMatrix3d.m[10] = (nearPlane + farPlane) / (nearPlane - farPlane);
    this->projMatrix3d.m[14] = ((farPlane + farPlane) * nearPlane) / (nearPlane - farPlane);
}


static const double g_dss1_gravscale_8ac10 = 0;

void PaintCanvas::DrawSpriteSystem(unsigned int index) {
    if (index >= this->spriteSystems.count) {
        return;
    }
    if (((void **) this->spriteSystems.data_)[index] == 0) {
        return;
    }

    float worldM[16];
    memset(worldM, 0, sizeof(worldM));
    worldM[0] = 1.0f;
    worldM[5] = 1.0f;
    worldM[14] = 1.0f;

    if (this->currentCamera < this->cameras.count) {
        if (this->initialized == 0) {
            float inv[16];
            memset(inv, 0, sizeof(inv));
            inv[0] = 1.0f;
            inv[5] = 1.0f;
            inv[14] = 1.0f;
            paintcanvas_ext_dss1_mtx_getinv(inv, worldM);
            paintcanvas_ext_dss1_mtx_assign(worldM, inv);
        } else {
            float rotM[16];
            char scratch[60];
            memset(rotM, 0, sizeof(rotM));
            rotM[0] = 1.0f;
            rotM[5] = 1.0f;
            rotM[14] = 1.0f;
            paintcanvas_ext_dss1_matidentity(scratch, rotM);

            void *grav = paintcanvas_ext_dss1_getgrav(this->engine);
            double angle = ((PCGravView *) grav)->angle * g_dss1_gravscale_8ac10;
            float a = (float) angle;
            int orient = this->gameOrientation;
            float rot = (orient == 1) ? a : -a;
            float s = paintcanvas_ext_dss1_sinf(rot);
            float c = paintcanvas_ext_dss1_cosf(rot);
            rotM[0] = c;
            rotM[5] = c;
            *(unsigned int *) &rotM[1] = *(unsigned int *) &s ^ 0x80000000;
            rotM[4] = s;

            PCCameraView *cam = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
            paintcanvas_ext_dss1_memcpy(scratch, cam->localMatrix, 0x3c);
            paintcanvas_ext_dss1_mtx_muleq(scratch, rotM);
            paintcanvas_ext_dss1_mtx_getinv(scratch, scratch);
            paintcanvas_ext_dss1_mtx_assign(worldM, scratch);
        }
    }

    float ident[16];
    memset(ident, 0, sizeof(ident));
    ident[0] = 1.0f;
    ident[5] = 1.0f;
    ident[14] = 1.0f;
    paintcanvas_ext_dss1_ssdraw(this->engine, ident, worldM,
                                ((void **) this->spriteSystems.data_)[index]);
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


static char g_dl_flag_794ee_storage = 0;
static char *const g_dl_flag_794ee = &g_dl_flag_794ee_storage;

void PaintCanvas::DrawLine(int x0, int y0, int x1i, int y1i) {
    char abuf[60];

    float x1 = paintcanvas_ext_dl_signedtofloat(x0 + 1, 0);
    float y1 = paintcanvas_ext_dl_signedtofloat(y0, 0);
    float x2 = paintcanvas_ext_dl_signedtofloat(x1i + 1, 0);
    float y2 = paintcanvas_ext_dl_signedtofloat(y1i, 0);

    float *m = (float *) abuf;
    memset(m, 0, 60);
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[14] = 1.0f;

    float *v = this->lineVerts;

    if (*g_dl_flag_794ee == 0) {
        paintcanvas_ext_dl_glLineWidth(1.0f);
        paintcanvas_ext_dl_glEnable(this->engine, true);
        v[0] = x1;
        v[1] = y1;
        v[2] = x2;
        v[3] = y2;
        paintcanvas_ext_dl_setwvm(this, abuf);
        paintcanvas_ext_dl_glVertexPointer(2, 0x1406, 0, this->lineVerts);
        paintcanvas_ext_dl_glColorMask(this->engine, 0x8074, 1);
        paintcanvas_ext_dl_glColorMask(this->engine, 0x8078, 0);
        paintcanvas_ext_dl_glColorMask(this->engine, 0x8075, 0);
        paintcanvas_ext_dl_glColorMask(this->engine, 0x8076, 0);
        paintcanvas_ext_dl_glDrawArrays(1, 0, 2);
        paintcanvas_ext_dl_glEnable(this->engine, true);
    } else {
        paintcanvas_ext_dl_setwvm(this, abuf);
        v[0] = x1;
        v[1] = y1;
        v[2] = x2;
        v[3] = y2;
        paintcanvas_ext_dl_drawline2d(this->engine, this->lineVerts, true);
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
    return paintcanvas_ext_end3d(this);
}

void PaintCanvas::MeshCloneMaterial(unsigned int index, unsigned int &out) {
    int result;
    if (index < this->meshCount) {
        char *obj = (char *) paintcanvas_ext_alloc(0x74);
        PCMeshView *mesh = (PCMeshView *) (this->meshes)[index];
        paintcanvas_ext_material_clone(obj, mesh->material);
        paintcanvas_ext_material_add(obj, &this->materials);
        result = (int) this->materials.count - 1;
    } else {
        result = -1;
    }
    out = result;
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
    PCResourceView *res = (PCResourceView *) paintcanvas_ext_fc_findres(this, resId);
    if (res == 0) {
        return;
    }
    unsigned short *info = (unsigned short *) res->payload;
    PCResourceView *texres = (PCResourceView *) paintcanvas_ext_fc_findres(this, *info);
    if (texres == 0) {
        return;
    }
    if (texres->handle == -1) {
        paintcanvas_ext_fc_texcreate(this, *info, true);
    }
    if (res->handle != -1) {
        out = (unsigned int) res->handle;
        return;
    }
    void *font = 0;
    char *texpath = *(char **) texres->payload;
    int ok = paintcanvas_ext_fc_fontfromfile(this->engine, texpath, info[1], &font);
    if (ok != 1) {
        return;
    }
    if (texres->handle != -1) {
        *(int *) font = texres->handle;
    }
    PCArrayAdd<AbyssEngine::ImageFont *>((AbyssEngine::ImageFont *) font, &this->fonts);
    int idx = this->fonts.count - 1;
    res->handle = idx;
    out = idx;

    Engine *eng = (Engine *) this->engine;
    int cur = eng->field_0x78;
    if (cur == -1) {
        eng->field_0x78 = idx;
    } else {
        PCFontView *curFont = (PCFontView *) (this->fonts.data_)[cur];
        if (curFont->key <= ((PCFontView *) font)->key) {
            int curH = paintcanvas_ext_fc_fontheight(curFont);
            int newH = paintcanvas_ext_fc_fontheight(font);
            if (newH < curH) {
                eng->field_0x78 = out;
            }
        }
    }
}

void PaintCanvas::SetResourceList(AbyssEngine::Resource * const * list, unsigned int count) {
    return paintcanvas_ext_set_reslist(list, count, &this->resources);
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
        char **arr = this->transforms;
        PCTransformView *p = (PCTransformView *) arr[parent];
        char *c = arr[child];
        ArrayAdd<Transform *>((Transform *) c, *reinterpret_cast<Array<Transform *> *>(&p->childCount));
        char **arr2 = this->transforms;
        PCTransformView *p2 = (PCTransformView *) arr2[parent];
        char *c2 = arr2[child];
        paintcanvas_ext_child_link(&p2->bsphere, c2, p2);
        char **arr3 = this->transforms;
        return paintcanvas_ext_transform_dirty(arr3[parent]);
    }
}

int tcg_TextureCreateFromFile(void *engine, const char *path, void *cb, void *ud,
                                         unsigned *outId, bool b, float f);


static int *g_tcg_canary_storage = 0;
static int **g_tcg_canary = &g_tcg_canary_storage;

void PaintCanvas::TextureCreateGlobal(AbyssEngine::String name, unsigned int unit) {
    int *canary = *g_tcg_canary;
    int saved = *canary;

    char *path = name.GetAEChar();
    unsigned outId;
    int rc = tcg_TextureCreateFromFile(this->engine, path, 0, 0, &outId, false,
                                       0.0f);
    if (rc == 1) {
        tcg_glActiveTexture(unit + 0x84c0);
        tcg_glBindTexture(0xde1, 0);
        tcg_glActiveTexture(0x84c0);
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

void paintcanvas_ext_transform_addmesh(AbyssEngine::PaintCanvas *, void *,
                                                  unsigned short, bool);

void PaintCanvas::TransformAddMesh(unsigned int transformIndex,
                                   unsigned short meshId, bool b) {
    if (transformIndex < this->transformCount) {
        void *t = (this->transforms)[transformIndex];
        return paintcanvas_ext_transform_addmesh(this, t, meshId, b);
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
        paintcanvas_ext_ss2_matcreate(this, matResId, &mat);
        if (mat <= this->materials.count) {
            ::Node *node = ((PCSpriteSystemView2 *) ss)->node;
            node->field_0x30 =
                    (unsigned int) (uintptr_t) this->materials.data_[mat];
        }
        unsigned int i;
        for (i = 0; i < this->spriteSystems.count; i++) {
            void **slot = (void **) &this->spriteSystems.data_[i * 4];
            if (*slot == nullptr) {
                *slot = ss;
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

void PaintCanvas::GetScreenPosition(const AbyssEngine::AEMath::Matrix &srcMatrix,
                                    AbyssEngine::AEMath::Vector &outVec) {
    if (this->currentCamera >= this->cameras.count) {
        return;
    }

    float src[3];
    src[0] = srcMatrix.m[3];
    src[1] = srcMatrix.m[7];
    src[2] = srcMatrix.m[11];
    paintcanvas_ext_gsp_vec_assign(&outVec, src);

    PCCameraView *cam = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
    float z = outVec[2];
    float denomX = cam->factor4c * z;
    if (denomX == 0.0f) {
        return;
    }
    float denomY = z * cam->factor48;
    if (denomY == 0.0f) {
        return;
    }

    float px = outVec[0];
    int w0 = paintcanvas_ext_gsp_getwidth(this);
    int w1 = paintcanvas_ext_gsp_getwidth(this);
    double fw = (double) paintcanvas_ext_gsp_signedtofloat(w0, 0);
    double termY = ((double) outVec[1] * 0.5) / (double) denomY;
    double halfW = (double) paintcanvas_ext_gsp_signedtofloat(w1 >> 1, 0);
    outVec[0] = (float) (halfW - (((double) px * 0.5) / (double) denomX) * fw);

    int h0 = paintcanvas_ext_gsp_getheight(this);
    double fh = (double) paintcanvas_ext_gsp_signedtofloat(h0, 0);
    int h1 = paintcanvas_ext_gsp_getheight(this);
    double halfH = (double) paintcanvas_ext_gsp_signedtofloat(h1 >> 1, 0);
    outVec[1] = (float) (halfH + termY * fh);

    PCCameraView *cam2 = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
    if (outVec[2] <= cam2->param1) {
        float fy = outVec[1];
        if (fy >= 0.0f) {
            float fx = outVec[0];
            if (fx >= 0.0f) {
                int ww = paintcanvas_ext_gsp_getwidth(this);
                float fww = paintcanvas_ext_gsp_signedtofloat(ww, 0);
                if (fx < fww) {
                    int hh = paintcanvas_ext_gsp_getheight(this);
                    paintcanvas_ext_gsp_signedtofloat(hh, 0);
                }
            }
        }
    }
}


void PaintCanvas::SpriteSystemCreate(unsigned short resId,
                                     bool flag, unsigned int &out) {
    void *ss = 0;
    unsigned int result;
    int ok = paintcanvas_ext_sscreate(this->engine, resId, flag, &ss);
    if (ok == 1) {
        unsigned int i;
        for (i = 0; i < this->spriteSystems.count; i++) {
            void **slot = (void **) &this->spriteSystems.data_[i * 4];
            if (*slot == nullptr) {
                *slot = ss;
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
    char *obj = (char *) paintcanvas_ext_alloc(0x74);
    paintcanvas_ext_material_ctor(obj);
    (void) p4;
    PCMaterialView *mat = (PCMaterialView *) obj;
    mat->textureSlots[0] = textures;
    mat->flags0 = mode;
    paintcanvas_ext_material_add(obj, &this->materials);
    out = this->materials.count - 1;
}


static const double g_cisvf_gravscale_7bcd8 = 0;

int PaintCanvas::CameraIsSphereinViewFrustum(const AbyssEngine::AEMath::Vector &point, float radius) {
    if (radius == 0.0f ||
        this->cameras.count <= this->currentCamera) {
        return 1;
    }

    float m[16];
    memset(m, 0, sizeof(m));
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[14] = 1.0f;
    char scratch[60];
    paintcanvas_ext_cisvf_matidentity(scratch, m);

    void *grav = paintcanvas_ext_cisvf_getgrav(this->engine);
    double angle = ((PCGravView *) grav)->angle * g_cisvf_gravscale_7bcd8;
    float a = (float) angle;
    int orient = this->gameOrientation;
    float rot = (orient == 1) ? a : -a;
    float s = paintcanvas_ext_cisvf_sinf(rot);
    float c = paintcanvas_ext_cisvf_cosf(rot);
    m[0] = c;
    m[5] = c;
    *(unsigned int *) &m[1] = *(unsigned int *) &s ^ 0x80000000;
    m[4] = s;

    void *cam = ((void **) this->cameras.data_)[this->currentCamera];
    return paintcanvas_ext_cisvf_inner(point, radius, m, cam);
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
    return paintcanvas_ext_shader_anim(this, mesh->animation);
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
    PCTransformView *tf = (PCTransformView *) (this->transforms)[transformIndex];
    PCMeshView *mesh = (PCMeshView *) (this->meshes)[meshIndex];
    ArrayAdd<AbyssEngine::Mesh *>((AbyssEngine::Mesh *) mesh->sub0,
                                  *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&tf->meshCount));
    paintcanvas_ext_tami_bsphere_merge(&tf->bsphere, &mesh->bounds3c);

    PCTransformView *res = (PCTransformView *) mesh->materialRes;
    if (res != 0) {
        long long resLen = res->animLength;
        long long tfLen = tf->animLength;
        if (tfLen < resLen) {
            paintcanvas_ext_tami_setanimlen(tf, (int) (resLen >> 32), (int) resLen);
            tf = (PCTransformView *) (this->transforms)[transformIndex];
        }
        long long tfStart = tf->animStart;
        PCTransformView *res2 = (PCTransformView *) mesh->materialRes;
        long long resStart = res2->animStart;
        if (tfStart == 0 || resStart < tfStart) {
            tf->animStart = resStart;
        }
        paintcanvas_ext_tami_setanimstate(tf, 2, 0);
    }
    paintcanvas_ext_tami_finalize((this->transforms)[transformIndex]);
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


static const double g_cipvf_gravscale_7bba8 = 0;

int PaintCanvas::CameraIsPointinViewFrustum(const AbyssEngine::AEMath::Vector &point) {
    if (this->currentCamera >= this->cameras.count) {
        return 1;
    }

    float m[16];
    memset(m, 0, sizeof(m));
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[14] = 1.0f;
    char scratch[60];
    paintcanvas_ext_cipvf_matidentity(scratch, m);

    void *grav = paintcanvas_ext_cipvf_getgrav(this->engine);
    double angle = ((PCGravView *) grav)->angle * g_cipvf_gravscale_7bba8;
    float a = (float) angle;
    int orient = this->gameOrientation;
    float rot = (orient == 1) ? a : -a;
    float s = paintcanvas_ext_cipvf_sinf(rot);
    float c = paintcanvas_ext_cipvf_cosf(rot);
    m[0] = c;
    m[5] = c;
    *(unsigned int *) &m[1] = *(unsigned int *) &s ^ 0x80000000;
    m[4] = s;

    void *cam = ((void **) this->cameras.data_)[this->currentCamera];
    return paintcanvas_ext_cipvf_inner(point, m, cam);
}

void PaintCanvas::SetTexture(unsigned int, unsigned int) {
    return paintcanvas_ext_set_texture(this->engine);
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
    PCResourceView *res = (PCResourceView *) paintcanvas_ext_i2d_findres(this, resId);
    if (res == 0) {
        return;
    }
    unsigned short *info = (unsigned short *) res->payload;
    PCResourceView *texres = (PCResourceView *) paintcanvas_ext_i2d_findres(this, *info);
    if (texres == 0) {
        return;
    }
    if (texres->handle == -1) {
        paintcanvas_ext_i2d_texcreate(this, *info, true);
    }
    unsigned int idx = (unsigned int) res->handle;
    if (idx == 0xffffffff) {
        PCImage2DView *img = (PCImage2DView *) operator new(0x18);
        memset(img, 0, 0x18);
        char *texpath = *(char **) texres->payload;
        int ok = paintcanvas_ext_i2d_imgregion(this->engine, texpath,
                                               info[1], img);
        if (ok != 1) {
            return;
        }
        if (texres->handle != -1) {
            img->textureId = (uint32_t) texres->handle;
        }
        PCArrayAdd<AbyssEngine::Image2D *>((AbyssEngine::Image2D *) img, &this->images);
        idx = this->images.count - 1;
        res->handle = (int) idx;
    }
    out = idx;
}


static const double g_gsp2_gravscale_8bfa8 = 0;

void PaintCanvas::GetScreenPosition(AbyssEngine::AEMath::Matrix &m,
                                    const AbyssEngine::AEMath::Vector &worldPos,
                                    AbyssEngine::AEMath::Vector &outVec) {
    (void) m;

    char transformed[16];
    paintcanvas_ext_gsp2_transformvec(transformed, &worldPos);

    if (this->currentCamera >= this->cameras.count) {
        return;
    }

    char invMat[60];
    PCCameraView *cam = (PCCameraView *) ((void **) this->cameras.data_)[this->currentCamera];
    if (this->initialized == 0) {
        paintcanvas_ext_gsp2_invtransformvec(invMat, cam->localMatrix);
        paintcanvas_ext_gsp2_vec_assign(&outVec, invMat);
    } else {
        float m[16];
        char scratch[60];
        memset(m, 0, sizeof(m));
        m[0] = 1.0f;
        m[5] = 1.0f;
        m[14] = 1.0f;
        paintcanvas_ext_gsp2_matidentity(scratch, m);

        void *grav = paintcanvas_ext_gsp2_getgrav(this->engine);
        double angle = ((PCGravView *) grav)->angle * g_gsp2_gravscale_8bfa8;
        float a = (float) angle;
        int orient = this->gameOrientation;
        float rot = (orient == 1) ? a : -a;
        float s = paintcanvas_ext_gsp2_sinf(rot);
        float c = paintcanvas_ext_gsp2_cosf(rot);
        m[0] = c;
        m[5] = c;
        *(unsigned int *) &m[1] = *(unsigned int *) &s ^ 0x80000000;
        m[4] = s;

        paintcanvas_ext_gsp2_memcpy(scratch, cam->localMatrix, 0x3c);
        paintcanvas_ext_gsp2_mtx_muleq(scratch, m);
        paintcanvas_ext_gsp2_invtransformvec(invMat, scratch);
        paintcanvas_ext_gsp2_vec_assign(&outVec, invMat);
    }

    float z = outVec[2];
    PCCameraView *cam2 = (PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera];
    if (z > cam2->param1) {
        return;
    }
    float denomX = z * cam2->factor4c;
    if (denomX == 0.0f) {
        return;
    }
    float denomY = z * cam2->factor48;
    if (denomY == 0.0f) {
        return;
    }

    float px = outVec[0];
    int w0 = paintcanvas_ext_gsp2_getwidth(this);
    int w1 = paintcanvas_ext_gsp2_getwidth(this);
    float py = outVec[1];
    double fw = (double) paintcanvas_ext_gsp2_signedtofloat(w0, 0);
    double halfW = (double) paintcanvas_ext_gsp2_signedtofloat(w1 >> 1, 0);
    outVec[0] = (float) (halfW - (((double) px * 0.5) / (double) denomX) * fw);

    int h0 = paintcanvas_ext_gsp2_getheight(this);
    double fh = (double) paintcanvas_ext_gsp2_signedtofloat(h0, 0);
    int h1 = paintcanvas_ext_gsp2_getheight(this);
    double halfH = (double) paintcanvas_ext_gsp2_signedtofloat(h1 >> 1, 0);
    outVec[1] = (float) (halfH + (((double) py * 0.5) / (double) denomY) * fh);

    float nx = outVec[0];
    if (nx >= 0.0f) {
        float ny = outVec[1];
        if (ny >= 0.0f) {
            int ww = paintcanvas_ext_gsp2_getwidth(this);
            float fww = paintcanvas_ext_gsp2_signedtofloat(ww, 0);
            if (nx < fww) {
                int hh = paintcanvas_ext_gsp2_getheight(this);
                paintcanvas_ext_gsp2_signedtofloat(hh, 0);
            }
        }
    }
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
        void *t = (this->transforms)[index];
        return paintcanvas_ext_transform_tricount(this, t);
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
            paintcanvas_ext_di_restore(img->restoreFlag, img);
            img = (PCImage2DView *) (this->images.data_)[index];
        }
        paintcanvas_ext_di_settexture(this, img->textureId, -1);

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

        paintcanvas_ext_di_setwvm(this, abuf);
        paintcanvas_ext_di_gldisable(0xb44);
        paintcanvas_ext_di_meshdraw(this->engine,
                                    ((PCImage2DView *) (this->images.data_)[index])->mesh);
        paintcanvas_ext_di_glenable(0xb44);
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

void PaintCanvas::ClearBuffer(unsigned int mask) {
    paintcanvas_ext_enable(0xb71);
    paintcanvas_ext_depthmask(1);
    return paintcanvas_ext_clear2(this->engine, mask);
}

void PaintCanvas::ClearDepth() {
    ClearBuffer(0x100);
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

int paintcanvas_ext_tc_texfromfile(void *eng, char *path,
                                              void (*cb)(AbyssEngine::Image *, void *), void *ud,
                                              unsigned int *out, bool b, float f);

int paintcanvas_ext_tc_texfromfileintern(void *eng, char *path,
                                                    void (*cb)(AbyssEngine::Image *, void *), void *ud,
                                                    unsigned int *out, float f, void *lt, bool b);

void PaintCanvas::TextureCreate(unsigned short resId, void (*callback)(AbyssEngine::Image *, void *),
                                void *userData, unsigned int &out, bool useCallbackLoader) {
    Engine *eng = (Engine *) this->engine;
    eng->boundTextures[0] = -1;
    eng->boundTextures[1] = -1;

    PCResourceView *res = (PCResourceView *) paintcanvas_ext_tc_findres(this, resId);
    if (res != 0) {
        unsigned int idx = (unsigned int) res->handle;
        if (idx == 0xffffffff) {
            PCTexInfoView *info = (PCTexInfoView *) res->payload;
            float f = (float) (int) info->param;
            char *path = info->path;
            int ok;
            if (!useCallbackLoader) {
                ok = paintcanvas_ext_tc_texfromfileintern(this->engine, path,
                                                          callback, userData, &idx, f, 0, false);
            } else {
                ok = paintcanvas_ext_tc_texfromfile(this->engine, path,
                                                    callback, userData, &idx, true, f);
            }
            if (ok != 1) {
                return;
            }
            idx = 0;
            res->handle = 0;
        }
        out = idx;
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

void paintcanvas_ext_dt_drawmesh(void *self, void *mesh, void *m, void *m2,
                                            unsigned int flag, void *m3);

void PaintCanvas::DrawTransform(Transform *tf, const AbyssEngine::AEMath::Matrix &m2,
                                AbyssEngine::AEMath::Matrix &m3) {
    char buf[60];
    if (tf && tf->visible) {
        paintcanvas_ext_mtx_mul(buf, &m2, tf);
        if (tf->keyFrames.size() != 0) {
            paintcanvas_ext_mtx_muleq(buf, tf->rotationMatrix);
        }
        for (unsigned int i = 0; i < tf->meshes.size(); i++) {
            paintcanvas_ext_dt_drawmesh(this, tf->meshes[i], buf, &m3, tf->id, tf->localMatrix);
        }
        for (unsigned int i = 0; i < tf->children.size(); i++) {
            if (this->currentCamera < this->cameras.count &&
                paintcanvas_ext_dt_incamvf(tf->children[i], buf,
                                           (this->cameras.data_)[this->currentCamera])) {
                paintcanvas_ext_dt_drawtransform_rec(this, tf->children[i], buf, &m3);
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
        paintcanvas_ext_di4_restore(img->restoreFlag, img);
        img = (PCImage2DView *) ((char **) this->images.data_)[index];
    }
    paintcanvas_ext_di4_settexture(this, img->textureId);

    int baseX = x;
    int hOff;
    if ((alignFlags & 7) == 4) {
        hOff = paintcanvas_ext_di4_getwidth(this) >> 1;
    } else if ((alignFlags & 7) == 2) {
        baseX = -x;
        hOff = paintcanvas_ext_di4_getwidth(this);
    } else {
        hOff = 0;
    }

    int spanW = w;
    if (alignFlags & 8) {
        spanW = paintcanvas_ext_di4_getwidth(this) - (w + x);
    }
    float fSpanW = paintcanvas_ext_di4_signedtofloat(spanW, 0);
    PCRegionView *region = (PCRegionView *)
        ((PCMeshView *) ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh)->positions;
    float regW = region->width;

    int spanH = h;
    if (alignFlags & 0x80) {
        spanH = paintcanvas_ext_di4_getheight(this) - (h + y);
        region = (PCRegionView *)
            ((PCMeshView *) ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh)->positions;
    }
    float fSpanH = paintcanvas_ext_di4_signedtofloat(spanH, 0);
    float regH = region->height;

    int baseY = y;
    int vOff;
    if ((alignFlags & 0x70) == 0x40) {
        vOff = paintcanvas_ext_di4_getheight(this) >> 1;
    } else if ((alignFlags & 0x70) == 0x20) {
        vOff = paintcanvas_ext_di4_getheight(this);
        baseY = -y;
    } else {
        vOff = 0;
    }

    int anchorX;
    if ((anchorFlags & 7) == 4) {
        if ((alignFlags & 8) == 0) {
            anchorX = -(w >> 1);
        } else {
            anchorX = ((w + x) - paintcanvas_ext_di4_getwidth(this)) >> 1;
        }
    } else if ((anchorFlags & 7) == 2) {
        if ((alignFlags & 8) == 0) {
            anchorX = -w;
        } else {
            anchorX = (w + x) - paintcanvas_ext_di4_getwidth(this);
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
            anchorY = ((h + y) - paintcanvas_ext_di4_getheight(this)) >> 1;
        }
    } else if ((anchorFlags & 0x70) == 0x20) {
        if ((alignFlags & 0x80) == 0) {
            anchorY = -h;
        } else {
            anchorY = (h + y) - paintcanvas_ext_di4_getheight(this);
        }
    } else {
        anchorY = 0;
    }

    float m[16];
    memset(m, 0, sizeof(m));
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
    m[3] = paintcanvas_ext_di4_signedtofloat(baseX + hOff + anchorX, 0);
    m[7] = paintcanvas_ext_di4_signedtofloat(anchorY + vOff + baseY, 0);
    m[0] = scaleX;

    if (flipFlags & 1) {
        m[0] = -scaleX;
        m[3] = paintcanvas_ext_di4_signedtofloat(w, 0) + m[3];
    }
    m[5] = scaleY;
    if (flipFlags & 2) {
        m[5] = -scaleY;
        m[7] = paintcanvas_ext_di4_signedtofloat(h, 0) + m[7];
    }

    paintcanvas_ext_di4_setwvm(this, m);
    paintcanvas_ext_di4_gldisable(0xb44);
    paintcanvas_ext_di4_meshdraw(this->engine,
                                 ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh);
    paintcanvas_ext_di4_glenable(0xb44);
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


static const unsigned int g_sgo_const_8e6b4 = 0;

static const unsigned int g_sgo_const_8e6b8 = 0;

static void zero16(void *p) {
    memset(p, 0, 0x10);
}

void PaintCanvas::SetGameOrientation(AbyssEngine::LandscapeMode orientation) {
    if (this->gameOrientation == orientation) {
        return;
    }
    this->gameOrientation = orientation;
    paintcanvas_ext_sgo_setorientation(this->engine, orientation);

    this->projMatrix3d.m[1] = -this->projMatrix3d.m[1];
    this->projMatrix3d.m[4] = -this->projMatrix3d.m[4];

    int w = paintcanvas_ext_sgo_dispwidth(this->engine);
    float fw = paintcanvas_ext_sgo_signedtofloat(w, 0);
    int h = paintcanvas_ext_sgo_dispheight(this->engine);
    float fh = paintcanvas_ext_sgo_signedtofloat(h, 0);

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
        this->worldViewMatrix.m[0] = g_sgo_const_8e6b8;
        this->worldViewMatrix.m[1] = g_sgo_const_8e6b8;
        this->projOrthoMatrix.m[10] = g_sgo_const_8e6b4;
        this->projOrthoMatrix.m[12] = g_sgo_const_8e6b8;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = 1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fw;
        this->projOrthoMatrix.m[5] = -(2.0f / fh);
        this->worldViewMatrix.m[15] = 1.0f;
        int w2 = paintcanvas_ext_sgo_dispwidth(this->engine);
        this->worldViewMatrix.m[12] = paintcanvas_ext_sgo_signedtofloat(w2, 0);
        int h2 = paintcanvas_ext_sgo_dispheight(this->engine);
        this->worldViewMatrix.m[13] = paintcanvas_ext_sgo_signedtofloat(h2, 0);
    } else if (orientation == 1) {
        zero16(&this->projOrthoMatrix.m[9]);
        zero16(&this->worldViewMatrix.m[8]);
        zero16(&this->worldViewMatrix.m[4]);
        zero16(&this->projOrthoMatrix.m[5]);
        zero16(&this->projOrthoMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[11]);
        zero16(&this->worldViewMatrix.m[1]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[1] = 1.0f;
        this->projOrthoMatrix.m[10] = g_sgo_const_8e6b4;
        this->projOrthoMatrix.m[12] = g_sgo_const_8e6b8;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = g_sgo_const_8e6b8;
        this->projOrthoMatrix.m[0] = 2.0f / fh;
        this->projOrthoMatrix.m[5] = -(2.0f / fw);
        this->worldViewMatrix.m[15] = 1.0f;
        int h2 = paintcanvas_ext_sgo_dispheight(this->engine);
        this->worldViewMatrix.m[12] = paintcanvas_ext_sgo_signedtofloat(h2, 0);
    } else if (orientation != 0) {
        zero16(&this->projOrthoMatrix.m[9]);
        zero16(&this->worldViewMatrix.m[8]);
        zero16(&this->worldViewMatrix.m[4]);
        zero16(&this->projOrthoMatrix.m[5]);
        zero16(&this->projOrthoMatrix.m[1]);
        zero16(&this->worldViewMatrix.m[1]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->worldViewMatrix.m[13] = 0;
        this->worldViewMatrix.m[14] = 0;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[0] = 1.0f;
        this->projOrthoMatrix.m[10] = g_sgo_const_8e6b4;
        this->projOrthoMatrix.m[12] = g_sgo_const_8e6b8;
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
        zero16(&this->worldViewMatrix.m[1]);
        this->projOrthoMatrix.m[14] = 0;
        this->projOrthoMatrix.m[13] = 1.0f;
        this->projOrthoMatrix.m[15] = 1.0f;
        this->worldViewMatrix.m[1] = g_sgo_const_8e6b8;
        this->projOrthoMatrix.m[10] = g_sgo_const_8e6b4;
        this->projOrthoMatrix.m[12] = g_sgo_const_8e6b8;
        this->worldViewMatrix.m[10] = 1.0f;
        this->worldViewMatrix.m[4] = 1.0f;
        this->projOrthoMatrix.m[0] = 2.0f / fh;
        this->projOrthoMatrix.m[5] = -(2.0f / fw);
        this->worldViewMatrix.m[15] = 1.0f;
        int w2 = paintcanvas_ext_sgo_dispwidth(this->engine);
        this->worldViewMatrix.m[13] = paintcanvas_ext_sgo_signedtofloat(w2, 0);
    }

    if (this->currentCamera == -1) {
        return;
    }
    float *cam = ((float **) this->cameras.data_)[this->currentCamera];
    paintcanvas_ext_sgo_setpersp(this, cam[0], cam[1], cam[2]);
}

int paintcanvas_ext_mc_meshcreate(void *eng, unsigned short a, unsigned short b,
                                             signed char c, void **out);


void PaintCanvas::MeshCreate(unsigned short vertexCount, unsigned short triangleCount,
                             signed char meshType, unsigned short matResId, unsigned int &out) {
    int result = -1;
    unsigned int mat = 0xffffffff;
    void *mesh = 0;
    paintcanvas_ext_mc_matcreate(this, matResId, &mat);
    int ok = paintcanvas_ext_mc_meshcreate(this->engine, vertexCount, triangleCount,
                                           meshType, &mesh);
    if (ok == 1) {
        if (0xfffffffe < this->materials.count) {
            void *m = this->materials.data_[-4];
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


static const float g_dm_255_8ee80 = 0;

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
    AbyssEngine::AEMath::Matrix worldM;
    AbyssEngine::AEMath::Matrix uvM;
    paintcanvas_ext_dm_memcpy(&worldM, worldMatrix, 0x3c);
    paintcanvas_ext_dm_memcpy(&uvM, uvMatrix, 0x3c);

    unsigned int meshColor = color;

    if (mesh->animation != 0) {
        Transform *resTf = mesh->animation;
        float a[16], b[16];
        memset(a, 0, sizeof(a));
        a[0] = 1.0f;
        a[5] = 1.0f;
        a[14] = 1.0f;
        memset(b, 0, sizeof(b));
        b[0] = 1.0f;
        b[5] = 1.0f;
        b[14] = 1.0f;

        float pivotM[16];
        paintcanvas_ext_dm_settrans(pivotM, mesh->pivotY);
        paintcanvas_ext_dm_settrans(pivotM, -mesh->pivotY);
        memset(pivotM, 0, sizeof(pivotM));
        pivotM[0] = 1.0f;
        pivotM[5] = 1.0f;
        pivotM[14] = 1.0f;

        float posM[16];
        float resLocal[16];
        paintcanvas_ext_dm_getpos(posM);
        paintcanvas_ext_dm_settrans_vec(resLocal, pivotM);
        paintcanvas_ext_dm_memcpy(resLocal, resTf->rotationMatrix, 0x3c);

        float s1[16], s2[16], s3[16];
        paintcanvas_ext_dm_mtx_mul(s1, worldMatrix, pivotM);
        paintcanvas_ext_dm_mtx_mul(s2, s1, b);
        paintcanvas_ext_dm_mtx_mul(s3, s2, resLocal);
        paintcanvas_ext_dm_mtx_mul(posM, s3, a);
        paintcanvas_ext_dm_mtx_assign(&worldM, posM);

        unsigned int resColor = resTf->id;
        paintcanvas_ext_dm_mtx_assign(&uvM, resTf->localMatrix);
        meshColor = mulColors(resColor, color);
    }

    paintcanvas_ext_dm_transformvec(&uvM, &worldM);

    float ext[3];
    ext[0] = mesh->boundsRadius;
    ext[1] = ext[0];
    ext[2] = ext[0];
    paintcanvas_ext_dm_rotatevec(&uvM, &worldM);
    paintcanvas_ext_dm_vec_assign(ext, &uvM);

    float ax = ext[0] < 0.0f ? -ext[0] : ext[0];
    float ay = ext[1] < 0.0f ? -ext[1] : ext[1];
    float az = ext[2] < 0.0f ? -ext[2] : ext[2];
    float maxxy = ax > ay ? ax : ay;
    float radius = maxxy;

    if (mesh->vertexCount != 0) {
        float r2 = maxxy > az ? maxxy : az;
        int vis = paintcanvas_ext_dm_spherefrustum(this, &uvM, r2 * mesh->boundsRadiusSq);
        if (vis == 0) {
            this->culledCount += 1;
            return;
        }
        if (mesh->vertexCount != 0) {
            if (mesh->material == 0) {
                float fr = paintcanvas_ext_dm_unsignedtofloat(meshColor >> 24, 0);
                float fg = paintcanvas_ext_dm_unsignedtofloat((meshColor >> 16) & 0xff, 0);
                float fb = paintcanvas_ext_dm_unsignedtofloat((meshColor >> 8) & 0xff, 0);
                float fa = paintcanvas_ext_dm_unsignedtofloat(meshColor & 0xff, 0);
                paintcanvas_ext_dm_setcolor(
                    this->engine,
                    (this->colorR * fr) / g_dm_255_8ee80,
                    (this->colorG * fg) / g_dm_255_8ee80,
                    (this->colorB * fb) / g_dm_255_8ee80,
                    (this->colorA * fa) / g_dm_255_8ee80);

                paintcanvas_ext_dm_mtx_muleq(&worldM, &viewMatrix);
                paintcanvas_ext_dm_setwvm(this, &worldM);
                paintcanvas_ext_dm_setmodelmatrix(this->engine);
                paintcanvas_ext_dm_setuvmatrix(this->engine, &uvM);
                paintcanvas_ext_dm_meshdraw(this->engine, mesh);
                paintcanvas_ext_dm_resetuvmatrix(this->engine);
            } else {
                AbyssEngine::Material *res = (AbyssEngine::Material *) mesh->material;
                paintcanvas_ext_dm_addcached_mesh(mesh, &res->meshes);
                paintcanvas_ext_dm_pushmat(worldM, &res->arr_2c);
                paintcanvas_ext_dm_pushmat(uvM, &res->arr_38);
                paintcanvas_ext_dm_pushmat(viewMatrix, &res->arr_5c);
                paintcanvas_ext_dm_addcached_uint(meshColor, &res->arr_50);
            }
        }
    }

    if (mesh->animation != 0) {
        Transform *res = mesh->animation;
        unsigned int n = res->meshes.size();
        while ((int) (--n) >= 0) {
            this->DrawMesh(res->meshes[n], worldM, viewMatrix, color, uvM);
        }
        for (unsigned int i = 0; i < res->children.size(); i++) {
            if (this->currentCamera < this->cameras.count) {
                void *cam = ((void **) this->cameras.data_)[this->currentCamera];
                Transform *tf = res->children[i];
                if (paintcanvas_ext_dm_incamvf(tf, &worldMatrix, cam)) {
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

void paintcanvas_ext_drawstring_raw(void *, const unsigned short *, int, int,
                                               AbyssEngine::PaintCanvas *, void *, bool);

void PaintCanvas::DrawString(unsigned int index, const unsigned short *str,
                             int x, int y, bool b) {
    if (index < this->fonts.count) {
        PCFontView *font = (PCFontView *) (this->fonts.data_)[index];
        paintcanvas_ext_string_prep(this, font->atlas, -1);
        void *font2 = (this->fonts.data_)[index];
        paintcanvas_ext_drawstring_raw(font2, str, x, y, this,
                                       this->engine, b);
    }
}


static const char g_dsc_pipe_882c4[] = "|";

static const char g_dsc_fmt_882ee[] = "%x";

void paintcanvas_ext_dsc_fontdraw(void *font, unsigned short *txt, unsigned int len,
                                             int x, int y, void *self, void *eng, bool b);

void PaintCanvas::DrawStringColor(unsigned int index, const AbyssEngine::String &text,
                                  int x, int y, bool b) {
    if (index >= this->fonts.count) {
        return;
    }
    PCFontView *font0 = (PCFontView *) ((char **) this->fonts.data_)[index];
    paintcanvas_ext_dsc_settexture(this, (unsigned int) (uintptr_t) font0->atlas);
    paintcanvas_ext_dsc_getcolor(this);

    char str[16];
    char sep[16];
    paintcanvas_ext_dsc_str_copy(str, &text, false);
    paintcanvas_ext_dsc_str_fromchar(sep, g_dsc_pipe_882c4, false);
    PCSplitArrayView *parts = (PCSplitArrayView *) paintcanvas_ext_dsc_splittags(str, sep);
    paintcanvas_ext_dsc_str_dtor(sep);

    if (parts != 0) {
        bool draw = true;
        for (unsigned int i = 0; i < parts->count; i++) {
            char **data = parts->data;
            char *part = data[i];
            if (draw) {
                void *font = ((char **) this->fonts.data_)[index];
                unsigned short *txt = paintcanvas_ext_dsc_str_cast(part);
                paintcanvas_ext_dsc_fontdraw(font, txt, (unsigned int) ((PCStrPartView *) part)->length, x,
                                             y, this, this->engine, b);
                x += paintcanvas_ext_dsc_textwidth(this, index, part);
            } else if (((PCStrPartView *) part)->length == 0) {
                paintcanvas_ext_dsc_setcolor(this);
            } else {
                int color = 0;
                char *s = paintcanvas_ext_dsc_getAEChar(part);
                paintcanvas_ext_dsc_sscanf(s, g_dsc_fmt_882ee, &color);
                paintcanvas_ext_dsc_setcolor(this);
            }
            draw = !draw;
        }
        paintcanvas_ext_dsc_setcolor(this);
        paintcanvas_ext_dsc_releaseclasses(parts);
        void *p = paintcanvas_ext_dsc_arr_dtor(parts);
        paintcanvas_ext_dsc_op_delete(p);
    }
    paintcanvas_ext_dsc_str_dtor(str);
}


void PaintCanvas::SetMatForGlow(AbyssEngine::Material *glowSource) {
    for (unsigned int i = 0; i < glowSource->meshes.size_; i++) {
        ArrayAdd<AbyssEngine::Mesh *>(glowSource->meshes.data_[i],
                                      *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->glowMeshes_count));

        paintcanvas_ext_smfg_pushmat(glowSource->arr_2c.data_[i], &this->glowMatA_count);
        paintcanvas_ext_smfg_pushmat(glowSource->arr_38.data_[i], &this->glowMatB_count);

        PCArrayAdd<unsigned int>(glowSource->arr_50.data_[i], &this->glowUints_count);

        paintcanvas_ext_smfg_pushmat(glowSource->arr_5c.data_[i], &this->glowMatC_count);
    }
}

void PaintCanvas::EnableClip(int x, int y, int w, int h) {
    paintcanvas_ext_ec_glEnable(0xc11);
    int sx, sy, sw, sh;
    switch (this->gameOrientation) {
        case 0:
            sx = x;
            sy = y;
            sw = w;
            sh = h;
            break;
        case 1: {
            int dispH = paintcanvas_ext_ec_getHeight(this);
            int dispW = paintcanvas_ext_ec_getWidth(this);
            sx = dispH - (y + h);
            sy = dispW - (x + w);
            sw = h;
            sh = w;
            break;
        }
        case 2: {
            int dispH = paintcanvas_ext_ec_getHeight(this);
            sx = dispH - (y + h);
            sy = x;
            sw = w;
            sh = h;
            break;
        }
        case 3: {
            int dispW = paintcanvas_ext_ec_getWidth(this);
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
    paintcanvas_ext_ec_glScissor(sx, sy, sw, sh);
}

void PaintCanvas::DrawImage2D(unsigned int index, int x, int y,
                              unsigned char regionAlignFlags, unsigned char placeFlags) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    if (img->restoreFlag != 0) {
        paintcanvas_ext_di3_restore(img->restoreFlag, img);
    }

    int hOff;
    if ((placeFlags & 7) == 4) {
        hOff = paintcanvas_ext_di3_getwidth(this) >> 1;
    } else if ((placeFlags & 7) == 2) {
        hOff = paintcanvas_ext_di3_getwidth(this);
        y = -y;
    } else {
        hOff = 0;
    }

    int vOff;
    unsigned int yShift = regionAlignFlags;
    if ((placeFlags & 0x70) == 0x40) {
        vOff = paintcanvas_ext_di3_getheight(this) >> 1;
    } else if ((placeFlags & 0x70) == 0x20) {
        vOff = paintcanvas_ext_di3_getheight(this);
        yShift = (unsigned int) (-(int) yShift);
    } else {
        vOff = 0;
    }

    int rx;
    PCRegionView *region = (PCRegionView *) img->regionPtr;
    if ((regionAlignFlags & 7) == 4) {
        double w = (double) paintcanvas_ext_di3_signedtofloat((int) region->width, 0);
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
        double h = (double) paintcanvas_ext_di3_signedtofloat((int) region2->height, 0);
        ry = (int) (long long) (h * -0.5);
    } else {
        ry = 0;
    }

    PCImage2DView *img2 = (PCImage2DView *) ((char **) this->images.data_)[index];
    paintcanvas_ext_di3_settexture(this, img2->textureId);

    float fx = paintcanvas_ext_di3_signedtofloat(hOff + y + rx, 0);
    float fy = paintcanvas_ext_di3_signedtofloat(vOff + (int) yShift + ry, 0);

    float m[16];
    memset(m, 0, sizeof(m));
    m[0] = 1.0f;
    m[5] = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
    m[3] = fx;
    m[7] = fy;

    paintcanvas_ext_di3_setwvm(this, m);
    paintcanvas_ext_di3_meshdraw(this->engine,
                                 ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh);
}

void PaintCanvas::MeshCreate(unsigned short a, unsigned short b,
                             signed char c, unsigned int &out) {
    char mesh[4];
    *(void **) mesh = 0;
    int result = paintcanvas_ext_meshcreate(this->engine, mesh);
    if (result == 1) {
        ArrayAdd<AbyssEngine::Mesh *>(*(AbyssEngine::Mesh **) mesh,
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


static char g_meshcreate_vboflag_79d5c_storage = 0;
static char *const g_meshcreate_vboflag_79d5c = &g_meshcreate_vboflag_79d5c_storage;


void PaintCanvas::MeshCreate(unsigned short resId, unsigned int &out,
                             bool forceClone) {
    PCResourceView *res = (PCResourceView *) paintcanvas_ext_mc2_findres(this, resId);
    if (res == 0) {
        return;
    }
    unsigned int idx = (unsigned int) res->handle;
    if (idx == 0xffffffff) {
        PCMeshInfoView *info = (PCMeshInfoView *) res->payload;
        unsigned int mat = 0xffffffff;
        paintcanvas_ext_mc2_matcreate(this, info->matResId, &mat);
        void *matptr = 0;
        if (0xfffffffe < this->materials.count) {
            matptr = this->materials.data_[-4];
        }
        void *mesh = 0;
        int ok = paintcanvas_ext_mc2_meshfromfile(this->engine,
                                                  info->path, &mesh, matptr);
        if (ok != 1) {
            return;
        }
        if (*g_meshcreate_vboflag_79d5c != 0) {
            if (mesh) {
                ((AbyssEngine::Mesh *) mesh)->field_0x84 = 1;
            }
            paintcanvas_ext_mc2_converttovbo(mesh);
        }
        ArrayAdd<AbyssEngine::Mesh *>((AbyssEngine::Mesh *) mesh,
                                      *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->meshCount));
        idx = this->meshCount - 1;
        res->handle = (int) idx;
    } else {
        char **meshes = this->meshes;
        PCMeshView *existing = (PCMeshView *) meshes[idx];
        if (existing->materialRes != 0 || forceClone) {
            void *clone = paintcanvas_ext_mc2_new_mesh_copy(
                ((void **) meshes)[(unsigned int) res->handle]);
            ArrayAdd<AbyssEngine::Mesh *>((AbyssEngine::Mesh *) clone,
                                          *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&this->meshCount));
            idx = this->meshCount - 1;
        }
    }
    out = idx;
}


static char g_fsp_flag_8cf40_storage = 0;
static char *const g_fsp_flag_8cf40 = &g_fsp_flag_8cf40_storage;

static const double g_fsp_255d_8d070 = 0;

static const float g_fsp_255f_8d078 = 0;

void PaintCanvas::FogSetParameter(AbyssEngine::FogMode mode, float fogStart, float fogEnd,
                                  float fogDensity, unsigned int color) {
    float col[4];
    if (*g_fsp_flag_8cf40 == 0) {
        float modeF = paintcanvas_ext_fsp_unsignedtofloat((unsigned int) mode, 0);
        paintcanvas_ext_fsp_glFogf(0xb65, modeF);
        paintcanvas_ext_fsp_glFogf(0xb62, fogDensity);
        paintcanvas_ext_fsp_glFogf(0xb63, fogStart);
        paintcanvas_ext_fsp_glFogf(0xb64, fogEnd);

        float r = paintcanvas_ext_fsp_unsignedtofloat((color >> 16) & 0xff, 0);
        float g = paintcanvas_ext_fsp_unsignedtofloat((color >> 8) & 0xff, 0);
        paintcanvas_ext_fsp_unsignedtofloat(color >> 24, 0);
        float b = paintcanvas_ext_fsp_unsignedtofloat(color & 0xff, 0);
        col[0] = r / g_fsp_255f_8d078;
        col[1] = g / g_fsp_255f_8d078;
        col[2] = b / g_fsp_255f_8d078;
        col[3] = fogStart;
        paintcanvas_ext_fsp_glFogfv(0xb66, col);
    } else {
        double r = (double) paintcanvas_ext_fsp_unsignedtofloat((color >> 16) & 0xff, 0);
        r = r / g_fsp_255d_8d070;
        paintcanvas_ext_fsp_unsignedtofloat(color >> 24, 0);
        double g = (double) paintcanvas_ext_fsp_unsignedtofloat((color >> 8) & 0xff, 0);
        Engine *eng = (Engine *) this->engine;
        g = g / g_fsp_255d_8d070;
        eng->fogMinDist = fogStart;
        eng->fogMaxDist = fogEnd;
        col[0] = (float) r;
        col[1] = (float) g;
        paintcanvas_ext_fsp_vec_assign(&eng->fogColor, col);
    }
}


static const unsigned int g_dr3_const_88808 = 0;

void PaintCanvas::DrawRegion2D(unsigned int index, int srcX, int srcY,
                               int destW, int destH, float /*unused*/, int transY, int pivotX, int pivotY,
                               int transX) {
    if (index >= this->images.count) {
        return;
    }
    PCImage2DView *img = (PCImage2DView *) ((char **) this->images.data_)[index];
    paintcanvas_ext_dr3_settexture(this, img->textureId);
    PCImage2DView *mesh = (PCImage2DView *) ((char **) this->images.data_)[index];
    PCMeshView *meshObj = (PCMeshView *) mesh->mesh;

    mesh->restoreFlag = 1;

    float fw = paintcanvas_ext_dr3_signedtofloat(destW, 0);
    float fh = paintcanvas_ext_dr3_signedtofloat(destH, 0);
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

    float u0 = paintcanvas_ext_dr3_unsignedtofloat(mesh->regionB, 0);
    float w = paintcanvas_ext_dr3_unsignedtofloat(mesh->regionA, 0);
    float h = paintcanvas_ext_dr3_unsignedtofloat(mesh->u0, 0);
    float v0 = paintcanvas_ext_dr3_unsignedtofloat(mesh->v0, 0);
    float a2 = paintcanvas_ext_dr3_signedtofloat(srcX, 0);
    float a3 = paintcanvas_ext_dr3_signedtofloat(srcY, 0);
    float a2s = paintcanvas_ext_dr3_signedtofloat((short) srcX, 0);

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
    blend[2] = g_dr3_const_88808;

    float pivotM[16];
    memset(pivotM, 0, sizeof(pivotM));
    pivotM[0] = 1.0f;
    pivotM[5] = 1.0f;
    pivotM[10] = 1.0f;
    pivotM[15] = 1.0f;
    pivotM[3] = paintcanvas_ext_dr3_signedtofloat(-pivotX, 0);
    pivotM[7] = paintcanvas_ext_dr3_signedtofloat(-pivotY, 0);

    float invPivotM[16];
    memset(invPivotM, 0, sizeof(invPivotM));
    invPivotM[0] = 1.0f;
    invPivotM[5] = 1.0f;
    invPivotM[10] = 1.0f;
    invPivotM[15] = 1.0f;
    invPivotM[3] = paintcanvas_ext_dr3_signedtofloat(transX + pivotX, 0);
    invPivotM[7] = paintcanvas_ext_dr3_signedtofloat(transY + pivotY, 0);

    float rotAng = invPivotM[7];
    float s = paintcanvas_ext_dr3_sinf(rotAng);
    float c = paintcanvas_ext_dr3_cosf(rotAng);
    float rotM[16];
    memset(rotM, 0, sizeof(rotM));
    rotM[0] = c;
    rotM[5] = c;
    rotM[10] = 1.0f;
    rotM[15] = 1.0f;
    *(unsigned int *) &rotM[1] = *(unsigned int *) &s ^ 0x80000000;
    rotM[4] = s;

    float composed[16];
    float scratch[16];
    paintcanvas_ext_dr3_mtx_mul(scratch, rotM);
    paintcanvas_ext_dr3_mtx_assign(composed, scratch);
    paintcanvas_ext_dr3_mtx_mul(scratch, pivotM);
    paintcanvas_ext_dr3_mtx_assign(composed, scratch);

    paintcanvas_ext_dr3_setwvm(this, composed);
    paintcanvas_ext_dr3_gldisable(0xb44);
    paintcanvas_ext_dr3_meshdraw(this->engine,
                                 ((PCImage2DView *) ((char **) this->images.data_)[index])->mesh);
    paintcanvas_ext_dr3_glenable(0xb44);
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
        void *font = (this->fonts.data_)[index];
        void *data = paintcanvas_ext_str_text(&str);
        return paintcanvas_ext_check_string(font, (unsigned int) (uintptr_t) data, str.size());
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
    PCResourceView *res = (PCResourceView *) paintcanvas_ext_matc_findres(this, resId);
    if (res == 0) {
        return;
    }
    unsigned int idx = (unsigned int) res->handle;
    if (idx == 0xffffffff) {
        PCMaterialInfoView *info = (PCMaterialInfoView *) res->payload;
        PCMaterialView *mat = (PCMaterialView *) paintcanvas_ext_matc_new_material();
        for (unsigned int i = 0; i < 8; i++) {
            unsigned short tid = info->textureIds[i];
            if (tid != 0xffff) {
                PCResourceView *tres = (PCResourceView *) paintcanvas_ext_matc_findres(this, tid);
                if (tres == 0) {
                    break;
                }
                int t = tres->handle;
                if (t == -1) {
                    paintcanvas_ext_matc_texcreate(this, info->textureIds[i], true);
                    t = tres->handle;
                }
                mat->textureSlots[i] = (uint32_t) t;
            }
        }
        mat->flags0 = info->flags0;
        mat->flags1 = info->flags1;
        mat->flags2 = info->flags2;
        paintcanvas_ext_matc_vec_assign(mat->vec, info->vec);
        PCArrayAdd<AbyssEngine::Material *>((AbyssEngine::Material *) mat, &this->materials);
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


static const double g_dt_gravscale_898d8 = 0;

void PaintCanvas::DrawTransform(unsigned int index, const AbyssEngine::AEMath::Matrix *viewMatrix) {
    if (index >= this->transformCount) {
        return;
    }
    char *tf = ((char **) this->transforms)[index];
    if (((PCTransformView *) tf)->visible == 0) {
        return;
    }

    float worldM[16];
    memset(worldM, 0, sizeof(worldM));
    worldM[0] = 1.0f;
    worldM[5] = 1.0f;
    worldM[14] = 1.0f;

    if (this->currentCamera < this->cameras.count) {
        void *cam = ((void **) this->cameras.data_)[this->currentCamera];
        if (this->initialized == 0) {
            int vis = paintcanvas_ext_dt2_incamvf(((void **) this->transforms)[index], 0, cam);
            if (vis == 0) {
                this->culledCount += 1;
                return;
            }
            float inv[16];
            paintcanvas_ext_dt2_mtx_getinv(inv, worldM);
            paintcanvas_ext_dt2_mtx_assign(worldM, inv);
        } else {
            float rotM[16];
            char scratch[60];
            memset(rotM, 0, sizeof(rotM));
            rotM[0] = 1.0f;
            rotM[5] = 1.0f;
            rotM[14] = 1.0f;
            paintcanvas_ext_dt2_matidentity(scratch, rotM);

            void *grav = paintcanvas_ext_dt2_getgrav(this->engine);
            double angle = ((PCGravView *) grav)->angle * g_dt_gravscale_898d8;
            float a = (float) angle;
            int orient = this->gameOrientation;
            float ang = (orient == 1) ? a : -a;
            float s = paintcanvas_ext_dt2_sinf(ang);
            float c = paintcanvas_ext_dt2_cosf(ang);
            rotM[0] = c;
            rotM[5] = c;
            *(unsigned int *) &rotM[1] = *(unsigned int *) &s ^ 0x80000000;
            rotM[4] = s;

            int vis = paintcanvas_ext_dt2_incamvf(((void **) this->transforms)[index], rotM, cam);
            if (vis == 0) {
                this->culledCount += 1;
                return;
            }

            float viewM[16];
            const float *src;
            if (viewMatrix == 0) {
                src = ((PCCameraView *) ((char **) this->cameras.data_)[this->currentCamera])->localMatrix;
            } else {
                src = *viewMatrix;
            }
            paintcanvas_ext_dt2_mtx_assign(viewM, src);
            paintcanvas_ext_dt2_mtx_muleq(viewM, rotM);
            paintcanvas_ext_dt2_mtx_getinv(scratch, viewM);
            paintcanvas_ext_dt2_mtx_assign(worldM, scratch);
        }
        PCCameraEyeView *cam2 = (PCCameraEyeView *) ((void **) this->cameras.data_)[this->currentCamera];
        paintcanvas_ext_dt2_seteye(this->engine,
                                   cam2->eyeX,
                                   cam2->eyeY,
                                   cam2->eyeZ);
        tf = ((char **) this->transforms)[index];
    }

    float ident[16];
    memset(ident, 0, sizeof(ident));
    ident[0] = 1.0f;
    ident[5] = 1.0f;
    ident[14] = 1.0f;
    paintcanvas_ext_dt2_drawrec(this, tf, ident, worldM);
}

void PaintCanvas::FontSetSpacing(unsigned int index, short spacing) {
    if (index < this->fonts.count) {
        void *font = (this->fonts.data_)[index];
        return paintcanvas_ext_font_set_spacing(font, spacing);
    }
}

int PaintCanvas::GetWidth() {
    return paintcanvas_ext_get_width(this->engine);
}

void PaintCanvas::CameraCreate(unsigned int &out) {
    void *cam = operator new(0x5c);
    int w = pc_GetWidth(this);
    int h = pc_GetHeight(this);
    pc_Camera_ctor(cam, (float) h, (float) w);
    pc_ArrayAdd_Camera(cam, &this->cameras);
    out = this->cameras.count - 1;
}


static const double g_dss_gravscale_8ada0 = 0;

void PaintCanvas::DrawSpriteSystem(unsigned int index, AbyssEngine::AEMath::Matrix mat) {
    if (index >= this->spriteSystems.count) {
        return;
    }
    void *ss = ((void **) this->spriteSystems.data_)[index];
    if (ss == 0) {
        return;
    }

    float local[15];
    memcpy(local, mat, sizeof(local));

    float identbuf[16];
    char scratch[60];
    float inv[16];

    if (this->initialized == 0) {
        paintcanvas_ext_dss_mtx_getinv(inv, local);
        paintcanvas_ext_dss_mtx_assign(local, inv);
    } else {
        memset(identbuf, 0, sizeof(identbuf));
        identbuf[0] = 1.0f;
        identbuf[5] = 1.0f;
        identbuf[14] = 1.0f;
        paintcanvas_ext_dss_matidentity(scratch, identbuf);

        void *grav = paintcanvas_ext_dss_getgrav(this->engine);
        double angle = ((PCGravView *) grav)->angle * g_dss_gravscale_8ada0;
        float a = (float) angle;
        int orient = this->gameOrientation;
        float rot = (orient == 1) ? a : -a;
        float s = paintcanvas_ext_dss_sinf(rot);
        float c = paintcanvas_ext_dss_cosf(rot);
        identbuf[0] = c;
        identbuf[5] = c;
        *(unsigned int *) &identbuf[1] = *(unsigned int *) &s ^ 0x80000000;
        identbuf[4] = s;

        paintcanvas_ext_dss_mtx_muleq(local, identbuf);
        paintcanvas_ext_dss_mtx_getinv(scratch, local);
        paintcanvas_ext_dss_mtx_assign(local, scratch);
    }

    float ident2[16];
    memset(ident2, 0, sizeof(ident2));
    ident2[0] = 1.0f;
    ident2[5] = 1.0f;
    ident2[14] = 1.0f;
    paintcanvas_ext_dss_ssdraw(this->engine, ident2, local,
                               ((void **) this->spriteSystems.data_)[index]);
}

void paintcanvas_ext_drawstring_str(void *, unsigned int, unsigned int, int, int,
                                               AbyssEngine::PaintCanvas *, void *, bool);

void PaintCanvas::DrawString(unsigned int index, const AbyssEngine::String &str,
                             int x, int y, bool b) {
    if (index < this->fonts.count) {
        PCFontView *font = (PCFontView *) (this->fonts.data_)[index];
        paintcanvas_ext_string_prep(this, font->atlas, -1);
        void *font2 = (this->fonts.data_)[index];
        void *data = paintcanvas_ext_str_text(&str);
        paintcanvas_ext_drawstring_str(font2, (unsigned int) (uintptr_t) data, str.size(), x, y,
                                       this, this->engine, b);
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
            void *m = transform->meshes[i];
            paintcanvas_ext_mesh_shaderanim(this, m, value, mode);
        }
        for (unsigned int i = 0; i < transform->children.size(); ++i) {
            void *c = transform->children[i];
            paintcanvas_ext_transform_shaderanim(this, c, value, mode);
        }
    }
}

void PaintCanvas::ReleaseSpriteSystemResource(unsigned int index) {
    if (index < this->spriteSystems.count) {
        void *ctx = this->engine;
        char **arr = (char **) this->spriteSystems.data_;
        return paintcanvas_ext_release_sprite_res(ctx, arr + index);
    }
}


static const double g_dss2_gravscale_8af58 = 0;

void PaintCanvas::DrawSpriteSystem(unsigned int index,
                                   AbyssEngine::AEMath::Matrix matA, AbyssEngine::AEMath::Matrix matB) {
    if (index >= this->spriteSystems.count) {
        return;
    }
    if (((void **) this->spriteSystems.data_)[index] == 0) {
        return;
    }

    float world[15];
    float view[15];
    memcpy(world, matA, sizeof(world));
    memcpy(view, matB, sizeof(view));

    if (this->initialized == 0) {
        float inv[16];
        memset(inv, 0, sizeof(inv));
        inv[0] = 1.0f;
        inv[5] = 1.0f;
        inv[14] = 1.0f;
        paintcanvas_ext_dss2_mtx_getinv(inv, view);
        paintcanvas_ext_dss2_mtx_assign(view, inv);
    } else {
        float rotM[16];
        char scratch[60];
        memset(rotM, 0, sizeof(rotM));
        rotM[0] = 1.0f;
        rotM[5] = 1.0f;
        rotM[14] = 1.0f;
        paintcanvas_ext_dss2_matidentity(scratch, rotM);

        void *grav = paintcanvas_ext_dss2_getgrav(this->engine);
        double angle = ((PCGravView *) grav)->angle * g_dss2_gravscale_8af58;
        int ia = (int) (long long) angle;
        if (this->gameOrientation == 1) {
            ia = -ia;
        }
        float a = paintcanvas_ext_dss2_signedtofloat(ia, 0);
        float s = paintcanvas_ext_dss2_sinf(a);
        float c = paintcanvas_ext_dss2_cosf(a);
        rotM[0] = c;
        rotM[5] = c;
        *(unsigned int *) &rotM[1] = *(unsigned int *) &s ^ 0x80000000;
        rotM[4] = s;

        paintcanvas_ext_dss2_mtx_muleq(view, rotM);
        paintcanvas_ext_dss2_mtx_getinv(scratch, view);
        paintcanvas_ext_dss2_mtx_assign(view, scratch);
        paintcanvas_ext_dss2_mtx_muleq(world, rotM);
    }

    paintcanvas_ext_dss2_ssdraw(this->engine, world, view,
                                ((void **) this->spriteSystems.data_)[index]);
}
