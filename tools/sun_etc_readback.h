// Independent ETC1 oracle, per Khronos OES_compressed_ETC1_RGB8_texture,
// tables 3.17.1-3.17.3. It never feeds pixels into the production upload.
#pragma once
#include <algorithm>

static std::vector<unsigned char> decodeSunEtc(const unsigned char *bytes, unsigned side) {
    const int modifiers[8][4] = {{2,8,-2,-8},{5,17,-5,-17},{9,29,-9,-29},{13,42,-13,-42},
        {18,60,-18,-60},{24,80,-24,-80},{33,106,-33,-106},{47,183,-47,-183}};
    std::vector<unsigned char> pixels(side * side * 4, 255);
    unsigned blocks = (side + 3) / 4;
    for (unsigned by = 0; by < blocks; ++by) for (unsigned bx = 0; bx < blocks; ++bx) {
        const unsigned char *b = bytes + 8 * (by * blocks + bx);
        uint32_t selectors = uint32_t(b[4]) << 24 | uint32_t(b[5]) << 16 | uint32_t(b[6]) << 8 | b[7];
        int colors[2][3];
        for (unsigned c = 0; c < 3; ++c) {
            if (b[3] & 2) {
                int first = b[c] >> 3, delta = b[c] & 7;
                if (delta >= 4) delta -= 8;
                int second = first + delta;
                assert(second >= 0 && second < 32);
                colors[0][c] = (first << 3) | (first >> 2);
                colors[1][c] = (second << 3) | (second >> 2);
            } else {
                colors[0][c] = (b[c] >> 4) * 17;
                colors[1][c] = (b[c] & 15) * 17;
            }
        }
        for (unsigned y = 0; y < 4 && by * 4 + y < side; ++y)
            for (unsigned x = 0; x < 4 && bx * 4 + x < side; ++x) {
                unsigned half = (b[3] & 1) ? y / 2 : x / 2;
                unsigned table = (b[3] >> (half ? 2 : 5)) & 7;
                unsigned bit = x * 4 + y;
                unsigned selector = ((selectors >> bit) & 1) | (((selectors >> (16 + bit)) & 1) << 1);
                unsigned pixel = ((by * 4 + y) * side + bx * 4 + x) * 4;
                for (unsigned c = 0; c < 3; ++c)
                    pixels[pixel + c] = static_cast<unsigned char>(std::clamp(colors[half][c] + modifiers[table][selector], 0, 255));
            }
    }
    return pixels;
}

static GLuint sunShader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    assert(compiled);
    return shader;
}

static void checkSunMipPixels(GLuint texture, const unsigned char *bytes, unsigned length) {
    // A fixture sampler pass, not a native game frame. Smaller viewport gives
    // the matching implicit mip LOD; nearest filtering avoids interpolation.
    GLuint vertex = sunShader(GL_VERTEX_SHADER,
        "attribute vec2 p; varying highp vec2 uv; void main(){uv=(p+1.0)*0.5;gl_Position=vec4(p,0.0,1.0);}");
    GLuint fragment = sunShader(GL_FRAGMENT_SHADER,
        "precision highp float; varying highp vec2 uv; uniform sampler2D image; void main(){gl_FragColor=texture2D(image,uv);}");
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex); glAttachShader(program, fragment);
    glBindAttribLocation(program, 0, "p"); glLinkProgram(program);
    GLint linked = 0; glGetProgramiv(program, GL_LINK_STATUS, &linked); assert(linked);
    glUseProgram(program); glUniform1i(glGetUniformLocation(program, "image"), 0);
    GLuint output, fbo, vbo;
    glGenTextures(1, &output); glBindTexture(GL_TEXTURE_2D, output);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glGenFramebuffers(1, &fbo); glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture);
    GLint minFilter, magFilter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
    assert(minFilter == GL_LINEAR_MIPMAP_LINEAR && magFilter == GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    const float positions[] = {-1,-1,1,-1,-1,1,1,1};
    glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glDisable(GL_BLEND); glDisable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST); glDisable(GL_DITHER);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    unsigned offset = 0, total = 0;
    for (unsigned level = 0, side = 256; level < 9; ++level, side = std::max(1u, side / 2)) {
        unsigned size = ((side + 3) / 4) * ((side + 3) / 4) * 8;
        assert(offset + size <= length);
        auto expected = decodeSunEtc(bytes + offset, side);
        std::vector<unsigned char> actual(expected.size());
        glViewport(0, 0, side, side); glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glReadPixels(0, 0, side, side, GL_RGBA, GL_UNSIGNED_BYTE, actual.data());
        assert(glGetError() == GL_NO_ERROR);
        if (actual != expected) {
            unsigned differences = 0;
            for (unsigned i = 0; i < actual.size(); ++i) differences += actual[i] != expected[i];
            std::fprintf(stderr, "ETC mip %u: %u mismatched bytes\n", level, differences);
            assert(actual == expected);
        }
        total += unsigned(actual.size()); offset += size;
    }
    assert(offset == length && total == 349524);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glDisableVertexAttribArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); glUseProgram(0); glViewport(0, 0, 16, 16);
    glDeleteBuffers(1, &vbo); glDeleteFramebuffers(1, &fbo); glDeleteTextures(1, &output);
    glDeleteProgram(program); glDeleteShader(vertex); glDeleteShader(fragment);
    assert(glGetError() == GL_NO_ERROR);
    std::printf("Sun GPU sampling: 9 ETC1 mips / %u RGBA bytes match independent decoder\n", total);
}
