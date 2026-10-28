#ifndef GOF2_EMBEDDED_GENERIC_SHADER1_H
#define GOF2_EMBEDDED_GENERIC_SHADER1_H

namespace AbyssEngine { namespace GenericShader1Source {
static constexpr const char vertex[] =
    "attribute highp vec4 \ta_position;"
    "  attribute mediump vec2  a_texCoord;"
    "   \tattribute highp vec3  \ta_normal;"
    " \t\tattribute lowp vec3 \ta_tangent;"
    "\t\tattribute lowp vec3 \ta_bitangent;"
    "\tuniform highp mat4 u_ModelViewProjectionMatrix;"
    "uniform highp mat3 u_ModelMatrix;"
    "uniform highp vec3 u_lightdirmodel;"
    "uniform highp vec3 u_eyeposmodel;"
    "varying mediump    vec2 v_texCoord;"
    "void main(){\tv_texCoord = a_texCoord;"
    "\tgl_Position = u_ModelViewProjectionMatrix * a_position;"
    "\t}";

static constexpr const char fragment[] =
    "precision lowp float;"
    "uniform sampler2D  s_texture;"
    "uniform lowp vec4  glColor;"
    "uniform lowp vec4  u_AmbientColor;"
    "uniform lowp vec4  u_DiffuseColor;"
    "uniform lowp vec4  u_SpecularColor;"
    "varying mediump    vec2 v_texCoord;"
    "void main(){\tgl_FragColor =  texture2D( s_texture, v_texCoord );"
    "}\t";

} } // namespace AbyssEngine::GenericShader1Source

#endif
