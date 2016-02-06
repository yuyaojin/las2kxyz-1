// hilbertstring.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "hilbert2.h"
#include <stdlib.h>
#include <string.h>

static const char* const BASE64_TABLE_E = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

static const unsigned char BASE64_TABLE_D[0x80] = {
	/*00-07*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*08-0f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*10-17*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*18-1f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*20-27*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*28-2f*/ 0xFF, 0xFF, 0xFF, 0x3e, 0xFF, 0xFF, 0xFF, 0x3f, /*2 = '+' and '/'*/
	/*30-37*/ 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, /*8 = '0'-'7'*/
	/*38-3f*/ 0x3c, 0x3d, 0xFF, 0xFF, 0xFF, 0x40, 0xFF, 0xFF, /*2 = '8'-'9' and '='*/
	/*40-47*/ 0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /*7 = 'A'-'G'*/
	/*48-4f*/ 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /*8 = 'H'-'O'*/
	/*50-57*/ 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /*8 = 'P'-'W'*/
	/*58-5f*/ 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /*3 = 'X'-'Z'*/
	/*60-67*/ 0xFF, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, /*7 = 'a'-'g'*/
	/*68-6f*/ 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, /*8 = 'h'-'o'*/
	/*70-77*/ 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, /*8 = 'p'-'w'*/
	/*78-7f*/ 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  /*3 = 'x'-'z'*/
};


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
	/////////////////////////////////////////////////////
	///input 3D point
	pt.cord[0] = 3;//1010
	pt.cord[1] = 4;//1011
	pt.cord[2] = 5;//0011
	//pt.cord[3] = 13;//1101
	//pt.cord[4] = 5;//0101

	for (unsigned int i = 0; i < DIM; i++)
	{
		printBits(sizeof(U_int), &(pt.cord[i]));
	}
	puts("");

	////////////////////////////////////////////////////////////
	///calculate the hilbert code 
	HKey h2 = { 0 };
	h2 = H_encode(pt);

	//U_int nidx = 0;
	//print each key and the whole key
	for (unsigned int i = 0; i < ORDER; i++)
	{
		//nidx |= (h2.hcode[i] << (ORDER - i - 1)*DIM);
		printBits(sizeof(U_int), &(h2.hcode[i]));		
	}
	//printf_s("%d\n", nidx);

	/////////////////////////////////////////////////////////
	///convert the hilbert code to hilbert string
	int ntotalbits = DIM * ORDER;
	int nstrlen = ntotalbits / 6;

	char* szstr = (char*)malloc(nstrlen + 1); //last char for zero
	memset(szstr, 0, nstrlen + 1);
	int valid = strlen(szstr); //should be zero here

	U_int ncombine; //first|second
	for (unsigned int i = 0; i < ORDER; i += 2) //the dim number are 3 here, combine every 2 parts, we get 6 bits
	{
		ncombine = 0;
		ncombine |= h2.hcode[i] << DIM; //first 3 bits
		ncombine |= h2.hcode[i + 1]; //second 3 bits

		printf("%d \n", ncombine);
		//printf("%d \n", h2.hcode[i]);
		//printf("%d \n", h2.hcode[i+1]);

		*(szstr + i / 2) = BASE64_TABLE_E[ncombine]; //get the BASE64 character directly, just copy it

		//valid = strlen(szstr);
	}

	printf("hilbert string is %s\n", szstr);

	///////////////////////////////////////////////////////
	//decode the hilbert string to hilbert code
	U_int pParts[ORDER] = { 0 }; //for hibert code parts, number is ORDER, each part is DIM bits
	U_int mask = ((U_int)1 << DIM ) - 1;

	unsigned int nhilberstrlen = strlen(szstr);
	ncombine = 0;
	for (unsigned int i = 0; i < nhilberstrlen; i++)
	{
		ncombine = BASE64_TABLE_D[*(szstr + i)]; //find the index by the BASE64 character

		//printf("%d \n", ncombine);
				
		pParts[2 * i] = (ncombine >> DIM ) & mask; //the first part
		pParts[2 * i + 1] = ncombine & mask; //the second part

		if (pParts[2 * i] != h2.hcode[2 * i] || pParts[2 * i + 1] != h2.hcode[2 * i + 1])
		{
			printf(" error! not equal \n");
		}

		printBits(sizeof(U_int), &(pParts[2 * i]));
		printBits(sizeof(U_int), &(pParts[2 * i + 1]));
	}

	free(szstr);
	szstr = NULL;
	return 0;
}

