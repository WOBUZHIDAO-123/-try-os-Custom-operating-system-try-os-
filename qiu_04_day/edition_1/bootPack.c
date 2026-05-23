
void io_hlt(void);	//执行HLT指令（停机指令）

void write_mem8(int addr, int data);	//向内存地址addr写入数据data

void HariMain(void) {
	int i;

	for ( i = 0xa00; i < 0xaffff; i++)
	{
		write_mem8(i, i & 0x0f);				//向内存地址i写入数据,数据为i和0x0f的按位与（注意是按位与）结果
		//例如，当i为0100时，i & 0x0f的结果为0000，因为0100的二进制表示为0000(高4位) 0100（低4位），
		//而0x0f的二进制表示为0000 1111，按位与的结果为0000 0100，即4，而低4位最多只能表示到15，
		//所以显存的值就会随着i的增加而显在0到15的颜色中间循环，形成一个彩虹色的效果。
	}
	
	for ( ;;)
	{
		io_hlt();						//执行HLT指令，停机等待下一次中断
	}
	
}