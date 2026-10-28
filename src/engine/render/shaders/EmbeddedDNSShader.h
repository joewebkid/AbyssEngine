#ifndef GOF2_EMBEDDED_DNS_SHADER_H
#define GOF2_EMBEDDED_DNS_SHADER_H

namespace AbyssEngine { namespace DNSShaderSource {
static constexpr const char vertex[] =
    "attribute highp vec3 \ta_position;  \n"
    "uniform highp mat4 u_ModelViewProjectionMatrix;  \n"
    "void main()  \n"
    "{  \n"
    "\tgl_Position = u_ModelViewProjectionMatrix * vec4(a_position, 1.0);  \n"
    "}  \n";

static constexpr const char fragment[] =
    "precision lowp float;  \n"
    "void main()  \n"
    "{  \n"
    "       gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0); \n"
    "}  \n";

} } // namespace AbyssEngine::DNSShaderSource

#endif
