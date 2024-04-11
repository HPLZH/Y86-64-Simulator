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
#include "cpudef.h"
#include "memory.h"
#include "breakpoints.h"

int64_t regs[16];
uint64_t rip;
flags_t flags;
statCode stat;

int64_t& reg(uint8_t regid) {
	return regs[regid & 0b00001111];
}

uint64_t& pc() {
	return rip;
}

flags_t& cc() {
	return flags;
}

statCode cpuStat() {
	return stat;
}

void initCpu() {
	rip = 0x10000;
	RBP = RSP = 0x20000;
	stat = AOK;
}

struct command_t
{
	struct opcode_t
	{
		uint8_t fn : 4;
		uint8_t op : 4;
	} opcode;

	struct rargs_t
	{
		uint8_t rb : 4;
		uint8_t ra : 4;
	} rargs;

	uint64_t data;
} command;

size_t offset;

int64_t vA;
int64_t vB;
uint64_t vC;
int64_t vE;
uint64_t vIP;

void fetch(uint64_t addr, accessType src) {
	offset = 0;
	(uint8_t&)(command.opcode) = memRead(addr, src);
	offset++;
	switch (command.opcode.op)
	{
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 0xA:
	case 0xB:
		(uint8_t&)(command.rargs) = memRead(addr + offset, src);
		offset++;
		if (command.opcode.op != 3 &&
			command.opcode.op != 4 &&
			command.opcode.op != 5) {
			break;
		}
	case 7:
	case 8:
		memRead(addr + offset, (char*)(&(command.data)), sizeof(command.data), src);
		offset += 8;
	default:
		break;
	}
	vIP = rip + offset;
	vA = regs[rA];
	vB = regs[rB];
	vC = command.data;
}

bool cond_check() {
	switch (command.opcode.fn)
	{
	case 0:
		return true;
	case 1:
		return SF || ZF;
	case 2:
		return SF;
	case 3:
		return ZF;
	case 4:
		return !ZF;
	case 5:
		return !SF;
	case 6:
		return !(SF || ZF);
	default:
		stat = INS;
		return false;
	}
}

void exec_halt() {
	stat = HLT;
}

void exec_nop() {}

void exec_rrmov() {
	vE = cond_check() ? vA : vB;
	regs[rB] = vE;
}

void exec_mov() {
	if (command.opcode.op == 3)
		vB = 0;
	vE = vB + vC;
	switch (command.opcode.op)
	{
	case 3:
		regs[rB] = vE;
		break;
	case 4:
		memWrite(vE, (char*)(&vA), sizeof(vA), DAT);
		break;
	case 5:
		memRead(vE, (char*)(regs + rA), 8, DAT);
		break;
	default:
		break;
	}
}

void exec_op() {
	bool sA = vA < 0;
	bool sB = vB < 0;
	switch (command.opcode.fn)
	{
	case 0:
		vE = vB + vA;
		break;
	case 1:
		vE = vB - vA;
		break;
	case 2:
		vE = vB & vA;
		break;
	case 3:
		vE = vB ^ vA;
		break;
	default:
		stat = INS;
		break;
	}
	SF = vE < 0;
	ZF = vE == 0;
	switch (command.opcode.fn)
	{
	case 1:
		sA = !sA;
	case 0:
		OF = (sA == sB) && (SF != sB);
		break;
	default:
		OF = false;
		break;
	}
	regs[rB] = vE;
}

void exec_jmp() {
	if (cond_check())
		vIP = vC;
}

void exec_call() {
	vB = RSP;
	vE = vB - 8;
	memWrite(vE, (char*)(&vIP), sizeof(vIP), STK);
	RSP = vE;
	vIP = vC;
}

void exec_ret() {
	vA = RSP;
	vB = RSP;
	vE = vB + 8;
	memRead(vA, (char*)(&vIP), sizeof(vIP), STK);
	RSP = vE;
}

void exec_stack() {
	vB = RSP;
	if (command.opcode.op == 0xB) {
		vA = RSP;
		vE = vB + 8;
		memRead(vA, (char*)(regs + rA), 8, STK);
	}
	else
	{
		vE = vB - 8;
		memWrite(vE, (char*)(&vA), sizeof(vA), STK);
	}
	RSP = vE;
}

