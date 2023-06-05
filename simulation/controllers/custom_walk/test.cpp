#include <cmath>
#include <iostream>

using namespace std;

int main() {
    cout << atan2(1,1)*180.0/M_PI << endl;
    cout << atan2(-1,1)*180.0/M_PI << endl;
    cout << atan2(1,-1)*180.0/M_PI << endl;
    cout << atan2(-1,-1)*180.0/M_PI << endl;
}