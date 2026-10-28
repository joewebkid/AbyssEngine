#ifndef GOF2_EMBEDDED_BLOOM_SHADER_H
#define GOF2_EMBEDDED_BLOOM_SHADER_H

// Android ARM libgof2hdaa.so SHA-256:
// 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine { namespace BloomShaderSource {
// File offset 0x1eb170, 220 source bytes.
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;\n"
    "attribute mediump vec2 a_texCoord;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform highp mat4 u_WorldMatrix;\n"
    "void main()\n"
    "{\n"
    "\tgl_Position = u_WorldMatrix * a_position;\n"
    "\tv_texCoord = a_texCoord;\n"
    "}\n";

// File offset 0x1eb24d, 197 source bytes.
static constexpr const char luma[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "void main()\n"
    "{\n"
    "\tvec4 textureColor = texture2D( s_texture_base, v_texCoord );\n"
    "\tgl_FragColor = textureColor;\n"
    "}\n";

// File offset 0x1eb313, 621 source bytes.
static constexpr const char downSample[] =
    "precision mediump float;\n"
    "//const float Luminance = 0.005; \n"
    "//const float fMiddleGray = 0.04;\n"
    "//const float fWhiteCutoff = 0.1;\n"
    "const float Luminance = 0.08; \n"
    "const float fMiddleGray = 0.005;\n"
    "const float fWhiteCutoff = 0.014;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "void main()\n"
    "{\n"
    "\tmediump vec3 ColorOut = texture2D( s_texture_base, v_texCoord ).rgb;\n"
    "\tColorOut *= fMiddleGray / ( Luminance + 0.001 );\n"
    "\tColorOut *= ( 1.0 + ( ColorOut / ( fWhiteCutoff * fWhiteCutoff ) ) );\n"
    "\tColorOut -= 5.0;\n"
    "\tColorOut = max( ColorOut, 0.0 );\n"
    "\tColorOut /= ( 10.0 + ColorOut );\n"
    "\tgl_FragColor = vec4( ColorOut, 1.0 );\n"
    "}\n";

// File offset 0x1eb581, 817 source bytes.
static constexpr const char blurHorizontal[] =
    "precision lowp float;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform mediump float texSize; \n"
    "mediump float offset[3];// = float[]( 0.0, 1.3846153846, 3.2307692308 ); \n"
    "float weight[3];// = float[]( 0.2270270270, 0.3162162162, 0.0702702703 ); \n"
    "void main()\n"
    "{\n"
    "\t  offset[0] = 0.0; \n"
    "\t  offset[1] = 1.3846153846; \n"
    "\t  offset[2] = 3.2307692308; \n"
    "\t  weight[0] = 0.2270270270; \n"
    "\t  weight[1] = 0.3162162162; \n"
    "\t  weight[2] = 0.0702702703; \n"
    "\t  mediump vec3 tc = vec3(1.0, 0.0, 0.0);\n"
    "\t\tvec2 uv = v_texCoord;\n"
    "\t\ttc = texture2D(s_texture_base, uv).rgb * weight[0];\n"
    "\t\tfor (int i=1; i<3; i++)\n"
    "\t\t{\n"
    "\t\t  tc += texture2D(s_texture_base, uv + vec2(offset[i])/texSize, 0.0).rgb * weight[i];\n"
    "\t\t  tc += texture2D(s_texture_base, uv - vec2(offset[i])/texSize, 0.0).rgb * weight[i];\n"
    "\t\t}\n"
    "\t  gl_FragColor = vec4(tc, 1.0);\n"
    "}\n";

// File offset 0x1eb8b3, 820 source bytes.
static constexpr const char blurVertical[] =
    "precision lowp float;\n"
    "varying mediump vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform mediump float texSize; \n"
    "mediump float offset[3];// = float[]( 0.0, 1.3846153846, 3.2307692308 ); \n"
    "float weight[3];// = float[]( 0.2270270270, 0.3162162162, 0.0702702703 ); \n"
    "void main()\n"
    "{\n"
    "\t  offset[0] = 0.0; \n"
    "\t  offset[1] = 1.3846153846; \n"
    "\t  offset[2] = 3.2307692308; \n"
    "\t  weight[0] = 0.2270270270; \n"
    "\t  weight[1] = 0.3162162162; \n"
    "\t  weight[2] = 0.0702702703; \n"
    "\t  mediump vec3 tc = vec3(1.0, 0.0, 0.0);\n"
    "\t\tvec2 uv = v_texCoord;\n"
    "\t\ttc = texture2D(s_texture_base, uv).rgb * weight[0];\n"
    "\t\tfor (int i=1; i<3; i++)\n"
    "\t\t{\n"
    "\t\t  tc += texture2D(s_texture_base, uv + vec2(0.0, offset[i])/texSize).rgb * weight[i];\n"
    "\t\t  tc += texture2D(s_texture_base, uv - vec2(0.0, offset[i])/texSize).rgb * weight[i];\n"
    "\t\t}\n"
    "\t  gl_FragColor = vec4(tc, 1.0);\n"
    "}\n";

// File offset 0x1ebbe8, 564 source bytes.
static constexpr const char finalPass[] =
    "precision lowp float;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform sampler2D s_texture_bloom;\n"
    "void main()\n"
    "{\n"
    "\tvec3 textureColorBase = texture2D( s_texture_base, v_texCoord ).rgb;\n"
    "\tvec3 bloomBase = texture2D( s_texture_bloom, v_texCoord ).rgb; \n"
    "\tvec3 vFinalGlareColor = clamp(bloomBase + textureColorBase, vec3(0.0), vec3(1.0)); \n"
    "\tfloat fLum = dot(textureColorBase, vec3(0.3, 0.59, 0.11)); \n"
    "\tgl_FragColor.rgb = clamp( vFinalGlareColor * ( 1.0 - fLum ) + vFinalGlareColor * fLum, vec3(0.0), vec3(1.0) ); \n"
    "\t//gl_FragColor.rgb = bloomBase;\n"
    "}\n";

} } // namespace AbyssEngine::BloomShaderSource

#endif
