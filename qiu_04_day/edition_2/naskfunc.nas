; naskfunc
; TAB=4

[FORMAT "WCOFF"]      ; 指定输出文件格式为 WCOFF (Windows Common Object File Format)
                    ; 这是 Windows 下的对象文件格式，用于后续链接

[BITS 32]            ; 指定处理器运行模式为 32 位保护模式
                    ; 告诉汇编器生成 32 位指令

[INSTRSET "i486p"]              ; 告知汇编器允许使用 486 指令集

[FILE "naskfunc.nas"] ; 指定源文件名，用于调试信息
    GLOBAL _io_hlt   ; 声明全局符号 _io_hlt，使其可以在其他模块中引用
    GLOBAL _write_mem8   ; 声明全局符号 _write_mem8
                    ; GLOBAL 表示该符号对外可见，可供链接器使用

[SECTION .text]      ; 定义代码段（可执行代码区域）
                    		; .text 段通常存放程序的可执行指令

_io_hlt:             ; 函数标签定义，创建一个名为 _io_hlt 的符号
			; 表示这是一个标号（label），用于标识代码位置
			;函数入口标签（下划线前缀是 C 语言调用约定）

    HLT              ; x86 汇编指令：Halt（暂停）
                    ; 让 CPU 进入等待状态，直到发生中断

    RET              ; 返回指令
                    ; 从函数调用中返回到调用者的位置

_write_mem8:        ; void write_mem8(int addr, int data);
        MOV     ECX,[ESP+4]     ; 写入地址 ECX = addr (第一个参数，位于返回地址之上4字节)
                                ;ESP (Extended Stack Pointer) 是 x86 32位模式下的栈指针寄存器，它始终指向当前栈顶的内存地址。

        MOV     AL,[ESP+8]    ; 写入数据 AL = data (第二个参数，位于返回地址之上8字节)
        MOV     [ECX],AL      ; 将 AL 写入地址 ECX 指向的内存 (即 *addr = data)
        RET                   ; 返回调用者
