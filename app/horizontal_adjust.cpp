#include"horizontal_adjust.h"
#include<fstream>
#include<sstream>
#include<iostream>
#include<string>

void horiControlNet::readData(const char* fileName) {
	std::ifstream file(fileName);
	if (!file) {
		std::cerr << "File open error!" << std::endl;
		exit(1);
	}

	int surveyNum1, surveyNum2, num1, num2;
	file >> PointNum >> KnownPointNum >> angleStationNum >> angleNum_All >> lengthNum >> azimuthNum;
	int n1, n2, n3, n4;
	file >> n1 >> n2 >> n3 >> n4;
	
	// 点数据读取
	pointData = new horiPoint[PointNum];
	char name[50];
	double x, y;
	for (int i = 0; i < KnownPointNum; i++) {
		file >> name >> x >> y;
		pointData[i] = horiPoint(name, x, y);
	}
	int tempPoint = KnownPointNum;

	// 角度观测读取
	angleStations = new angleStation[angleStationNum];
	horiPoint* begin = nullptr,  *end = nullptr;
	int surveyNum;
	for (int i = 0; i < angleStationNum; i++) {
		file >> name >> surveyNum;
		begin = findPoint(name);
		if (!begin) {
			pointData[tempPoint] = horiPoint(name);
			begin = &pointData[tempPoint++];
		}
		angleStations[i].pBegin = begin;
		angleStations[i].setValueNum(surveyNum);

		for (int j = 0; j < surveyNum; j++) {
			file >> name >> x;
			end = findPoint(name);
			if (!end) {
				pointData[tempPoint] = horiPoint(name);
				end = &pointData[tempPoint++];
			}
			angleStations[i].values[j].end = end;
			angleStations[i].values[j].surveyVal = x;
		}
	}

	// 边长与方位角读取
	char name2[50];
	if (lengthNum) {
		lengthData = new lengthSurveyVal[lengthNum];
		for (int i = 0; i < lengthNum; i++) {
			file >> name >> name2 >> x;
			lengthData[i] = lengthSurveyVal(findPoint(name), findPoint(name2), x);
		}
	}
	if (azimuthNum) {
		azimuthData = new AzimuthVal[azimuthNum];
		for (int i = 0; i < azimuthNum; i++) {
			file >> name >> name2 >> x;
			azimuthData[i] = AzimuthVal(findPoint(name), findPoint(name2), x);
		}
	}
}

horiPoint* horiControlNet::findPoint(const char* name) {
	for (int i = 0; i < PointNum; ++i) {
		if (strcmp(pointData[i].Name, name) == 0)
			return &pointData[i];
	}
	return nullptr;
}

void horiControlNet::solve() {

}