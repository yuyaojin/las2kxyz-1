// hilbertstring.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "hilbert2.h"

void printBits(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*)ptr;
	unsigned char byte;
	int i, j;

	for (i = size - 1; i >= 0; i--)
	{
		for (j = 7; j >= 0; j--)
		{
			byte = b[i] & (1 << j);
			byte >>= j;
			printf("%u", byte);
			//cout << byte;
		}
	}
	puts("");
	//cout << "";
}

int _tmain(int argc, _TCHAR* argv[])
{
	Point pt;
	//////////////////////////////////
	///Butz's sample
	pt.cord[0] = 10;//1010
	pt.cord[1] = 11;//1011
	pt.cord[2] = 3;//0011
	pt.cord[3] = 13;//1101
	pt.cord[4] = 5;//0101

	for (unsigned int i = 0; i < DIM; i++)
	{
		printBits(sizeof(U_int), &(pt.cord[i]));
	}
	puts("");

	HKey h2 = { 0 };
	h2 = H_encode(pt);

	U_int nidx = 0;

	//print each key and the whole key
	for (unsigned int i = 0; i < ORDER; i++)
	{
		nidx |= (h2.hcode[i] << (ORDER - i - 1)*DIM);
		printBits(sizeof(U_int), &(h2.hcode[i]));
	}
	printf_s("%d\n", nidx);

	return 0;
}

