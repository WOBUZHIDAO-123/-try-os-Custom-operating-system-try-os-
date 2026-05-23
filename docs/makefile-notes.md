# Makefile 变化记录

每天的学习都会在 `qiu_XX_day/` 下新增一个 Makefile，随着引入新知识，Makefile 也越来越复杂。

## day01 — 没有 Makefile

只有 `asm.bat`（编译）和 `run.bat`（运行），纯汇编写着玩的。

## day02 — 简单的 Makefile

- 用**硬编码路径**：`../z_tools/nask.exe`（手动写路径，没有变量）
- 只有 1 个 .nas → .bin 的规则
- 功能：nask 编译 ipl.nas → 用 edimg 打包成 helloos.img

## day03 — 引入 C 语言，Makefile 进化

- 引入 **TOOLPATH 变量**：`TOOLPATH = ../z_tools/`
- 规则变多：ipl10.nas → ipl10.bin, asmhead.nas → asmhead.bin, bootpack.c → .gas → .nas → .obj
- 链接：bootpack.obj → bootpack.bim → bootpack.hrb
- 最终合并 asmhead.bin + bootpack.hrb → haribote.sys → haribote.img

## day04 — 多目录版（edition_1/2/3）

- **和 day03 几乎一样**，但源码在 `qiu_04_day/edition_X/` 下（多了一层目录）
- 新增 `naskfunc.obj` 链接（汇编函数独立文件）
- ⚠️ **路径坑**：`edition_X/` 在 `qiu_04_day/` 下面，所以路径要写 `../../z_tools/` 而不是 `../z_tools/`

## 路径速查表

| 位置 | 工具链路径 |
|------|-----------|
| `qiu_01_day/` | `../z_tools/` |
| `qiu_02_day/` | `../z_tools/` |
| `qiu_03_day/` | `../z_tools/` |
| `qiu_04_day/edition_X/` | `../../z_tools/` |

## 核心规则（后面每天基本不变）

```
.nas  → nask    → .bin
.c    → cc1     → .gas → gas2nask → .nas → nask → .obj
.obj  → obj2bim (+ .rul) → .bim → bim2hrb → .hrb
.hrb  + asmhead.bin → haribote.sys
.sys  + ipl10.bin → edimg → .img → QEMU
```

## 开始新一天的建议

复制前一天 `qiu_XX_day/` 的 Makefile，根据实际情况改路径：
- 如果 `qiu_XX_day/` 下直接放源码 → `TOOLPATH = ../z_tools/`
- 如果 `qiu_XX_day/` 下有子目录放源码 → `TOOLPATH = ../../z_tools/`
