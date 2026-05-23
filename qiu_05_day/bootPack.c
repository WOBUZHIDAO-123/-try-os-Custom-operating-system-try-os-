
// ============================================================
// I/O 底层函数声明（实现在 naskfunc.nas 中，C 语言无法直接操作硬件端口）
// ============================================================
void io_hlt(void);                // 执行 HLT 指令，CPU 进入停机状态，等待下一次中断唤醒
void io_cli(void);                // 执行 CLI 指令，清除 EFLAGS 的 IF 位，禁止可屏蔽中断
void io_out8(int port, int data); // 执行 OUT 指令，向指定 I/O 端口写入 1 字节数据
int io_load_eflags(void);         // 执行 PUSHF+POP EAX，读取当前 EFLAGS 寄存器值并返回
void io_store_eflags(int eflags); // 执行 PUSH EAX+POPF，将给定值写入 EFLAGS 寄存器

// ============================================================
// 本地函数前置声明（在文件末尾定义，此处声明避免编译警告）
// ============================================================
void init_palette(void);  // 初始化前 16 色的调色板
void set_palette(int start, int end, unsigned char *rgb);// ↑ 向 VGA 硬件写入调色板数据，参数: 起始色号, 结束色号, RGB 数组指针(每色 3 字节)
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);// ↑ 在显存中填充矩形区域
void init_sceen(char *vram, int xsize, int ysize);// ↑ 绘制桌面：浅蓝背景 + 底部灰色任务栏 + 左右两个按钮（对应原作者 init_screen）
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);// ↑ 在指定坐标绘制一个 8×16 的等宽字符

// ============================================================
// 颜色号宏 —— 给调色板编号起个有意义的名字，方便代码中使用
// 注意：这里的值只是调色板中的编号，不是真实颜色。真实颜色由 init_palette() 设定。
// ============================================================
#define COL8_000000 0  // 黑
#define COL8_FF0000 1  // 亮红
#define COL8_00FF00 2  // 亮绿
#define COL8_FFFF00 3  // 亮黄
#define COL8_0000FF 4  // 亮蓝
#define COL8_FF00FF 5  // 亮紫
#define COL8_00FFFF 6  // 浅亮蓝
#define COL8_FFFFFF 7  // 白
#define COL8_C6C6C6 8  // 亮灰
#define COL8_840000 9  // 暗红
#define COL8_008400 10 // 暗绿
#define COL8_848400 11 // 暗黄
#define COL8_000084 12 // 暗蓝
#define COL8_840084 13 // 暗紫
#define COL8_008484 14 // 暗浅蓝
#define COL8_848484 15 // 暗灰

// ============================================================
// BOOTINFO 结构体 —— 对应 asmhead.nas 写入到 0x0ff0 地址的启动信息
// 成员排列顺序必须与 asmhead.nas 中的 EQU 偏移严格一致，否则读取错位
// ============================================================
//  内存地址      | 大小 | 成员      | asmhead 写入内容
//  ─────────────┼──────┼───────────┼─────────────────
//  0x0ff0       | 1B   | cyls      | 柱面数（IPL 探测）
//  0x0ff1       | 1B   | leds      | 键盘 LED 状态
//  0x0ff2       | 1B   | vmode     | 显示模式色彩位数 (8)
//  0x0ff3       | 1B   | reserve   | 保留/对齐填充
//  0x0ff4       | 2B   | scrnx     | 屏幕宽度 (320)
//  0x0ff6       | 2B   | scrny     | 屏幕高度 (200)
//  0x0ff8       | 4B   | vram      | 显存起始地址 (0xA0000)
// ============================================================
struct BOOTINFO
{
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
};

// ============================================================
// 字符'A'的 8×16 点阵字体数据
// 每个字节对应字符的一行像素（共 16 行），bit7=最左列, bit0=最右列
// 例如 0x18 = 00011000? → 第 4、5 列亮，其余暗（构成 A 的左右竖笔）
// ============================================================
static char font_A[16] = {
	0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
	0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00};


// ============================================================
// 内核主函数 —— 由 asmhead.nas 跳转至此（已处于 32 位保护模式）
// ============================================================
void HariMain(void)
{
	// 通过 BOOTINFO 结构体读取 asmhead 保存的启动信息
	// 只需记住一个基址 0x0ff0，成员偏移由编译器自动计算
	struct BOOTINFO *binfo = (struct BOOTINFO *)0x0ff0;

	init_palette();                                        // ① 设定调色板（16 种颜色）
	init_sceen(binfo->vram, binfo->scrnx, binfo->scrny);   // ② 绘制桌面背景 + 任务栏 + 按钮
	putfont8(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, font_A); // ③ 在 (8,8) 显示白色 A

	for (;;) {      // ④ 主循环：空闲即停机，由硬件中断唤醒
		io_hlt();
	}
}

