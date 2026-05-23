; naskfunc
; TAB=4

[FORMAT "WCOFF"]      ; 指定输出文件格式为 WCOFF (Windows Common Object File Format)
                    ; 这是 Windows 下的对象文件格式，用于后续链接

[BITS 32]            ; 指定处理器运行模式为 32 位保护模式
                    ; 告诉汇编器生成 32 位指令

[FILE "naskfunc.nas"] ; 指定源文件名，用于调试信息
    GLOBAL _io_hlt   ; 声明全局符号 _io_hlt，使其可以在其他模块中引用
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