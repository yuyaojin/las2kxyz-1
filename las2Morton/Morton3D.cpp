#include "stdafx.h"
#include "Morton3D.h"


Morton3D::Morton3D(double inputX, double inputY,double inputZ)
{
    //Morton2D morton
	this->morton2d = Morton2D(inputX, inputY);
	this->inputZ = inputZ;
}

double Morton3D::calResolutionZ()
{
	return Morton::calResolution(Level, MinZ, MaxZ);
}

int Morton3D::calDecimalValueZ()
{

	return Morton::calDecimalValue(this->inputZ, MinZ, MaxZ, Level);
}

inline uint64_t Morton3D::splitBy3(uint64_t decimalValue)
{

	uint64_t x = decimalValue & 0x1fffff; // we only look at the first 21 bits
	x = (x | x << 32) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
	x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
	x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
	x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
	x = (x | x << 2) & 0x1249249249249249;
	return x;
}

uint64_t Morton3D::mortonEncode3D(uint64_t decimalX, uint64_t decimalY, uint64_t decimalZ)
{
	uint64_t answer = 0;
	answer |= splitBy3(decimalZ) | splitBy3(decimalY) << 1 | splitBy3(decimalX) << 2;
	return answer;

}

uint64_t Morton3D::returnMorton3D(double inputX, double inputY,double inputZ)
{
	uint64_t decimalX = Morton::calDecimalValue(inputX, MinX, MaxX, Level);
	uint64_t decimalY = Morton::calDecimalValue(inputY, MinY, MaxY, Level);
	uint64_t decimalZ = Morton::calDecimalValue(inputZ, MinZ, MaxZ, Level);
	return mortonEncode3D(decimalX, decimalY,decimalZ);
	//return mortonEncode(decimalX, decimalY);
}
