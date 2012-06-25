#ifndef GLCOMMON_HH
#define GLCOMMON_HH

#pragma pack(1)
struct GLVertex {
    float x, y, z;
    float nx, ny, nz;
    // 6 * 4 bytes = 24

    // float u, v;

    GLVertex(float x_, float y_, float z_):
        x(x_), y(y_), z(z_),
        nx(0.0), ny(0.0), nz(0.0)//,
        // u(0.0), v(0.0)
    {}

    void setNormal(float x_, float y_, float z_) {
        nx = x_; ny = y_; nz = z_;
    }

    void setTexture(float u_, float v_) {
        // u = u_; v = v_;
    }
};
#pragma pack()

#pragma pack(1)
struct GLFace {
    unsigned int a;
    unsigned int b;
    unsigned int c;

    // 3 * 2 bytes = 6... fuu
    // char padding[2];

    GLFace(unsigned int a_, unsigned int b_, unsigned int c_):
        a(a_), b(b_), c(c_)
    {}
};
#pragma pack()

#endif
