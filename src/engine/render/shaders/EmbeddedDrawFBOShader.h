#ifndef GOF2_EMBEDDED_DRAW_FBO_SHADER_H
#define GOF2_EMBEDDED_DRAW_FBO_SHADER_H

namespace AbyssEngine { namespace DrawFBOSource {
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
    "void main(void)\n"
    "{\n"
    "   gl_FragColor.rgb = texture2D(s_texture_base, v_texCoord).rgb;\n"
    "}\n";

} } // namespace AbyssEngine::DrawFBOSource

#endif
