#pragma once
// Original Android ARM libgof2hdaa.so SHA-256:
// 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedSandboxShader {
// ELF virtual address 0x1f9380; 1030 bytes; SHA-256 52a78e763e05fec1af82fcd67a32b1140935ad85f3a97ef5fc3368b501022e36
static constexpr const char vertex[] =
    "attribute highp vec4 \ta_position;   \t\n"
    "attribute mediump vec2  a_texCoord;   \t\n"
    "attribute highp vec3  \ta_normal; \t\t\n"
    "attribute lowp vec3 \ta_tangent;\t\t\n"
    "attribute lowp vec3 \ta_bitangent;\t\n"
    "varying mediump vec2 v_texCoord;\n"
    "varying mediump vec3 v_light_dir;\n"
    "varying mediump vec3 v_eye_dir;\n"
    "varying mediump vec3 v_lightvec;\n"
    "uniform highp mat4 u_WorldMatrix;\n"
    "uniform highp vec3 u_light_dir;\n"
    "uniform highp vec3 u_eye_pos;\n"
    "void main()\n"
    "{\n"
    "\tgl_Position = u_WorldMatrix * a_position;\t\t\n"
    "\tv_texCoord  = a_texCoord;  \n"
    "\t//mediump vec3 bitangent = normalize(cross(a_normal, a_tangent)); \n"
    "\thighp mat3 tangentSpaceXform = mat3(a_tangent, a_bitangent, a_normal);\n"
    "\t//v_eye_dir   = tangentSpaceXform * normalize(u_eye_pos - a_position.xyz);\n"
    "\t//v_light_dir = tangentSpaceXform * u_light_dir;\n"
    "\tvec3 shininesDirection = normalize(normalize(u_eye_pos - a_position.xyz) + u_light_dir) ;\n"
    "\tv_lightvec = tangentSpaceXform * shininesDirection;\n"
    "\t//u_lightposmodel\n"
    "\t//v_DiffuseLight.rgb = vec3(max(dot(a_normal, u_LightDirection), 0.0));\n"
    "\t//v_DiffuseLight.a = 1.0;\n"
    "}\n";
// ELF virtual address 0x1f9787; 799 bytes; SHA-256 99b12309629bc295a7a953f373bfd5bad214338039cebf77748e9182d3ba0891
static constexpr const char fragment[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "varying mediump vec3 v_light_dir;\n"
    "varying mediump vec3 v_eye_dir;\n"
    "varying mediump vec3 v_lightvec;\n"
    "uniform sampler2D  s_texture_norm;\n"
    "uniform sampler2D  s_texture_base;\n"
    "uniform lowp vec4  glColor;\n"
    "uniform lowp vec4  u_AmbientColor;\n"
    "void main()\n"
    "{\n"
    "\tlowp vec3 normal = normalize(texture2D(s_texture_norm, v_texCoord).rgb*2.0 - 1.0);\n"
    "\t//lowp vec3 lightvec = normalize(v_eye_dir+v_light_dir);\n"
    "\tlowp vec3 lightvec = normalize(v_lightvec);\n"
    "\tlowp float specularIntensity = abs( dot(lightvec, normal) );\n"
    "\t//lowp float diffuseIntensity  = 0.0; //max( dot(v_light_dir, normal), 0.0 );\n"
    "\tlowp vec4 textureColor = texture2D( s_texture_base, v_texCoord );// * v_color;\n"
    "\tgl_FragColor =  ( specularIntensity/* + diffuseIntensity*/) * textureColor * glColor;\n"
    "}\t\n";
}
}
