namespace AbyssEngine {
    class Engine;
    class Mesh;

    char *g_Camera_frustumEnabledFlag;
    char *g_Engine_fboEnabledFlag;
    char *g_Engine_shaderModeFlag;
    char *g_GameText_arabicEnabledFlag;
    char *g_MeshIntersect_flipVFlag;
    char *g_Mesh_extraArraysFlag;
    char *g_Mesh_keepCpuCopyFlag;
    char *g_Mesh_shaderPathFlag;
    char *g_Mesh_tangentDelFlag;
    char *g_Mesh_tangentEnabledFlag;
    int *g_Mesh_vboByteCounter;
    char *g_Mesh_vboEnabledFlag;
    char *g_SpriteSystem_tangentFlag;
    char *g_SpriteSystem_uvFlipFlag;

    void (*g_MeshRelease_freeFn)(AbyssEngine::Engine *, AbyssEngine::Mesh **);
}
