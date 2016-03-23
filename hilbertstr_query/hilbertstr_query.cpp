// hilbertstr_query.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#include "hilbert2.h"
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#include <vector>
using namespace std;


///////////////////////////////////////////////////////
//3D Morton
// method to seperate bits from a given integer 3 positions apart
inline uint64_t splitBy3(unsigned int a)
{
	uint64_t x = a & 0x1fffff; // we only look at the first 21 bits
	x = (x | x << 32) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
	x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
	x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
	x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
	x = (x | x << 2) & 0x1249249249249249;
	return x;
}

inline uint64_t mortonEncode_magicbits(unsigned int x, unsigned int y,
	unsigned int z)
{
	uint64_t answer = 0;
	answer |= splitBy3(x) | splitBy3(y) << 1 | splitBy3(z) << 2;
	return answer;
}
//////////////////////////////////////////////////////////


/////the original BASE64 table
////////////////////////////////////DEC////0                       25|26                    51|52      61|626364
////////////////////////////////////HEX////00                      19|1a                    33|34      3d|3e3f40
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

/////////////////////////////////////DEC////012      11|12                     37|38                    63|64
/////////////////////////////////////HEX////012      0b|0c                     25|26                    3f|40  
static const char* const BASE64_TABLE_E2 = "+/0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=";

