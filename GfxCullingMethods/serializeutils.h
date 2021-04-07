#pragma once
#include <fstream>

void BinWrite(int n, std::ofstream& f)
{
    f.write((char*)&n, sizeof(n));
}