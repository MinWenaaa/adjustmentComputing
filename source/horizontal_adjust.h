#pragma once
#include<string>

class horiPoint {
public:
	horiPoint() : Name(""), known(false), X(0), Y(0) {}
	horiPoint(const char name[]) { strcpy_s(this->Name, name); }
	horiPoint(const char name[], double X, double Y) : X(X), Y(Y), known(true) {
		strcpy_s(this->Name, name);
	}

	char Name[50];
	bool known;
	double X, Y;
};
// ----------------------------------------------------------------------------
struct angleSurvey {
	horiPoint* end;
	double surveyVal;
	angleSurvey() : end(nullptr), surveyVal(0) {}
};
class angleStation {
public:
	angleStation() : pBegin(nullptr), values(nullptr), first_azimuth(0) {}
	~angleStation() { delete[] values;}
	bool setValueNum(int num) {
		if (num <= 0) return false;
		valueNum = num;
		values = new angleSurvey[num];
		return true;
	}

	int valueNum;
	double first_azimuth;
	horiPoint* pBegin;
	angleSurvey* values;
};
//------------------------------------------------------------------------------
struct lengthSurveyVal {
	lengthSurveyVal() : pBegin(nullptr), pEnd(nullptr), length(0) {}
	lengthSurveyVal(horiPoint* begin, horiPoint* end, double l) : pBegin(begin), pEnd(end), length(l) {}
	horiPoint* pBegin, * pEnd;
	double length;
};
struct AzimuthVal {
	AzimuthVal() : pBegin(nullptr), pEnd(nullptr), azimuth(0) {}
	AzimuthVal(horiPoint* begin, horiPoint* end, double l): pBegin(begin), pEnd(end), azimuth(l) {}
	horiPoint* pBegin, * pEnd;
	double azimuth;
};
//------------------------------------------------------------------------------
class horiControlNet {
public:
	horiControlNet() : PointNum(0), KnownPointNum(0), angleStationNum(0), angleNum_All(0), lengthNum(0), azimuthNum(0),
		pointData(nullptr), angleStations(nullptr), lengthData(nullptr), azimuthData(nullptr), painted(false) {}
	~horiControlNet() {
		delete[] pointData;
		delete[] angleStations;
		delete[] lengthData;
		delete[] azimuthData;
	}

	void readData(const char* filename);
	std::string solve();
	std::string toString();
	bool painted;
	void OnDraw(CDC* pDC);

private:
	horiPoint* findPoint(const char* name);
	angleStation* findStation(const horiPoint* begin);
	double getAzimuth(const horiPoint* begin, const horiPoint* end);
	bool forwardIntersection(const horiPoint* known1, const horiPoint* known2, horiPoint* unknown);
	void approxiCoordTriangulateration();
	void approxiCoordTriangulation(bool* visited);
	void getInitialAzimuth();

	int PointNum, KnownPointNum, angleStationNum, angleNum_All, lengthNum, azimuthNum;
	horiPoint* pointData;
	angleStation* angleStations;
	lengthSurveyVal* lengthData;
	AzimuthVal* azimuthData;
};
