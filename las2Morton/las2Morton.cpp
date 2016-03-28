// las2Morton.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Morton2D.h"
#include "Morton3D.h"

#include<bitset>
#include<iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	int x = 20;
	int y = 50;
	int z = 50;

	uint64_t answer = 0;
	Morton2D morton2d(x,y);
//	for (int j = 0; j < 31; j++){
		answer = morton2d.returnMorton2D(x, y,16);
	//	morton2d.decimal2BinaryString(x, j);
		std::cout << answer<< "\n";
		std::bitset<sizeof(uint64_t) * 8> a(answer);
		std::cout << a << "\n";
		string string1 = morton2d.returnMortonBinaryString2D(x,y,16);
		cout<<"final" << string1 << "\n";





//	Morton3D morton3d(x, y,z);
//	answer = morton3d.returnMorton3D(x, y,z);
//	std::bitset<sizeof(uint64_t)* 8> b(answer);
//	std::cout << b << "\n";2D
	//printf("%d", answer);
	system("pause");
}

