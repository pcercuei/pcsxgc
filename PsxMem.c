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
* PSX memory functions.
*/

#include "psxmem.h"
#include "r3000a.h"
#include "psxhw.h"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif
#include "Gamecube/fileBrowser/fileBrowser.h"
#include "Gamecube/fileBrowser/fileBrowser-libfat.h"
#include "Gamecube/fileBrowser/fileBrowser-CARD.h"
#include "Gamecube/wiiSXconfig.h"

s8 *psxP = NULL; // Parallel Port (64K)
s8 *psxH = NULL; // Scratch Pad (1K) & Hardware Registers (8K)


/*  Playstation Memory Map (from Playstation doc by Joshua Walker)
0x0000_0000-0x0000_ffff		Kernel (64K)
0x0001_0000-0x001f_ffff		User Memory (1.9 Meg)

0x1f00_0000-0x1f00_ffff		Parallel Port (64K)

0x1f80_0000-0x1f80_03ff		Scratch Pad (1024 bytes)

0x1f80_1000-0x1f80_2fff		Hardware Registers (8K)

0x1fc0_0000-0x1fc7_ffff		BIOS (512K)

0x8000_0000-0x801f_ffff		Kernel and User Memory Mirror (2 Meg) Cached
0x9fc0_0000-0x9fc7_ffff		BIOS Mirror (512K) Cached

0xa000_0000-0xa01f_ffff		Kernel and User Memory Mirror (2 Meg) Uncached
0xbfc0_0000-0xbfc7_ffff		BIOS Mirror (512K) Uncached
*/

int psxMemInit() {
	int i;

	memset(psxCore.psxMemRLUT, 0, 0x10000 * sizeof(void *));
	memset(psxCore.psxMemWLUT, 0, 0x10000 * sizeof(void *));

	psxP = &psxCore.psxM[0x200000];
	psxH = &psxCore.psxM[0x210000];

// MemR
	for (i = 0; i < 0x80; i++) psxCore.psxMemRLUT[i + 0x0000] = (u8 *)&psxCore.psxM[(i & 0x1f) << 16];

	memcpy(psxCore.psxMemRLUT + 0x8000, psxCore.psxMemRLUT, 0x80 * sizeof(void *));
	memcpy(psxCore.psxMemRLUT + 0xa000, psxCore.psxMemRLUT, 0x80 * sizeof(void *));

	psxCore.psxMemRLUT[0x1f00] = (u8 *)psxP;
	psxCore.psxMemRLUT[0x1f80] = (u8 *)psxH;

	for (i = 0; i < 0x08; i++) psxCore.psxMemRLUT[i + 0x1fc0] = (u8 *)&psxCore.psxR[i << 16];

	memcpy(psxCore.psxMemRLUT + 0x9fc0, psxCore.psxMemRLUT + 0x1fc0, 0x08 * sizeof(void *));
	memcpy(psxCore.psxMemRLUT + 0xbfc0, psxCore.psxMemRLUT + 0x1fc0, 0x08 * sizeof(void *));

// MemW
	for (i = 0; i < 0x80; i++) psxCore.psxMemWLUT[i + 0x0000] = (u8 *)&psxCore.psxM[(i & 0x1f) << 16];

	memcpy(psxCore.psxMemWLUT + 0x8000, psxCore.psxMemWLUT, 0x80 * sizeof(void *));
	memcpy(psxCore.psxMemWLUT + 0xa000, psxCore.psxMemWLUT, 0x80 * sizeof(void *));

	psxCore.psxMemWLUT[0x1f00] = (u8 *)psxP;
	psxCore.psxMemWLUT[0x1f80] = (u8 *)psxH;

	return 0;
}

void psxMemReset() {
	int temp;
	memset(psxCore.psxM, 0, 0x00200000);
	memset(psxP, 0, 0x00010000);
  memset(psxCore.psxR, 0, 0x80000);
  if(!biosFile || (biosDevice == BIOSDEVICE_HLE)) {
    Config.HLE = BIOS_HLE;
    return;
  }
  else {
    biosFile->offset = 0; //must reset otherwise the if statement will fail!
  	if(biosFile_readFile(biosFile, &temp, 4) == 4) {  //bios file exists
  	  biosFile->offset = 0;
  		if(biosFile_readFile(biosFile, psxCore.psxR, 0x80000) != 0x80000) { //failed size
  		  //printf("Using HLE\n");
  		  Config.HLE = BIOS_HLE;
  	  }
  		else {
    		//printf("Using BIOS file %s\n",biosFile->name);
  		  Config.HLE = BIOS_USER_DEFINED;
  	  }
  	}
  	else {  //bios fails to open
    	Config.HLE = BIOS_HLE;
  	}
	}
}

void psxMemShutdown() {
}

static int writeok = 1;

u8 psxMemRead8(u32 mem) {
	psxCore.cycle += 0;
	
	u32 t = mem >> 16;
	if (t == 0x1f80) {
		return (mem < 0x1f801000) ? psxHu8(mem) : psxHwRead8(mem);
	} else {
		char *p = (char *)(psxCore.psxMemRLUT[t]);
		return (p == NULL) ? 0 : *(u8 *)(p + (mem & 0xffff));
	}
}

u16 psxMemRead16(u32 mem) {
	psxCore.cycle += 1;

	u32 t = mem >> 16;
	if (t == 0x1f80) {
		return (mem < 0x1f801000) ? psxHu16(mem) : psxHwRead16(mem);
	} else {
		char *p = (char *)(psxCore.psxMemRLUT[t]);
		return (p == NULL) ? 0 : SWAPu16(*(u16 *)(p + (mem & 0xffff)));
	}
}

