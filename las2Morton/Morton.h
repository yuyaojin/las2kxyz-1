#pragma once
#include "stdafx.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
using namespace std;

class Morton
{
public:
	//calculate the resoution of binary tree
	uint64_t calResolution(int level, double minValue, double maxValue);
	//convert the inputValue to the decimal value in Morton space
	uint64_t calDecimalValue(double inputValue, double minValue, double maxValue, int level);
	//convert the decimal value into binary string
	vector<int> decimal2BinaryVector(uint64_t decimalValue,int level);
	string decimal2BinaryString(uint64_t decimalValue, int level);
};

