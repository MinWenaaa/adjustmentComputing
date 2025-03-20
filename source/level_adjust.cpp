#include "pch.h"
#include "level_adjust.h"
#include "matrix.hpp"
#include "math.h"

using namespace WenMin;

Point* levelAdjust::FindPoint(char* name) {
	for (int i = 0; i < pointNum; ++i) {
		if (strcmp(pointData[i].name, name) == 0)
			return &pointData[i];
	}
	return nullptr;
}

bool levelAdjust::ReadData(const char* fileName) {
	bool attached;
	std::ifstream file(fileName);
	if (!file) {
		std::cerr << "File open error!" << std::endl;
		exit(1);
	}
	file >> pointNum >> knownPointNum >> svNum >> dFh0;
	if (svNum - (pointNum - knownPointNum) < 2) attached = true;
	else attached = false;
	pointData = new Point[pointNum];
	svData = new SurveyValue[svNum];

	char Name[50]; double Height; int tempPointNum = knownPointNum;
	for (int i = 0; i < knownPointNum; ++i) {
		file >> Name >> Height;
		pointData[i] = Point(Name, Height, true);
	}
	Point* temp = nullptr;
	char Begin[50], End[50]; double SVHigh, S;
	for (int i = 0; i < svNum; ++i) {
		file >> Begin >> End >> SVHigh >> S;
		svData[i].begin = FindPoint(Begin);
		temp = FindPoint(End);
		if (temp) {
			svData[i].end = temp;
		}
		else {
			pointData[tempPointNum] = Point(End, FindPoint(Begin)->Height+SVHigh, false);
			svData[i].end = &pointData[tempPointNum++];
		}
		svData[i].svHigh = SVHigh; svData[i].s = S;
	}
	file.close();


	// 寻找最大联通分量
	//bool* visited = new bool[pointNum];
	//visited[0] = true;
	//for (int i = 1; i < pointNum; ++i) {
	//	visited[i] = false;
	//}
	//Point* p = &pointData[0];
	//while (p) {
	//	visited[p - pointData] = true;
	//	for (int i = 0; i < svNum; ++i) {
	//		if (svData[i].begin == p && !visited[svData[i].end - pointData]) {
	//			p = svData[i].end;
	//			break;
	//		}
	//		if (svData[i].end == p && !visited[svData[i].begin - pointData]) {
	//			p = svData[i].begin;
	//			break;
	//		}
	//	}
	//}
	//for (int i = 0; i < pointNum; ++i) {
	//	if (!visited[i]) {
	//		std::cout << "The network is not connected!" << std::endl;
	//		return false;
	//	}
	//}
	return true;
}

bool levelAdjust::SolveAttachedLevel() {
	dFh = pointData[1].Height - pointData[0].Height;
	double DistanceSum = 0;
	for (int i = 0; i < svNum; ++i) {
		dFh -= svData[i].svHigh;
		DistanceSum += svData[i].s;
	}
	for (int i = 0; i < svNum; ++i) {
		svData[i].v = svData[i].s / DistanceSum * dFh;
		svData[i].adjHigh = svData[i].svHigh + svData[i].v;
		svData[i].end->Height = svData[i].begin->Height + svData[i].adjHigh;
	}
	return true;
}

std::string levelAdjust::getAttachedPointString() {
	std::ostringstream oss{};
	oss << std::left << std::setw(15) << "点名：" << std::left << std::setw(15) << "高程：" << std::endl;
	for (int i = 0; i < pointNum; i++) {
		oss << std::left << std::setw(15) << pointData[i].name <<std::fixed << std::setprecision(3) << pointData[i].Height << std::endl;
	}
	return oss.str();
}

std::string levelAdjust::getAttachedEdgeString() {
	std::ostringstream oss{};
	//oss << "闭合差：" << std::fixed << std::setprecision(3) << dFh << std::endl << std::endl;
	oss << "路线名称：   " << "观测值：   " << "距离：   " << "改正数：   " << "改正后高差：   " << std::endl;
	for (int i = 0; i < svNum; ++i) {
		oss << svData[i].begin->name << "->" << svData[i].end->name << "      ";
		oss << std::fixed << std::setprecision(3) << svData[i].svHigh << "     " << svData[i].s << "     " << svData[i].v << "     " << svData[i].adjHigh << std::endl;
	}
	return oss.str();
}

bool levelAdjust::SolveNetworkLevel(std::string &matrix) {
	Matrix<double> B(svNum, pointNum-knownPointNum);
	Matrix<double> L(svNum, 1);
	for (int i = 0; i < svNum; ++i) {
		L(i, 0) = svData[i].svHigh -(svData[i].end->Height-svData[i].begin->Height);
		if (!svData[i].begin->type)
			B(i, svData[i].begin - pointData - knownPointNum) = -1;
		if (!svData[i].end->type)
			B(i, svData[i].end - pointData - knownPointNum) = 1;
	}

	Matrix<double> P(svNum, svNum);
	for (int i = 0; i < svNum; ++i) {
		P(i, i) = 10 / svData[i].s;
	}

	Matrix<double> temp = B.transpose() * P;
	Matrix<double> temp2 = temp * B;
	Matrix<double> temp3 = temp * L;
	Matrix<double> x = temp2.inverse() * temp3;
	for (int i = 0; i < pointNum - knownPointNum; ++i) {
		pointData[i + knownPointNum].Height += x(i, 0);
	}

	Matrix<double> V = B * x - L;
	for (int i = 0; i < svNum; ++i) {
		svData[i].v = V(i, 0);
		svData[i].adjHigh = svData[i].svHigh + svData[i].v;
		svData[i].end->Height = svData[i].begin->Height + svData[i].adjHigh;
	}

	Matrix<double> omega = V.transpose() * P * V;
	double percise = omega(0, 0) / (svNum - pointNum + knownPointNum);

	matrix = "B:\n" + B.toString() + "\nl:\n" + L.toString() + "\nP: \n" + P.toString() + "\n\n单位权中误差：\n" + std::to_string(percise);

	return true;
}
