#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedColorMixAdd {

// ELF address 0x1fa230; 496 bytes; SHA-256 c9ba168f2bb17f9ab2e2b886bbac820f41b7da91fc48709ac5a064d215cea4b8
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;   \n"
    "attribute mediump vec2 a_texCoord;   \n"
    "varying mediump vec2 v_texCoord;     \n"
    "uniform highp mat4 u_WorldMatrix;  \n"
    "uniform highp mat4 u_UvMatrix;  \n"
    "uniform bool u_UVAnimation; \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = u_WorldMatrix * a_position;\t\t\n"
    "   if ( u_UVAnimation ) \n"
    "       v_texCoord = (u_UvMatrix*vec4(a_texCoord.x, a_texCoord.y, 0.0, 1.0)).xy;  \n"
    "   else \n"
    "       v_texCoord = a_texCoord; \n"
    "}                            \n";

// ELF address 0x1fa421; 523 bytes; SHA-256 ecd0c2e6b641c5ee9decbc7c9b2760104326db6e65b1d49ffdd3a5076df73f9c
static constexpr const char fragment[] =
    "precision lowp float;                            \n"
    "varying mediump vec2 v_texCoord;                            \n"
    "uniform sampler2D s_texture;                        \n"
    "uniform lowp vec4 glColor;\t\t\t\t\t\t\t\t \n"
    "uniform float u_DarkenValue; \n"
    "void main()                                         \n"
    "{                                                   \n"
    "\tvec4 textureColor = texture2D( s_texture, v_texCoord );\t\n"
    "\tvec3 outColor = glColor.rgb + textureColor.rgb;\t\t\n"
    "   gl_FragColor.rgb = outColor; \n"
    "   gl_FragColor.a = textureColor.a;\n"
    "}   \t\t  \n";

} // namespace EmbeddedColorMixAdd
} // namespace AbyssEngine
