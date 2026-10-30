#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {

namespace EmbeddedSpecCubeMapping {

// ELF address 0x1fa8f0; 1777 bytes; SHA-256 befc82c5f8f0e70724c5f2e26ab7d29eb2b69c5855c51687d4d8f653a32376f4
static constexpr const char vertex[] =
    "attribute highp\tvec3 a_position;   \n"
    "attribute mediump\tvec3 a_normal;\t\t\n"
    "attribute mediump\tvec2 a_texCoord;   \n"
    "uniform highp\t mat4 u_WorldMatrix;  \n"
    "uniform mediump mat3 u_ModelMatrix;  \n"
    "uniform mediump vec3 u_eyeposmodel;  \n"
    "uniform highp\t vec4 u_lightposmodel;  \n"
    "varying mediump vec3 v_reflectdir;   \n"
    "varying mediump    vec2 v_texCoord;     \n"
    "varying lowp vec3  v_DiffuseLight;\t\n"
    "varying lowp vec3  v_SpecularLight;\t\n"
    "varying highp float\tv_rimFactor; \n"
    "uniform lowp vec3   u_AmbientColor;\n"
    "uniform lowp vec3   u_DiffuseColor;\n"
    "uniform lowp vec3\tu_SpecularColor;\n"
    "uniform mediump float u_SpecularPower;\n"
    "void main()                  \n"
    "{                            \n"
    "\t// Transform position\n"
    "   gl_Position = u_WorldMatrix * vec4(a_position, 1.0);\t\t\n"
    "\t// Calculate eye direction in model space\n"
    "\tmediump vec3 eyeDir = normalize( a_position - u_eyeposmodel);\n"
    "\t//mediump vec3 eyeDir = normalize( u_eyeposmodel - a_position);\n"
    "\t// reflect eye direction over normal and transform to world space\n"
    "\tv_reflectdir = u_ModelMatrix * reflect(eyeDir, a_normal);\n"
    "\thighp vec3 lightDirLocal; \n"
    "\thighp float nDotL; \n"
    "\tlightDirLocal = u_lightposmodel.xyz; \n"
    "\tnDotL = dot(a_normal, lightDirLocal);\n"
    "\tv_DiffuseLight = vec3(max(nDotL, 0.0)); \n"
    "\t// Compute\treflection vector \n"
    "\thighp vec3 reflection = (2.0 * a_normal * nDotL) - lightDirLocal;\t\n"
    "   reflection = normalize(-reflect(lightDirLocal, a_normal)); \n"
    "\t// Compute R.V \n"
    "\thighp float rDotV = max(0.0, dot(reflection, normalize(u_eyeposmodel)));\n"
    "\t// Compute Specular term \n"
    "\tv_SpecularLight = u_SpecularColor * pow(rDotV, u_SpecularPower);\t\n"
    "\tv_DiffuseLight *= u_DiffuseColor;\n"
    "\tv_DiffuseLight += u_AmbientColor;\n"
    "   v_texCoord = a_texCoord;  \n"
    "\t//v_rimFactor = clamp( 1.2 - dot( normalize( u_eyeposmodel - a_position), a_normal ), 0.0, 1.0);  \n"
    "}                            \n";

// ELF address 0x1fafe2; 1231 bytes; SHA-256 eebc2e0bb83335ad2c44fc81171ba2d92692d540c1d354f8cb0dfa50af70a104
static constexpr const char fragment[] =
    "precision mediump float;                            \n"
    "uniform samplerCube u_texture_cubemap;\t\t\t \n"
    "uniform sampler2D\t u_texture_base;                        \n"
    "uniform highp float u_envValue;  \n"
    "uniform lowp vec4   u_glColor;\t\n"
    "varying mediump vec3 v_reflectdir;   \n"
    "varying mediump vec2 v_texCoord;                            \n"
    "varying lowp vec3    v_DiffuseLight;\t\n"
    "varying lowp vec3  v_SpecularLight;\t\n"
    "varying highp float\tv_rimFactor; \n"
    "void main()                                         \n"
    "{                                                   \n"
    "\t//lowp vec4 combineColor = mix(textureCube(u_texture_cubemap, v_reflectdir), texture2D( u_texture_base, v_texCoord ), (1.0- v_rimFactor)); \n"
    "\t//lowp vec4 combineColor = mix(textureCube(u_texture_cubemap, v_reflectdir), texture2D( u_texture_base, v_texCoord ), u_envValue); \n"
    "   lowp vec4 diffuseColor = texture2D( u_texture_base, v_texCoord ); \n"
    "\t//gl_FragColor.xyz =  u_glColor.xyz*((v_DiffuseLight) * combineColor.xyz + v_SpecularLight);\n"
    "\tgl_FragColor.xyz =  u_glColor.xyz*((v_DiffuseLight) * diffuseColor.xyz + 0.5*diffuseColor.a * textureCube(u_texture_cubemap, v_reflectdir).xyz);\n"
    "\tgl_FragColor.a = u_glColor.a; \n"
    "   //gl_FragColor = vec4(v_rimFactor, v_rimFactor, v_rimFactor, 1.0); \n"
    "}   \t\t  \n";

} // namespace EmbeddedSpecCubeMapping

