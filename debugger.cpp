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

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include "cpurun.h"
#include "memory.h"
using namespace std;

void display() {
	char buf[256];
	switch (STAT)
	{
	case AOK:
		printf("[AOK]\n");
		break;
	case HLT:
		printf("[HLT]\n");
		break;
	case ADR:
		printf("[ADR]\n");
		break;
	case INS:
		printf("[INS]\n");
		break;
	default:
		break;
	}
	decode(RIP, buf);
	printf("RIP = 0x%016llX\n%s\n\n", RIP, buf);
	for (uint8_t ri = 0; ri < 16; ri++) {
		printf("%s = 0x%016llX\n", regname(ri), reg(ri));
	}
	printf(" ZF = %d\n", ZF);
	printf(" SF = %d\n", SF);
	printf(" OF = %d\n", OF);

}

void decodeCommand(uint64_t addr) {
	char buf[256];
	decode(addr, buf);
	printf("Command at 0x%016llX\n%s\n", addr, buf);
}

const char* memGetBlock(uint32_t pageid, uint16_t blockid);
bool outputMemBlock(uint32_t page, uint16_t block, char* outfile) {
	char file[256] = { 0 };
	sprintf(file, "MEM-%08X-%04hX.block", page, block);
	if (outfile)
		strcpy(outfile, file);

	const char* data = memGetBlock(page, block);
	if (data == nullptr)
		return false;

	ofstream f;
	f.open(file, ios::out | ios::trunc | ios::binary);
	f.write(data, 65536);
	f.close();
	return true;
}

// requires Visual Studio Code
bool openMemBlock(uint32_t page, uint16_t block, char* outfile) {
	char file[256];
	char cmd[256];
	bool r = outputMemBlock(page, block, file);
	if(outfile)
		strcpy(outfile, file);
	if (r) {
		sprintf(cmd, "code %s", file);
		system(cmd);
	}
	return r;
}
