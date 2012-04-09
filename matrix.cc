
#include "matrix.hh"

Matrix::Matrix(const unsigned int m, const unsigned int n) {
    init(m, n);
}

Matrix::Matrix(const double x, const double y, const double z) {
    init(4, 1);
    *this << x << y << z << 1;
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
    d_ = new double[m_ * n_];
}

Matrix& Matrix::operator<<(const double d) {
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
            double r = 0;
            // vasemman sarakkeet
            for (unsigned int k = 0; k < n_; ++k) {
                r += get(i, k) * b.get(k, j);
            }
            result.set(i, j, r);
        }
    }
    return result;
}

Matrix& Matrix::operator+=(const Matrix& b) {
    assertCoordinateMatrix();
    set(X, get(X) + b.get(X));
    set(Y, get(Y) + b.get(Y));
    set(Z, get(Z) + b.get(Z));
    return *this;
}

void Matrix::set(const unsigned int i, const unsigned int j, const double d) {
    assertRowColumn(i, j);
    d_[n_ * i + j] = d;
}

void Matrix::set(const Coordinate i, const double d) {
    assertCoordinateMatrix();
    set(i, 0, d);
}

double Matrix::get(const unsigned int i, const unsigned int j) const {
    assertRowColumn(i, j);
    return d_[n_ * i + j];
}

double Matrix::get(const Coordinate i) const {
    assertCoordinateMatrix();
    return get(i, 0);
}

void Matrix::unhomogenize() {
    assertCoordinateMatrix();
    set(X, get(X) / get(W));
    set(Y, get(Y) / get(W));
    set(Z, 0);
    set(W, 0);
}
