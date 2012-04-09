#ifndef MATRIX_HH
#define MATRIX_HH

// projektio työstä

#include <cassert>

enum Coordinate { X, Y, Z, W };

class Matrix {
public:
    // M*N materisi, m rivit, n sarakkeet
    Matrix(const unsigned int m, const unsigned int n);
    // Homogeenisten koordinaattimatriisien rakentaja
    Matrix(const double x, const double y, const double z);
    Matrix(const Matrix& b);
    ~Matrix();

    Matrix& operator=(const Matrix& b);
    Matrix& operator<<(const double d);
    // Matrix * Matrix
    Matrix operator*(const Matrix& b) const;
    // Matrix += Matrix
    Matrix& operator+=(const Matrix& b);

    // i rivi, j sarake
    void set(const unsigned int i, const unsigned int j, const double d);
    double get(const unsigned int i, const unsigned int j) const;
    // helpompi käsitellä piste-matriiseita
    void set(const Coordinate i, const double d);
    double get(const Coordinate i) const;

    void unhomogenize();
private:
    // Alustuksen apu. varaa sopivan taulukon
    void init(const unsigned int m, const unsigned int n, const unsigned int k = 0);

    // Tarkistukset joita tehdään useammasta kohtaa
    // Inline saattaa olla hyvä idea jotta jos assertit halutaan optimoida pois,
    // että ei kutsuta tyhjää funktiota. Kääntäjä tosin varmaan tajuaa tämän.
    inline void assertCoordinateMatrix() const {
        assert(m_ == 4 && n_ == 1 && "This operation is only for coordinate matrixes.");
    }
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
