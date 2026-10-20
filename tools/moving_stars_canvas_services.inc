// Fixture mesh storage only; MovingStars and Globals::createBillBoard stay production bodies.
Globals::Globals() {}
Globals::~Globals() {}

void PaintCanvas::MeshCreate(unsigned short vertices, unsigned short triangles,
                             signed char meshType, unsigned short materialId,
                             unsigned int &out) {
    assert(vertices == 12 && triangles == 6 && meshType == 19 && materialId == 20052);
    auto *resource = static_cast<AbyssEngine::Resource *>(FindResource(materialId));
    assert(resource && resource->kind == 6);
    unsigned material = 0;
    MaterialCreate(materialId, material);
    auto *mesh = new AbyssEngine::Mesh(nullptr);
    mesh->animation = nullptr;
    mesh->vertexCount = vertices;
    mesh->indexCount = triangles * 3;
    mesh->positions = new float[vertices * 3]{};
    mesh->texCoords = new float[vertices * 2]{};
    mesh->indices = new uint16_t[triangles * 3]{};
    ArrayAdd(mesh, *reinterpret_cast<::Array<AbyssEngine::Mesh *> *>(&meshCount));
    out = meshCount - 1;
}

float PaintCanvas::MeshSetPoint(unsigned int id, unsigned short vertex,
                                float x, float y, float z) {
    assert(id < meshCount);
    auto *mesh = reinterpret_cast<AbyssEngine::Mesh *>(meshes[id]);
    assert(vertex < mesh->vertexCount);
    float *point = static_cast<float *>(mesh->positions) + vertex * 3;
    point[0] = x;
    point[1] = y;
    point[2] = z;
    return z;
}

void PaintCanvas::MeshSetUv(unsigned int id, unsigned short vertex, float u, float v) {
    assert(id < meshCount);
    auto *mesh = reinterpret_cast<AbyssEngine::Mesh *>(meshes[id]);
    assert(vertex < mesh->vertexCount);
    float *uv = static_cast<float *>(mesh->texCoords) + vertex * 2;
    uv[0] = u;
    uv[1] = v;
}

void PaintCanvas::MeshSetTriangle(unsigned int id, unsigned short triangle,
                                  unsigned short a, unsigned short b, unsigned short c) {
    assert(id < meshCount);
    auto *mesh = reinterpret_cast<AbyssEngine::Mesh *>(meshes[id]);
    assert(triangle * 3 + 2 < mesh->indexCount);
    auto *indices = static_cast<uint16_t *>(mesh->indices) + triangle * 3;
    indices[0] = a;
    indices[1] = b;
    indices[2] = c;
}