// ============================================================
// init_palette — 初始化前 16 种颜色的调色板
// 显存中每个像素只存颜色编号(0~255)，VGA 硬件用调色板把编号翻译成真实 RGB
// static char table_rgb[16*3] 等于汇编的 DB 指令：编译时在数据段分配 48 字节
// ============================================================
void init_palette(void)
{
	static unsigned char table_rgb[16 * 3] = {   // 16 色 × 3 分量(R,G,B)
		0x00, 0x00, 0x00,   //  0: 黑     (  0,   0,   0)
		0xff, 0x00, 0x00,   //  1: 亮红   (255,   0,   0)
		0x00, 0xff, 0x00,   //  2: 亮绿   (  0, 255,   0)
		0xff, 0xff, 0x00,   //  3: 亮黄   (255, 255,   0)
		0x00, 0x00, 0xff,   //  4: 亮蓝   (  0,   0, 255)
		0xff, 0x00, 0xff,   //  5: 亮紫   (255,   0, 255)
		0x00, 0xff, 0xff,   //  6: 浅亮蓝 (  0, 255, 255)
		0xff, 0xff, 0xff,   //  7: 白     (255, 255, 255)
		0xc6, 0xc6, 0xc6,   //  8: 亮灰   (198, 198, 198)
		0x84, 0x00, 0x00,   //  9: 暗红   (132,   0,   0)
		0x00, 0x84, 0x00,   // 10: 暗绿   (  0, 132,   0)
		0x84, 0x84, 0x00,   // 11: 暗黄   (132, 132,   0)
		0x00, 0x00, 0x84,   // 12: 暗蓝   (  0,   0, 132)
		0x84, 0x00, 0x84,   // 13: 暗紫   (132,   0, 132)
		0x00, 0x84, 0x84,   // 14: 暗浅蓝 (  0, 132, 132)
		0x84, 0x84, 0x84    // 15: 暗灰   (132, 132, 132)
	};
	set_palette(0, 15, table_rgb);  // 将 0~15 号全部写入 VGA
	return;
}

// ============================================================
// set_palette — 将 RGB 数组写入 VGA 硬件调色板
// 硬件协议：
//   ① 向 0x03C8 写入起始编号 → VGA 内部计数器 = start
//   ② 向 0x03C9 连续写入 R、G、B（每色 6 位，共 18 位）
//   ③ VGA 接收完 3 字节后，内部计数器自动 +1，指向下一编号
// 为什么要关中断？写入过程中若被中断打断，中断代码也可能操作调色板，导致颜色错乱。
// 为什么要 /4？VGA 每个颜色分量只有 6 位(0~63)，而 table_rgb 是 8 位(0~255)，/4=右移 2 位。
// ============================================================
void set_palette(int start, int end, unsigned char *rgb)
{
	int i, eflags;
	eflags = io_load_eflags();   // 保存中断许可标志（可能之前已经开中断）
	io_cli();                    // 关闭中断，保证调色板写入是原子操作
	io_out8(0x03c8, start);      // 告诉 VGA："从编号 start 开始改写"

	for (i = start; i <= end; i++) {
		// i 仅作循环计数；VGA 自己会递增内部编号，代码无需干预
		io_out8(0x03c9, rgb[0] / 4);   // 写入当前编号的 R 分量
		io_out8(0x03c9, rgb[1] / 4);   // 写入当前编号的 G 分量
		io_out8(0x03c9, rgb[2] / 4);   // 写入当前编号的 B 分量（3 字节写完→编号+1）
		rgb += 3;                      // 指针跳到下一个颜色的 RGB 数据
	}
	io_store_eflags(eflags);     // 恢复中断许可标志
	return;
}

// ============================================================
// boxfill8 — 在显存中填充一个纯色矩形
// 参数: vram=显存基址, xsize=屏幕每行像素数(用于寻址),
//       c=颜色编号, (x0,y0)/(x1,y1)=左上角/右下角坐标(均为包含)
// 偏移公式: vram[y * xsize + x]
//   显存是一维数组，屏幕是二维的。y*xsize 跳过前 y 整行，
//   +x 在该行内右移 x 列。等价于"第 y 页第 x 个字"。
// ============================================================
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1)
{
	int x, y;
	for (y = y0; y <= y1; y++) {       // 逐行
		for (x = x0; x <= x1; x++) {   // 逐列
			vram[y * xsize + x] = c;   // 写入 1 字节颜色号
		}
	}
	return;
}


