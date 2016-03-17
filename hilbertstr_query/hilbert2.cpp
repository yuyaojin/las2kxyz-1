// hilbert2.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "hilbert2.h"

/*
This code assumes the following:
The macro ORDER corresponds to the order of curve and is 32,
thus coordinates of points are 32 bit values.
A U_int should be a 32 bit unsigned integer.
The macro DIM corresponds to the number of dimensions in a
space.
The derived-key of a Point is stored in an Hcode which is an
array of U_int. The bottom bit of a derived-key is held in the
bottom bit of the hcode[0] element of an Hcode and the top bit
of a derived-key is held in the top bit of the hcode[DIM-1]
element of an Hcode.
g_mask is a global array of masks which helps simplyfy some
calculations - it has DIM elements. In each element, only one
bit is zeo valued - the top bit in element no. 0 and the bottom
bit in element no. (DIM - 1). eg.
#if DIM == 5 const U_int g_mask[] = {16, 8, 4, 2, 1}; #endif
#if DIM == 6 const U_int g_mask[] = {32, 16, 8, 4, 2, 1}; #endif
etc...
*/



/*===========================================================*/
/* calc_P */
/*===========================================================*/
/*U_int calc_P(int i, HCord H)
{
	int element;
	U_int P, temp1;//, temp2
	element = i / ORDER;
	P = H.hcode[element];
	if (i % ORDER > ORDER - DIM)
	{
		temp1 = H.hcode[element + 1];
		P >>= i % ORDER;
		temp1 <<= ORDER - i % ORDER;
		P |= temp1;
	}
	else
		P >>= i % ORDER; /* P is a DIM bit hcode */
/* the & masks out spurious highbit values */
/*#if DIM < ORDER
	P &= (1 << DIM) - 1;
#endif
	return P;
}*/

/*===========================================================*/
/* calc_P ---Get each key part from the input key*/ 
/*===========================================================*/
U_int calc_P3(int i, HKey H)
{
	/*U_int mask = ((U_int)1 << (ORDER+1)) - 1;
	U_int P;//, temp2
	
	P = ((H.hcode[0] >> i) & mask);*/

	U_int P = H.hcode[(ORDER * DIM - i) / DIM - 1];
	
	return P ;
}

/*===========================================================*/
/* calc_P2 */
/*===========================================================*/
U_int calc_P2(U_int S)
{
	int i;
	U_int P;
	P = S & g_mask[0];
	for (i = 1; i < DIM; i++)
		if (S & g_mask[i] ^ (P >> 1) & g_mask[i])
			P |= g_mask[i];
	return P;
}

U_int calc_J(U_int P)
{
	int i;
	U_int J;
	J = DIM;
	for (i = 1; i < DIM; i++)
		if ((P >> i & 1) == (P & 1))
			continue;
		else
			break;
	if (i != DIM)
		J -= i;
	return J;
}

/*===========================================================*/
/* calc_T */
/*===========================================================*/
U_int calc_T(U_int P)
{
	if (P < 3)
		return 0;
	if (P % 2)
		return (P - 1) ^ (P - 1) / 2;
	return (P - 2) ^ (P - 2) / 2;
}
/*===========================================================*/
/* calc_tS_tT */
/*===========================================================*/
U_int calc_tS_tT(U_int xJ, U_int val)
{
	U_int retval, temp1, temp2;
	retval = val;
	if (xJ % DIM != 0)
	{
		temp1 = val >> (xJ % DIM);
		temp2 = val << (DIM - xJ % DIM);
		retval = temp1 | temp2;
		retval &= ((U_int)1 << DIM) - 1;
	}
	return retval;
}

/*===========================================================*/
/* calc_tS_tT */
/*===========================================================*/
U_int calc_tS_tT2(U_int xJ, U_int val)
{
	U_int retval, temp1, temp2;
	retval = val;
	if (xJ % DIM != 0)
	{
		temp1 = val << (xJ % DIM);
		temp2 = val >> (DIM - xJ % DIM);
		retval = temp1 | temp2;
		retval &= ((U_int)1 << DIM) - 1;
	}
	return retval;
}


/*===========================================================*/
/* H_decode */
/*===========================================================*/
/* For mapping from one dimension to DIM dimensions */
Point H_decode(HKey H)
{
	U_int mask = (U_int)1 << ORDER - 1,
		A, W = 0, S, tS, T, tT, J, P = 0, xJ;
	Point pt = { 0 };
	int i = ORDER * DIM - DIM, j;

	P = calc_P3(i, H); //get key part from the hilbert key

	J = calc_J(P);
	xJ = J - 1;
	A = S = tS = P ^ P / 2;
	T = calc_T(P);
	tT = T;
	/*--- distrib bits to coords ---*/
	for (j = DIM - 1; A > 0; A >>= 1, j--)
	{ 
		if (A & 1)
			pt.cord[j] |= mask;
	}
	/*pt.hcode[(DIM*ORDER -i)/ORDER -1] = A;*/

	for (i -= DIM, mask >>= 1; i >= 0; i -= DIM, mask >>= 1)
	{
		P = calc_P3(i, H); //get key part from the hilbert key

		S = P ^ P / 2;
		tS = calc_tS_tT(xJ, S);
		W ^= tT;
		A = W ^ tS;
		/*--- distrib bits to coords ---*/
		for (j = DIM - 1; A > 0; A >>= 1, j--)
		{ 
			if (A & 1)
				pt.cord[j] |= mask;
		}
		/*pt.hcode[(DIM*ORDER - i) / ORDER - 1] = A;*/

		if (i >= 0)
		{
			T = calc_T(P);
			tT = calc_tS_tT(xJ, T);
			J = calc_J(P);
			xJ += J - 1;
		}
	}
	return pt;
}

