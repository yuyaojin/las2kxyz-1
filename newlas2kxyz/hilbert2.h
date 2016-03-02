

typedef unsigned long long U_int;

//for 2d sample
//#define DIM 2 
//#define ORDER  2
//const U_int g_mask[] = { 2, 1 }; //the number is equal to the DIM

//for butz's sample
#define DIM 3 
#define ORDER 32
const U_int g_mask[] = {/* 16, 8,*/ 4, 2, 1 };


typedef struct {
	U_int cord[DIM];
}HCord;//The numeber  of each point's coordiantes is DIM, each coordinate has ORDER bits

typedef struct {
	U_int hcode[ORDER];
}KPart;///The derived key has ORDER parts, each part has DIM bits

typedef HCord Point;
typedef KPart HKey;

HKey H_encode(Point pt);
Point H_decode(HKey H);

int ConvertHkey2Str(HKey key, char* szstr, int strlen);
int ConvertStr2Hkey(char* szstr, int strlen, HKey key);