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

#pragma once
#include <cstdint>

enum accessType
{
	DAT = 0,
	CMD = 1,
	STK = 2,
	DBG = 3
};

char memRead(uint64_t addr, accessType src);
void memWrite(uint64_t addr, char data, accessType src);
void memRead(uint64_t addr, char* buf, uint64_t len, accessType src);
void memWrite(uint64_t addr, char* buf, uint64_t len, accessType src);