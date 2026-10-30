#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedBumpShaderRefract {

// ELF address 0x1f4a10; 568 bytes; SHA-256 3bccbc7706cd2fff627eb34127168a392835f3a41ba83f6b79aefb17994f3021
static constexpr const char vertex[] =
    "attribute highp vec3 a_position;\n"
    "attribute mediump vec2 a_texCoord;\n"
    "attribute mediump vec4 a_VertexColor;   \n"
    "varying mediump vec2 v_texCoord;  \n"
    "varying mediump vec4 v_VertexColor;     \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;\n"
    "uniform highp mat4 u_UvMatrix;  \n"
    "uniform bool u_UVAnimation; \n"
    "void main()  \n"
    "{  \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);\n"
    "   if ( u_UVAnimation ) \n"
    "       v_texCoord = (u_UvMatrix*vec4(a_texCoord.x, a_texCoord.y, 0.0, 1.0)).xy;  \n"
    "   else \n"
    "       v_texCoord = a_texCoord; \n"
    "   v_VertexColor = a_VertexColor;  \n"
    "}\n";

// ELF address 0x1f4c49; 930 bytes; SHA-256 265d1255b39e8f6ec759b256d129833b47fae4ca3efb244172f7fd99fe3ff776
static constexpr const char fragment[] =
    "precision lowp float;  \n"
    "varying mediump vec2 v_texCoord;\n"
    "varying mediump vec4 v_VertexColor;     \n"
    "uniform sampler2D  s_texture[2];\n"
    "uniform mediump vec2 u_texelSize;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform lowp vec4 glColor;\t\t\t\t\t\t\t\t \n"
    "void main()\n"
    "{\n"
    "\tlowp vec4 colorTex   = (texture2D( s_texture[0], v_texCoord) * glColor);\n"
    "\tmediump vec4 normalTex  = texture2D( s_texture[1], v_texCoord);\n"
    "\tmediump vec3 normalTexX = normalTex.rgb * 2.0 - 1.0;\n"
    "   mediump vec2 coord;\n"
    "   mediump vec2 coord1;\n"
    "   coord1.x = (gl_FragCoord.x ) * u_texelSize.x;\n"
    "   coord1.y = (gl_FragCoord.y ) * u_texelSize.y;\n"
    "   coord.x = (gl_FragCoord.x + glColor.r*v_VertexColor.r*(normalTexX.x*40.0 * (0.75))) * u_texelSize.x;\n"
    "   coord.y = (gl_FragCoord.y + glColor.r*v_VertexColor.r*(normalTexX.y*30.0 * (0.75))) * u_texelSize.y;\n"
    "\tgl_FragColor.rgb = (colorTex.rgb*v_VertexColor.rgb + texture2D(s_texture_base, coord).rgb) - texture2D(s_texture_base, coord1).rgb;\n"
    "}\n";

} // namespace EmbeddedBumpShaderRefract
} // namespace AbyssEngine