u32 psxMemRead32(u32 mem) {
	psxCore.cycle += 1;
	
	u32 t = mem >> 16;
	if (t == 0x1f80) {
		return (mem < 0x1f801000) ? psxHu32(mem) : psxHwRead32(mem);
	} else {
		char *p = (char *)(psxCore.psxMemRLUT[t]);
		return (p == NULL) ? 0 : SWAPu32(*(u32 *)(p + (mem & 0xffff)));
	}
}

// These will assume mem is within the special mem range 0x1f800000 0x1f80FFFF
u8 psxDynaMemRead8(u32 mem) {
	return (mem < 0x1f801000) ? psxHu8(mem) : psxHwRead8(mem);
}

u16 psxDynaMemRead16(u32 mem) {
	return (mem < 0x1f801000) ? psxHu16(mem) : psxHwRead16(mem);
}
u32 psxDynaMemRead32(u32 mem) {
	return (mem < 0x1f801000) ? psxHu32(mem) : psxHwRead32(mem);
}

void psxDynaMemWrite8(u32 mem, u8 value) {
	if (mem < 0x1f801000)
		psxHu8(mem) = value;
	else
		psxHwWrite8(mem, value);
}

// Assumes mem is either between 0x1F800000 -> 0x1F80FFFF, or a psxMemWLUT lookup failed.
void psxDynaMemWrite32(u32 mem, u32 value) {
	u32 t = mem >> 16;
	if (t == 0x1f80) {
		if (mem < 0x1f801000)
			psxHu32ref(mem) = SWAPu32(value);
		else
			psxHwWrite32(mem, value);
	} else {
		if (mem != 0xfffe0130) {
			if(!writeok)
				psxCpu->Clear(mem, 1);
		} else {
			int i;

			// a0-44: used for cache flushing
			switch (value) {
				case 0x800: case 0x804:
					if (writeok == 0) break;
					writeok = 0;
					memset(psxCore.psxMemWLUT + 0x0000, 0, 0x80 * sizeof(void *));
					memset(psxCore.psxMemWLUT + 0x8000, 0, 0x80 * sizeof(void *));
					memset(psxCore.psxMemWLUT + 0xa000, 0, 0x80 * sizeof(void *));

					psxCore.ICache_valid = 0;
					break;
				case 0x00: case 0x1e988:
					if (writeok == 1) break;
					writeok = 1;
					for (i = 0; i < 0x80; i++) psxCore.psxMemWLUT[i + 0x0000] = (void *)&psxCore.psxM[(i & 0x1f) << 16];
					memcpy(psxCore.psxMemWLUT + 0x8000, psxCore.psxMemWLUT, 0x80 * sizeof(void *));
					memcpy(psxCore.psxMemWLUT + 0xa000, psxCore.psxMemWLUT, 0x80 * sizeof(void *));
					break;
				default:
					break;
			}
		}
	}
}


void psxMemWrite8(u32 mem, u8 value) {
	psxCore.cycle += 1;

	u32 t = mem >> 16;
	if (t == 0x1f80) {
		if (mem < 0x1f801000)
			psxHu8(mem) = value;
		else
			psxHwWrite8(mem, value);
	} else {
		char *p = (char *)(psxCore.psxMemWLUT[t]);
		if (p != NULL) {
			*(u8 *)(p + (mem & 0xffff)) = value;
			psxCpu->Clear((mem & (~3)), 1);
		}
	}
}

void psxMemWrite16(u32 mem, u16 value) {
	psxCore.cycle += 1;

	u32 t = mem >> 16;
	if (t == 0x1f80) {
		if (mem < 0x1f801000)
			psxHu16ref(mem) = SWAPu16(value);
		else
			psxHwWrite16(mem, value);
	} else {
		char *p = (char *)(psxCore.psxMemWLUT[t]);
		if (p != NULL) {
			*(u16 *)(p + (mem & 0xffff)) = SWAPu16(value);
			psxCpu->Clear((mem & (~3)), 1);
		}
	}
}

void psxMemWrite32(u32 mem, u32 value) {
	psxCore.cycle += 1;

	u32 t = mem >> 16;
	if (t == 0x1f80) {
		if (mem < 0x1f801000)
			psxHu32ref(mem) = SWAPu32(value);
		else
			psxHwWrite32(mem, value);
	} else {
		char *p = (char *)(psxCore.psxMemWLUT[t]);
		if (p != NULL) {
			*(u32 *)(p + (mem & 0xffff)) = SWAPu32(value);
			psxCpu->Clear(mem, 1);
		} else {
			if (mem != 0xfffe0130) {
				if(!writeok)
					psxCpu->Clear(mem, 1);
			} else {
				int i;

				// a0-44: used for cache flushing
				switch (value) {
					case 0x800: case 0x804:
						if (writeok == 0) break;
						writeok = 0;
						memset(psxCore.psxMemWLUT + 0x0000, 0, 0x80 * sizeof(void *));
						memset(psxCore.psxMemWLUT + 0x8000, 0, 0x80 * sizeof(void *));
						memset(psxCore.psxMemWLUT + 0xa000, 0, 0x80 * sizeof(void *));

						psxCore.ICache_valid = 0;
						break;
					case 0x00: case 0x1e988:
						if (writeok == 1) break;
						writeok = 1;
						for (i = 0; i < 0x80; i++) psxCore.psxMemWLUT[i + 0x0000] = (void *)&psxCore.psxM[(i & 0x1f) << 16];
						memcpy(psxCore.psxMemWLUT + 0x8000, psxCore.psxMemWLUT, 0x80 * sizeof(void *));
						memcpy(psxCore.psxMemWLUT + 0xa000, psxCore.psxMemWLUT, 0x80 * sizeof(void *));
						break;
					default:
						break;
				}
			}
		}
	}
}


