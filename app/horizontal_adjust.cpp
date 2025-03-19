#include"horizontal_adjust.h"
#include"basic_func.h"
#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<queue>

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
angleStation* horiControlNet::findStation(const horiPoint* begin) {
	for (int i = 0; i < angleStationNum; ++i) {
		if (angleStations[i].pBegin == begin)
			return &angleStations[i];
	}
	return nullptr;
}

void horiControlNet::solve() {
	if (lengthNum) {	// 边角网
		approxiCoordTrilateration();
	} else {	// 测角网
		approxiCoordTriangulation();
	}
}

void horiControlNet::approxiCoordTrilateration() {
	getInitialAzimuth();
}

void horiControlNet::getInitialAzimuth() {
	bool* visited = new bool[angleStationNum];
	angleStation* tempStation = nullptr;
	horiPoint* begin = nullptr, * end = nullptr;
	for (int i = 0; i < angleStationNum; i++) {	// 根据已知点计算方向角
		tempStation = &angleStations[i];
		if (!tempStation->pBegin->known) continue;
		for (int j = 0; j < tempStation->valueNum; j++) {
			if (tempStation->values[j].end->known) {
				begin = tempStation->pBegin; end = tempStation->values[j].end;
				tempStation->first_azimuth = azimuth(end->X - begin->X, end->Y - begin->Y) - tempStation->values[j].surveyVal;
				visited[i] = true;
				break;
			}
		}
	}

	AzimuthVal* tempAzimuth = nullptr;
	horiPoint* target = nullptr;
	for (int i = 0; i < azimuthNum; i++) {	// 根据方向角观测值计算
		tempAzimuth = &azimuthData[i];
		for (int j = 0; j < angleStationNum; j++) {
			if (visited[j]) continue;
			tempStation = &angleStations[j];
			if (tempStation->pBegin == tempAzimuth->pBegin) target = tempAzimuth->pEnd;
			else if (tempStation->pBegin == tempAzimuth->pEnd) target = tempAzimuth->pBegin;
			else continue;

			for (int k = 0; k < tempStation->valueNum; k++) {
				if (tempStation->values[k].end == target) {
					tempStation->first_azimuth = subDeg(tempAzimuth->azimuth, tempStation->values[k].surveyVal);
					visited[j] = true;
					break;
				}
			}
		}
	}

	std::queue<angleStation*> stationQueue;		// 广度优先遍历整张网
	for (int i = 0; i < angleStationNum; i++) {
		if (visited[i]) stationQueue.push(&angleStations[i]);
	}
	angleStation* nextStation = nullptr;
	while (!stationQueue.empty()) {
		tempStation = stationQueue.front();
		stationQueue.pop();
		for (int i = 0; i < tempStation->valueNum; i++) {	// 对每条观测，尝试将终点测站标记并加入队列
			nextStation = findStation(tempStation->values[i].end);
			if (!nextStation) continue;		// 终点不是测站
			if (visited[nextStation - angleStations]) continue;	// 已经标记过
			for (int j = 0; j < nextStation->valueNum; i++) {	// 计算方向角并标记
				if (nextStation->values[j].end != tempStation->pBegin) continue;
				double temp = sumDeg(tempStation->first_azimuth, tempStation->values[i].surveyVal) + 180;
				if (temp > 360) temp -= 360;
				nextStation->first_azimuth = subDeg(temp, nextStation->values[j].surveyVal);
				stationQueue.push(nextStation);
				visited[nextStation - angleStations] = true;
			}
		}
	}

	delete[] visited;
	return;
}