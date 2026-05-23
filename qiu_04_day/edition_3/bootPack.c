
void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);

#define COL8_000000		0	// 黑
#define COL8_FF0000		1	// 亮红
#define COL8_00FF00		2	// 亮绿
#define COL8_FFFF00		3	// 亮黄
#define COL8_0000FF		4	// 亮蓝
#define COL8_FF00FF		5	// 亮紫
#define COL8_00FFFF		6	// 浅亮蓝
#define COL8_FFFFFF		7	// 白
#define COL8_C6C6C6		8	// 亮灰
#define COL8_840000		9	// 暗红
#define COL8_008400		10	// 暗绿
#define COL8_848400		11	// 暗黄
#define COL8_000084		12	// 暗蓝
#define COL8_840084		13	// 暗紫
#define COL8_008484		14	// 暗浅蓝
#define COL8_848484		15	// 暗灰

void HariMain(void)
{
	// 生成三个矩形，分别是红、绿、蓝
	// char *p;
	// init_palette();
	// p = (char *) 0xa0000;
	// boxfill8(p, 320, COL8_FF0000,  20,  20, 120, 120);
	// boxfill8(p, 320, COL8_00FF00,  70,  50, 170, 150);
	// boxfill8(p, 320, COL8_0000FF, 120,  80, 220, 180);

	char*vram;
	int xsize,ysize;
	init_palette();
	vram=(char*)0xa0000;
	xsize=320;
	ysize=200;
	boxfill8(vram, xsize, COL8_008484,  0,         0,          xsize -  1, ysize - 29); //背景
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28); //分割线1
	boxfill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27); //分割线2
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1); //分割线3

	boxfill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24); //按钮1的上边框
	boxfill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4); //按钮1的左边框
	boxfill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4); //按钮1的下边
	boxfill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5); //按钮1的右边框
	boxfill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3); //按钮1的内部
	boxfill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3); //按钮1的右边框

	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24); //按钮2的上边框
	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4); //按钮2的左边框
	boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3); //按钮2的下边框
	boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3); //按钮2的右边框

	for (;;) {
		io_hlt();
	}
}

//色号设定
void init_palette(void) {
	static unsigned char table_rgb[16 * 3] = {	//table_rgb[16*3]类似于table_rgb[16][3]
		0x00, 0x00, 0x00,	// 0:黑
		0xff, 0x00, 0x00,	// 1:亮红
		0x00, 0xff, 0x00,	// 2:亮绿
		0xff, 0xff, 0x00,	// 3:亮黄
		0x00, 0x00, 0xff,	// 4:亮蓝
		0xff, 0x00, 0xff,	// 5:亮紫
		0x00, 0xff, 0xff,	// 6:浅亮蓝
		0xff, 0xff, 0xff,	// 7:白
		0xc6, 0xc6, 0xc6,	// 8:亮灰
		0x84, 0x00, 0x00,	// 9:暗红
		0x00, 0x84, 0x00,	//10:暗绿
		0x84, 0x84, 0x00,	//11:暗黄
		0x00, 0x00, 0x84,	//12:暗蓝
		0x84, 0x00, 0x84,	//13:暗紫
		0x00, 0x84, 0x84,	//14:暗浅蓝
		0x84, 0x84, 0x84	//15:暗灰
	};
	set_palette(0 ,15 ,table_rgb);
	return;
	//c中的static char只能用于数据，类似于汇编中的DB指令，定义一个16*3字节的数组，存储16种颜色的RGB值
}

void set_palette(int start, int end, unsigned char *rgb) {	//这里用无符号字符指针是因为RGB值是8位的，范围是0-255
	int i, eflags;
	eflags = io_load_eflags();	//保存中断许可标志
	io_cli();					//禁止中断
	io_out8(0x03c8, start);		//指定调色板数据写入的起始色号
	//上述函数都会在naskfunc.asm中定义
	for (i = start; i <= end; i++) {
		//依次写入RGB值，除以4是因为VGA的调色板每个颜色分量只有6位（0-63）而定义的RGB值是8位（0-255）
		//在除以4后相当于降了两位就变成了6位0-63的范围，适合VGA的调色板使用	
		io_out8(0x03c9, rgb[0] / 4);	
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;						//移动到下一个颜色的RGB值，因为每个颜色有3个分量（R、G、B），所以要加3
	}
	io_store_eflags(eflags);	//恢复中断许可标志
	return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x,y;
	for(y=y0;y<=y1;y++){
		for(x=x0;x<=x1;x++){
			vram[y*xsize+x]=c;
		}
	}
	return;
}