//here use the ASCII as index
static const unsigned char BASE64_TABLE_D2[0x80] = {
	/*00-07*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*08-0f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*10-17*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*18-1f*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*20-27*/ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	/*28-2f*/ 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x01, /*2 = '+' and '/'*/
	/*30-37*/ 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, /*8 = '0'-'7'*/
	/*38-3f*/ 0x0a, 0x0b, 0xFF, 0xFF, 0xFF, 0x40, 0xFF, 0xFF, /*2 = '8'-'9' and '='*/
	/*40-47*/ 0xFF, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, /*7 = 'A'-'G'*/
	/*48-4f*/ 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, /*8 = 'H'-'O'*/
	/*50-57*/ 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, /*8 = 'P'-'W'*/
	/*58-5f*/ 0x23, 0x24, 0x25, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /*3 = 'X'-'Z'*/
	/*60-67*/ 0xFF, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, /*7 = 'a'-'g'*/
	/*68-6f*/ 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, /*8 = 'h'-'o'*/
	/*70-77*/ 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, /*8 = 'p'-'w'*/
	/*78-7f*/ 0x3d, 0x3e, 0x3f, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  /*3 = 'x'-'z'*/
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

int compare(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}

class rect
{
public:
	int x0;
	int x1;
	int y0;
	int y1;

	rect() : x0(0), x1(0), y0(0), y1(0)
	{
		;
	}

	rect(int ix0, int iy0, int ix1, int iy1)
		: x0(ix0), y0(iy0), x1(ix1), y1(iy1)
	{
		;
	}

	void set(int ix0, int iy0, int ix1, int iy1)
	{
		x0 = ix0; y0 = iy0; x1 = ix1;  y1 = iy1;
	}
};


class node
{
public:
	//int ndim;

	int x0;
	int y0;
	int width;
	int level;

	//0~3 for 2d; upper 2|3;
	//            lower 0|1

	node getchildnode(int idx)
	{
		node nchild;

		if (idx == 0 || idx == 2)
			nchild.x0 = this->x0;
		else
			nchild.x0 = this->x0 + this->width / 2;

		if (idx == 0 || idx == 1)
			nchild.y0 = this->y0;
		else
			nchild.y0 = this->y0 + this->width / 2;

		nchild.width = this->width / 2;
		nchild.level = this->level + 1;

		return nchild;
	}

	int spatialrelationship(rect qrt)//0 = equal; 1=contain;2=intersect; default = -1  not overlap ;
	{
		rect nrt(this->x0, this->y0, this->x0 + this->width, this->y0 + this->width);
		///equal
		if (nrt.x0 == qrt.x0 && nrt.y0 == qrt.y0 && \
			nrt.x1 == qrt.x1 && nrt.y1 == qrt.y1)
			return 0;

		//fully contain(how about edge touch?)
		if (nrt.x0 <= qrt.x0 && nrt.y0 <= qrt.y0 && \
			nrt.x1 >= qrt.x1 && nrt.y1 >= qrt.y1)
			return 1;

		//intersect
		///http://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other
		///RectA.Left < RectB.Right && RectA.Right > RectB.Left && RectA.Top > RectB.Bottom && RectA.Bottom < RectB.Top
		if (nrt.x0 < qrt.x1 && nrt.x1 > qrt.x0 && \
			nrt.y1 > qrt.y0 && nrt.y0 < qrt.y1)
			return 2;

		return -1; //not overlap
	}
};

int point2hilbertcode(int x, int y)
{
	Point pt;
	pt.cord[0] = y;//col
	pt.cord[1] = x;//row

	HKey h3 = { 0 };
	h3 = H_encode(pt);

	int nidx2 = 0;
	for (unsigned int p = 0; p < ORDER; p++)
	{
		nidx2 |= (h3.hcode[p] << (ORDER - p - 1)*DIM);
	}

	return nidx2;
}

//vector<int> g_sfcinterval;
void treenode2sfcinterval(node nd)
{
	if (nd.width == 1) //leaf node
	{
		int code = point2hilbertcode(nd.x0, nd.y0);
		//g_sfcinterval.push_back(code);
		//g_sfcinterval.push_back(code);

		printf("sfc interval: %d --- %d\n", code, code);
	}
	else //middle node
	{
		//get four corners, then get min and max sfc code
		int x0, y0, x1, y1;
		x0 = nd.x0; y0 = nd.y0;
		x1 = nd.x0 + nd.width - 1; //point to cell center
		y1 = nd.y0 + nd.width - 1; //point to cell center

		int sfccode[4];
		sfccode[0] = point2hilbertcode(x0, y0);
		sfccode[1] = point2hilbertcode(x0, y1);
		sfccode[2] = point2hilbertcode(x1, y0);
		sfccode[3] = point2hilbertcode(x1, y1);

		qsort(sfccode, 4, sizeof(int), compare);

		//g_sfcinterval.push_back(sfccode[0]);
		//g_sfcinterval.push_back(sfccode[3]);

		printf("sfc interval: %d --- %d\n", sfccode[0], sfccode[3]);
	}
}


/////////////////////////////////////////////////////////////
///comparison between the tree node and the query rectangle
int query_approximate(node nd, rect qrt)
{
	////this tree node now fully contains the input query rectangle
	////firstly to check this node is the leaf node: yes, stop here
	if (nd.level == ORDER)
	{
		printf("node level : %d, width %d, orig (%d, %d), dest (%d, %d)\n", \
			nd.level, nd.width, nd.x0, nd.y0, nd.x0 + nd.width, nd.y0 + nd.width);

		treenode2sfcinterval(nd);
		return 0;
	}

	////check the spatial relationship between this query rectangle and four children
	node nchild[4];

	for (int i = 0; i < 4; i++)
	{
		nchild[i] = nd.getchildnode(i);
	}

	for (int i = 0; i < 4; i++)
	{
		int nrt = nchild[i].spatialrelationship(qrt);

		if (nrt == 0)//equal to one child, that's enough, stop here
		{
			printf("node level : %d, width %d, orig (%d, %d), dest (%d, %d)\n", \
				nchild[i].level, nchild[i].width, nchild[i].x0, nchild[i].y0, \
				nchild[i].x0 + nchild[i].width, nchild[i].y0 + nchild[i].width);

			treenode2sfcinterval(nchild[i]);
			return 0;
		}
		if (nrt == 1)//fully contained by one child,then go down directly
		{
			query_approximate(nchild[i], qrt);
			return 0;
		}

		//intersect one child (i.e. smaller than this node and bigger than one child), this means further division
		if (nrt == 2)
		{
			break;
		}
	}

	/////this tree node is divided to get the 4 child nodes
	/////also to divide the input query rectangle into 2 or 4 parts(ONLY 2 or 4 here!!!)---5 CASES!!!
	//0~3 for 2d; upper 2|3;
	//            lower 0|1
	rect rt0, rt1, rt2, rt3;

	int midx, midy;
	midx = nd.x0 + nd.width / 2;
	midy = nd.y0 + nd.width / 2;

	if (midx > qrt.x0 && midx < qrt.x1) //x dimesion: mid in the qrt center
	{
		if (midy > qrt.y0 && midy < qrt.y1) //y dimesion: mid in the qrt center--- 4 parts:0,1,2,3
		{
			rt0.set(qrt.x0, qrt.y0, midx, midy);
			query_approximate(nchild[0], rt0);

			rt1.set(midx, qrt.y0, qrt.x1, midy);
			query_approximate(nchild[1], rt1);

			rt2.set(qrt.x0, midy, midx, qrt.y1);
			query_approximate(nchild[2], rt2);

			rt3.set(midx, midy, qrt.x1, qrt.y1);
			query_approximate(nchild[3], rt3);
		}

		if (midy <= qrt.y0) //x dimesion: qrt on the top ----2 parts: 2,3 
		{
			rt2.set(qrt.x0, qrt.y0, midx, qrt.y1);
			query_approximate(nchild[2], rt2);

			rt3.set(midx, qrt.y0, qrt.x1, qrt.y1);
			query_approximate(nchild[3], rt3);
		}

		if (midy >= qrt.y1) //x dimesion: qrt on the bottom----2 parts:  0,1 
		{
			rt0.set(qrt.x0, qrt.y0, midx, qrt.y1);
			query_approximate(nchild[0], rt0);

			rt1.set(midx, qrt.y0, qrt.x1, qrt.y1);
			query_approximate(nchild[1], rt1);
		}

	}

	if (midx >= qrt.x1) //x dimesion: qrt on the left----2 parts: 0, 2
	{
		rt0.set(qrt.x0, qrt.y0, qrt.x1, midy);
		query_approximate(nchild[0], rt0);

		rt2.set(qrt.x0, midy, qrt.x1, qrt.y1);
		query_approximate(nchild[2], rt2);
	}

	if (midx <= qrt.x0) //x dimesion: qrt on the right ----2 parts: 1,3
	{
		rt1.set(qrt.x0, qrt.y0, qrt.x1, midy);
		query_approximate(nchild[1], rt1);

		rt3.set(qrt.x0, midy, qrt.x1, qrt.y1);
		query_approximate(nchild[3], rt3);
	}

	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	clock_t begin, duration;

	Point pt;
	//////////////////
	//U_int ncombine;

	int ntotalbits = DIM * ORDER;
	int nstrlen = ntotalbits / 6;

	U_int mask2 = ((U_int)1 << (6 + 1)) - 1;
	char* szstr = (char*)malloc(nstrlen + 1); //last char for zero
	memset(szstr, 0, nstrlen + 1);

	HKey h2 = { 0 };

	begin = clock();
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			pt.cord[0] = col;//1010
			pt.cord[1] = row;//1011

			h2 = H_encode(pt);

			memset(szstr, 0, nstrlen + 1);
			//for (unsigned int i = 0; i < ORDER; i += 2) //the dim number are 3 here, combine every 2 parts, we get 6 bits
			//{
			//	ncombine = 0;
			//	ncombine |= h2.hcode[i] << DIM; //first 3 bits
			//	ncombine |= h2.hcode[i + 1]; //second 3 bits

			//	*(szstr + i / 2) = BASE64_TABLE_E2[ncombine]; //get the BASE64 character directly, just copy it				
			//}
			//print
			int nidx2 = 0;
			for (unsigned int p = 0; p < ORDER; p++)
			{
				nidx2 |= (h2.hcode[p] << (ORDER - p - 1)*DIM);
				//printBits(sizeof(U_int), &(h2.hcode[i]));
			}

			szstr[0] = BASE64_TABLE_E2[nidx2];

			printf("point: %d, %d ---   %d   %s \n", row, col, nidx2, szstr);
		}//end for col
	}//end for row

	duration = clock() - begin;

	printf("hibert time: %d ms\n", duration * 1000 / CLOCKS_PER_SEC);


	/////////////////////////////////////////////////////////////////
	///range query---brute-force
	printf("--------range query by brute-force--------\n");
	int x0, x1;
	int y0, y1;

	x0 = 3; x1 = 5;
	y0 = 2; y1 = 5;

	/*x0 = 2; x1 = 4;
	y0 = 1; y1 = 2;*/

	int rows = x1 - x0 + 1;
	int cols = y1 - y0 + 1;

	int* pcodelist = new int(rows*cols);
	memset(pcodelist, 0, sizeof(int)*rows*cols);

	//int idx = 0;
	//for (int row = x0; row <= x1; row++)
	//{
	//	for (int col = y0; col <= y1; col++)
	//	{
	//#pragma omp for schedule(dynamic)
	for (int i = 0; i < rows*cols; i++)
	{
		int col, row;
		col = i % cols + y0;
		row = i / cols + x0;
		////////////
		pt.cord[0] = col;//1010
		pt.cord[1] = row;//1011

		HKey h3 = { 0 };
		h3 = H_encode(pt);

		//memset(szstr, 0, nstrlen + 1);

		int nidx2 = 0;
		for (unsigned int p = 0; p < ORDER; p++)
		{
			nidx2 |= (h3.hcode[p] << (ORDER - p - 1)*DIM);
			//printBits(sizeof(U_int), &(h2.hcode[i]));
		}

		//szstr[0] = BASE64_TABLE_E2[nidx2];

		pcodelist[i] = nidx2;
		//pcodelist[(row - x0)*cols + (col - y0)] = nidx2;
		//pcodelist[idx] = nidx2;
		//idx++;

		printf("query cell: %d, %d, %d ---   %d \n", row, col, (row - x0)*cols + (col - y0), nidx2);
	}
	/*	}
	}*/

	for (int i = 0; i < rows*cols; i++)
	{
		printf("before sort: %d, %d \n", i, pcodelist[i]);
	}


	qsort(pcodelist, rows*cols, sizeof(int), compare);

	for (int i = 0; i < rows*cols; i++)
	{
		printf("after sort: %d, %d \n", i, pcodelist[i]);
	}

	int nstart = 0;
	int ncur = nstart;
	while (ncur < cols*rows)
	{
		ncur++;
		if (pcodelist[ncur] - pcodelist[ncur - 1] != 1) // not continuous
		{
			printf_s("%d, %d\n", pcodelist[nstart], pcodelist[ncur - 1]);

			nstart = ncur;
		}
	}

	//delete[] pcodelist;
	////////////////////////////////////////////////////////////////
	////range query by recursive approximation
	printf("--------range query by recursive approximation--------\n");
	node root;
	root.level = 0;
	root.x0 = 0;
	root.y0 = 0;
	root.width = 8;

	rect qrt;
	qrt.x0 = 3; qrt.x1 = 6; //here,point coordidate is located on lef-bottom of the correspoind cell
	qrt.y0 = 2; qrt.y1 = 6; //so the right-top coordidate equals cell center + 1

	/*qrt.x0 = 2; qrt.x1 = 5;
	qrt.y0 = 1; qrt.y1 = 3;*/

	///check if the root node contains or eqauls the query rectangle
	int res = root.spatialrelationship(qrt);
	if (res == 0) ///equals
	{
		printf("node level : %d, width %d, orig (%d, %d), dest (%d, %d)\n", \
			root.level, root.width, root.x0, root.y0, root.x0 + root.width, root.y0 + root.width);
	}

	if (res == 1) //fully contain 
	{
		query_approximate(root, qrt);
	}

	//free(szstr);
	free(szstr);
	//szstr = NULL;
	return 0;
}


