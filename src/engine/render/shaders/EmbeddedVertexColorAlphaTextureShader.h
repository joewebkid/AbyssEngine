#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedVertexColorAlphaTextureShader {

// ELF address 0x1f9ad0; 725 bytes; SHA-256 037c06d5f6e41fc451f4bf3ac18fde234dba612d72e3f405e3c25fca4870185a
static constexpr const char vertex[] =
    "attribute highp   vec4 \ta_position;   attribute mediump vec2  a_texCoord;   \tattribute highp   vec3  a_normal; \t\t\tattribute lowp    vec3 \ta_tangent;\t\tattribute lowp    vec3 \ta_bitangent;\tattribute highp    vec4  a_vertexColor;varying mediump vec2 v_texCoord;varying mediump vec3 v_light_dir;varying mediump vec3 v_eye_dir;varying mediump vec3 v_lightvec;varying mediump vec3 v_normal;varying mediump vec3 v_specular_dir;varying highp   vec4 v_color;uniform highp mat4 u_ModelViewProjectionMatrix;uniform highp vec3 u_lightdirmodel;uniform highp vec3 u_eyeposmodel;\n"
    "//uniform highp mat3 u_ModelMatrix;\n"
    "void main(){\tgl_Position = u_ModelViewProjectionMatrix * a_position;\tv_color     = a_vertexColor;\tv_texCoord  = a_texCoord;}";

// ELF address 0x1f9da6; 615 bytes; SHA-256 b98ad00d260dd96d27741c2bae34ab5603b48092daaadd1e7a39f8868edb90e4
static constexpr const char fragment[] =
    "precision lowp float;varying mediump vec2 v_texCoord;varying mediump vec3 v_light_dir;varying mediump vec3 v_eye_dir;varying mediump vec3 v_lightvec;varying mediump vec3 v_normal;varying mediump vec3 v_specular_dir;varying highp    vec4 v_color;uniform sampler2D  s_texture;\n"
    "//uniform lowp vec4  u_AmbientColor;\n"
    "//uniform lowp vec4  u_DiffuseColor;\n"
    "//uniform lowp vec4  u_SpecularColor;\n"
    "uniform highp vec4  glColor;\t\n"
    "void main(){\tlowp vec4 colorTex   = texture2D( s_texture, v_texCoord );\tif ( colorTex.a < 0.5)\t\tdiscard;\tgl_FragColor = colorTex * v_color; // * glColor;  (Color scheint nicht ben\303\266tigt zu werden)\n"
    "}";

} // namespace EmbeddedVertexColorAlphaTextureShader
} // namespace AbyssEngine
