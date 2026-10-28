#ifndef GOF2_EMBEDDED_BUMP_MAPPING_H
#define GOF2_EMBEDDED_BUMP_MAPPING_H

namespace AbyssEngine { namespace BumpMappingSource {
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;\n"
    "attribute mediump vec3 a_normal;\n"
    "attribute mediump vec3 a_tangent;\n"
    "attribute mediump vec3 a_bitangent;\n"
    "attribute mediump vec2 a_texCoord;\n"
    "varying mediump vec2 v_texCoord;\n"
    "varying mediump vec3 v_lightvec;\n"
    "uniform highp mat4 u_WorldMatrix;\n"
    "uniform highp vec3 u_lightposmodel;\n"
    "void main()\n"
    "{\n"
    "\tgl_Position = u_WorldMatrix * a_position;\n"
    "\thighp mat3 tangentSpaceXform = mat3(a_tangent, a_bitangent, a_normal);\n"
    "\tv_lightvec =   u_lightposmodel * tangentSpaceXform;\n"
    "\tv_texCoord = a_texCoord;\n"
    "}\n";

static constexpr const char fragment[] =
    "precision mediump float;   \n"
    "varying mediump vec2 v_texCoord;    \n"
    "varying mediump vec3 v_lightvec;    \n"
    "uniform sampler2D s_texture_base;   \n"
    "uniform sampler2D s_texture_normal;\n"
    "void main()                      \n"
    "{                              \n"
    "\tmediump vec3 normal = normalize(texture2D(s_texture_normal, v_texCoord).rgb*2.0 - 1.0);\n"
    "\tmediump vec3 lightvec = normalize(v_lightvec);\n"
    "\tmediump float lightIntensity = max(dot(lightvec, normal), 0.0);\n"
    "\tlowp vec4 textureColor = texture2D( s_texture_base, v_texCoord );// * v_color;\t\n"
    "\t// Compute Ambient term \n"
    "\tlowp vec4 ambient = vec4(0.3, 0.3, 0.3, 1.0) * textureColor; \n"
    "\tgl_FragColor = ambient;\n"
    "\tif (lightIntensity>0.0) {\t\n"
    "\t\t// Compute Diffuse term \n"
    "\t\tlowp vec4 diffuse =  vec4(0.7, 0.7, 0.7, 1.0) * lightIntensity * textureColor;\n"
    "\t\tgl_FragColor += diffuse; \n"
    "\t}\n"
    "\t//gl_FragColor = vec4(0.5,0.5,0.5,0.5);\n"
    "}\n";

} } // namespace AbyssEngine::BumpMappingSource

#endif
