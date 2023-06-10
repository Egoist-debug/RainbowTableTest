#include "RainbowTable.h"

int main()
{
	//RainbowTable rainbowTable(1000000, 60, 7, "md5");
	//rainbowTable.CreateTable();
	//rainbowTable.SaveTable();

	RainbowTableCrack rainbowTableCrack("87c6ce388d0a4a0807d0653075920165","table_1000000_md5_7_120_0.txt");
	rainbowTableCrack.Crack();
	return 0;
}