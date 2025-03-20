#include "pch.h"
#include"horizontal_adjust.h"
#include"basic_func.h"
#include"matrix.hpp"
#include<fstream>
#include<sstream>
#include<iostream>
#include<string>
#include<iomanip>
#include<queue>

void horiControlNet::readData(const char* fileName) {
	std::ifstream file(fileName);
	if (!file) {
		std::cerr << "File open error!" << std::endl;
		exit(1);
	}

	file >> PointNum >> KnownPointNum >> angleStationNum >> angleNum_All >> lengthNum >> azimuthNum;
	double n1, n2, n3, n4;
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
	double length1 = abs(length12 * sin(angle2) / sin(angle12));
	double length2 = abs(length12 * sin(angle1) / sin(angle12));
	double azimuth1 = deg2rad(getAzimuth(known1, unknown)), azimuth2 = deg2rad(getAzimuth(known2, unknown));
	unknown->X = (known1->X + length1 * cos(azimuth1) + known2->X + length2 * cos(azimuth2)) / 2;
	unknown->Y = (known1->Y + length1 * sin(azimuth1) + known2->Y + length2 * sin(azimuth2)) / 2;
	return true;
}

std::string horiControlNet::solve() {
	getInitialAzimuth();
	if (lengthNum) {	// 边角网
		approxiCoordTriangulateration();
	} else {	// 测角网
		bool* visited = new bool[PointNum];
		for (int i = 0; i < PointNum; i++)
			visited[i] = i < KnownPointNum;
		approxiCoordTriangulation(visited);
	}
	painted = true;

	WenMin::Matrix<double> B(angleNum_All + lengthNum + azimuthNum, PointNum * 2 + angleStationNum);
	WenMin::Matrix<double> L(angleNum_All + lengthNum + azimuthNum, 1);
	WenMin::Matrix<double> P(angleNum_All + lengthNum + azimuthNum, angleNum_All + lengthNum + azimuthNum);

	int i = 0, stationNum = 0;
	angleStation* temp = nullptr;
	horiPoint* begin = nullptr, * end = nullptr;
	double S2 = 0, aij = 0, bij = 0;
	for (; i < angleNum_All;) {
		temp = &angleStations[stationNum]; begin = temp->pBegin;
		for (int j = 0; j < temp->valueNum; j++,i++) {	// 一个观测值
			end = temp->values[j].end;
			S2 = pow(begin->X - end->X, 2) + pow(begin->Y - end->Y, 2);
			aij = (begin->Y - end->Y) / S2; bij = -(begin->X - end->X) / S2;
			B(i, (begin - pointData) * 2) = aij;
			B(i, (begin - pointData) * 2 + 1) = bij;
			B(i, (end - pointData) * 2) = -aij;
			B(i, (end - pointData) * 2 + 1) = -bij;
			B(i, PointNum * 2 + stationNum) = -1;
			L(i, 0) = subDeg(subDeg(azimuth(end->X - begin->X, end->Y - begin->Y), temp->first_azimuth), temp->values[j].surveyVal);
			P(i, i) = 1;
		}
		stationNum++;
	}

	lengthSurveyVal* temp2 = nullptr;
	for (; i < angleNum_All + lengthNum; i++) {
		temp2 = &lengthData[i - angleNum_All];
		begin = temp2->pBegin; end = temp2->pEnd;
		aij = -(begin->X - end->X) / temp2->length; bij = -(begin->Y - end->Y) / temp2->length;
		B(i, (begin - pointData) * 2) = aij;
		B(i, (begin - pointData) * 2 + 1) = bij;
		B(i, (end - pointData) * 2) = -aij;
		B(i, (end - pointData) * 2 + 1) = -bij;
		L(i, 0) = sqrt(pow(begin->X - end->X, 2) + pow(begin->Y - end->Y, 2)) - temp2->length;
		P(i, i) = 1;
	}

	AzimuthVal* temp3 = nullptr;
	for (; i < angleNum_All + lengthNum + azimuthNum; i++) {
		temp3 = &azimuthData[i - angleNum_All - lengthNum];
		begin = temp3->pBegin; end = temp3->pEnd;
		S2 = pow(begin->X - end->X, 2) + pow(begin->Y - end->Y, 2);
		aij = (begin->Y - end->Y) / S2; bij = -(begin->X - end->X) / S2;
		B(i, (begin - pointData) * 2) = aij;
		B(i, (begin - pointData) * 2 + 1) = bij;
		B(i, (end - pointData) * 2) = -aij;
		B(i, (end - pointData) * 2 + 1) = -bij;
		L(i, 0) = subDeg(azimuth(end->X - begin->X, end->Y - begin->Y), temp3->azimuth);
		P(i, i) = 1;
	}

	WenMin::Matrix<double> tep = B.transpose() * P * B;
	WenMin::Matrix<double> tep2 = B.transpose() * P * L;
	WenMin::Matrix<double> x = tep.inverse() * tep2;
	WenMin::Matrix<double> V = B * x - L;
	for (int i = 0; i < PointNum; i++) {
		pointData[i].X += x(i * 2, 0);
		pointData[i].Y += x(i * 2 + 1, 0);
	}
	
	return tep.inverse().toString();
}

