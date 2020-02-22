#pragma once

/* NOT TESTED PROPERLY YET! */
#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  256 // first 8 bits added to distinguish new registers (1 << 8 | 0 .. 7)
#define R9  257
#define R10 258
#define R11 259
#define R12 260
#define R13 261
#define R14 262
#define R15 263 

#define CONCAT(left, right) left ## right
#define LSB(value) ((value) & 0xFF)
#define SHIFT_N_BYTES(value, n) ((value) >> (8 * (n)))
#define TO_BYTES_8(value) LSB(value)
#define TO_BYTES_16(value) LSB((value) & 0xFF), LSB(SHIFT_N_BYTES(value, 1)) // *(note)* unnecessary (& 0xFF)?
#define TO_BYTES_32(value) TO_BYTES_16(value), TO_BYTES_16(SHIFT_N_BYTES(value, 2))
#define TO_BYTES_64(value) TO_BYTES_32(value), TO_BYTES_32(SHIFT_N_BYTES(value, 4))
#define REX_PRFX (0b01000000)
#define REX_B
#define REX_R
#define REX_W
#define REX_X

#define IS_RSP_OR_R12_0(t, f) f
#define IS_RSP_OR_R12_1(t, f) f
#define IS_RSP_OR_R12_2(t, f) f
#define IS_RSP_OR_R12_3(t, f) f
#define IS_RSP_OR_R12_4(t, f) t
#define IS_RSP_OR_R12_5(t, f) f
#define IS_RSP_OR_R12_6(t, f) f
#define IS_RSP_OR_R12_7(t, f) f
#define IS_RSP_OR_R12_256(t, f) f
#define IS_RSP_OR_R12_257(t, f) f
#define IS_RSP_OR_R12_258(t, f) f
#define IS_RSP_OR_R12_259(t, f) f
#define IS_RSP_OR_R12_260(t, f) t
#define IS_RSP_OR_R12_261(t, f) f
#define IS_RSP_OR_R12_262(t, f) f
#define IS_RSP_OR_R12_263(t, f) f

#define IS_NEW_REG_0(t, f) f
#define IS_NEW_REG_1(t, f) f
#define IS_NEW_REG_2(t, f) f
#define IS_NEW_REG_3(t, f) f
#define IS_NEW_REG_4(t, f) f
#define IS_NEW_REG_5(t, f) f
#define IS_NEW_REG_6(t, f) f
#define IS_NEW_REG_7(t, f) f
#define IS_NEW_REG_256(t, f) t
#define IS_NEW_REG_257(t, f) t
#define IS_NEW_REG_258(t, f) t
#define IS_NEW_REG_259(t, f) t
#define IS_NEW_REG_260(t, f) t
#define IS_NEW_REG_261(t, f) t
#define IS_NEW_REG_262(t, f) t
#define IS_NEW_REG_263(t, f) t


#define IS_NOT_RSP_OR_R12_8(disp) TO_BYTES_8(disp)
#define IS_RSP_OR_R12_8(disp) (0x24), TO_BYTES_8(disp)
#define IS_NOT_RSP_OR_R12_32(disp) TO_BYTES_32(disp)
#define IS_RSP_OR_R12_32(disp) (0x24), TO_BYTES_32(disp)

#define RSP_OR_R12_CHECK(reg, t, f) CONCAT(IS_RSP_OR_R12_, reg(t, f))

#define REG_REG(dst_reg, src_reg, op) ((REX_PRFX | 0b00001000) | ((dst_reg) & 0xFF00? 0b00000001: 0) | ((src_reg) & 0xFF00? 0b00000100: 0)), op,  ((0b11000000) | ((dst_reg) & 0xFF) | ((src_reg) & 0xFF) << 3)
#define REG_MEM_DISP(dst_reg, src_mem_reg, disp, op) ((REX_PRFX | 0b00001000) | ((src_mem_reg) & 0xFF00? 0b00000001: 0) | ((dst_reg) & 0xFF00? 0b00000100: 0)), op, (0b01000000) | (((src_mem_reg) & 0xFF) | (((dst_reg) & 0xFF) << 3)), RSP_OR_R12_CHECK(src_mem_reg, IS_RSP_OR_R12_8, IS_NOT_RSP_OR_R12_8)(disp)
#define REG_MEM(dst_reg, src_mem_reg, op) REG_MEM_DISP(dst_reg, src_mem_reg, 0, op)
#define MEM_REG_DISP(dst_mem_reg, src_reg, disp, op) ((REX_PRFX | 0b00001000) | ((dst_mem_reg) & 0xFF00? 0b00000001: 0) | ((src_reg) & 0xFF00? 0b00000100: 0)), op, (0b01000000) | (((dst_mem_reg) & 0xFF) | (((src_reg) & 0xFF) << 3)), RSP_OR_R12_CHECK(dst_mem_reg, IS_RSP_OR_R12_8, IS_NOT_RSP_OR_R12_8)(disp)
#define MEM_REG(dst_mem_reg, src_reg, op) MEM_REG_DISP(dst_mem_reg, src_reg, 0, op)

#define MOVABS_REG(reg, value) (0x48), (reg), TO_BYTES_64(value)

#define MOVABS_64(reg, value) (REX_PRFX | 0b00001000) | (reg & 0xFF00? 0b00000001: 0), (0xB8 | (reg & 0xFF)), TO_BYTES_64(value)

