#include "shell.h"
#include "user_task.h"

/*******************************USER�б�******************************/
_shellCmdTable shell_table[]=
{
	/********************************************
	*cmdstr;functionname paramname1 paramname2 ...���ո����
	*cmdparamtype;paramtype1 paramtype2 ...���ո����
	*cmdhelp;��������ʹ�÷��������ܵȡ�
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
	,"createfile: createfile filedir=[value]\r\n-filedir:�ļ�·��.\r\n",
*/
};
/********************ָ���б���*************************/
s16 shell_table_length = sizeof(shell_table)/sizeof(_shellCmdTable);