void horiControlNet::approxiCoordTriangulateration() {
	bool* visited = new bool[PointNum];
	std::queue<horiPoint*> pointQueue;
	for (int i = 0; i < PointNum; i++){
		visited[i] = i < KnownPointNum;
		if (visited[i]) pointQueue.push(&pointData[i]);
	}

	horiPoint* begin = nullptr, * target = nullptr;	// 处理边长观测值
	while (!pointQueue.empty()) {
		begin = pointQueue.front();
		pointQueue.pop();
		for (int i = 0; i < lengthNum; i++) {
			if (lengthData[i].pBegin == begin || lengthData[i].pEnd == begin) 
				target = lengthData[i].pBegin == begin ? lengthData[i].pEnd : lengthData[i].pBegin;
			else continue;
			if (visited[target - pointData]) continue;
			double azimuth = getAzimuth(begin, target);
			target->X = begin->X + lengthData[i].length * cos(deg2rad(azimuth));
			target->Y = begin->Y + lengthData[i].length * sin(deg2rad(azimuth));
			visited[target - pointData] = true;
			pointQueue.push(target);
		}
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
	for (int i = 0; i < angleStationNum; i++) {
		visited[i] = false;
	}
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
	angleStation* station = nullptr;
	for (int i = 0; i < azimuthNum; i++) {	// 根据方向角观测值计算
		tempAzimuth = &azimuthData[i];
		station = findStation(tempAzimuth->pBegin);
		if (!visited[station-angleStations]) { 
			station->first_azimuth = tempAzimuth->azimuth; 
			visited[station - angleStations] = true;
		}
		station = findStation(tempAzimuth->pEnd);
		if (!visited[station - angleStations]) {
			double value = sumDeg(tempAzimuth->azimuth, 180);
			if (value > 360) value -= 360;
			station->first_azimuth = value;
			visited[station - angleStations] = true;
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
			for (int j = 0; j < nextStation->valueNum; j++) {	// 计算方向角并标记
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

std::string horiControlNet::toString() {
	std::ostringstream os;
	os << "PointNum: " << PointNum << "\n";
	for (int i = 0; i < PointNum; i++) {
		os << std::setw(5) << std::left << pointData[i].Name << " " << std::fixed << std::setprecision(4) << pointData[i].X << " " << std::fixed << std::setprecision(4) << pointData[i].Y << "\n";
	}

	//os << "stationNum: " << angleStationNum << "\n";
	//for (int i = 0; i < angleStationNum; i++) {
	//	os << "name: " << angleStations[i].pBegin->Name << "  first angle: " << angleStations[i].first_azimuth << "\n";
	//	for (int j = 0; j < angleStations[i].valueNum; j++) {
	//		os << "end: " << angleStations[i].values[j].end->Name << "  survey: " << sumDeg(angleStations[i].first_azimuth, angleStations[i].values[j].surveyVal) << "\n";
	//	}
	//	os << "\n";
	//}
	return os.str();
}

void horiControlNet::OnDraw(CDC* pDC) {
	double maxX, minX, maxY, minY;
	maxX = minX = pointData[0].X;
	maxY = minY = pointData[0].Y;
	for (int i = 1; i < PointNum; i++) {
		if (pointData[i].X > maxX) maxX = pointData[i].X;
		if (pointData[i].X < minX) minX = pointData[i].X;
		if (pointData[i].Y > maxY) maxY = pointData[i].Y;
		if (pointData[i].Y < minY) minY = pointData[i].Y;
	}

	double scale = max((maxX - minX) / 600, (maxY - minY)/850);
	CPen redPen(PS_SOLID, 3, RGB(255, 0, 0)); // 创建红色画笔
	CPen* pOldPen = pDC->SelectObject(&redPen); // 选择画笔
	CFont font;
	font.CreatePointFont(120, _T("Arial")); // 创建一个字体对象，大小为 200/10 = 20 磅，字体为 Arial
	CFont* pOldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(RGB(255, 0, 0)); // 设置文本颜色为红色
	pDC->SetBkMode(TRANSPARENT); // 设置背景模式为透明
	double x, y;
	for (int i = 0; i < PointNum; i++) {
		x = 50 + (pointData[i].Y - minY) / scale; y = 650 + (pointData[i].X - minX) / -scale;
		pDC->Rectangle(x - 3, y - 3, x + 3, y + 3);
		CString cstr(pointData[i].Name);
		pDC->TextOut(x-30, y-30, cstr);
	}
	pDC->SelectObject(pOldPen); // 恢复旧的画笔
	pDC->SelectObject(pOldFont);

	CPen blackPen(PS_SOLID, 2, RGB(20, 20, 20)); // 创建黑色画笔
	double x1, x2, y1, y2;
	angleStation* temp = nullptr;
	for (int i = 0; i < angleStationNum; i++) {
		temp = &angleStations[i];
		x1 = 50 + (temp->pBegin->Y - minY) / scale; y1 = 650 + (temp->pBegin->X - minX) / -scale;
		for (int j = 0; j < temp->valueNum; j++) {
			x2 = 50 + (temp->values[j].end->Y - minY) / scale; y2 = 650 + (temp->values[j].end->X - minX) / -scale;
			pDC->MoveTo(x1, y1);
			pDC->LineTo(x2, y2);
		}
	}
	pDC->SelectObject(pOldPen); // 恢复旧的画笔
}