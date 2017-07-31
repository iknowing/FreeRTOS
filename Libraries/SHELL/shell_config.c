#include "shell.h"
#include "user_task.h"

/*******************************USER列表******************************/
_shellCmdTable shell_table[]=
{
	/********************************************
	*cmdstr;functionname paramname1 paramname2 ...，空格隔开
	*cmdparamtype;paramtype1 paramtype2 ...，空格隔开
	*cmdhelp;描述命令使用方法及功能等。
	********************************************/
	/* type
	VOID,
	UCHAR,
	UCHARP,
	USHORT,
	SSHORT,
	FLOAT,
	DOUBLE,
	ULONG,
	SLONG
	*/
    
	(void *)reset,"reset","void",0,0
	,"reset\r\n",
/*
	//
	(void *)create_file,"createfile filedir","UCHARP",0,1
	,"createfile: createfile filedir=[value]\r\n-filedir:文件路劲.\r\n",
*/
};
/********************指令列表长度*************************/
s16 shell_table_length = sizeof(shell_table)/sizeof(_shellCmdTable);

