#include "engine/render/ShaderBaseStruct.h"
#include "engine/core/AbyssEngine.h"
#include "game/core/String.h"
#include "engine/file/AEFile.h"
#include <GLES2/gl2.h>


namespace AbyssEngine {
    int ShaderBaseStruct::shaderIndexIntern;

    ShaderBaseStruct::ShaderBaseStruct() {
        this->program = -1;
        this->flags = 0x100;
        ++shaderIndexIntern;
        this->vertexPath = 0;
        this->fragmentPath = 0;
        this->name = "";
    }

    ShaderBaseStruct::~ShaderBaseStruct() {
    }

    String ShaderBaseStruct::GetShaderName() {
        String copy;
        copy = this->name;
        return copy;
    }

    void ShaderBaseStruct::UseShader(bool /*useExtra*/) {
        glUseProgram(this->program);
    }

    void ShaderBaseStruct::DeleteShader() {
        glDeleteProgram(this->program);
    }

    void ShaderBaseStruct::RenderEffect(FBOContainer * /*source*/, ::Engine * /*engine*/) {
    }

    void ShaderBaseStruct::RenderEffect(FBOContainer * /*source*/, FBOContainer *& /*target*/,
                                        AbyssEngine::Engine * /*engine*/) {
    }

    FBOContainer *ShaderBaseStruct::RenderEffect(FBOContainer *source, Engine * /*engine*/, float /*strength*/,
                                                 AEMath::Vector /*tint*/) {
        return source;
    }

    FBOContainer *ShaderBaseStruct::RenderEffect(FBOContainer *source, FBOContainer *& /*target*/, Engine * /*engine*/,
                                                 float /*strength*/, AEMath::Vector /*tint*/) {
        return source;
    }

    void ShaderBaseStruct::Update() {
        this->dirty = 1;
    }

    uint32_t ShaderBaseStruct::ES2LoadProgram(const char *vertexSource, const char *fragmentSource) {
        uint32_t program = 0;

        uint32_t vertexShader = this->ES2LoadShader(0x8b31, vertexSource);
        if (vertexShader != 0) {
            uint32_t fragmentShader = this->ES2LoadShader(0x8b30, fragmentSource);
            if (fragmentShader == 0) {
                glDeleteShader(vertexShader);
            } else {
                program = glCreateProgram();
                if (program != 0) {
                    glAttachShader(program, vertexShader);
                    glAttachShader(program, fragmentShader);
                    glLinkProgram(program);

                    int status;
                    glGetProgramiv(program, 0x8b82, &status);
                    if (status != 0) {
                        glDeleteShader(vertexShader);
                        glDeleteShader(fragmentShader);

                        char *name = (char *) this->name.GetAEChar();
                        AELabelObject(0x8b40, program, name);
                        operator delete[](name);
                    } else {
                        int logLength = 0;
                        glGetProgramiv(program, 0x8b84, &logLength);
                        if (logLength >= 2) {
                            char *log = (char *) malloc(logLength);
                            glGetProgramInfoLog(program, logLength, 0, log);
                            free(log);
                        }
                        glDeleteProgram(program);
                        program = 0;
                    }
                }
            }
        }

        return program;
    }

    uint32_t ShaderBaseStruct::ES2LoadShader(uint32_t type, const char *source) {
        const char *localSource = source;
        uint32_t shader = glCreateShader(type);

        if (shader != 0) {
            int status;
            glShaderSource(shader, 1, &localSource, 0);
            glCompileShader(shader);
            glGetShaderiv(shader, 0x8b81, &status);
            if (status == 0) {
                int logLength = 0;
                glGetShaderiv(shader, 0x8b84, &logLength);
                if (logLength >= 2) {
                    char *log = (char *) malloc(logLength);
                    glGetShaderInfoLog(shader, logLength, 0, log);
                    free(log);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }

        return shader;
    }

    uint32_t ShaderBaseStruct::LoadBindShader(const char *vertexPath, const char *fragmentPath) {
        uint32_t result = 0;

        if (vertexPath != 0 && fragmentPath != 0) {
            this->vertexPath = vertexPath;
            this->fragmentPath = fragmentPath;

            uint32_t handle;
            if (AEFile::OpenRead(vertexPath, &handle) != 0) {
                uint32_t vertexSize = AEFile::GetFileSize(handle);
                char *vertexSource = new char[vertexSize + 1];
                AEFile::Read(vertexSize, vertexSource, handle);
                AEFile::Close(handle);
                vertexSource[vertexSize] = 0;

                if (AEFile::OpenRead(fragmentPath, &handle) != 0) {
                    uint32_t fragmentSize = AEFile::GetFileSize(handle);
                    char *fragmentSource = new char[fragmentSize + 1];
                    AEFile::Read(fragmentSize, fragmentSource, handle);
                    AEFile::Close(handle);
                    fragmentSource[fragmentSize] = 0;

                    result = ES2LoadProgram(vertexSource, fragmentSource);
                    operator delete[](fragmentSource);
                    operator delete[](vertexSource);
                }
            }
        }

        return result;
    }
}
