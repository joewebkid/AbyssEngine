#pragma once
// Original Android ARM shader sources used by GreenShader::Init.
namespace AbyssEngine {
namespace EmbeddedGreenShader {

// ELF address 0x1e9070; 201 bytes; SHA-256 872de6bce52990e1da7a47e017095852cd13c4ffa864c2bc6c4ce2f1db05dc5f
static constexpr const char vertex[] =
    "attribute highp vec4 a_position; \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;\n"
    "void main()                 \n"
    "{                          \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * a_position;\t\t\n"
    "} \n";

// ELF address 0x1e913a; 283 bytes; SHA-256 e017c8e01069f979abd6b28e7a3a13f6e60c11c5df5c50447055e6f72c5662d6
static constexpr const char fragment[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform sampler2D  s_texture;\n"
    "uniform highp float u_timevalue;\n"
    "void main()\n"
    "{\n"
    "\tvec4 textureColor = texture2D(  s_texture, v_texCoord );\n"
    "\tgl_FragColor = textureColor * vec4(vec3( u_timevalue ), 1.0); // * vec4(4.0,4.0,4.0,1.0);\n"
    "}\n";

} // namespace EmbeddedGreenShader
} // namespace AbyssEngine
