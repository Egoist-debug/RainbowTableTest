#include "RainbowTable.h"

int main()
{
	//RainbowTable rainbowTable(100000, 60, 5, "md5");
	//rainbowTable.CreateTable();
	//rainbowTable.SaveTable();

	RainbowTableCrack rainbowTableCrack("0ff632a25f1d8b8e9df90c8dad1daf22","table_10000_md5_5_30.txt");
	rainbowTableCrack.Crack();
	return 0;
}