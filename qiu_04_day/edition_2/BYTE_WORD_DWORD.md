# BYTE / WORD / DWORD 与 C 类型对应关系

## 汇编尺寸限定符

| 指令 | 位数 | 字节数 |
|------|------|--------|
| `BYTE` | 8 bit | **1 字节** |
| `WORD` | 16 bit | **2 字节** |
| `DWORD` | 32 bit | **4 字节** |

## 在你的代码中

**`asmhead.nas`** 三种都用到了：

```nasm
MOV BYTE [VMODE], 8      ; 写 1 字节 → 地址 0x0ff2 → [0x08]
MOV WORD [SCRNX], 320    ; 写 2 字节 → 地址 0x0ff4 → [0x40, 0x01]
MOV DWORD [VRAM],0xa0000 ; 写 4 字节 → 地址 0x0ff8 → [0x00,0x00,0x0a,0x00]
```

**`bootpack.nas`**（C 编译结果）:
```nasm
MOV BYTE [EDX], AL   ; ← 对应 C 的 *P = i & 0x0f（char 写入）
```

## 与 C 类型的对应（32位保护模式）

| C 类型 | 字节 | 等价汇编 | 你代码中的例证 |
|--------|------|---------|---------------|
| `char` | 1 | **`BYTE`** | `char *P; *P = ...` |
| `short` | 2 | **`WORD`** | （未使用） |
| `int` | 4 | **`DWORD`** | `int i;` 循环变量 |

## 核心规律

> **C 类型告诉编译器生成多大宽度的指令，汇编限定符告诉 CPU 访问多少字节的内存，两者本质是同一件事的不同表达。**

```
C 代码:  char *P;  *P = val;    →  MOV BYTE [addr], val
C 代码:  int  *P;  *P = val;    →  MOV DWORD [addr], val
```

指针步进也同理：`char*` 的 `P++` 前进 1 字节，`int*` 的 `P++` 前进 4 字节。
