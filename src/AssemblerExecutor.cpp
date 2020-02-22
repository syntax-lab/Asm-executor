#include <iostream>
#include <Windows.h>
#include "Preprocessor_helper.h"

//TODO(SyntaX): clean this code up!!!

#define DEBUG_INFO(object) std::cout << std::hex << std::boolalpha << "DEBUG>" << #object << ": " << object << std::dec << std::endl
#define NOT_TEST_CASE 1
#define STACK_SIZE 0x200 //should be divided by 4, first 0x20 elements reserved for internal purposes
#define STACK_SPACE_RESERVED 0x20
#define STACK_INITIAL_POINTER 0x100

extern "C" void user_asm_function();

#pragma optimize("", off)

extern "C" volatile size_t stack_ptr = 0;

//TODO(SyntaX): add more registers
struct regs_state{
	union {
		size_t regs[16]{ 0 };
		struct {
			size_t RAX_;
			size_t RBX_;
			size_t RCX_;
			size_t RDX_;
			size_t RSI_;
			size_t RDI_;
			size_t RSP_;
			size_t RBP_;
			size_t R8_;
			size_t R9_;
			size_t R10_;
			size_t R11_;
			size_t R12_;
			size_t R13_;
			size_t R14_;
			size_t R15_;
		};
	};
	uint32_t eflags = 0;
};

/*
	Explanation:
	until end of store_regs(...) function, code is generated in compiler compatible manner then
	`code_environment_preparation`(array of opcodes) is executed, that section is partially controlled by you (you set stack up as you need) then
	your assembly code `user_asm_function()` is executed and results are printed then
	load_regs(...) function is executed which restore registers state and stack as it was before store_regs(...) function was called
	after that point control flow of the program is preserved
*/

extern "C" void store_regs(volatile struct regs_state&, size_t rdx = 0, size_t r8 = 0, size_t r9 = 0); //NOTE: registers are set to desired value(0) and PF\ZF flags are also set to 1 by performing XOR operation !!!it works that way because of Windows calling convention, tested on 19.24.28316 version of (x64)msvc and latest versions(7.1 - 9.1) of gcc!!!
extern "C" void store_results();
//extern "C" void load_regs();

void print_results(const volatile size_t(&stack)[STACK_SIZE]){
	printf("RAX: %p\n", reinterpret_cast<void*>(stack[4]));
	printf("RBX: %p\n", reinterpret_cast<void*>(stack[5]));
	printf("RCX: %p\n", reinterpret_cast<void*>(stack[6]));
	printf("RDX: %p\n", reinterpret_cast<void*>(stack[7]));
	printf("RSI: %p\n", reinterpret_cast<void*>(stack[8]));
	printf("RDI: %p\n", reinterpret_cast<void*>(stack[9]));
	printf("RSP: %p\n", reinterpret_cast<void*>(stack[10]));
	printf("RBP: %p\n", reinterpret_cast<void*>(stack[11]));
	printf("R8 : %p\n", reinterpret_cast<void*>(stack[12]));
	printf("R9 : %p\n", reinterpret_cast<void*>(stack[13]));
	printf("R10: %p\n", reinterpret_cast<void*>(stack[14]));
	printf("R11: %p\n", reinterpret_cast<void*>(stack[15]));
	printf("R12: %p\n", reinterpret_cast<void*>(stack[16]));
	printf("R13: %p\n", reinterpret_cast<void*>(stack[17]));
	printf("R14: %p\n", reinterpret_cast<void*>(stack[18]));
	printf("R15: %p\n", reinterpret_cast<void*>(stack[19]));
	
	printf("stack dump:\n");
	for(unsigned int idx = STACK_SPACE_RESERVED; idx < STACK_SIZE; idx += 4){
		printf("%.4x: %p %p %p %p\n", idx - STACK_SPACE_RESERVED, reinterpret_cast<void*>(stack[idx]), reinterpret_cast<void*>(stack[idx + 1]), reinterpret_cast<void*>(stack[idx + 2]), reinterpret_cast<void*>(stack[idx + 3]));
	}
}

void asm_code_executor(){
	volatile size_t internal_stack[STACK_SIZE];

	memset((void*)&internal_stack[0], 0xCC, STACK_SIZE * sizeof(size_t));

	volatile regs_state regs_state_table;

	stack_ptr = (size_t)&internal_stack[0];

#if NOT_TEST_CASE
	unsigned char code_environment_preparation[]{
		MOVABS_64(RDI, (size_t)&internal_stack[0]),
		MOVABS_64(RSI, (size_t)&internal_stack[STACK_SPACE_RESERVED]),
		MOV_MEM_REG(RDI, RSI),
		MOVABS_64(RSI, (size_t)&internal_stack[STACK_SIZE - 1]),
		MOV_MEM_REG_DISP_8(RDI, RSI, 8),
		MOVABS_64(RSI, (size_t)&regs_state_table),
		MOV_MEM_REG_DISP_8(RDI, RSI, 0x10),
		MOVABS_64(RSI, (size_t)&internal_stack[STACK_INITIAL_POINTER]),
		MOV_MEM_REG_DISP_8(RDI, RSP, 0x18),
		MOV_REG_REG(RSP, RSI),
		/* prepare stack here... */ 
		PUSH_REG_VALUE(RSI, 0xB8C70926), //EXAMPLE: PicoCTF 2019 asm3 RE task
		PUSH_REG_VALUE(RSI, 0xF55018AFFE8CF7A4),
		/* ret_addr */ 
		PUSH_REG_VALUE(RSI, (size_t)&store_results)  
		/* ...up to here */,
		MOVABS_64(RSI, (size_t)&user_asm_function),
		JMP_REG(RSI), /* INT_3 */ };
	/*
		MOVABS RDI, &internal_stack[0]
		MOVABS RSI, &internal_stack[0x20]
		MOV [RDI], RSI
		MOVABS RSI, &internal_stack[0x1FF]
		MOV [RDI + 8], RSI
		MOVABS RSI, &regs_state_table
		MOV [RDI + 0x10], RSI
		MOVABS RSI, &internal_stack[0x100]
		MOV [RDI + 0x18], RSP
		MOV RSP, RSI
		MOVABS RSI, 0
		PUSH RSI
		MOVABS RSI, 1
		PUSH RSI
		MOVABS RSI, &store_results
		PuSH RSI
		MOVABS RSI, &user_asm_function
		JMP RSI
	*/
#else
	unsigned char code_environment_preparation[]{
		#include "test_opcodes.tc"
	};
#endif
	DWORD dwOldProt = 0;

	VirtualProtect((LPVOID)((size_t)&code_environment_preparation & 0xFFFFFFFFFFFFF000), 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProt);

	store_regs(regs_state_table);

	((void(*)())&code_environment_preparation)();

	print_results(internal_stack);

	VirtualProtect((LPVOID)((size_t)&code_environment_preparation & 0xFFFFFFFFFFFFF000), 0x1000, PAGE_READWRITE, &dwOldProt);

}
#pragma optimize("", on)

int main(){
	asm_code_executor();
}