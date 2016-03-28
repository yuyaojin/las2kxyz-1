#include "stdafx.h"
#include "Morton.h"
#include <stdio.h>
#include <math.h> 
#include <vector>



uint64_t Morton::calResolution(int level, double minValue, double maxValue)
{
	if (minValue >= maxValue)
	{
		//error
		throw "error,the minValue is larger than maxVlaue";
	}
	int difference = maxValue - minValue;
	uint64_t Resolution = (difference / pow(double(2), level));
	return Resolution;
}

uint64_t Morton::calDecimalValue(double inputValue, double minValue, double maxValue, int level)
{
	double difference1 = inputValue - minValue;
	double difference2 = maxValue - minValue;
	double powValue = pow(2.0, level);
	uint64_t decimalValue = (int)floor(((difference1 * powValue) / difference2));
	return decimalValue;
}

vector<int>  Morton::decimal2BinaryVector(uint64_t decimalValue, int level)
{
	vector<int> BinaryVector;
	while (decimalValue > 0)
	{
		BinaryVector.insert(BinaryVector.begin(), (decimalValue % 2));
		decimalValue = decimalValue / 2;
	}
	int null_num = level - BinaryVector.size();

	for (int i = 0; i < null_num; i++)
	{
		BinaryVector.insert(BinaryVector.begin(), 0);
	}
	return  BinaryVector;
}

string  Morton::decimal2BinaryString(uint64_t decimalValue, int level)
{
	string binaryString;
	std::stringstream binaryStringStream;
	int singleIntBinary;
	while (decimalValue > 0)
	{
		singleIntBinary = decimalValue % 2;
		binaryStringStream << singleIntBinary;
		decimalValue = decimalValue / 2;
	}
	binaryString = binaryStringStream.str();
	std::reverse(binaryString.begin(), binaryString.end());
	int null_num = level - binaryString.size();
	for (int i = 0; i < null_num; i++)
	{
		binaryString.insert(binaryString.begin(), '0');
	}
	std::cout <<binaryString;
	std::cout << "\n";
	return binaryString;
}

