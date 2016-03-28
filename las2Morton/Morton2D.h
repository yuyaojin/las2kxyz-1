#pragma once
#include "Morton.h"

#define MinX  -90
#define MaxX   90
#define MinY -180
#define MaxY  180
#define Level 22

class Morton2D :
	public Morton
{
public:
	uint64_t resolutionX;
	uint64_t resolutionY;
	uint64_t binaryX;            //the binary code of  x-axis 
	uint64_t binaryY;            //the binary code of  y-axis
	uint64_t decimalX;
	uint64_t decimalY;
	uint64_t inputX;
    uint64_t inputY;


public:
	Morton2D();
	Morton2D(double inputX, double inputY);
	//~Morton2D();
	double calResolutionX();
	double calResolutionY();
	int calDecimalValueX();
	int calDecimalValueY();
	inline uint64_t splitBy2(uint64_t decimalValue);
	uint64_t mortonEncode2D(uint64_t decimalX, uint64_t decimalY);
	uint64_t returnMorton2D(double inputX, double intputY,int level);
	string returnMortonBinaryString2D(double inputX, double intputY, int level);
};


