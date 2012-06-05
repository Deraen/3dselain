#include <cassert>

#include "matrix.hh"

Matrix::Matrix(const unsigned int m, const unsigned int n) {
    init(m, n);
}

Matrix::Matrix(const Matrix& b) {
    init(b.m_, b.n_, b.k_);
    for (unsigned int i = 0; i < m_ * n_; ++i) {
        d_[i] = b.d_[i];
    }
}

Matrix::~Matrix() {
    delete[] d_;
}

void Matrix::init(const unsigned int m, const unsigned int n, const unsigned int k) {
    m_ = m;
    n_ = n;
    k_ = k;
    d_ = new float[m_ * n_];
}

Matrix& Matrix::operator<<(const float d) {
    assert(k_ < m_ * n_ && "Can't add more values.");
    d_[k_] = d;
    ++k_;
    return *this;
}

Matrix& Matrix::operator=(const Matrix& b) {
    init(b.m_, b.n_, b.k_);
    // kopioidaan sisältö
    for (unsigned int i = 0; i < m_ * n_; ++i) {
        d_[i] = b.d_[i];
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& b) const {
    assert(n_ == b.m_ && "Can't multiplicate matrixes, left column count != right row count.");
    Matrix result(m_, b.n_);

    // rivit - molemmilla saman verran
    for (unsigned int i = 0; i < result.m_; ++i) {
        // oikean sarakkeet = tulos sarakkeet
        for (unsigned int j = 0; j < result.n_; ++j) {
            float r = 0;
            // vasemman sarakkeet
            for (unsigned int k = 0; k < n_; ++k) {
                r += get(i, k) * b.get(k, j);
            }
            result.set(i, j, r);
        }
    }
    return result;
}


// Matrix& Matrix::operator*=(const float& b) {
//     // rivit - molemmilla saman verran
//     for (unsigned int i = 0; i < m_ * n_; ++i) {
//         d_[i] *= b;
//     }
//     return *this;
// }

void Matrix::set(const unsigned int i, const unsigned int j, const float d) {
    assertRowColumn(i, j);
    d_[n_ * i + j] = d;
}

float Matrix::get(const unsigned int i, const unsigned int j) const {
    assertRowColumn(i, j);
    return d_[n_ * i + j];
}

// void Matrix::unhomogenize() {
//     assertCoordinateMatrix();
//     set(X, get(X) / get(W));
//     set(Y, get(Y) / get(W));
//     set(Z, 0);
//     set(W, 0);
// }


//         1
//         2
//         3
//         0 // turha
// a b c d = a + 2*b + 3*c + 0*d
// e f h i = e + 2*f + 3*h + 0*i
// j k l m = j + 2*k + 3*l + 0*m
Vec3 Matrix::operator *(Vec3 const& b) const {
    assert(m_ == 4 && n_ == 4 && "Fuu");
    return Vec3(b.x * get(0, 0) + b.y * get(0, 1) + b.z * get(0, 2),
               b.x * get(1, 0) + b.y * get(1, 1) + b.z * get(1, 2),
               b.x * get(2, 0) + b.y * get(2, 1) + b.z * get(2, 2));
}

float* Matrix::data() {
    return d_;
}
