#pragma once
// Original Android ARM shader sources used by TextureConference::Init.
namespace AbyssEngine {
namespace EmbeddedTextureConference {

// ELF address 0x1f1220; 505 bytes; SHA-256 27a3d6e4b6f5d4c2169d8236514417d1b754180ec9de4eed20d87c5359f0eedd
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;   \n"
    "attribute mediump vec2 a_texCoord;   \n"
    "varying mediump vec2 v_texCoord;     \n"
    "uniform highp mat4 u_WorldMatrix;  \n"
    "uniform highp int u_time;   \n"
    "void main()                  \n"
    "{                            \n"
    "\tfloat angle= (mod(u_time,360.0)*2.0); \n"
    "\tvec4 pos = a_position; \n"
    "\tpos.z  = sin( radians(pos.y+angle)); \n"
    "\tpos.z += sin( radians(pos.x/2.0+angle));\n"
    "\tpos.z *= pos.y * 0.09;\n"
    "\tgl_Position = u_WorldMatrix * pos;\t\t\n"
    "   v_texCoord = a_texCoord;  \n"
    "}                            \n";

// ELF address 0x1f141a; 449 bytes; SHA-256 9de74d6a3557f81e2a91da385261b9c44960fac0cf4f4869fcb74ed2916fdda4
static constexpr const char fragment[] =
    "precision lowp float;                            \n"
    "varying mediump vec2 v_texCoord;                            \n"
    "varying highp float v_dummy;     \n"
    "uniform sampler2D s_texture;                        \n"
    "uniform lowp vec4 glColor;\t\t\t\t\t\t\t\t \n"
    "void main()                                         \n"
    "{                                                   \n"
    "\tvec4 textureColor = texture2D( s_texture, v_texCoord );\t\n"
    "\tgl_FragColor = glColor * textureColor;\t\t\n"
    "}   \t\t  \n";

} // namespace EmbeddedTextureConference
} // namespace AbyssEngine
