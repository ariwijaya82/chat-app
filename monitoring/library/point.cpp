#include "point.hpp"
#include <cmath>

Point::Point(double x, double y, int cs) {
    this->x = x;
    this->y = y;
    this->coordinate_system = cs;
}

double Point::len() {
    return sqrt(x*x+y*y);
}

Point Point::operator+(Point p) {
    return Point(x+p.x, y+p.y);
}

Point Point::operator-(Point p) {
    return Point(x-p.x, y-p.y);
}

Point Point::operator*(double s) {
    return Point(x*s, y*s);
}

Point Point::operator/(double s) {
    return Point(x/s, y/s);
}

Point Point::operator==(Point p) {
    return Point(p.x, p.y);
}

ostream& operator<<(ostream& os, Point p) {
    return os << "{" << p.x << ", " << p.y << "}";
}