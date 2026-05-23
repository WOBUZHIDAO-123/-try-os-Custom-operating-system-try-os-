# ============================================================
# Makefile — haribote 操作系统构建脚本
# 定义工具链路径、编译规则和便捷命令
# ============================================================

# 工具链路径
TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/haribote/

# 工具宏定义（简化命令行调用）
MAKE     = $(TOOLPATH)make.exe -r     # make 工具自身
NASK     = $(TOOLPATH)nask.exe        # 汇编器（NASM 风格）
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet  # C 编译器
GAS2NASK = $(TOOLPATH)gas2nask.exe -a # GNU 汇编 → NASM 格式转换器
OBJ2BIM  = $(TOOLPATH)obj2bim.exe     # 目标文件 → BIM 链接器
BIM2HRB  = $(TOOLPATH)bim2hrb.exe     # BIM → HRB 格式转换器
RULEFILE = $(TOOLPATH)haribote/haribote.rul  # 链接规则文件
EDIMG    = $(TOOLPATH)edimg.exe        # 磁盘映像制作工具
IMGTOL   = $(TOOLPATH)imgtol.com       # 映像写入软盘工具
COPY     = copy                        # Windows 文件复制命令
DEL      = del                         # Windows 文件删除命令

# ============================================================
# 默认目标
# ============================================================

default :
	$(MAKE) img

# ============================================================
# 文件生成规则
# ============================================================

# 汇编 IPL（引导扇区）→ 生成 512 字节二进制文件
ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

# 汇编 asmhead（保护模式切换等初始化代码）→ 生成二进制文件
asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

# 编译 C 源码 → 生成 GNU 汇编格式的 .gas 文件
bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

# 将 GNU 汇编格式转换为本工具链可用的 NASM 格式
bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

# 汇编转换后的 .nas → 生成目标文件 .obj
bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

# 汇编手写的汇编函数（naskfunc.nas）→ 生成目标文件 .obj
naskfunc.obj : naskfunc.nas Makefile
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst

# 链接所有 .obj 文件 → 生成 BIM 格式可执行文件
# 栈大小 3136KB（3MB + 64KB），同时输出链接映射文件
bootpack.bim : bootpack.obj naskfunc.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj naskfunc.obj

# BIM → HRB（haribute 专用可执行格式），加载基址偏移 0
bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

# 将 asmhead.bin 和 bootpack.hrb 合并为系统文件
# /B 以二进制模式合并，asmhead 作为头部，bootpack.hrb 作为后续
haribote.sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb haribote.sys

# 制作 FAT12 格式软盘映像
# 使用模板 fdimg0at.tek，将 ipl10.bin 写入 0 扇区，再复制 sys 文件进去
haribote.img : ipl10.bin haribote.sys Makefile
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		imgout:haribote.img

# ============================================================
# 便捷命令
# ============================================================

# 生成磁盘映像
img :
	$(MAKE) haribote.img

# 生成映像并在 QEMU 虚拟机中运行
run :
	$(MAKE) img
	$(COPY) haribote.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

# 生成映像并写入真实软盘（A: 盘）
install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img

# 清理编译中间产物（保留最终映像 haribote.img）
clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys

# 彻底清理（包括最终映像）
src_only :
	$(MAKE) clean
	-$(DEL) haribote.img