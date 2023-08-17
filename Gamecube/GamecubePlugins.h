/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GAMECUBE_PLUGINS_H
#define GAMECUBE_PLUGINS_H

#ifndef NULL
#define NULL ((void*)0)
#endif
#include <libpcsxcore/decode_xa.h>
#include <libpcsxcore/plugins.h>
#include <psemu_plugin_defs.h>

#define SYMS_PER_LIB 22
typedef struct {
	const char* lib;
	int   numSyms;
	struct {
		const char* sym;
		void* pntr;
	} syms[SYMS_PER_LIB];
} PluginTable;
#define NUM_PLUGINS 6

/* GPULIB GPU */
long GPULIB_GPUopen(unsigned long *, char *, char *); 
long GPULIB_GPUinit(void);
long GPULIB_GPUshutdown(void);
long GPULIB_GPUclose(void);
void GPULIB_GPUwriteStatus(unsigned long);
void GPULIB_GPUwriteData(unsigned long);
void GPULIB_GPUwriteDataMem(unsigned long *, int);
unsigned long GPULIB_GPUreadStatus(void);
unsigned long GPULIB_GPUreadData(void);
void GPULIB_GPUreadDataMem(unsigned long *, int);
long GPULIB_GPUdmaChain(unsigned long *,unsigned long);
void GPULIB_GPUupdateLace(void);
void GPULIB_GPUdisplayText(char *);
long GPULIB_GPUfreeze(unsigned long,GPUFreeze_t *);


/* PAD */
//typedef long (* PADopen)(unsigned long *);
extern long PAD__init(long);
extern long PAD__shutdown(void);	
extern long PAD__open(void);
extern long PAD__close(void);
extern long PAD1__readPort1(PadDataS *pad);
extern long PAD2__readPort2(PadDataS *pad);
unsigned char CALLBACK PAD1__poll(unsigned char value);
unsigned char CALLBACK PAD2__poll(unsigned char value);
unsigned char CALLBACK PAD1__startPoll(int pad);
unsigned char CALLBACK PAD2__startPoll(int pad);


/* DFSound Plugin */
int CALLBACK DFS_SPUplayCDDAchannel(short *pcm, int nbytes, unsigned int cycle, int is_start);
void CALLBACK DFS_SPUplayADPCMchannel(xa_decode_t *xap, unsigned int cycle, int is_start);
void CALLBACK DFS_SPUupdate(void);
void CALLBACK DFS_SPUasync(unsigned int cycle, unsigned int flags);
long CALLBACK DFS_SPUinit(void);
long CALLBACK DFS_SPUshutdown(void);
long CALLBACK DFS_SPUtest(void);
long CALLBACK DFS_SPUconfigure(void);
void CALLBACK DFS_SPUabout(void);
void CALLBACK DFS_SPUregisterCallback(void (CALLBACK *callback)(void));
void CALLBACK DFS_SPUregisterCDDAVolume(void (CALLBACK *CDDAVcallback)(short, short));
void CALLBACK DFS_SPUregisterScheduleCb(void (CALLBACK *callback)(unsigned int));
void CALLBACK DFS_SPUwriteDMAMem(unsigned short *pusPSXMem, int iSize, unsigned int cycles);
void CALLBACK DFS_SPUwriteDMA(unsigned short val);
void CALLBACK DFS_SPUreadDMAMem(unsigned short *pusPSXMem, int iSize, unsigned int cycles);
unsigned short CALLBACK DFS_SPUreadDMA(void);
unsigned short CALLBACK DFS_SPUreadRegister(unsigned long reg);
void CALLBACK DFS_SPUwriteRegister(unsigned long reg, unsigned short val, unsigned int cycles);
long CALLBACK DFS_SPUopen(void);
long CALLBACK DFS_SPUclose(void);
long CALLBACK DFS_SPUfreeze(uint32_t ulFreezeMode, SPUFreeze_t * pF, uint32_t cycles);

#define EMPTY_PLUGIN \
	{ NULL,      \
	  0,         \
	  { { NULL,  \
	      NULL }, } }

#define PAD1_PLUGIN \
	{ "plugins/builtin_pad",      \
	  7,         \
	  { { "PADinit",  \
	      (void*)PAD__init }, \
	    { "PADshutdown",	\
	      (void*)PAD__shutdown}, \
	    { "PADopen", \
	      (void*)PAD__open}, \
	    { "PADclose", \
	      (void*)PAD__close}, \
	    { "PADreadPort1", \
	      (void*)PAD1__readPort1}, \
	    { "PADstartPoll", \
	      (void*)PAD1__startPoll}, \
	    { "PADpoll", \
	      (void*)PAD1__poll} \
	       } }
	    
