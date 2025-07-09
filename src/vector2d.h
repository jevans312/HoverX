#pragma once

class vector2d {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    // Constructors
    vector2d() = default;
    vector2d(float X, float Y) : x(X), y(Y), z(0.0f) {}

    // Arithmetic operators
    vector2d operator+(const vector2d& tvec) const {
        return vector2d(x + tvec.x, y + tvec.y);
    }
    vector2d operator-(const vector2d& tvec) const {
        return vector2d(x - tvec.x, y - tvec.y);
    }
    vector2d operator*(const vector2d& tvec) const {
        return vector2d(x * tvec.x, y * tvec.y);
    }
    vector2d operator/(const vector2d& tvec) const {
        return vector2d(x / tvec.x, y / tvec.y);
    }

    // Compound assignment operators
    vector2d& operator+=(const vector2d& tvec) {
        x += tvec.x;
        y += tvec.y;
        return *this;
    }
    vector2d& operator-=(const vector2d& tvec) {
        x -= tvec.x;
        y -= tvec.y;
        return *this;
    }

    // Scalar operations
    vector2d& scalefr(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    void scalef(float scalar) {
        x *= scalar;
        y *= scalar;
    }
    vector2d& divfr(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    void divf(float scalar) {
        x /= scalar;
        y /= scalar;
    }

    // Utility functions
    vector2d getperp() const {
        return vector2d(-y, x);
    }
    float magsqr() const {
        return x * x + y * y;
    }
    void setpos(float tx, float ty) {
        x = tx;
        y = ty;
    }
};
