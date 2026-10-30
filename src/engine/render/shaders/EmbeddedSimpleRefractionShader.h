#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedSimpleRefractionShader {

// ELF address 0x1f0bc0; 774 bytes; SHA-256 1b709dd6e304dbf2460997b2bd6afa248a01dc3bee6802de9322780aabee59e0
static constexpr const char vertex[] =
    "attribute highp    vec3 a_position;\n"
    "attribute mediump  vec2 a_texCoord;\n"
    "attribute lowp\t\tvec3  \ta_normal; \t  \n"
    "attribute lowp \tvec3 \ta_tangent;\t\t  \n"
    "attribute lowp \tvec3 \ta_bitangent;\t  \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;\n"
    "varying lowp vec3 v_eye_dir;  \n"
    "uniform highp vec3 u_eyeposmodel;  \n"
    "varying mediump vec2 v_texCoord;\n"
    "varying mediump vec3 v_normal;  \n"
    "void main()  \n"
    "{  \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);\n"
    "\tv_texCoord  = a_texCoord;\n"
    "\tv_normal\t= a_normal;\n"
    "\thighp mat3 tangentSpaceXform = mat3(a_tangent.x, a_bitangent.x, a_normal.x,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.y, a_bitangent.y, a_normal.y,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.z, a_bitangent.z, a_normal.z  \n"
    "\t\t\t\t\t\t\t\t\t\t);  \n"
    "   v_eye_dir = normalize(tangentSpaceXform*(u_eyeposmodel - a_position)); \n"
    "}\n";

// ELF address 0x1f0ec7; 803 bytes; SHA-256 c74ac42b249bf78f084c7826bfecc314deaa766efeaced2253e81d626b396541
static constexpr const char fragment[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "varying lowp vec3 v_eye_dir;\n"
    "uniform sampler2D  s_texture[2];\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform lowp vec4  glColor; \n"
    "uniform highp float u_TexBias; \n"
    "uniform highp vec2 u_texelSize;\n"
    "uniform mediump float u_CloakRate;\n"
    "void main()\n"
    "{\n"
    "\tlowp vec4 colorTex   = texture2D( s_texture[0], v_texCoord, u_TexBias);\n"
    "\tlowp vec4 normalTex  = texture2D( s_texture[1], v_texCoord, u_TexBias);\n"
    "\tlowp vec3 normalTexX = colorTex.rgb * 2.0 - 1.0;\n"
    "\tfloat rim_intensity = (smoothstep(0.0, 1.0, 1.0 - dot( v_eye_dir,normalTexX )));\n"
    "\tfloat time = (u_CloakRate);\n"
    "   vec2 coord;\n"
    "   coord = (gl_FragCoord.xy + ((glColor.a*(colorTex.xy-0.5)*rim_intensity)*180.0)) * u_texelSize;\n"
    "\tgl_FragColor.rgb = texture2D(s_texture_base, coord).rgb;\n"
    "\tgl_FragColor.a = colorTex.a;\n"
    "}\n";

} // namespace EmbeddedSimpleRefractionShader
} // namespace AbyssEngine