#define MOV_REG_REG(dst_reg, src_reg) REG_REG(dst_reg, src_reg, 0x89)

//#define MOV_REG_MEM_DISP(dst_reg, src_mem_reg, disp) ((REX_PRFX | 0b00001000) | ((dst_reg) & 0xFF00? 0b00000001: 0) | ((src_mem_reg) & 0xFF00? 0b00000100: 0)), 0x8B, disp < 0x256? (0b01000000): (0b10000000) | ((dst_reg) & 0xFF) | ((src_mem_reg) & 0xFF) << 3)), CHECK_DISP(, TO_BYTES_8, TO_BYTES_32)(value)
#define MOV_REG_MEM_DISP_8(dst_reg, src_mem_reg, disp) REG_MEM_DISP(dst_reg, src_mem_reg, disp, 0x8B) //((REX_PRFX | 0b00001000) | ((dst_reg) & 0xFF00? 0b00000001: 0) | ((src_mem_reg) & 0xFF00? 0b00000100: 0)), 0x8B, (0b01000000) | (((src_mem_reg) & 0xFF) | (((dst_reg) & 0xFF) << 3)), RSP_OR_R12_CHECK(src_mem_reg, IS_RSP_OR_R12_8, IS_NOT_RSP_OR_R12_8)(disp)
#define MOV_REG_MEM_DISP_32(dst_reg, src_mem_reg, disp) //((REX_PRFX | 0b00001000) | ((dst_reg) & 0xFF00? 0b00000001: 0) | ((src_mem_reg) & 0xFF00? 0b00000100: 0)), 0x8B, (0b10000000) | (((src_mem_reg) & 0xFF) | (((dst_reg) & 0xFF) << 3)), RSP_OR_R12_CHECK(src_mem_reg, IS_RSP_OR_R12_32, IS_NOT_RSP_OR_R12_32)(disp)

#define MOV_MEM_REG_DISP_8(dst_mem_reg, src_reg, disp) MEM_REG_DISP(dst_mem_reg, src_reg, disp, 0x89) //((REX_PRFX | 0b00001000) | ((dst_mem_reg) & 0xFF00? 0b00000001: 0) | ((src_reg) & 0xFF00? 0b00000100: 0)), 0x89, (0b01000000) | (((dst_mem_reg) & 0xFF) | (((src_reg) & 0xFF) << 3)), RSP_OR_R12_CHECK(dst_mem_reg, IS_RSP_OR_R12_8, IS_NOT_RSP_OR_R12_8)(disp)
#define MOV_MEM_REG_DISP_32(dst_mem_reg, src_reg, disp) //((REX_PRFX | 0b00001000) | ((dst_mem_reg) & 0xFF00? 0b00000001: 0) | ((src_reg) & 0xFF00? 0b00000100: 0)), 0x89, (0b10000000) | (((dst_mem_reg) & 0xFF) | (((src_reg) & 0xFF) << 3)), RSP_OR_R12_CHECK(dst_mem_reg, IS_RSP_OR_R12_32, IS_NOT_RSP_OR_R12_32)(disp)

#define MOV_REG_MEM(dst_reg, src_mem_reg) MOV_REG_MEM_DISP_8(dst_reg, src_mem_reg, 0)
#define MOV_MEM_REG(dst_mem_reg, src_reg) MOV_MEM_REG_DISP_8(dst_mem_reg, src_reg, 0)

#define XOR_REG_REG(dst_reg, src_reg) REG_REG(dst_reg, src_reg, 0x31) 

//TODO(SyntaX): add more instructions
#define ADD
#define CMP
#define JE
#define JL
#define LEA_REG_MEM(dst_reg, src_mem_reg) REG_MEM_DISP(dst_reg, src_mem_reg, 0, 0x8D)
#define LEA_REG_MEM_DISP(dst_reg, src_mem_reg, disp) REG_MEM_DISP(dst_reg, src_mem_reg, disp, 0x8D)
#define POP

//#define MOV_M64_RSP (0x44)
//#define MOV_M64_DISP_8 ()
//#define MOV_M64_DISP_32 ()

//#define MOV_M64_DISP_REG(mem, disp, reg) 0x48, 0x89, , 0x24, TO_BYTES_64(disp)

#define PUSH_NEW_REG(reg) (REX_PRFX | 0b00000001), (0x50 | (reg >> 8))
#define PUSH_OLD_REG(reg) (0x50 | (reg))

#define CHECK_REG(reg, t, f) CONCAT(IS_NEW_REG_, reg(t, f))

#define PUSH_REG(reg) CHECK_REG(reg, PUSH_NEW_REG, PUSH_OLD_REG)(reg)
#define PUSH_REG_VALUE(reg, value) MOVABS_64(reg, value), PUSH_REG(reg)
#define PUSH_RAX PUSH_REG(RAX)
#define PUSH_RAX_VALUE(value) MOVABS_64(RAX, value), PUSH_RAX

#define RET (0xC3)

#define PUSH_IMM_16(value) (0x68), TO_BYTES_16(value)
#define PUSH_IMM_32(value) (0x68), TO_BYTES_32(value)

#define JMP_OLD_REG(reg) 0xFF, 0xE0 | reg
#define JMP_NEW_REG(reg) 0x41, 0xFF, 0xE0 | reg
#define JMP_REG(reg) CHECK_REG(reg, JMP_NEW_REG, JMP_OLD_REG)(reg)

#define INT_3 (0xCC)
