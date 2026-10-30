#include "game/core/BumpShaderParticle.h"
#include "game/core/EmbeddedBumpShaderParticle.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int BumpShaderParticle::ShaderIndex;

    BumpShaderParticle::BumpShaderParticle() {
        ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        String shaderName("BumpShaderParticle");
        this->name = shaderName;
    }

    void BumpShaderParticle::Init(Engine *) {
        this->program = this->ES2LoadProgram(EmbeddedBumpShaderParticle::vertex,
                                             EmbeddedBumpShaderParticle::fragment);

        attribA0 = glGetAttribLocation(this->program, "a_position");
        attribA1 = glGetAttribLocation(this->program, "a_texCoord");
        attribA2 = glGetAttribLocation(this->program, "a_normal");
        attribA3 = glGetAttribLocation(this->program, "a_tangent");
        attribA4 = glGetAttribLocation(this->program, "a_bitangent");

        uniformU0 = glGetAttribLocation(this->program, "a_VertexColor");
        uniformU1 = glGetUniformLocation(this->program, "u_ModelViewProjectionMatrix");
        uniformU2 = glGetUniformLocation(this->program, "u_ModelMatrix");
        uniformU3 = glGetUniformLocation(this->program, "u_lightdirmodel");
        uniformU4 = glGetUniformLocation(this->program, "u_eyeposmodel");
        uniformU5 = glGetUniformLocation(this->program, "s_texture[0]");
        uniformU6 = glGetUniformLocation(this->program, "s_texture[1]");
        uniformU7 = glGetUniformLocation(this->program, "glColor");
        uniformU8 = glGetUniformLocation(this->program, "u_AmbientColor");
        uniformU9 = glGetUniformLocation(this->program, "u_DiffuseColor");
        uniformU10 = glGetUniformLocation(this->program, "u_SpecularColor");
        uniformU11 = glGetUniformLocation(this->program, "u_SpecularPower");
        uniformU12 = glGetUniformLocation(this->program, "u_TexBiasDiffuse");
        uniformU13 = glGetUniformLocation(this->program, "u_TexBiasNormal");

        glUseProgram(this->program);

        int *samplers = &uniformU5;
        for (int i = 0; i != 2; i++) {
            int loc = samplers[i];
            if (loc >= 0)
                glUniform1i(loc, i);
        }
    }

    void BumpShaderParticle::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (uniformU1 >= 0)
            glUniformMatrix4fv(uniformU1, 1, 0, engine->worldViewProjMatrix);
        if (uniformU2 >= 0)
            glUniformMatrix3fv(uniformU2, 1, 0, engine->normalMatrix);
        if (uniformU12 >= 0)
            glUniform1f(uniformU12, Engine::lodBiasDiffuse);
        if (uniformU13 >= 0)
            glUniform1f(uniformU13, Engine::lodBiasNormal);

        if (this->dirty != 0) {
            glUniform3f(uniformU3, engine->lightDir.x,
                        engine->lightDir.y, engine->lightDir.z);
            if (uniformU4 >= 0)
                glUniform3f(uniformU4, engine->lightColor.x,
                            engine->lightColor.y, engine->lightColor.z);
            if (uniformU7 >= 0)
                glUniform4fv(uniformU7, 1, engine->glColor);
            if (uniformU8 >= 0)
                glUniform3fv(uniformU8, 1, (float *) &engine->field_0x314);
            if (uniformU9 >= 0)
                glUniform3fv(uniformU9, 1, (float *) &engine->field_0x2fc);
            if (uniformU10 >= 0)
                glUniform3fv(uniformU10, 1, (float *) &engine->lightDiffuseShaded);
            if (uniformU11 >= 0)
                glUniform1f(uniformU11, engine->materialShininess);
            this->dirty = 0;
        }

        if (attribA0 >= 0)
            glEnableVertexAttribArray(attribA0);
        if (attribA1 >= 0)
            glEnableVertexAttribArray(attribA1);
        if (attribA2 >= 0)
            glEnableVertexAttribArray(attribA2);
        if (attribA3 >= 0)
            glEnableVertexAttribArray(attribA3);
        if (attribA4 >= 0)
            glEnableVertexAttribArray(attribA4);
        if (uniformU0 >= 0)
            glEnableVertexAttribArray(uniformU0);

        if (mesh->uploaded == 0) {
            if (attribA0 >= 0)
                glVertexAttribPointer(attribA0, 3, 0x1406, 0, 0, mesh->positions);
            if (attribA1 >= 0)
                glVertexAttribPointer(attribA1, 2, 0x1406, 0, 0, mesh->texCoords);
            if (attribA2 >= 0)
                glVertexAttribPointer(attribA2, 3, 0x1406, 0, 0, mesh->normals);
            if (attribA3 >= 0)
                glVertexAttribPointer(attribA3, 3, 0x1406, 0, 0, mesh->tangents);
            if (attribA4 >= 0)
                glVertexAttribPointer(attribA4, 3, 0x1406, 0, 0, mesh->binormals);
            if (uniformU0 >= 0)
                glVertexAttribPointer(uniformU0, 4, 0x1406, 0, 0, mesh->colors);
        } else {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(attribA0, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(attribA1, 2, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(attribA2, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(attribA3, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(attribA4, 3, 0x1406, 0, 0, 0);
            glBindBuffer(0x8892, mesh->colorVBO);
            glVertexAttribPointer(uniformU0, 4, 0x1406, 0, 0, 0);
        }
    }

    void BumpShaderParticle::SetInActive() {
        if (attribA0 >= 0)
            glDisableVertexAttribArray(attribA0);
        if (attribA1 >= 0)
            glDisableVertexAttribArray(attribA1);
        if (attribA2 >= 0)
            glDisableVertexAttribArray(attribA2);
        if (attribA3 >= 0)
            glDisableVertexAttribArray(attribA3);
        if (attribA4 >= 0)
            glDisableVertexAttribArray(attribA4);
        if (uniformU0 >= 0)
            glDisableVertexAttribArray(uniformU0);
    }
}
