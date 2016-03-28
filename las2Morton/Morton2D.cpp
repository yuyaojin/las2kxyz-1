#include "stdafx.h"
#include "Morton2D.h"


Morton2D::Morton2D()
{
	this->inputX = 0;
	this->inputY = 0;
}


Morton2D::Morton2D(double inputX, double inputY)
{
	this->inputX = inputX;
	this->inputY = inputY;
}

double Morton2D::calResolutionX()
{
	return calResolution(Level, MinX, MaxX);
}

double Morton2D::calResolutionY()
{
	return calResolution(Level, MinY, MaxY);
}

int Morton2D::calDecimalValueX()
{
	return calDecimalValue(this->inputX, MinX, MaxX,Level);
}

int Morton2D::calDecimalValueY()
{
	return calDecimalValue(this->inputY, MinY, MaxY,Level);
}

uint64_t Morton2D::splitBy2(uint64_t x)
{

	static const uint64_t  B[] =
	{ 0x5555555555555555,
	  0x3333333333333333,
	  0x0F0F0F0F0F0F0F0F,
	  0x00FF00FF00FF00FF,
	  0x0000FFFF0000FFFF };

	static const unsigned int S[] = { 1, 2, 4, 8,16 };
	x = (x | (x << S[4])) & B[4];
	x = (x | (x << S[3])) & B[3];
	x = (x | (x << S[2])) & B[2];
	x = (x | (x << S[1])) & B[1];
	x = (x | (x << S[0])) & B[0];
	return x;

}

uint64_t Morton2D::mortonEncode2D(uint64_t x, uint64_t y)
{
	uint64_t answer = 0;
	answer = splitBy2(y) | (splitBy2(x) << 1);
	return answer;
}

uint64_t Morton2D::returnMorton2D(double inputX, double inputY,int level)
{

	uint64_t decimalX = Morton::calDecimalValue(inputX, MinX, MaxX, level);
	uint64_t decimalY = Morton::calDecimalValue(inputY, MinY, MaxY,level);
	uint64_t decimalZ = mortonEncode2D(decimalX, decimalY);
	return decimalZ;

}

string Morton2D::returnMortonBinaryString2D(double inputX, double intputY, int level)
{
	uint64_t decimalX = Morton::calDecimalValue(inputX, MinX, MaxX, level);
	uint64_t decimalY = Morton::calDecimalValue(inputY, MinY, MaxY, level);
	string binaryStringX = Morton::decimal2BinaryString(decimalX,level);
	string binaryStringY = Morton::decimal2BinaryString(decimalY, level);
	string mortonBinaryString;
	std::stringstream binaryStringStream;

	for (int i = 0; i < level; i++)
	{
		binaryStringStream << binaryStringX.at(i);
		binaryStringStream << binaryStringY.at(i);
		int j = 0;
	}
	mortonBinaryString = binaryStringStream.str();
	return mortonBinaryString;
}


