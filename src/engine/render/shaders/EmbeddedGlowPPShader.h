#ifndef GOF2_EMBEDDED_GLOW_PP_SHADER_H
#define GOF2_EMBEDDED_GLOW_PP_SHADER_H

namespace AbyssEngine { namespace GlowPPSource {
static constexpr const char vertex[] =
    "attribute highp vec4 a_position;\n"
    "attribute mediump vec2 a_texCoord;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform highp mat4 u_WorldMatrix;\n"
    "void main()\n"
    "{\n"
    "\tgl_Position = u_WorldMatrix * a_position;\n"
    "\tv_texCoord = a_texCoord;\n"
    "}\n";

static constexpr const char copyFragment[] =
    "precision lowp float;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "void main(void)\n"
    "{\n"
    "   gl_FragColor.rgb = texture2D(s_texture_base, v_texCoord).rgb * texture2D(s_texture_base, "
    "v_texCoord).a;\n"
    "}\n";

static constexpr const char blurXFragment[] =
    "precision lowp float;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform float texSize; \n"
    "mediump float offset[3];// = float[]( 0.0, 1.3846153846, 3.2307692308 ); \n"
    "float weight[3];\n"
    "void main()\n"
    "{\n"
    "\toffset[0] = 0.0; \n"
    "\toffset[1] = 1.3846153846; \n"
    "\toffset[2] = 3.2307692308; \n"
    "\tweight[0] = 0.6; //0.5 \n"
    "\tweight[1] = 0.2; \n"
    "\tweight[2] = 0.05; \n"
    "\tvec2 uv = v_texCoord;\n"
    "\tmediump vec3 tc = texture2D(s_texture_base, uv).rgb * weight[0];\n"
    "\tfor (int i=1; i<3; i++) {\n"
    "\t\ttc += texture2D(s_texture_base, uv + vec2(offset[i] * texSize, 0.0)).rgb * weight[i];\n"
    "\t\ttc += texture2D(s_texture_base, uv - vec2(offset[i] * texSize, 0.0)).rgb * weight[i];\n"
    "\t}\n"
    "\tgl_FragColor = vec4(tc, 1.0);\n"
    "}\n";

static constexpr const char blurYFragment[] =
    "precision lowp float;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform float texSize; \n"
    "mediump float offset[3];// = float[]( 0.0, 1.3846153846, 3.2307692308 ); \n"
    "float weight[3];\n"
    "void main()\n"
    "{\n"
    "\toffset[0] = 0.0; \n"
    "\toffset[1] = 1.3846153846; \n"
    "\toffset[2] = 3.2307692308; \n"
    "\tweight[0] = 0.6; //0.5 \n"
    "\tweight[1] = 0.2; \n"
    "\tweight[2] = 0.05; \n"
    "\tvec2 uv = v_texCoord;\n"
    "\tmediump vec3 tc = texture2D(s_texture_base, uv).rgb * weight[0];\n"
    "\tfor (int i=1; i<3; i++) {\n"
    "\t\ttc += texture2D(s_texture_base, uv + vec2(0.0, offset[i] * texSize)).rgb * weight[i];\n"
    "\t\ttc += texture2D(s_texture_base, uv - vec2(0.0, offset[i] * texSize)).rgb * weight[i];\n"
    "\t}\n"
    "\tgl_FragColor = vec4(tc, 1.0);\n"
    "}\n";

static constexpr const char combineFragment[] =
    "precision lowp float;\n"
    "varying lowp vec2 v_texCoord;\n"
    "uniform sampler2D s_texture_base;\n"
    "uniform sampler2D s_texture_glow;\n"
    "void main(void)\n"
    "{\n"
    "\tvec2 TexCoord = v_texCoord;\n"
    "\tgl_FragColor.rgb = texture2D(s_texture_base, v_texCoord).rgb + texture2D(s_texture_glow, v_texCoord).rgb;\n}\n";

} } // namespace AbyssEngine::GlowPPSource

#endif