void execute() {
	switch (command.opcode.op)
	{
	case 0:
		exec_halt();
		break;
	case 1:
		exec_nop();
		break;
	case 2:
		exec_rrmov();
		break;
	case 3:
	case 4:
	case 5:
		exec_mov();
		break;
	case 6:
		exec_op();
		break;
	case 7:
		exec_jmp();
		break;
	case 8:
		exec_call();
		break;
	case 9:
		exec_ret();
		break;
	case 0xA:
	case 0xB:
		exec_stack();
		break;
	default:
		stat = INS;
		break;
	}
	rip = vIP;
}

bool runStep() {
	if (stat == AOK) {
		fetch(rip, CMD);
		execute();
		return true;
	}
	return false;
}

statCode run() {
	while (stat == AOK)
	{
		fetch(rip, CMD);
		execute();
	}
	return stat;
}

bool runTo(uint64_t addr) {
	while (stat == AOK && rip != addr)
	{
		fetch(rip, CMD);
		execute();
	}
	return rip == addr;
}

bool runToBreakpoint() {
	while (stat == AOK && !isBreakPoint(rip))
	{
		fetch(rip, CMD);
		execute();
	}
	return isBreakPoint(rip);
}

const char* condstr(uint8_t cond) {
	switch (cond)
	{
	case 1:
		return "le";
	case 2:
		return "l ";
	case 3:
		return "e ";
	case 4:
		return "ne";
	case 5:
		return "ge";
	case 6:
		return "g ";
	default:
		return "  ";
	}
}

const char* regname(uint8_t regid) {
	switch (regid)
	{
	case 0:
		return "RAX";
	case 1:
		return "RCX";
	case 2:
		return "RDX";
	case 3:
		return "RBX";
	case 4:
		return "RSP";
	case 5:
		return "RBP";
	case 6:
		return "RSI";
	case 7:
		return "RDI";
	case 8:
		return "R8 ";
	case 9:
		return "R9 ";
	case 10:
		return "R10";
	case 11:
		return "R11";
	case 12:
		return "R12";
	case 13:
		return "R13";
	case 14:
		return "R14";
	case 15:
		return "R15";
	default:
		return "   ";
		break;
	}
}

void decode(uint64_t addr, char* output) {
	fetch(addr, DBG);
	switch (command.opcode.op)
	{
	case 0:
		sprintf(output, "halt");
		break;
	case 1:
		sprintf(output, "nop");
		break;
	case 2:
		if (command.opcode.fn == 0)
			sprintf(output, "mov  %s, %s", regname(rB), regname(rA));
		else if (command.opcode.fn > 6)
			sprintf(output, "data");
		else
			sprintf(output, "cm%s %s, %s", condstr(command.opcode.fn), regname(rB), regname(rA));
		break;
	case 3:
		sprintf(output, "mov  %s, 0x%llX", regname(rB), command.data);
		break;
	case 4:
		sprintf(output, "mov  [%s+0x%llX], %s", regname(rB), command.data, regname(rA));
		break;
	case 5:
		sprintf(output, "mov  %s, [%s+0x%llX]", regname(rA), regname(rB), command.data);
		break;
	case 6:
		switch (command.opcode.fn)
		{
		case 0:
			sprintf(output, "add  %s, %s", regname(rB), regname(rA));
			break;
		case 1:
			sprintf(output, "sub  %s, %s", regname(rB), regname(rA));
			break;
		case 2:
			sprintf(output, "and  %s, %s", regname(rB), regname(rA));
			break;
		case 3:
			sprintf(output, "xor  %s, %s", regname(rB), regname(rA));
			break;
		default:
			sprintf(output, "data");
			break;
		}
		break;
	case 7:
		if (command.opcode.fn == 0)
			sprintf(output, "jmp  0x%llX", command.data);
		else if (command.opcode.fn > 6)
			sprintf(output, "data");
		else
			sprintf(output, "j%s   0x%llX", condstr(command.opcode.fn), command.data);
		break;
	case 8:
		sprintf(output, "call  0x%llX", command.data);
		break;
	case 9:
		sprintf(output, "ret");
		break;
	case 0xA:
		sprintf(output, "push  %s", regname(rA));
		break;
	case 0xB:
		sprintf(output, "pop   %s", regname(rA));
		break;
	default:
		sprintf(output, "data");
		break;
	}
}