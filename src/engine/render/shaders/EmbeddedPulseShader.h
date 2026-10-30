#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedPulseShader {

// ELF address 0x1fa650; 343 bytes; SHA-256 a519c4a2d04681002f7d60d3da1e22176727421e35df07f880f3bc98c210eceb
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;   \n"
    "attribute mediump vec2 a_texCoord;   \n"
    "varying mediump vec2 v_texCoord;     \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;  \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = u_ModelViewProjectionMatrix * a_position;\t\t\n"
    "   v_texCoord = a_texCoord;  \n"
    "}                            \n";

// ELF address 0x1fa7a8; 283 bytes; SHA-256 e017c8e01069f979abd6b28e7a3a13f6e60c11c5df5c50447055e6f72c5662d6
static constexpr const char fragment[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform sampler2D  s_texture;\n"
    "uniform highp float u_timevalue;\n"
    "void main()\n"
    "{\n"
    "\tvec4 textureColor = texture2D(  s_texture, v_texCoord );\n"
    "\tgl_FragColor = textureColor * vec4(vec3( u_timevalue ), 1.0); // * vec4(4.0,4.0,4.0,1.0);\n"
    "}\n";

} // namespace EmbeddedPulseShader
} // namespace AbyssEngine
