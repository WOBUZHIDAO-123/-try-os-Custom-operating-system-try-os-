
void io_hlt(void);	//函数声明用“;”而不是“｛｝”代表此函数在其他文件中

void HariMain(void) {
fin:	//标签
	goto fin;	//goto会被编译为JMP
}