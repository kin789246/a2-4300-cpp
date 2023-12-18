#include "Vec2.h"
#include <math.h>

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin) {}

Vec2 Vec2::operator+(const Vec2& rhs) const {
    // todo
    return Vec2(0, 0);
}

Vec2 Vec2::operator-(const Vec2& rhs) const {
    // todo
    return Vec2(0, 0);
}

Vec2 Vec2::operator/(const float val) const {
    // todo
    return Vec2(0, 0);
}

Vec2 Vec2::operator*(const float val) const {
    // todo
    return Vec2(0, 0);
}

bool Vec2::operator==(const Vec2& rhs) const {
    // todo
    return false;
}

bool Vec2::operator!=(const Vec2& rhs) const {
    // todo
    return false;
}

void Vec2::operator+=(const Vec2& rhs) {
    // todo
}

void Vec2::operator-=(const Vec2& rhs) {
    // todo
}

void Vec2::operator*=(const float val) {
    // todo
}

void Vec2::operator/=(const float val) {
    // todo
}

float Vec2::dist(const Vec2& rhs) const {
    // todo
    return 0;
}