// ============================================================
// init_sceen — 绘制桌面布局
// 布局结构（y=屏幕高度=200）：
//   y=0    ┌──────────────────────────┐
//          │  浅蓝背景 (COL8_008484)    │  ← 0 .. y-29
//   y=172  ├──────────────────────────┤  ← 分割线 1 (COL8_C6C6C6)
//   y=173  │  白线 (COL8_FFFFFF)       │  ← 分割线 2（高光效果）
//   y=174  ├──────────────────────────┤  ← 分割线 3 (COL8_C6C6C6)
//          │         任务栏            │
//          │  [按钮1 消る]  [按钮2]    │  ← y-24 .. y-1
//   y=199  └──────────────────────────┘
//
// 按钮坐标说明：
//   按钮1（左）: 写死 x=3~59，因为左侧位置不随屏幕宽度变化
//   按钮2（右）: 用 xsize-47 .. xsize-4，自动贴右边缘（适应不同分辨率）
// 按钮立体感：上+左白边 → 高光；下+右灰边 → 阴影；内部黑底
// ============================================================
void init_sceen(char *vram, int xsize, int ysize)
{
	struct BOOTINFO *bootinfo;
	// 从地址 0x0ff0 读取 asmhead.nas 写入的启动信息
	bootinfo = (struct BOOTINFO *)0x0ff0;
	xsize = bootinfo->scrnx;   // 屏幕宽 (320)
	ysize = bootinfo->scrny;   // 屏幕高 (200)
	vram  = bootinfo->vram;    // 显存地址 (0xA0000)

	// ---- 桌面背景 ----
	boxfill8(vram, xsize, COL8_008484, 0, 0, xsize - 1, ysize - 29);          // 浅蓝背景

	// ---- 三条分割线（把桌面和任务栏隔开） ----
	boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28); // 线1: 灰色
	boxfill8(vram, xsize, COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27); // 线2: 白色（高光）
	boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);  // 线3: 灰色 → 任务栏底

	// ---- 按钮1（左侧 "消る"） ----
	boxfill8(vram, xsize, COL8_FFFFFF,  3,        ysize - 24, 59,        ysize - 24); // 上边: 白色高光
	boxfill8(vram, xsize, COL8_FFFFFF,  2,        ysize - 24,  2,        ysize -  4); // 左边: 白色高光
	boxfill8(vram, xsize, COL8_848484,  3,        ysize -  4, 59,        ysize -  4); // 下边: 灰色阴影
	boxfill8(vram, xsize, COL8_848484, 59,        ysize - 23, 59,        ysize -  5); // 右边: 灰色阴影
	boxfill8(vram, xsize, COL8_000000,  2,        ysize -  3, 59,        ysize -  3); // 内部: 黑色填充
	boxfill8(vram, xsize, COL8_000000, 60,        ysize - 24, 60,        ysize -  3); // 右缝: 黑色填充

	// ---- 按钮2（右侧） ----
	// 坐标用 xsize-47 而非硬编码 273，这样换分辨率时自动贴右边缘
	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24); // 上边
	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4); // 左边
	boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3); // 下边: 白色高光
	boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3); // 右边: 白色高光
	return;
}


// ============================================================
// putfont8 — 在屏幕上绘制一个 8×16 的等宽字符
// 参数: vram=显存, xsize=行宽, (x,y)=字符左上角坐标, c=颜色, font=16 字节点阵
//
// 工作原理：遍历 16 行点阵数据，每行一个字节 font[i]。
//   p = vram + (y + i) * xsize + x   → 第 i 行的首像素地址
//   p[0]~p[7]  → 该行从左到右 8 列（p[0]=最左列, p[7]=最右列）
//   若 font[i] 的某一位为 1，就把对应列涂成颜色 c
//
// 例如 font_A[1]=0x18=00011000?：
//   第4列(p[3])和第5列(p[4])亮，其余暗 → 形成 'A' 的左右竖笔
// ============================================================
void putfont8(char *vram, int xsize, int x, int y, char c, char *font)
{
	int i;
	char d, *p;                      // d=当前行点阵字节, p=当前行首像素地址
	for (i = 0; i < 16; i++) {       // 16 行，每行 1 字节控制 8 列
		p = vram + (y + i) * xsize + x;  // 预计算第 i 行首地址，(y+i) 在屏幕上垂直下移 i 行
		d = font[i];                     // 取第 i 行的点阵数据
		// 逐位检查：bit7→p[0](最左列) ... bit0→p[7](最右列)
		if ((d & 0x80) != 0) { p[0] = c; }   // bit7=1 → 第1列亮
		if ((d & 0x40) != 0) { p[1] = c; }   // bit6=1 → 第2列亮
		if ((d & 0x20) != 0) { p[2] = c; }   // bit5=1 → 第3列亮
		if ((d & 0x10) != 0) { p[3] = c; }   // bit4=1 → 第4列亮
		if ((d & 0x08) != 0) { p[4] = c; }   // bit3=1 → 第5列亮
		if ((d & 0x04) != 0) { p[5] = c; }   // bit2=1 → 第6列亮
		if ((d & 0x02) != 0) { p[6] = c; }   // bit1=1 → 第7列亮
		if ((d & 0x01) != 0) { p[7] = c; }   // bit0=1 → 第8列亮
	}
	return;
}