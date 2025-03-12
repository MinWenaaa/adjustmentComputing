#include "pch.h"
#include "basic_func.h"
#include <math.h>

const double M_PI = 3.14159265358979323846;

double deg2rad(int deg, int min, int sec) {
	return (deg + min / 60 + sec / 3600) * M_PI / 180;
}
double rad2deg(double rad) {
	return rad * 180 / M_PI;
}

double azimuth(double delx, double dely) {
	if (delx == 0) {
		if (dely > 0) return 90;
		else if (dely < 0) return 270;
		else return 0;
	}
	else if (delx > 0 && dely >= 0) 
		return rad2deg(atan(dely / delx));
	else if (delx < 0)
		return rad2deg(atan(dely / delx)) + 180;
	else 
		return rad2deg(atan(dely / delx)) + 360;	
}