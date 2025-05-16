#pragma once

#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>

namespace WenMin {
	class Point {
	public:
		char name[50];
		double Height;
		bool type;

		Point(): name(""), Height(0), type(false) {}
		Point(char* name, double Height, bool type) : Height(Height), type(type) {
			strcpy_s(this->name, name);
		}
	};

	class SurveyValue {
	public: 
		Point* begin;
		Point* end;
		double svHigh;
		double s, v, adjHigh;
	};

	class levelAdjust {
	private:
		int pointNum, knownPointNum;
		int svNum;
		double dFh, dFh0;
		Point* FindPoint(char* name);

	public:
		Point* pointData;
		SurveyValue* svData;

		levelAdjust(): pointNum(0), knownPointNum(0), svNum(0), pointData(nullptr), svData(nullptr), dFh(0), dFh0(0) {}
		~levelAdjust() {
			delete[] pointData;
			delete[] svData;
		}
		bool ReadData(const char* fileName);
		bool SolveAttachedLevel();
		bool SolveNetworkLevel(std::string &matrix);
		std::string getAttachedPointString();
		std::string getAttachedEdgeString();
		//void WriteData(char* fileName);
	};

	
}
