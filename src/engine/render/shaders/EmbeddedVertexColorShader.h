#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedVertexColorShader {

// ELF address 0x1f63b0; 721 bytes; SHA-256 deeef1f45579525f38c7360f72053ac03cdc1c68ac4137c0b2cbc1a682f8b3ac
static constexpr const char vertex[] =
    "attribute highp   vec4 \ta_position; attribute mediump vec2  a_texCoord;   attribute highp   vec3  a_normal; \t\tattribute lowp    vec3 \ta_tangent;\t\tattribute lowp    vec3 \ta_bitangent;\tattribute highp    vec4  a_vertexColor;varying mediump vec2 v_texCoord;varying mediump vec3 v_light_dir;varying mediump vec3 v_eye_dir;varying mediump vec3 v_lightvec;varying mediump vec3 v_normal;varying mediump vec3 v_specular_dir;varying mediump vec4 v_color;uniform highp mat4 u_ModelViewProjectionMatrix;uniform highp vec3 u_lightdirmodel;uniform highp vec3 u_eyeposmodel;\n"
    "//uniform highp mat3 u_ModelMatrix;\n"
    "void main(){\tgl_Position = u_ModelViewProjectionMatrix * a_position;\tv_color     = a_vertexColor;\tv_texCoord  = a_texCoord;}";

// ELF address 0x1f6682; 583 bytes; SHA-256 417043ab962b86c0740606636933dc5ca40ae3cc180f2fa081615aec14d69483
static constexpr const char fragment[] =
    "precision lowp float;varying mediump vec2 v_texCoord;varying mediump vec3 v_light_dir;varying mediump vec3 v_eye_dir;varying mediump vec3 v_lightvec;varying mediump vec3 v_normal;varying mediump vec3 v_specular_dir;varying highp    vec4 v_color;uniform sampler2D  s_texture;\n"
    "//uniform lowp vec4  u_AmbientColor;\n"
    "//uniform lowp vec4  u_DiffuseColor;\n"
    "//uniform lowp vec4  u_SpecularColor;\n"
    "uniform highp vec4  glColor;\t\n"
    "void main(){\tlowp vec4 colorTex   = texture2D( s_texture, v_texCoord );\n"
    "\tgl_FragColor = colorTex * v_color; // * glColor;  (Color scheint nicht ben\303\266tigt zu werden)\n"
    "}";

} // namespace EmbeddedVertexColorShader
} // namespace AbyssEngine
