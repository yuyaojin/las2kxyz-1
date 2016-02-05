// hilbertstring.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "hilbert2.h"
//#include "CyoEncode.h"
#include "CyoDecode.h"

#include <string.h>

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
	//pt.cord[3] = 13;//1101
	//pt.cord[4] = 5;//0101

	for (unsigned int i = 0; i < DIM; i++)
	{
		printBits(sizeof(U_int), &(pt.cord[i]));
	}
	puts("");

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

	int ntotalbits = DIM * ORDER;
	int nstrlen = ntotalbits / 6;
	
	char* szstr = (char*)malloc(nstrlen+2);
	memset(szstr, 0, nstrlen);

	char* szB64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

	int ncombine;
	for (unsigned int i = 0; i < ORDER; i+=2) //the dim number are 3 here, combine every 2 parts, we get 6 bits
	{
		ncombine = 0;
		ncombine |= h2.hcode[i] << DIM; //first 3 bits
		ncombine |= h2.hcode[i+1]; //second 3 bits

		printf("%d \n", ncombine);

		*(szstr + i / 2) = szB64[ncombine]; //get the BASE64 character directly, just copy it
	}

	printf("hilbert string is %s\n", szstr);

	//////////////////////////////
	//decode using the lib
	size_t required = 0;
	//char* encoded = NULL;
	char* decoded = NULL;

	int valid = strlen(szstr);
	required = cyoBase64DecodeGetLength(strlen(szstr));
	decoded = (char*)malloc(required);
	if (decoded == NULL)
	{
		printf("*** ERROR: Unable to allocate buffer for decoding ***\n");
		//goto exit;
	}
	cyoBase64Decode(decoded, szstr, strlen(szstr));

	printf("Decoded = '%s'\n", decoded);

	free(szstr);
	free(decoded);
	decoded = szstr = NULL;
	return 0;
}

