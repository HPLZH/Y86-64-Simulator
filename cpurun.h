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
#include "cpustruct.h"

#define RAX reg(0x0)
#define RCX reg(0x1)
#define RDX reg(0x2)
#define RBX reg(0x3)
#define RSP reg(0x4)
#define RBP reg(0x5)
#define RSI reg(0x6)
#define RDI reg(0x7)
#define R8  reg(0x8)
#define R9  reg(0x9)
#define R10 reg(0xA)
#define R11 reg(0xB)
#define R12 reg(0xC)
#define R13 reg(0xD)
#define R14 reg(0xE)
#define R15 reg(0xF)

#define PC  pc()
#define RIP pc()

#define ZF cc().zf
#define SF cc().sf
#define OF cc().of

#define STAT cpuStat()

int64_t& reg(uint8_t regid);
uint64_t& pc();
flags_t& cc();
statCode cpuStat();

bool runStep();
statCode run();
bool runTo(uint64_t addr);
bool runToBreakpoint();

const char* regname(uint8_t regid);
void decode(uint64_t addr, char* output);