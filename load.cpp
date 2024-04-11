/*
	Y86-64 Simulator
	Copyright (C) 2024  Li Zihan

	This file is part of Y86-64 Simulator.

	Y86-64 Simulator is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Y86-64 Simulator is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <fstream>
#include "cpurun.h"
#include "memory.h"
using namespace std;

void load(const char* file) {
	ifstream f;
	f.open(file, ios::in | ios::binary);

	uint64_t ui64;
	uint64_t start;
	uint64_t length;
	char* p64 = (char*)(&ui64);
	char* buf;

	f.read(p64, sizeof(ui64));
	if (f.gcount() != sizeof(ui64))
		return;
	RIP = ui64;
	f.read(p64, sizeof(ui64));
	if (f.gcount() != sizeof(ui64))
		return;
	RSP = RBP = ui64;

	f.read(p64, sizeof(ui64));
	if (f.gcount() != sizeof(ui64))
		ui64 = UINT64_MAX;
	while (ui64 != UINT64_MAX)
	{
		length = ui64;
		f.read(p64, sizeof(ui64));
		if (f.gcount() != sizeof(ui64))
			break;
		start = ui64;

		buf = new char[length];
		if (buf == 0)
			throw "Out of Memory";

		f.read(buf, length);
		memWrite(start, buf, length, DBG);

		f.read(p64, sizeof(ui64));
		if (f.gcount() != sizeof(ui64))
			ui64 = UINT64_MAX;
	}
}