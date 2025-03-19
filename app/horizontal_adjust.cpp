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

double horiControlNet::getAzimuth(const horiPoint* begin, const horiPoint* end) {
	angleStation* station = nullptr;
	station = findStation(begin);
	double result;
	if (station) {
		for (int i = 0; i < station->valueNum; i++) {
			if (station->values[i].end != end) continue;
			result = sumDeg(station->values[i].surveyVal, station->first_azimuth);
			if (result > 360) result -= 360;
			return result;
		}
	}
	return -1;
}

bool horiControlNet::forwardIntersection(const horiPoint* known1, const horiPoint* known2, horiPoint* unknown) {
	double angle1 = deg2rad(abs(subDeg(getAzimuth(known1, unknown), getAzimuth(known1, known2))));
	double angle2 = deg2rad(abs(subDeg(getAzimuth(known2, unknown), getAzimuth(known2, known1))));
	double angle12 = deg2rad(abs(subDeg(getAzimuth(unknown, known1), getAzimuth(unknown, known2))));
	double length12 = sqrt((known1->X - known2->X) * (known1->X - known2->X) + (known1->Y - known2->Y) * (known1->Y - known2->Y));
	double length1 = length12 * sin(angle2) / sin(angle12);
	double length2 = length12 * sin(angle1) / sin(angle12);
	double azimuth1 = deg2rad(getAzimuth(known1, unknown)), azimuth2 = deg2rad(getAzimuth(known2, unknown));
	unknown->X = (known1->X + length1 * cos(azimuth1) + known2->X + length2 * cos(azimuth2)) / 2;
	unknown->Y = (known1->Y + length1 * sin(azimuth1) + known2->Y + length2 * sin(azimuth2)) / 2;
}

void horiControlNet::solve() {
	getInitialAzimuth();
	if (lengthNum) {	// 边角网
		approxiCoordTriangulateration();
	} else {	// 测角网
		bool* visited = new bool[PointNum];
		for (int i = 0; i < PointNum; i++)
			visited[i] = i < KnownPointNum;
		approxiCoordTriangulation(visited);
	}
}

void horiControlNet::approxiCoordTriangulateration() {
	bool* visited = new bool[PointNum];
	for (int i = 0; i < PointNum; i++)
		visited[i] = i < KnownPointNum;
	
	horiPoint *begin = nullptr, *target = nullptr;	// 处理边长观测值
	for (int i = 0; i < lengthNum; i++) {
		if (visited[lengthData[i].pBegin - pointData] ^ visited[lengthData[i].pEnd - pointData]) {	// 边长观测有一个顶点标记过
			begin = visited[lengthData[i].pBegin - pointData] ? lengthData[i].pBegin : lengthData[i].pEnd;
			target = visited[lengthData[i].pBegin - pointData] ? lengthData[i].pEnd : lengthData[i].pBegin;
		} else continue;
		double azimuth = getAzimuth(begin, target);
		target->X = begin->X + lengthData[i].length * cos(deg2rad(azimuth));
		target->Y = begin->Y + lengthData[i].length * sin(deg2rad(azimuth));
		visited[target - pointData] = true;
	}
	
	bool flag = false;
	for (int i = 0; i < PointNum; i++) {
		if (!visited[i]) {
			flag = true; break;
		}		
	}

	if (flag) {
		approxiCoordTriangulation(visited);
	} else {
		delete[] visited;
		return;
	}

}

void horiControlNet::approxiCoordTriangulation(bool* visited) {
	horiPoint* unknown = nullptr, *known1 = nullptr, *known2 = nullptr;
	angleStation* tempStation1 = nullptr;
	bool flag = false;
	while (true) {
		flag = false;
		for (int i = 0; i < PointNum; i++) {	// 一个未知站点
			if (visited[i]) continue;
			unknown = &pointData[i];
			tempStation1 = findStation(unknown);
			if (!tempStation1) continue;
			for (int j = 0; j < tempStation1->valueNum; j++) {	// 该未知站点观测的两个已知点
				if (!visited[tempStation1->values[j].end - pointData]) continue;
				known1 = tempStation1->values[j].end;			// 第一个已知点
				for (int k = 0; k < tempStation1->valueNum; k++) {
					if (getAzimuth(known1, tempStation1->values[k].end) != -1) {
						known2 = tempStation1->values[k].end;	// 第二个已知点
						break;
					}
				}
				if (known1 && known2) {
					forwardIntersection(known1, known2, unknown);
					visited[i] = true;
					flag = true;
					break;
				}
			}
		}
		if (!flag) break;	// 所有点都已知，结束
	}
	delete[] visited;
	return;
}

void horiControlNet::getInitialAzimuth() {
	// 获取整张网的各边方位角
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