namespace EmbeddedSpecCubeAlphaMapping {

// ELF address 0x1e7dc0; 1539 bytes; SHA-256 060cdebbef081b36f04eb34fa51dba363170bf828ef133bb843dd392a221756b
static constexpr const char vertex[] =
    "attribute highp\tvec3 a_position;   \n"
    "attribute mediump\tvec3 a_normal;\t\t\n"
    "attribute mediump\tvec2 a_texCoord;   \n"
    "uniform highp\t mat4 u_WorldMatrix;  \n"
    "uniform mediump mat3 u_ModelMatrix;  \n"
    "uniform mediump vec3 u_eyeposmodel;  \n"
    "uniform highp\t vec4 u_lightposmodel;  \n"
    "varying mediump vec3 v_reflectdir;   \n"
    "varying mediump    vec2 v_texCoord;     \n"
    "varying lowp vec3  v_DiffuseLight;\t\n"
    "uniform lowp vec3   u_AmbientColor;\n"
    "uniform lowp vec3   u_DiffuseColor;\n"
    "uniform lowp vec3\tu_SpecularColor;\n"
    "uniform mediump float u_SpecularPower;\n"
    "void main()                  \n"
    "{                            \n"
    "\t// Transform position\n"
    "   gl_Position = u_WorldMatrix * vec4(a_position, 1.0);\t\t\n"
    "\t// Calculate eye direction in model space\n"
    "\tmediump vec3 eyeDir = normalize( a_position - u_eyeposmodel);\n"
    "\t//mediump vec3 eyeDir = normalize( u_eyeposmodel - a_position);\n"
    "\t// reflect eye direction over normal and transform to world space\n"
    "\tv_reflectdir = u_ModelMatrix * reflect(eyeDir, a_normal);\n"
    "\thighp vec3 lightDirLocal; \n"
    "\thighp float nDotL; \n"
    "\tlightDirLocal = u_lightposmodel.xyz; \n"
    "\tnDotL = dot(a_normal, lightDirLocal);\n"
    "\tv_DiffuseLight = vec3(max(nDotL, 0.0)); \n"
    "\t// Compute\treflection vector \n"
    "\thighp vec3 reflection = (2.0 * a_normal * nDotL) - lightDirLocal;\t\n"
    "   reflection = normalize(-reflect(lightDirLocal, a_normal)); \n"
    "\t// Compute R.V \n"
    "\thighp float rDotV = max(0.0, dot(reflection, normalize(u_eyeposmodel)));\n"
    "\t// Compute Specular term \n"
    "\tv_DiffuseLight *= u_DiffuseColor;\n"
    "\tv_DiffuseLight += u_AmbientColor;\n"
    "   v_texCoord = a_texCoord;  \n"
    "}                            \n";

// ELF address 0x1e83c4; 726 bytes; SHA-256 a41c0500c75f2a186c1b4695d93913982a4c8241c8ae0478e1a072baf1c409e4
static constexpr const char fragment[] =
    "precision mediump float;                            \n"
    "uniform samplerCube u_texture_cubemap;\t\t\t \n"
    "uniform sampler2D\t u_texture_base;                        \n"
    "uniform lowp vec4   u_glColor;\t\n"
    "uniform lowp float   u_cubeMapFactor;\t\n"
    "varying mediump vec3 v_reflectdir;   \n"
    "varying mediump vec2 v_texCoord;                            \n"
    "varying lowp vec3    v_DiffuseLight;\t\n"
    "void main()                                         \n"
    "{                                                   \n"
    "   lowp vec4 diffuseColor = texture2D( u_texture_base, v_texCoord ); \n"
    "\tgl_FragColor.xyz =  u_glColor.xyz*((v_DiffuseLight) * diffuseColor.xyz + u_cubeMapFactor * textureCube(u_texture_cubemap, v_reflectdir).xyz);\n"
    "\tgl_FragColor.a = diffuseColor.a; \n"
    "}   \t\t  \n";

} // namespace EmbeddedSpecCubeAlphaMapping

} // namespace AbyssEngine
