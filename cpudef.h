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

#define RAX regs[0x0]
#define RCX regs[0x1]
#define RDX regs[0x2]
#define RBX regs[0x3]
#define RSP regs[0x4]
#define RBP regs[0x5]
#define RSI regs[0x6]
#define RDI regs[0x7]
#define R8  regs[0x8]
#define R9  regs[0x9]
#define R10 regs[0xA]
#define R11 regs[0xB]
#define R12 regs[0xC]
#define R13 regs[0xD]
#define R14 regs[0xE]
#define R15 regs[0xF]

#define RIP rip

#define ZF flags.zf
#define SF flags.sf
#define OF flags.of

#define STAT stat

#define rA command.rargs.ra
#define rB command.rargs.rb