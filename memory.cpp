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

#include "memory.h"
using namespace std;

struct memBlock
{
	uint16_t addr;
	char* data;
	memBlock* prev;
	memBlock* next;
};

struct memPage
{
	uint32_t addr;
	memBlock* blocks;
	memBlock* currentBlock[4];
	memPage* prev;
	memPage* next;
};

memPage* memList = nullptr;
memPage* currentPage[4] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr
};

void deleteMem();
void initMem() {
	deleteMem();
	memList = nullptr;
	currentPage[0] = nullptr;
	currentPage[1] = nullptr;
	currentPage[2] = nullptr;
	currentPage[3] = nullptr;
}

memBlock* newBlock(uint16_t addr) {
	memBlock* b = new memBlock;
	if (b == nullptr)
		throw "Out of Memory";
	b->addr = addr;
	b->data = nullptr;
	b->prev = nullptr;
	b->next = nullptr;
	return b;
}

memPage* newPage(uint32_t addr) {
	memPage* p = new memPage;
	if (p == nullptr)
		throw "Out of Memory";
	p->addr = addr;
	p->blocks = nullptr;
	p->currentBlock[0] = nullptr;
	p->currentBlock[1] = nullptr;
	p->currentBlock[2] = nullptr;
	p->currentBlock[3] = nullptr;
	p->prev = nullptr;
	p->next = nullptr;
	return p;
}

void deleteBlock(memBlock* block) {
	if (block) {
		if (block->data)
			delete[] block->data;
		if (block->next)
			block->next->prev = block->prev;
		if (block->prev)
			block->prev->next = block->next;
		block->data = nullptr;
		block->next = nullptr;
		block->prev = nullptr;
		delete block;
	}
}

void deletePage(memPage* page) {
	if (page) {
		while (page->blocks && page->blocks->prev)
		{
			page->blocks = page->blocks->prev;
		}
		while (page->blocks)
		{
			memBlock* block = page->blocks;
			page->blocks = block->next;
			deleteBlock(block);
		}
		if (page->next)
			page->next->prev = page->prev;
		if (page->prev)
			page->prev->next = page->next;
		page->next = nullptr;
		page->prev = nullptr;
		page->currentBlock[0] = nullptr;
		page->currentBlock[1] = nullptr;
		page->currentBlock[2] = nullptr;
		page->currentBlock[3] = nullptr;
		delete[] page;
	}
}

void deleteMem() {
	while (memList && memList->prev)
	{
		memList = memList->prev;
	}
	while (memList)
	{
		memPage* page = memList;
		memList = memList->next;
		deletePage(page);
	}
	currentPage[0] = nullptr;
	currentPage[1] = nullptr;
	currentPage[2] = nullptr;
	currentPage[3] = nullptr;
}

memPage* requirePage(uint32_t addr, accessType src) {
	if (memList) {
		if (currentPage[src] == nullptr)
			currentPage[src] = memList;
		if (currentPage[src]->addr == addr)
			return currentPage[src];
		if (currentPage[src]->addr > addr) {
			while (currentPage[src]->prev && currentPage[src]->prev->addr >= addr)
				currentPage[src] = currentPage[src]->prev;
			if (currentPage[src]->addr == addr)
				return currentPage[src];
			else
			{
				memPage* page = newPage(addr);
				page->prev = currentPage[src]->prev;
				page->next = currentPage[src];
				if (currentPage[src]->prev)
					currentPage[src]->prev->next = page;
				else
					memList = page;
				currentPage[src]->prev = page;
				currentPage[src] = page;
				return page;
			}
		}
		else
		{
			while (currentPage[src]->next && currentPage[src]->next->addr <= addr)
				currentPage[src] = currentPage[src]->next;
			if (currentPage[src]->addr == addr)
				return currentPage[src];
			else
			{
				memPage* page = newPage(addr);
				page->next = currentPage[src]->next;
				page->prev = currentPage[src];
				if (currentPage[src]->next)
					currentPage[src]->next->prev = page;
				currentPage[src]->next = page;
				currentPage[src] = page;
				return page;
			}
		}
	}
	else
	{
		memList = newPage(addr);
		currentPage[src] = memList;
		return memList;
	}
}

memBlock* requireBlock(memPage* page, uint16_t addr, accessType src) {
	if (page) {
		if (page->blocks) {
			if (page->currentBlock[src] == nullptr)
				page->currentBlock[src] = page->blocks;
			if (page->currentBlock[src]->addr == addr)
				return page->currentBlock[src];
			if (page->currentBlock[src]->addr > addr) {
				while (page->currentBlock[src]->prev && page->currentBlock[src]->prev->addr >= addr)
					page->currentBlock[src] = page->currentBlock[src]->prev;
				if (page->currentBlock[src]->addr == addr)
					return page->currentBlock[src];
				else
				{
					memBlock* block = newBlock(addr);
					block->prev = page->currentBlock[src]->prev;
					block->next = page->currentBlock[src];
					if (page->currentBlock[src]->prev)
						page->currentBlock[src]->prev->next = block;
					else
						page->blocks = block;
					page->currentBlock[src]->prev = block;
					page->currentBlock[src] = block;
					return block;
				}
			}
			else
			{
				while (page->currentBlock[src]->next && page->currentBlock[src]->next->addr <= addr)
					page->currentBlock[src] = page->currentBlock[src]->next;
				if (page->currentBlock[src]->addr == addr)
					return page->currentBlock[src];
				else
				{
					memBlock* block = newBlock(addr);
					block->next = page->currentBlock[src]->next;
					block->prev = page->currentBlock[src];
					if (page->currentBlock[src]->next)
						page->currentBlock[src]->next->prev = block;
					page->currentBlock[src]->next = block;
					page->currentBlock[src] = block;
					return block;
				}
			}
		}
		else
		{
			page->blocks = newBlock(addr);
			page->currentBlock[src] = page->blocks;
			return page->blocks;
		}
	}
	else
		return nullptr;
}

char read(memBlock* block, uint16_t addr) {
	if (block) {
		if (block->data == nullptr)
			block->data = new char[65536] {0};
		return block->data[addr];
	}
	else
		return 0;
}

void write(memBlock* block, uint16_t addr, char data) {
	if (block) {
		if (block->data == nullptr)
			block->data = new char[65536] {0};
		block->data[addr] = data;
	}
}

char memRead(uint64_t addr, accessType src) {

	memPage* page = requirePage(addr >> 32,src);
	memBlock* block = requireBlock(page, addr >> 16,src);
	return read(block, addr);
}

void memWrite(uint64_t addr, char data, accessType src) {
	memPage* page = requirePage(addr >> 32,src);
	memBlock* block = requireBlock(page, addr >> 16,src);
	write(block, addr, data);
}

void memRead(uint64_t addr, char* buf, uint64_t len, accessType src) {
	for (uint64_t i = 0; i < len; i++)
		buf[i] = memRead(addr + i, src);
}

void memWrite(uint64_t addr, char* buf, uint64_t len, accessType src) {
	for (uint64_t i = 0; i < len; i++)
		memWrite(addr + i, buf[i], src);
}

const char* memGetBlock(uint32_t pageid, uint16_t blockid) {
	memPage* page = requirePage(pageid, DBG);
	memBlock* block = requireBlock(page, blockid, DBG);
	return block->data;
}