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
enum statCode
{
	AOK = 1,
	HLT = 2,
	ADR = 3,
	INS = 4
};

struct flags_t
{
	bool zf : 1;
	bool sf : 1;
	bool of : 1;
};