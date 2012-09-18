#ifndef GLDATA_HH
#define GLDATA_HH

/*
--- GlDataFile formaatti ---
binääri muotoisena.

32bit meshien lkm
32bit vertexien lkm
32bit pintojen lkm
32bit materiaalien lkm

materiaalien lkm *
    Scene::Material = (3 * 32bit float + 3 * 32bit float + 32bit float opacity)

meshien lkm *
    Scene::Mesh = (32bit meshin ensimmäisen pinnan numero + 32bit meshin pintojen lkm + 32bit materiaalin numero)

vertexien lkm *
    GLVertex = (3 * 64bit double + 3 * 32bit float)

pintojen lkm *
    GLFace = (3 * 32bit vertexin numero)

*/

struct GlDataHeader {
    // unsigned int materials;
    unsigned int meshes;
    unsigned int vertexes;
    unsigned int faces;
};

const unsigned int GLDATAHEADER_BYTES = sizeof(GlDataHeader);

union GlDataHeaderUnion {
    GlDataHeader data;
    char chars[GLDATAHEADER_BYTES];
};

#endif
