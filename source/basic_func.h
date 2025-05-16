#pragma once
const double M_PI = 3.14159265358979323846;
double deg2rad(int deg, int min, int sec);
double deg2rad(double deg);
double rad2deg(double rad);

double azimuth(double delx, double dely);
double subDeg(double deg1, double deg2);
double sumDeg(double deg1, double deg2);