/***************************************************************************
 *   Copyright (C) 2007 Ryan Schultz, PCSX-df Team, PCSX team              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

/*
* Internal PSX HLE functions.
*/

#include "psxhle.h"

static void hleDummy() {
	psxCore.pc = psxCore.GPR.n.ra;

	psxBranchTest();
}

static void hleA0() {
	u32 call = psxCore.GPR.n.t1 & 0xff;

	if (biosA0[call]) biosA0[call]();

	psxBranchTest();
}

static void hleB0() {
	u32 call = psxCore.GPR.n.t1 & 0xff;

	if (biosB0[call]) biosB0[call]();

	psxBranchTest();
}

static void hleC0() {
	u32 call = psxCore.GPR.n.t1 & 0xff;

	if (biosC0[call]) biosC0[call]();

	psxBranchTest();
}

static void hleBootstrap() { // 0xbfc00000
	SysPrintf("hleBootstrap\n");
	CheckCdrom();
	LoadCdrom();
	SysPrintf("CdromLabel: \"%s\": PC = %8.8lx (SP = %8.8lx)\n", CdromLabel, psxCore.pc, psxCore.GPR.n.sp);
}

typedef struct {                   
	u32 _pc0;      
	u32 gp0;      
	u32 t_addr;   
	u32 t_size;   
	u32 d_addr;   
	u32 d_size;   
	u32 b_addr;   
	u32 b_size;   
	u32 S_addr;
	u32 s_size;
	u32 _sp,_fp,_gp,ret,base;
} EXEC;

static void hleExecRet() {
	EXEC *header = (EXEC*)PSXM(psxCore.GPR.n.s0);

	SysPrintf("ExecRet %x: %x\n", psxCore.GPR.n.s0, header->ret);

	psxCore.GPR.n.ra = header->ret;
	psxCore.GPR.n.sp = header->_sp;
	psxCore.GPR.n.s8 = header->_fp;
	psxCore.GPR.n.gp = header->_gp;
	psxCore.GPR.n.s0 = header->base;

	psxCore.GPR.n.v0 = 1;
	psxCore.pc = psxCore.GPR.n.ra;
}

void (*psxHLEt[256])() = {
	hleDummy, hleA0, hleB0, hleC0,
	hleBootstrap, hleExecRet,
	hleDummy, hleDummy
};
