#pragma once
#include "Morton2D.h"
#define MinZ  -180
#define MaxZ  180
class Morton3D :
	public Morton2D
{

public:
	double resolutionZ;
	uint64_t binaryZ;            //the binary code of  x-axis           
	int decimalZ;
	double inputZ;
	Morton2D morton2d;

public:
	Morton3D(double inputX, double inputY,double inputZ);
	double calResolutionZ();
	int calDecimalValueZ();
	inline uint64_t splitBy3(uint64_t x);
	uint64_t mortonEncode3D(uint64_t deximalX, uint64_t decimalY, uint64_t decimalZ);
	uint64_t returnMorton3D(double inputX, double intputY, double inputZ);

};

