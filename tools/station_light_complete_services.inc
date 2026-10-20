static unsigned stationMaterialColorCalls, stationShininessCalls, stationLightColorCalls;
extern "C" void glMaterialfv(unsigned int face, unsigned int parameter, const float *values) {
    assert(face == 0x408 && values);
    assert(parameter == 0x1200 || parameter == 0x1201 || parameter == 0x1202);
    ++stationMaterialColorCalls;
}
extern "C" void glMaterialf(unsigned int face, unsigned int parameter, float value) {
    assert(face == 0x408 && parameter == 0x1601 && value == 0.7f);
    ++stationShininessCalls;
}
extern "C" void glLightfv(unsigned int light, unsigned int parameter, const float *values) {
    assert((light == 0x4000 || light == 0x4001) && values);
    assert(parameter == 0x1200 || parameter == 0x1201 || parameter == 0x1202);
    ++stationLightColorCalls;
}
void ShaderUpdateRimColor() { unexpected("ShaderUpdateRimColor outside fixed-function boundary"); }
void paintcanvas_ext_gl_disable(unsigned int) { unexpected("fog GL disable outside Pan boundary"); }
void paintcanvas_ext_gl_enable(unsigned int) { unexpected("fog GL enable outside Pan boundary"); }
