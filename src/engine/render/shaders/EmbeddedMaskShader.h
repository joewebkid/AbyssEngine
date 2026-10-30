#pragma once
// Original Android ARM shader sources used by MaskShader::Init.
namespace AbyssEngine {
namespace EmbeddedMaskShader {

// ELF address 0x1f8f80; 400 bytes; SHA-256 9b3b0cb4dfbe63b0be5194387fd597c6b1ee784ef8beb0d14287e284a0f96f46
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;   \n"
    "attribute mediump vec2 a_texCoord_0;   \n"
    "attribute mediump vec2 a_texCoord_1;   \n"
    "varying mediump vec2 v_texCoord[2];     \n"
    "uniform highp mat4 u_WorldMatrix;  \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = u_WorldMatrix * a_position;\t\t\n"
    "   v_texCoord[0] = a_texCoord_0;  \n"
    "   v_texCoord[1] = a_texCoord_1;  \n"
    "}                            \n";

// ELF address 0x1f9111; 577 bytes; SHA-256 99fcd78d6cd3d2b9e27f6fb04bf5c08ac4fcc5021d8877bdd03a7155ca4e19ee
static constexpr const char fragment[] =
    "precision lowp float;                            \n"
    "varying mediump vec2 v_texCoord[2];                     \n"
    "uniform sampler2D s_texture[2];                        \n"
    "uniform lowp vec4 glColor;\t\t\t\t\t\t\t\t \n"
    "void main()                                         \n"
    "{                                                   \n"
    "\tvec4 textureColorBase = texture2D( s_texture[0], v_texCoord[0] );\t\n"
    "\tvec4 textureColorMask = texture2D( s_texture[1], v_texCoord[1] );\t\n"
    "\tgl_FragColor = glColor * textureColorBase;\t\t\n"
    "\tgl_FragColor.a = glColor.a * (1.0 - textureColorMask.a) * textureColorBase.a;\t\t\n"
    "}   \t\t  \n";

} // namespace EmbeddedMaskShader
} // namespace AbyssEngine
