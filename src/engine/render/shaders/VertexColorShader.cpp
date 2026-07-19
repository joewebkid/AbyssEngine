#include "engine/render/shaders/VertexColorShader.h"
#include "engine/render/Engine.h"
#include "engine/render/Mesh.h"
#include <GLES2/gl2.h>

namespace AbyssEngine {
    int VertexColorShader::ShaderIndex;

    VertexColorShader::VertexColorShader() {
        VertexColorShader::ShaderIndex = ShaderBaseStruct::shaderIndexIntern;
        this->name = u"VertexColorShader";
    }

    void VertexColorShader::Init(Engine *) {
        this->program = this->ES2LoadProgram("vtx_color_vs", "vtx_color_fs");

        this->aColor = glGetAttribLocation(this->program, "color");
        this->aPosition = glGetAttribLocation(this->program, "position");
        this->aTexCoord = glGetAttribLocation(this->program, "texcoord");
        this->aNormal = glGetAttribLocation(this->program, "normal");
        this->aTangent = glGetAttribLocation(this->program, "tangent");
        this->aBiNormal = glGetAttribLocation(this->program, "binormal");

        this->uWorldViewProj = glGetUniformLocation(this->program, "worldViewProj");
        this->uNormalMatrix = glGetUniformLocation(this->program, "normalMatrix");
        this->uLightDir = glGetUniformLocation(this->program, "lightDir");
        this->uLightColor = glGetUniformLocation(this->program, "lightColor");
        this->uAmbientColor = glGetUniformLocation(this->program, "ambientColor");
        this->uMaterialDiffuse = glGetUniformLocation(this->program, "materialDiffuse");
        this->uMaterialAmbient = glGetUniformLocation(this->program, "materialAmbient");
        this->uMaterialSpecular = glGetUniformLocation(this->program, "materialSpecular");
        this->uMaterialShininess = glGetUniformLocation(this->program, "materialShininess");

        glUseProgram(this->program);
        glUniform1i(this->uAmbientColor, 0);
    }

    void VertexColorShader::UpdateMeshData(Mesh *mesh, Engine *engine) {
        if (this->uWorldViewProj >= 0) {
            glUniformMatrix4fv(this->uWorldViewProj, 1, false, engine->worldViewProjMatrix);
        }
        if (this->uNormalMatrix >= 0) {
            glUniformMatrix3fv(this->uNormalMatrix, 1, false, engine->normalMatrix);
        }

        if (this->dirty != 0) {
            if (this->uLightDir >= 0) {
                glUniform3f(this->uLightDir, engine->lightDir.x, engine->lightDir.y, engine->lightDir.z);
            }
            if (this->uLightColor >= 0) {
                glUniform3f(this->uLightColor, engine->lightColor.x, engine->lightColor.y, engine->lightColor.z);
            }
            if (this->uMaterialDiffuse >= 0) {
                glUniform4fv(this->uMaterialDiffuse, 1, engine->glColor);
            }
            if (this->uMaterialAmbient >= 0) {
                glUniform4fv(this->uMaterialAmbient, 1, engine->materialAmbient);
            }
            if (this->uMaterialSpecular >= 0) {
                glUniform4fv(this->uMaterialSpecular, 1, engine->materialDiffuse);
            }
            if (this->uMaterialShininess >= 0) {
                glUniform4fv(this->uMaterialShininess, 1, engine->materialSpecular);
            }
            this->dirty = 0;
        }

        if (this->aPosition >= 0) {
            glEnableVertexAttribArray(this->aPosition);
        }
        if (this->aTexCoord >= 0) {
            glEnableVertexAttribArray(this->aTexCoord);
        }
        if (this->aNormal >= 0) {
            glEnableVertexAttribArray(this->aNormal);
        }
        if (this->aTangent >= 0) {
            glEnableVertexAttribArray(this->aTangent);
        }
        if (this->aBiNormal >= 0) {
            glEnableVertexAttribArray(this->aBiNormal);
        }
        if (this->aColor >= 0) {
            glEnableVertexAttribArray(this->aColor);
        }

        if (mesh->uploaded != 0) {
            glBindBuffer(0x8892, mesh->positionVBO);
            glVertexAttribPointer(this->aPosition, 3, 0x1406, false, 0, 0);
            glBindBuffer(0x8892, mesh->texCoordVBO);
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, false, 0, 0);
            glBindBuffer(0x8892, mesh->normalVBO);
            glVertexAttribPointer(this->aNormal, 3, 0x1406, false, 0, 0);
            glBindBuffer(0x8892, mesh->tangentVBO);
            glVertexAttribPointer(this->aTangent, 3, 0x1406, false, 0, 0);
            glBindBuffer(0x8892, mesh->binormalVBO);
            glVertexAttribPointer(this->aBiNormal, 3, 0x1406, false, 0, 0);
            glBindBuffer(0x8892, mesh->colorVBO);
            glVertexAttribPointer(this->aColor, 4, 0x1406, false, 0, 0);
            return;
        }

        if (this->aPosition >= 0) {
            glVertexAttribPointer(this->aPosition, 3, 0x1406, false, 0, mesh->positions);
        }
        if (this->aTexCoord >= 0) {
            glVertexAttribPointer(this->aTexCoord, 2, 0x1406, false, 0, mesh->texCoords);
        }
        if (this->aNormal >= 0) {
            glVertexAttribPointer(this->aNormal, 3, 0x1406, false, 0, mesh->normals);
        }
        if (this->aTangent >= 0) {
            glVertexAttribPointer(this->aTangent, 3, 0x1406, false, 0, mesh->tangents);
        }
        if (this->aBiNormal >= 0) {
            glVertexAttribPointer(this->aBiNormal, 3, 0x1406, false, 0, mesh->binormals);
        }
        if (this->aColor >= 0) {
            glVertexAttribPointer(this->aColor, 4, 0x1406, false, 0, mesh->colors);
        }
    }

    void VertexColorShader::SetInActive() {
        if (this->aPosition >= 0) {
            glDisableVertexAttribArray(this->aPosition);
        }
        if (this->aTexCoord >= 0) {
            glDisableVertexAttribArray(this->aTexCoord);
        }
        if (this->aNormal >= 0) {
            glDisableVertexAttribArray(this->aNormal);
        }
        if (this->aTangent >= 0) {
            glDisableVertexAttribArray(this->aTangent);
        }
        if (this->aBiNormal >= 0) {
            glDisableVertexAttribArray(this->aBiNormal);
        }
        if (this->aColor >= 0) {
            glDisableVertexAttribArray(this->aColor);
        }
    }
}
