#pragma once
// Original Android ARM libgof2hdaa.so SHA-256:
// 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedCubeNormalMappingShader {
// ELF virtual address 0x1f76b0; 1393 bytes; SHA-256 73ba76e126d79bfaac59fd020422b0a319028179993601a5041788c0882823c9
static constexpr const char vertex[] =
    "attribute highp\tvec3 a_position;   \n"
    "attribute mediump\tvec3 a_normal;\t\t\n"
    "attribute mediump\tvec2 a_texCoord;   \n"
    "attribute lowp \tvec3 a_tangent;\t\t  \n"
    "attribute lowp \tvec3 a_bitangent;\t  \n"
    "uniform highp\t mat4 u_WorldMatrix;  \n"
    "uniform mediump mat3 u_ModelMatrix;  \n"
    "uniform mediump vec3 u_eyeposmodel;  \n"
    "uniform highp\t vec3 u_lightposmodel;  \n"
    "varying mediump vec3 v_reflectdir;   \n"
    "varying mediump    vec2 v_texCoord;     \n"
    "varying lowp vec4  v_DiffuseLight;\t\n"
    "varying lowp vec3 v_specular_dir;  \n"
    "void main()                  \n"
    "{                            \n"
    "\t// Transform position\n"
    "   gl_Position = u_WorldMatrix * vec4(a_position, 1.0);\t\t\n"
    "\thighp mat3 tangentSpaceXform = mat3(a_tangent.x, a_bitangent.x, a_normal.x,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.y, a_bitangent.y, a_normal.y,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.z, a_bitangent.z, a_normal.z  \n"
    "\t\t\t\t\t\t\t\t\t\t);  \n"
    "\tvec3 shininesDirection = normalize(normalize(u_eyeposmodel - a_position.xyz) + u_lightposmodel) ;  \n"
    "\tv_specular_dir = tangentSpaceXform * shininesDirection;  \n"
    "\t// Calculate eye direction in model space\n"
    "\tmediump vec3 eyeDir = normalize(a_position - u_eyeposmodel);\n"
    "\t// reflect eye direction over normal and transform to world space\n"
    "\tv_reflectdir = u_ModelMatrix * reflect(eyeDir, a_normal);\n"
    "   v_texCoord = a_texCoord;  \n"
    "\tv_DiffuseLight.rgb = vec3(max(dot(normalize(a_normal), normalize(u_lightposmodel)), 0.0)); \n"
    "\tv_DiffuseLight.a = 1.0; \n"
    "}                            \n";
// ELF virtual address 0x1f7c22; 1255 bytes; SHA-256 e44bedcb404e41a4dfa5b57075ed960812fa56aefaa29a4d03d7a949f66b1d60
static constexpr const char fragment[] =
    "precision mediump float;                            \n"
    "uniform samplerCube u_texture_cubemap;\t\t\t \n"
    "uniform sampler2D\t u_texture_base;                        \n"
    "uniform sampler2D\t u_normalTexture;\t\n"
    "uniform highp float u_envValue;  \n"
    "uniform lowp vec4   u_AmbientColor;\n"
    "uniform lowp vec4   u_DiffuseColor;\n"
    "uniform lowp vec4   u_SpecularColor;\n"
    "uniform lowp vec4   u_glColor;\t\n"
    "uniform mediump float  u_SpecularPower;\n"
    "varying mediump vec3 v_reflectdir;   \n"
    "varying mediump vec2 v_texCoord;                            \n"
    "varying lowp vec4    v_DiffuseLight;\t\n"
    "varying lowp vec3\t  v_specular_dir;  \n"
    "void main()                                         \n"
    "{                                                   \n"
    "\tlowp vec4 normalTex  = texture2D( u_normalTexture, v_texCoord, -1.0 );  \n"
    "\tlowp vec3 normalTexX = normalTex.rgb*2.0 -1.0;  \n"
    "\tfloat specularIntensity = pow( clamp(dot( v_specular_dir,normalTexX ), 0.0, 1.0), u_SpecularPower );  \n"
    "\tlowp vec4 combineColor = mix(textureCube(u_texture_cubemap, v_reflectdir), texture2D( u_texture_base, v_texCoord ), u_envValue); \n"
    "\tgl_FragColor =  u_glColor*(u_AmbientColor + v_DiffuseLight * u_DiffuseColor) * combineColor + (specularIntensity * u_SpecularColor);\n"
    "\t//gl_FragColor =  textureCube(u_texture_cubemap, v_reflectdir);\n"
    "}   \t\t  \n";
}
}