/*===========================================================*/
/* H_encode */
/*===========================================================*/
/* For mapping from DIM dimensions to one dimension */
HKey H_encode(Point pt)
{
	U_int mask = (U_int)1 << ORDER - 1, element,
		A, W = 0, S, tS, T, tT, J, P = 0, xJ;
	HKey h = { 0 };
	int i = ORDER * DIM - DIM, j;

	A = 0;
	for (j = 0; j < DIM; j++)
	{ 
		if (pt.cord[j] & mask)
			A |= g_mask[j];
	}
	S = tS = A;
	P = calc_P2(S);
	/* add in DIM bits to hcode */
	//element = i / ORDER;
	
	/*if (i % ORDER > ORDER - DIM)
	{
		h.hcode[element] |= P << (i % ORDER);
		h.hcode[element + 1] |= P >> (ORDER - i % ORDER);
	}
	else
		h.hcode[element] |= P << (i - element * ORDER);*/

	element = (ORDER * DIM - i) / DIM - 1;
	h.hcode[element] = P;
	///h.hcode[0] |= P << i;
	
	J = calc_J(P);
	xJ = J - 1;
	T = calc_T(P);
	tT = T;
	for (i -= DIM, mask >>= 1; i >= 0; i -= DIM, mask >>= 1)
	{
		A = 0;
		for (j = 0; j < DIM; j++)
		{ 
			if (pt.cord[j] & mask)
				A |= g_mask[j];
		}
		W ^= tT;
		tS = A ^ W;
		S = calc_tS_tT2(xJ, tS);
		P = calc_P2(S);
		/* add in DIM bits to hcode */
		//element = i / ORDER;
		/*if (i % ORDER > ORDER - DIM)
		{
			h.hcode[element] |= P << (i % ORDER);
			h.hcode[element + 1] |= P >> (ORDER - i % ORDER);
		}
		else
			h.hcode[element] |= P << (i - element * ORDER);*/
		
		element = (ORDER * DIM - i) / DIM - 1;
		h.hcode[element] = P;
		//h.hcode[0] |= P << i;

		if (i > 0)
		{
			T = calc_T(P);
			tT = calc_tS_tT2(xJ, T);
			J = calc_J(P);
			xJ += J - 1;
		}
	}
	return h;
}

/*
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
*/
/*
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
		nidx |= (h2.hcode[i] << (ORDER-i-1)*DIM);
		printBits(sizeof(U_int), &(h2.hcode[i]));
	}
	printf_s("%d\n", nidx);
	

	puts("");
	////////////////
	Point pt2 = { 0 };
	HKey h3 = { 0 };
	U_int ninput = 624824;

	U_int mask = ((U_int)1 << (ORDER + 1)) - 1;
	for (unsigned int i = 0; i < ORDER; i++)
	{
		//nidx |= (h2.hcode[i] << (ORDER - i - 1)*DIM);
		//printBits(sizeof(U_int), &(h2.hcode[i]));
		h3.hcode[i] = (ninput >> ((ORDER - i - 1)*DIM)) & mask;
	}
	
	pt2 = H_decode(h3);

	for (unsigned int i = 0; i < DIM; i++)
	{
		printBits(sizeof(U_int), &(pt2.cord[i]));
	}

	///////////////////////////////////
	///2D sample
	HKey h4 = { 0 };
	U_int nidx2 = 0;
	for (unsigned int i = 0; i < 1 << ORDER; i++)
	{
		for (unsigned int j = 0; j < 1 << ORDER; j++)
		{
			pt.cord[0] = j;
			pt.cord[1] = i; // x in the lowest part
			//coord[2] = 3;

			h4 = H_encode(pt);

			//printf_s("%d %d:  %d\n", i, j,  h4.hcode[0]);
			nidx2 = 0;
			for (unsigned int p = 0; p < ORDER; p++)
			{
				nidx2 |= (h4.hcode[p] << (ORDER - p - 1)*DIM);
				//printBits(sizeof(U_int), &(h2.hcode[i]));
			}
			printf_s("%d %d:  %d\n", i, j, nidx2);
		}
	}

	return 0;
}*/

