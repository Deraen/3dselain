#ifndef MATRIX_HH
#define MATRIX_HH

#include <cassert>
#include "vec3.hh"

class Matrix {
public:
    // M*N materisi, m rivit, n sarakkeet
    Matrix(const unsigned int m, const unsigned int n);
    Matrix(const Matrix& b);
    ~Matrix();

    Matrix& operator=(const Matrix& b);
    Matrix& operator<<(const double d);
    // Matrix * Matrix
    Matrix operator*(const Matrix& b) const;
    // Matrix += Matrix
    Matrix& operator+=(const Matrix& b);
    // Matrix * int
    // Matrix& operator*=(const double& b);
    // Vec3 = Vec * Matrix
    Vec3 operator*(const Vec3& b) const;

    // i rivi, j sarake
    void set(const unsigned int i, const unsigned int j, const double d);
    double get(const unsigned int i, const unsigned int j) const;

    void unhomogenize();

    double* data();
private:
    // Alustuksen apu. varaa sopivan taulukon
    void init(const unsigned int m, const unsigned int n, const unsigned int k = 0);

    // Tarkistukset joita tehdään useammasta kohtaa
    // Inline saattaa olla hyvä idea jotta jos assertit halutaan optimoida pois,
    // että ei kutsuta tyhjää funktiota. Kääntäjä tosin varmaan tajuaa tämän.
    inline void assertRowColumn(const unsigned int i, const unsigned int j) const {
        assert(i <= m_ && "Too great row number");
        assert(j <= n_ && "Too great column number");
    }

    unsigned int m_; // rivit
    unsigned int n_; // sarakket
    unsigned int k_; // montako alkiota lisätty
    double* d_; // matriisin sisältä kaksi ulotteisena taulukkona
};

#endif
