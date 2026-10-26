#pragma once
// Exact GLSL bytes from original Android ARM libgof2hdaa.so.
// SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedNoTexShader {
// File offset 0x1F6900, 209 bytes.
static constexpr const char vertex[] = "attribute highp vec4 a_position;   \nuniform highp mat4 u_WorldMatrix;  \nvoid main()                  \n{                            \n   gl_Position = u_WorldMatrix * a_position;\t\t\n}                            \n";
// File offset 0x1F69D2, 227 bytes.
static constexpr const char fragment[] = "precision lowp float;                            \nuniform lowp vec4 glColor;\t\t\t\t\t\t\t\t \nvoid main()                                         \n{                                                   \n\t\tgl_FragColor = glColor;\n}   \t\t  \n";
}
namespace EmbeddedNoTexVtxColorShader {
// File offset 0x1F33D0, 327 bytes.
static constexpr const char vertex[] = "attribute highp vec4 a_position;   \nattribute mediump vec4 a_VertexColor;   \nvarying mediump vec4 v_VertexColor;     \nuniform highp mat4 u_WorldMatrix;  \nvoid main()                  \n{                            \n   gl_Position = u_WorldMatrix * a_position;\t\t\n   v_VertexColor = a_VertexColor;  \n}                            \n";
// File offset 0x1F3518, 308 bytes.
static constexpr const char fragment[] = "precision lowp float;                            \nvarying mediump vec4 v_VertexColor;                            \nuniform lowp vec4 glColor;\t\t\t\t\t\t\t\t \nvoid main()                                         \n{                                                   \n\tgl_FragColor = glColor * v_VertexColor;\t\t\n}   \t\t  \n";
}
}
