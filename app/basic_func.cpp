#include "pch.h"
#include "basic_func.h"
#include <cmath>

const double M_PI = 3.14159265358979323846;

double deg2rad(int deg, int min, int sec) {
	return (deg + min / 60 + sec / 3600) * M_PI / 180;
}
double rad2deg(double rad) {
	double deg = rad * 180 / M_PI;
	double min = (deg - int(deg)) * 60;
	double sec = (min - int(min)) * 60;
	return int(deg) + int(min) * 0.01 + int(sec) * 0.0001;
}

double azimuth(double delx, double dely) {
	if (delx == 0) {
		if (dely > 0) return 90;
		else if (dely < 0) return 270;
		else return 0;
	}
	double temp = atan(std::abs(dely / delx));
	if (delx > 0) {
		if (dely > 0) return rad2deg(temp);
		else return rad2deg( 2*M_PI - temp );
	} else {
		if(dely > 0) return rad2deg(M_PI - temp);
		else return rad2deg(M_PI+temp);
	}
}

double subDeg(double deg1, double deg2) {
	int d1 = int(deg1), d2 = int(deg2);
	int m1 = int((deg1 - d1) * 100), m2 = int((deg2 - d2) * 100);
	int s1 = int((deg1 - d1 - m1 * 0.01) * 10000), s2 = int((deg2 - d2 - m2 * 0.01) * 10000);
	int result_s = s1 - s2;
	if (result_s < 0) {
		result_s += 60;
		m1--;
	}
	int result_m = m1 - m2;
	if (result_m < 0) {
		result_m += 60;
		d1--;
	}
	int result_d = d1 - d2;
	return result_d + result_m * 0.01 + result_s * 0.0001;
}

double sumDeg(double deg1, double deg2) {
	int d1 = int(deg1), d2 = int(deg2);
	int m1 = int((deg1 - d1) * 100), m2 = int((deg2 - d2) * 100);
	int s1 = int((deg1 - d1 - m1 * 0.01) * 10000), s2 = int((deg2 - d2 - m2 * 0.01) * 10000);
	int result_s = s1 + s2;
	if (result_s >= 60) {
		result_s -= 60;
		m1++;
	}
	int result_m = m1 + m2;
	if (result_m >= 60) {
		result_m -= 60;
		d1++;
	}
	int result_d = d1 + d2;
	return result_d + result_m * 0.01 + result_s * 0.0001;
}