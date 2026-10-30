#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedEnergyShield {

// ELF address 0x1ebe40; 755 bytes; SHA-256 b38dcdbb611ea480dc8a31addeab2da1b0cd4c56232a0800680f8f501f25fd0a
static constexpr const char vertex[] =
    "attribute highp    vec3 a_position;\n"
    "attribute mediump  vec2 a_texCoord;\n"
    "attribute highp    vec3 a_normal;\n"
    "varying mediump vec2 v_texCoord;  \n"
    "varying lowp vec3 v_eye_dir;  \n"
    "varying mediump vec3 v_normal;  \n"
    "varying mediump vec3 v_normalView;  \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;\n"
    "uniform highp vec3 u_lightdirmodel[2];\n"
    "uniform highp vec3 u_eyeposmodel;\n"
    "uniform mediump mat4 u_ModelViewMatrix;\n"
    "void main()  \n"
    "{  \n"
    "\thighp vec3 vertex = a_position;\n"
    "\tgl_Position = u_ModelViewProjectionMatrix * vec4(vertex, 1.0);\n"
    "\tv_texCoord  = a_texCoord;\n"
    "\tv_normalView = normalize((u_ModelViewProjectionMatrix * vec4(a_normal, 0.0)).xyz);\n"
    "\tv_normal = a_normal;\n"
    "   v_eye_dir = normalize((u_ModelViewProjectionMatrix * vec4(u_eyeposmodel - a_position, 0.0)).rgb); \n"
    "}\n";

// ELF address 0x1ec134; 791 bytes; SHA-256 74b283703e3b4b418bec04e0d4f184f44f311a999a197743668be56600ec1145
static constexpr const char fragment[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "varying lowp vec3 v_eye_dir;\n"
    "varying mediump vec3 v_normal;  \n"
    "varying mediump vec3 v_normalView;  \n"
    "uniform sampler2D   s_texture[2];\n"
    "uniform sampler2D   s_texture_base;\n"
    "uniform highp vec2 u_texelSize;\n"
    "void main()\n"
    "{\n"
    "\tmediump vec4 normalTex  = texture2D( s_texture[1], v_texCoord);\n"
    "\tmediump vec3 normalTexX = normalTex.rgb*2.0 -1.0;\n"
    "\tfloat rim_intensity = (smoothstep(0.0, 1.0, 1.0 - dot( v_eye_dir,v_normalView )));\n"
    "\tlowp vec3 rim_effect = smoothstep(0.0, 1.0, 1.0 - dot( v_eye_dir,v_normalView * normalTexX)) * vec3(0.0, 0.6, 1.0);\n"
    "   mediump vec2 coord;\n"
    "   coord = (gl_FragCoord.xy + ((normalTexX.xy * 0.4 + v_normalView.xy*rim_intensity)*150.0)) * u_texelSize;\n"
    "\tgl_FragColor.rgb = texture2D(s_texture_base, coord).rgb + rim_effect;\n"
    "}\n";

} // namespace EmbeddedEnergyShield
} // namespace AbyssEngine
