#ifndef GOF2_EMBEDDED_GENERIC_SHADER_H
#define GOF2_EMBEDDED_GENERIC_SHADER_H

namespace AbyssEngine { namespace GenericShaderSource {
static constexpr const char vertex[] =
    "attribute highp vec4 \ta_position;  \n"
    "attribute mediump vec2  a_texCoord;    \n"
    "attribute highp vec3  \ta_normal; \t  \n"
    "attribute lowp \tvec3 \ta_tangent;\t\t  \n"
    "attribute lowp \tvec3 \ta_bitangent;\t  \n"
    "varying mediump vec2 v_texCoord;  \n"
    "varying lowp vec3 v_light_dir;  \n"
    "varying lowp vec3 v_eye_dir;  \n"
    "varying lowp vec3 v_lightvec;  \n"
    "varying mediump vec3 v_normal;  \n"
    "varying lowp vec3 v_specular_dir;  \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;  \n"
    "uniform highp vec3 u_lightdirmodel;  \n"
    "uniform highp vec3 u_eyeposmodel;  \n"
    "uniform lowp float u_SpecularPower;\n"
    "void main()  \n"
    "{  \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * a_position;  \n"
    "\tv_texCoord  = a_texCoord;    \n"
    "\tv_normal \t= a_normal;  \n"
    "\thighp mat3 tangentSpaceXform = mat3(a_tangent.x, a_bitangent.x, a_normal.x,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.y, a_bitangent.y, a_normal.y,  \n"
    "\t\t\t\t\t\t\t\t\t\ta_tangent.z, a_bitangent.z, a_normal.z  \n"
    "\t\t\t\t\t\t\t\t\t\t);  \n"
    "\tvec3 shininesDirection = normalize(normalize(u_eyeposmodel - a_position.xyz) + u_lightdirmodel) ;  \n"
    "\tv_specular_dir = tangentSpaceXform * shininesDirection;  \n"
    "\tv_lightvec = tangentSpaceXform * u_lightdirmodel;  \n"
    "   v_eye_dir = normalize(tangentSpaceXform*(u_eyeposmodel - a_position.xyz)); \n"
    "}  \n";

static constexpr const char fragment[] =
    "precision lowp float;  \n"
    "varying mediump vec2 v_texCoord;  \n"
    "varying lowp vec3 v_light_dir;  \n"
    "varying lowp vec3 v_eye_dir;  \n"
    "varying lowp vec3 v_lightvec;  \n"
    "varying mediump vec3 v_normal;  \n"
    "varying lowp vec3 v_specular_dir;  \n"
    "uniform sampler2D  s_texture[3];  \n"
    "uniform lowp vec3  u_AmbientColor;  \n"
    "uniform lowp vec3 u_DiffuseColor;\n"
    "uniform lowp vec3 u_SpecularColor;\n"
    "void main()  \n"
    "{  \n"
    "\tlowp vec4 colorTex   = texture2D( s_texture[0], v_texCoord, -1.0 );  \n"
    "\tlowp vec4 normalTex  = texture2D( s_texture[1], v_texCoord, -1.0 );  \n"
    "\tlowp vec3 normalTexX = normalTex.rgb*2.0 - 1.0;  \n"
    "\tlowp vec4 specTex = vec4(vec3(normalTex.a), 0.0);\t//vec4(normalTex.a)  \n"
    "   mediump float powExp = 10.0; \n"
    "\tfloat specularIntensity = pow( clamp(dot( v_specular_dir,normalTexX ), 0.0, 1.0)\t, powExp );  \n"
    "\tlowp vec4 specular = texture2D( s_texture[2], vec2(abs(dot( v_specular_dir,normalTexX )),normalTex.a) );  \n"
    "\tlowp float diffuseIntensity  = clamp( dot( normalTexX    ,v_lightvec ), 0.0, 1.0 );// + 0.28;  \n"
    "\tgl_FragColor.xyz = (u_AmbientColor + diffuseIntensity*u_DiffuseColor) * colorTex.xyz  + specularIntensity * specTex.xyz * u_SpecularColor + (smoothstep(0.6, 1.0, 1.0 - dot( v_eye_dir,normalTexX )))* normalTex.a* vec3(2.0, 2.0, 2.0);//specular*u_SpecularColor;  \n"
    "}  \n";

} } // namespace AbyssEngine::GenericShaderSource

#endif
