#ifndef GOF2_EMBEDDED_TEX_ONLY_SHADER_H
#define GOF2_EMBEDDED_TEX_ONLY_SHADER_H

namespace AbyssEngine { namespace TexOnlySource {
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;\n"
    "attribute mediump vec2 a_texCoord;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform highp mat4 u_WorldMatrix;\n"
    "void main()\n"
    "{\n"
    "\tgl_Position = u_WorldMatrix * a_position;\n"
    "\tv_texCoord = a_texCoord;\n"
    "}\n";

static constexpr const char fragment[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "void main()\n"
    "{\n"
    "\tvec4 textureColor = texture2D( s_texture_base, v_texCoord );\n"
    "\tgl_FragColor = textureColor; // * vec4(4.0,4.0,4.0,1.0);\n"
    "}\n";

} } // namespace AbyssEngine::TexOnlySource

#endif