#define PAD2_PLUGIN \
	{ "plugins/builtin_pad2",      \
	  7,         \
	  { { "PADinit",  \
	      (void*)PAD__init }, \
	    { "PADshutdown",	\
	      (void*)PAD__shutdown}, \
	    { "PADopen", \
	      (void*)PAD__open}, \
	    { "PADclose", \
	      (void*)PAD__close}, \
	    { "PADreadPort2", \
	      (void*)PAD2__readPort2}, \
	    { "PADstartPoll", \
	      (void*)PAD2__startPoll}, \
	    { "PADpoll", \
	      (void*)PAD2__poll} \
	       } }

#define DFSOUND_PLUGIN \
	{ "plugins/builtin_spu",      \
	  20,         \
	  { { "SPUinit",  \
	      (void*)DFS_SPUinit }, \
	    { "SPUshutdown",	\
	      (void*)DFS_SPUshutdown}, \
	    { "SPUopen", \
	      (void*)DFS_SPUopen}, \
	    { "SPUclose", \
	      (void*)DFS_SPUclose}, \
	    { "SPUconfigure", \
	      (void*)DFS_SPUconfigure}, \
	    { "SPUabout", \
	      (void*)DFS_SPUabout}, \
	    { "SPUtest", \
	      (void*)DFS_SPUtest}, \
	    { "SPUwriteRegister", \
	      (void*)DFS_SPUwriteRegister}, \
	    { "SPUreadRegister", \
	      (void*)DFS_SPUreadRegister}, \
	    { "SPUwriteDMA", \
	      (void*)DFS_SPUwriteDMA}, \
	    { "SPUreadDMA", \
	      (void*)DFS_SPUreadDMA}, \
	    { "SPUwriteDMAMem", \
	      (void*)DFS_SPUwriteDMAMem}, \
	    { "SPUreadDMAMem", \
	      (void*)DFS_SPUreadDMAMem}, \
	    { "SPUplayADPCMchannel", \
	      (void*)DFS_SPUplayADPCMchannel}, \
	    { "SPUfreeze", \
	      (void*)DFS_SPUfreeze}, \
	    { "SPUregisterCallback", \
	      (void*)DFS_SPUregisterCallback}, \
	    { "SPUregisterCDDAVolume", \
	      (void*)DFS_SPUregisterCDDAVolume}, \
	    { "SPUplayCDDAchannel", \
	      (void*)DFS_SPUplayCDDAchannel}, \
		{ "SPUregisterScheduleCb", \
	      (void*)DFS_SPUregisterScheduleCb}, \
	    { "SPUasync", \
	      (void*)DFS_SPUasync} \
	       } }

#define GPU_GPULIB_PLUGIN \
	{ "plugins/builtin_gpu",      \
	  14,         \
	  { { "GPUinit",  \
	      (void*)GPULIB_GPUinit }, \
	    { "GPUshutdown",	\
	      (void*)GPULIB_GPUshutdown}, \
	    { "GPUopen", \
	      (void*)GPULIB_GPUopen}, \
	    { "GPUclose", \
	      (void*)GPULIB_GPUclose}, \
	    { "GPUwriteStatus", \
	      (void*)GPULIB_GPUwriteStatus}, \
	    { "GPUwriteData", \
	      (void*)GPULIB_GPUwriteData}, \
	    { "GPUwriteDataMem", \
	      (void*)GPULIB_GPUwriteDataMem}, \
	    { "GPUreadStatus", \
	      (void*)GPULIB_GPUreadStatus}, \
	    { "GPUreadData", \
	      (void*)GPULIB_GPUreadData}, \
	    { "GPUreadDataMem", \
	      (void*)GPULIB_GPUreadDataMem}, \
	    { "GPUdmaChain", \
	      (void*)GPULIB_GPUdmaChain}, \
	    { "GPUfreeze", \
	      (void*)GPULIB_GPUfreeze}, \
	    { "GPUupdateLace", \
	      (void*)GPULIB_GPUupdateLace} \
	       } }

#define PLUGIN_SLOT_0 EMPTY_PLUGIN
#define PLUGIN_SLOT_1 PAD1_PLUGIN
#define PLUGIN_SLOT_2 PAD2_PLUGIN
#define PLUGIN_SLOT_3 DFSOUND_PLUGIN
#define PLUGIN_SLOT_4 GPU_GPULIB_PLUGIN
#define PLUGIN_SLOT_5 EMPTY_PLUGIN

#endif

