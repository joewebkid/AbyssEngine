#include "engine/render/shaders/GenericShader1.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int GenericShader1::ShaderIndex;

    GenericShader1::GenericShader1() {
        GenericShader1::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"GenericShader1";
    }

    void GenericShader1::Init(Engine *) {
        this->program = this->ES2LoadProgram("GenericShader1.vsh", "GenericShader1.fsh");

        this->aPosition = glGetAttribLocation(this->program, "a_position");
        this->aNormal = glGetAttribLocation(this->program, "a_normal");
        this->aTangent = glGetAttribLocation(this->program, "a_tangent");
        this->aBinormal = glGetAttribLocation(this->program, "a_binormal");
        this->aTexCoord = glGetAttribLocation(this->program, "a_texCoord");

        this->uM0 = glGetUniformLocation(this->program, "u_m0");
        this->uM1 = glGetUniformLocation(this->program, "u_m1");
        this->uM2 = glGetUniformLocation(this->program, "u_m2");
        this->uM3 = glGetUniformLocation(this->program, "u_m3");
        this->uM4 = glGetUniformLocation(this->program, "u_m4");
        this->uM5 = glGetUniformLocation(this->program, "u_m5");
        this->uM6 = glGetUniformLocation(this->program, "u_m6");
        this->uM7 = glGetUniformLocation(this->program, "u_m7");
        this->uM8 = glGetUniformLocation(this->program, "u_m8");

        glUseProgram(this->program);
        glUniform1i(this->uM4, 0);
    }

    void GenericShader1::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uM0 >= 0)
            glUniformMatrix4fv(this->uM0, 1, 0, engine->worldViewProjMatrix);
        if (this->uM1 >= 0)
            glUniformMatrix3fv(this->uM1, 1, 0, engine->normalMatrix);

        if (this->dirty != 0) {
            if (this->uM2 >= 0)
                glUniform3f(this->uM2, engine->lightDir.x, engine->lightDir.y,
                            engine->lightDir.z);
            if (this->uM3 >= 0)
                glUniform3f(this->uM3, engine->lightColor.x, engine->lightColor.y,
                            engine->lightColor.z);
            if (this->uM5 >= 0)
                glUniform4fv(this->uM5, 1, engine->glColor);
            if (this->uM6 >= 0)
                glUniform4fv(this->uM6, 1, engine->materialAmbient);
            if (this->uM7 >= 0)
                glUniform4fv(this->uM7, 1, engine->materialDiffuse);
            if (this->uM8 >= 0)
                glUniform4fv(this->uM8, 1, engine->materialSpecular);
            this->dirty = 0;
        }

        if (this->aPosition >= 0)
            glEnableVertexAttribArray(this->aPosition);
        if (this->aNormal >= 0)
            glEnableVertexAttribArray(this->aNormal);
        if (this->aTangent >= 0)
            glEnableVertexAttribArray(this->aTangent);
        if (this->aBinormal >= 0)
            glEnableVertexAttribArray(this->aBinormal);
        if (this->aTexCoord >= 0)
            glEnableVertexAttribArray(this->aTexCoord);

        if (this->aPosition >= 0)
            glVertexAttribPointer(this->aPosition, 3, 0x1406, 0, 0, mesh->positions);
        if (this->aNormal >= 0)
            glVertexAttribPointer(this->aNormal, 2, 0x1406, 0, 0, mesh->texCoords);
        if (this->aTangent >= 0)
            glVertexAttribPointer(this->aTangent, 3, 0x1406, 0, 0, mesh->normals);
        if (this->aBinormal >= 0)
            glVertexAttribPointer(this->aBinormal, 3, 0x1406, 0, 0, mesh->tangents);
        if (this->aTexCoord >= 0)
            glVertexAttribPointer(this->aTexCoord, 3, 0x1406, 0, 0, mesh->binormals);
    }

    void GenericShader1::SetInActive() {
        if (this->aPosition >= 0)
            glDisableVertexAttribArray(this->aPosition);
        if (this->aNormal >= 0)
            glDisableVertexAttribArray(this->aNormal);
        if (this->aTangent >= 0)
            glDisableVertexAttribArray(this->aTangent);
        if (this->aBinormal >= 0)
            glDisableVertexAttribArray(this->aBinormal);
        if (this->aTexCoord >= 0)
            glDisableVertexAttribArray(this->aTexCoord);
    }
}
