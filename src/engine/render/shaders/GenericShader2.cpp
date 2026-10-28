#include "engine/render/shaders/GenericShader2.h"
#include "engine/render/shaders/EmbeddedGenericShader2.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int GenericShader2::ShaderIndex;

    void GenericShader2::SetInActive() {
        if (aPosition >= 0)
            glDisableVertexAttribArray(aPosition);
        if (aTexCoord >= 0)
            glDisableVertexAttribArray(aTexCoord);
        if (aNormal >= 0)
            glDisableVertexAttribArray(aNormal);
        if (aTangent >= 0)
            glDisableVertexAttribArray(aTangent);
        if (aBitangent >= 0)
            glDisableVertexAttribArray(aBitangent);
    }

    void GenericShader2::Init(Engine *) {
        this->program = this->ES2LoadProgram(GenericShader2Source::vertex, GenericShader2Source::fragment);

        aPosition = glGetAttribLocation(this->program, "a_position");
        aTexCoord = glGetAttribLocation(this->program, "a_texCoord");
        aNormal = glGetAttribLocation(this->program, "a_normal");
        aTangent = glGetAttribLocation(this->program, "a_tangent");
        aBitangent = glGetAttribLocation(this->program, "a_bitangent");

        uM0 = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        uM1 = glGetUniformLocation(this->program, "u_ModelMatrix");
        uM2 = glGetUniformLocation(this->program, "u_lightdirmodel");
        uM3 = glGetUniformLocation(this->program, "u_eyeposmodel");
        uM4 = glGetUniformLocation(this->program, "s_texture");
        uM5 = glGetUniformLocation(this->program, "glColor");
        uM6 = glGetUniformLocation(this->program, "u_AmbientColor");
        uM7 = glGetUniformLocation(this->program, "u_DiffuseColor");
        uM8 = glGetUniformLocation(this->program, "u_SpecularColor");

        glUseProgram(this->program);
        glUniform1i(uM4, 0);
    }

    void GenericShader2::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (uM0 >= 0)
            glUniformMatrix4fv(uM0, 1, 0, engine->worldViewProjMatrix);
        if (uM1 >= 0)
            glUniformMatrix3fv(uM1, 1, 0, engine->normalMatrix);

        if (this->dirty != 0) {
            if (uM2 >= 0)
                glUniform3f(uM2, engine->lightDir.x, engine->lightDir.y, engine->lightDir.z);
            if (uM3 >= 0)
                glUniform3f(uM3, engine->lightColor.x, engine->lightColor.y, engine->lightColor.z);
            if (uM5 >= 0)
                glUniform4fv(uM5, 1, engine->glColor);
            if (uM6 >= 0)
                glUniform4fv(uM6, 1, engine->materialAmbient);
            if (uM7 >= 0)
                glUniform4fv(uM7, 1, engine->materialDiffuse);
            if (uM8 >= 0)
                glUniform4fv(uM8, 1, engine->materialSpecular);
            this->dirty = 0;
        }

        if (aPosition >= 0)
            glEnableVertexAttribArray(aPosition);
        if (aTexCoord >= 0)
            glEnableVertexAttribArray(aTexCoord);
        if (aNormal >= 0)
            glEnableVertexAttribArray(aNormal);
        if (aTangent >= 0)
            glEnableVertexAttribArray(aTangent);
        if (aBitangent >= 0)
            glEnableVertexAttribArray(aBitangent);

        if (aPosition >= 0)
            glVertexAttribPointer(aPosition, 3, 0x1406, 0, 0, mesh->positions);
        if (aTexCoord >= 0)
            glVertexAttribPointer(aTexCoord, 2, 0x1406, 0, 0, mesh->texCoords);
        if (aNormal >= 0)
            glVertexAttribPointer(aNormal, 3, 0x1406, 0, 0, mesh->normals);
        if (aTangent >= 0)
            glVertexAttribPointer(aTangent, 3, 0x1406, 0, 0, mesh->tangents);
        if (aBitangent >= 0)
            glVertexAttribPointer(aBitangent, 3, 0x1406, 0, 0, mesh->binormals);
    }

    GenericShader2::GenericShader2() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("GenericShader2");
        this->name = shaderName;
    }
}
