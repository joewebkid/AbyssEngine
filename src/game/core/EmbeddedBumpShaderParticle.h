#pragma once
// Exact GLSL from original Android ARM libgof2hdaa.so.
// Binary SHA-256: 6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58
namespace AbyssEngine {
namespace EmbeddedBumpShaderParticle {

// ELF address 0x1e86d0; 1223 bytes; SHA-256 b3111eac32e811247b3a993fbd7af5755e3b0e81a501c6f69645061dfc0807e9
static constexpr const char vertex[] =
    "attribute highp vec4 \ta_position;  \n"
    "attribute mediump vec2  a_texCoord;    \n"
    "attribute highp vec3  \ta_normal; \t  \n"
    "attribute lowp \tvec3 \ta_tangent;\t\t  \n"
    "attribute lowp \tvec3 \ta_bitangent;\t  \n"
    "attribute mediump vec4 a_VertexColor;   \n"
    "varying mediump vec4 v_VertexColor;     \n"
    "varying mediump vec2 v_texCoord;  \n"
    "varying lowp vec3 v_eye_dir;  \n"
    "varying mediump vec3 v_normal;  \n"
    "varying lowp vec3 v_specular_dir;  \n"
    "varying lowp vec3 v_lightvec;  \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;  \n"
    "uniform highp vec3 u_lightdirmodel;  \n"
    "uniform highp vec3 u_eyeposmodel;  \n"
    "void main()  \n"
    "{  \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * a_position;  \n"
    "\tv_texCoord  = a_texCoord;    \n"
    "\tv_normal \t= a_normal;  \n"
    "\thighp mat3 tangentSpaceXform = mat3(a_tangent.x, a_bitangent.x, a_normal.x,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.y, a_bitangent.y, a_normal.y,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.z, a_bitangent.z, a_normal.z  \n"
    "\t\t\t\t\t\t\t\t\t\t);  \n"
    "   v_eye_dir = normalize(tangentSpaceXform*(u_eyeposmodel - a_position.xyz)); \n"
    "\tvec3 shininesDirection = normalize(normalize(u_eyeposmodel - a_position.xyz) + u_lightdirmodel) ;  \n"
    "\tv_specular_dir = tangentSpaceXform * shininesDirection;  \n"
    "\tv_lightvec = tangentSpaceXform * u_lightdirmodel;  \n"
    "   v_VertexColor = a_VertexColor;  \n"
    "}  \n";

// ELF address 0x1e8b98; 1181 bytes; SHA-256 89dc30ae67ef2d4c5fc79861e53691b2cb6db643fef90c35655206c785bb6391
static constexpr const char fragment[] =
    "precision lowp float;  \n"
    "varying mediump vec4 v_VertexColor;     \n"
    "varying mediump vec2 v_texCoord;  \n"
    "varying lowp vec3 v_eye_dir;  \n"
    "varying mediump vec3 v_normal;  \n"
    "varying lowp vec3 v_specular_dir;  \n"
    "varying lowp vec3 v_lightvec;  \n"
    "uniform sampler2D  s_texture[2];  \n"
    "uniform lowp vec3  u_AmbientColor;  \n"
    "uniform lowp vec3 u_DiffuseColor;\n"
    "uniform lowp vec3 u_SpecularColor;\n"
    "uniform mediump float u_SpecularPower;\n"
    "uniform highp float u_TexBiasDiffuse; \n"
    "uniform highp float u_TexBiasNormal; \n"
    "void main()  \n"
    "{  \n"
    "\tlowp vec4 colorTex   = texture2D( s_texture[0], v_texCoord, u_TexBiasDiffuse );  \n"
    "\tlowp vec4 normalTex  = texture2D( s_texture[1], v_texCoord, u_TexBiasNormal );  \n"
    "\tlowp vec3 normalTexX = normalTex.rgb*2.0 -1.0;  \n"
    "\tlowp vec4 specTex = vec4(vec3(normalTex.a), 0.0);\t \n"
    "\tfloat specularIntensity = pow( clamp(dot( v_specular_dir,normalTexX ), 0.0, 1.0), u_SpecularPower );  \n"
    "\tlowp float diffuseIntensity  = clamp( dot( normalTexX    ,v_lightvec ), 0.0, 1.0 );  \n"
    "\tgl_FragColor.rgb = v_VertexColor.rgb*((u_AmbientColor + diffuseIntensity*u_DiffuseColor) * colorTex.rgb  + (specularIntensity * u_SpecularColor) * specTex.rgb);  \n"
    "\tgl_FragColor.a = v_VertexColor.a*colorTex.a; \n"
    "}  \n";

} // namespace EmbeddedBumpShaderParticle
} // namespace AbyssEngine
