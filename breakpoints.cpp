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

#include <cstdint>
#include <cstdio>

struct bpNode
{
	uint64_t addr;
	bpNode* next;
};

bpNode* bpList = nullptr;

void clearBreakpoints() {
	while (bpList)
	{
		bpNode* p = bpList;
		bpList = bpList->next;
		delete p;
	}
}

void addBreakpoint(uint64_t addr) {
	bpNode* p = new bpNode;
	p->addr = addr;
	p->next = bpList;
	bpList = p;
}

void deleteBreakpoint(uint64_t addr) {
	bpNode* prev = nullptr;
	bpNode* p = bpList;
	while (p)
	{
		bpNode* nprev = p;
		bpNode* np = p->next;
		if (p->addr == addr) {
			if (prev)
				prev->next = p->next;
			else
				bpList = p->next;
			nprev = prev;
			delete p;
		}
		prev = nprev;
		p = np;
	}
}

void listBreakpoints() {
	if (bpList == nullptr) {
		printf("No breakpoint.\n");
		return;
	}
	bpNode* p = bpList;
	while (p)
	{
		printf("0x%016llX\n", p->addr);
		p = p->next;
	}
}

bool isBreakPoint(uint64_t addr) {
	bpNode* p = bpList;
	while (p)
	{
		if (p->addr == addr)
			return true;
		p = p->next;
	}
	return false;
}