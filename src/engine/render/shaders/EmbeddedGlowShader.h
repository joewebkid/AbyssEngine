#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedGlowShader {

// ELF address 0x1fa040; 273 bytes; SHA-256 da2e96b86576a0f5d1687d0190e2b0ec747da2019333567f5fb1de5051b7ddd4
static constexpr const char vertex[] =
    "attribute highp vec4 \ta_position;  \n"
    "attribute mediump vec2  a_texCoord;    \n"
    "varying mediump vec2 v_texCoord;  \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;  \n"
    "void main()  \n"
    "{  \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * a_position;  \n"
    "\tv_texCoord  = a_texCoord;    \n"
    "}  \n";

// ELF address 0x1fa152; 177 bytes; SHA-256 d369725fe849765f068b4362d3d6a3bffa6e44ffb7fd99884d31d88c00a7dbcd
static constexpr const char fragment[] =
    "precision lowp float;  \n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform sampler2D  s_texture;\n"
    "void main()\n"
    "{\n"
    "   gl_FragColor.a = step(0.5/*08*/, texture2D(s_texture, v_texCoord).a); \n"
    "}\n";

} // namespace EmbeddedGlowShader
} // namespace AbyssEngine
