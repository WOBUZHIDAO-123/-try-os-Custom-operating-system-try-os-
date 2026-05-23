TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/haribote/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com
COPY     = copy
DEL      = del

# ============================================================
# 默认目标
# ============================================================

default :
	$(MAKE) img

# ============================================================
# 文件生成规则
# ============================================================

# 汇编 IPL（初始程序加载器，引导扇区）→ 生成二进制文件
ipl10.bin : ipl10.nas Makefile
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

# 汇编 asmhead（保护模式切换、GDT 设置等）→ 生成二进制文件
asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

# 编译 C 源码 → 生成 GNU 汇编格式（.gas）中间文件
bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

# 将 GNU 汇编格式转换为 NASM 格式（.nas），适配本工具链
bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

# 汇编转换后的 .nas 文件 → 生成目标文件（.obj）
bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

# 链接目标文件 → 生成 BIM 格式的中间二进制
# 栈大小设定为 3136KB（3MB + 64KB），映射文件输出到 bootpack.map
bootpack.bim : bootpack.obj Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj

# 将 BIM 文件转换为 HRB 格式（haribote 可执行格式），加载基址偏移为 0
bootpack.hrb : bootpack.bim Makefile
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

# 合并 asmhead.bin（头部）与 bootpack.hrb（内核）→ 生成系统文件 haribote.sys
haribote.sys : asmhead.bin bootpack.hrb Makefile
	copy /B asmhead.bin+bootpack.hrb haribote.sys

# 制作磁盘映像 haribote.img
# 使用模板 fdimg0at.tek 创建标准 FAT12 格式软盘映像
# 将 ipl10.bin（512 字节）写入第一个扇区
# 将 haribote.sys 作为普通文件复制到映像内
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

# 生成映像并在 QEMU 中运行
run :
	$(MAKE) img
	$(COPY) haribote.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

# 生成映像并写入真实软盘（A: 盘）
install :
	$(MAKE) img
	$(IMGTOL) w a: haribote.img

# 清理编译中间产物，保留最终映像
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

# 仅保留源码（清理所有生成文件，包括磁盘映像）
src_only :
	$(MAKE) clean
	-$(DEL) haribote.img