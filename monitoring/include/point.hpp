#ifndef __POINT_HPP__
#define __POINT_HPP__

#include <iostream>
using namespace std;

class Point {
    public:
    double x;
    double y;
    int coordinate_system; // 0: kartesian 1: qt

    Point(double x=0, double y=0, int cs=0);
    double len();
    Point operator+(Point);
    Point operator-(Point);
    Point operator*(double);
    Point operator/(double);
    Point operator==(Point);
};

ostream& operator<<(ostream&, Point);

#endif