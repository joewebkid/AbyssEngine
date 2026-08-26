namespace AbyssEngine {
    class Engine;
    class Mesh;

    char *g_Camera_frustumEnabledFlag;
    char *g_Engine_fboEnabledFlag;
    char *g_Engine_shaderModeFlag;
    char *g_GameText_arabicEnabledFlag;
    char *g_MeshIntersect_flipVFlag;
    char *g_SpriteSystem_tangentFlag;
    char *g_SpriteSystem_uvFlipFlag;

    void (*g_MeshRelease_freeFn)(AbyssEngine::Engine *, AbyssEngine::Mesh **);
}
