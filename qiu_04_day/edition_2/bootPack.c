
void io_hlt(void);	//执行HLT指令（停机指令）

void write_mem8(int addr, int data);	//向内存地址addr写入数据data

void HariMain(void) {
	int i;
	char *P;
	for ( i = 0xa00; i < 0xaffff; i++)
	{
		P=(char *)i;
		*P=i & 0x0f;	//将i的低4位写入内存地址P
	}
	
	for ( ;;)
	{
		io_hlt();						//执行HLT指令，停机等待下一次中断
	}
	
}