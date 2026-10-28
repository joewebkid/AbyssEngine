#pragma once
// Exact GLSL bytes from original Android ARM libgof2hdaa.so.
// SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedBlurShader {
// File offset 0x1EC470, 311 bytes.
static constexpr const char vertex[] = "attribute highp vec4 a_position;\nuniform highp mat4 u_WorldMatrix;\nvarying mediump vec2 uv;\nvoid main(void)\n{\n\tgl_Position = vec4((u_WorldMatrix * a_position).xy, 0.0, 1.0 );\n\tgl_Position = sign( gl_Position );\n\t// Texture coordinate for screen aligned (in correct range):\n\tuv = (gl_Position.xy + 1.0) * 0.5;\n}\n";
// File offset 0x1EC5A8, 773 bytes.
static constexpr const char fragment[] = "precision lowp float;\nuniform sampler2D s_texture_base;\nuniform lowp vec2 u_radial_origin;   // blur origin\nuniform lowp float u_blur_strength;  // blur strength\nvarying mediump vec2 uv;\nconst lowp float sampleDist = 1.0;\nvoid main(void)\n{\n    mediump vec2 dir = u_radial_origin - uv;\n    float dist = length(dir);\n    dir = dir/dist;\n    mediump vec4 color = texture2D(s_texture_base, uv);\n    mediump vec4 sum = color;\n    float samples[6];\n    samples[0] = -0.03;\n    samples[1] = -0.02;\n    samples[2] = -0.01;\n    samples[3] =  0.01;\n    samples[4] =  0.02;\n    samples[5] =  0.03;\n    for (int i = 0; i < 5; i++)\n\t\tsum += texture2D( s_texture_base, uv + dir * samples[i]);\n\tsum *= 0.1429;\n\tgl_FragColor = mix( color, sum, clamp( dist * u_blur_strength ,0.0,1.0) );\n}\n";
}
}
