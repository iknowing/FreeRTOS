#include "shell.h"

u8 uc_shellCmdName[SHELLCMDSTRMAX];
s16 uc_shellCmdNum;//指令的列表号
u8 uc_shellCmdParamName[10][10];//参数名
u8 uc_shellCmdParamType[10];//参数类型
u8 uc_shellCmdParamValue[10][20];//参数值
s16 uc_shellCmdParamNum;//参数个数
extern u8 uc_shellSysCmdName[SHELLCMDSTRMAX];
extern s16 uc_shellSysCmdNum;//指令的列表号
extern u8 uc_shellSysCmdParamName[10][10];//参数名
extern u8 uc_shellSysCmdParamType[10];//参数类型
extern u8 uc_shellSysCmdParamValue[10][20];//参数值
extern s16 uc_shellSysCmdParamNum;
//u8 uc_shellCmdParamValueLength;
/*****************************须声明**********************************/
typedef struct __st_dbuf{
	u8 busy;
	u8 buf[SHELLCMDSTRMAX];//指令字符串
}st_dbuf;
st_dbuf shellCmdStr0[3]={{.busy=0},{.busy=0}};
u8 uc_shellCmdStr_cnt=0;
st_dbuf *shellCmdStr = &shellCmdStr0[0];//指令字符串
u8 uc_shellCmdStrCnt;//字符串计数器
extern _shellCmdTable shell_sys_table[];//系统指令表
extern s16 shell_sys_table_length;//系统指令列表长度
extern _shellCmdTable shell_table[];//指令列表
extern s16 shell_table_length;
/***************************shell_str.c*************************************/
extern s16 shell_str_funcandparam(u8 *cmdstr);
extern s16 shell_str_compare(u8 *str1, u8 *str2);
extern void shell_str_gettablefuncname(s16 num,u8 *fname);
extern void shell_str_getsystablefuncname(s16 num,u8 *fname);
extern s16 shell_str_getparamvalue(s16 num,s32 *des);
/*********************内部函数声明*********************************/
void shell_cmd_exe(u8 *cmdstr);

/********************************************************
 *函数名：shell_rec
 *参  数：无
 *返回值：无
 *说  明：用于接收shell指令字符串；
 *            指令以回车结尾；
 *            最大长度为100个字节。
 *******************************************************/
void shell_rec(u8 dat)
{
	static u8 temp[SHELLCMDSTRMAX];
	static u8 temp_cnt = 0;
	u8 i;
	temp[temp_cnt++]=dat;
	if(temp_cnt<=2)//无接收跳出
	{
		return;
	}
	if(temp[temp_cnt-1]=='\n')
	{
		if(temp[temp_cnt-2]=='\r')
		{
			temp[temp_cnt-1]=0;
			temp[temp_cnt-2]=0;
			for(i=0;i<temp_cnt;i++)
			{
				shellCmdStr->buf[i] = temp[i];
				temp[i]=0;
			}
			shellCmdStr->busy = 0x80 | temp_cnt;
			temp_cnt = 0;
			shellCmdStr->buf[i] = temp[i];
			uc_shellCmdStr_cnt++;
			if(uc_shellCmdStr_cnt == 3) {
				uc_shellCmdStr_cnt = 0;
			}
			shellCmdStr = &shellCmdStr0[uc_shellCmdStr_cnt];
		}
	}
	else if(temp_cnt>=SHELLCMDSTRMAX)
	{
		for(;temp_cnt!=0;temp_cnt--)
		{
			temp[temp_cnt]=0;
		}
		temp[temp_cnt]=0;
	}
}
void shell_loop(void)
{
	st_dbuf *tmp;
	for(u8 i=0; i<3; i++) {
		tmp = &shellCmdStr0[i];
		if(tmp->busy)
		{
			shell_cmd_exe(tmp->buf);
			for(u8 j=tmp->busy&0x7F;j>0;j--) {
				tmp->buf[j]=0;
			}
			tmp->busy = 0;
		}
	}
}
/********************************************************
 *函数名：shell_find_sys_cmd_code
 *参  数：shellcmdname,需要寻找的函数名
 *返回值：-1,无此函数,函数编号
 *描  述：
 ********************************************************/
s16 shell_find_sys_cmd_code(u8 *shellcmdname)
{
	u8 fname[SHELLCMDSTRMAX]={0};
	s16 i;
	for(i=0;i<shell_sys_table_length;i++)
	{
		shell_str_getsystablefuncname(i,fname);
		if(shell_str_compare(shellcmdname,fname)==0)
		{
			break;
		}
	}
	if(i>=shell_sys_table_length)
	{
		i=-1;
	}
	uc_shellSysCmdNum=i;
	return (i);
}
/********************************************************
 *函数名：shell_find_cmd_code
 *参数  ：shellcmdname,需要寻找的函数名
 *返回值：-1,无此函数,函数编号
 *描述  ：
 ********************************************************/
s16 shell_find_cmd_code(u8 *shellcmdname)
{
	u8 fname[SHELLCMDSTRMAX]={0};
	s16 i;
	for(i=0;i<shell_table_length;i++)
	{
		shell_str_gettablefuncname(i,fname);
		if(shell_str_compare(shellcmdname,fname)==0)
		{
			break;
		}
	}
	if(i>=shell_table_length)
	{
		return (-1);
	}
	return (i);
}
/********************************************************
 *每个系统指令按固定编号执行
 ********************************************************/
s16 shell_sys_cmd_exe(void)
{
	s16 err;
	err=shell_find_sys_cmd_code(uc_shellCmdName);
	switch(err)
	{
	/*case -1:
		return (-1);
		break;*/
	case 0:
		(void)(*(u32(*)())shell_sys_table[0].func)();//?
		break;
	case 1:
		(void)(*(u32(*)())shell_sys_table[1].func)();//list
		break;
	case 2:
		(void)(*(u32(*)(u8 *))shell_sys_table[2].func)(uc_shellCmdParamName[0]);//help funcname
		break;
	default:
		break;
	}
	return (err);
}

void shell_cmd_exe(u8 *cmdstr)
{
	s16 err;
	s32 des[10]={0};
	u32 ret=0;
	err = shell_str_funcandparam(cmdstr);
	uc_shellCmdParamNum=err;
	if(shell_sys_cmd_exe()==0)
	{
		return ;//系统指令，执行结束
	}
	if((uc_shellCmdNum=shell_find_cmd_code(uc_shellCmdName))==-1)
	{
		return ;
	}
	if(shell_str_getparamvalue(uc_shellCmdParamNum,des)==-1)
	{
#ifdef SHELL_DEBUG
		printp("参数解析出错");
#endif
		return ;
	}
	switch(err)
	{
	case -1://错误指令
		break;
	case 0:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)();
		break;
	case 1:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0]);
		break;
	case 2:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1]);
		break;
	case 3:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2]);
		break;
	case 4:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3]);
		break;
	case 5:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3],des[4]);
		break;
	case 6:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3],des[4],des[5]);
		break;
	case 7:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3],des[4],des[5],des[6]);
		break;
	case 8:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3],des[4],des[5],des[6],des[7]);
		break;
	case 9:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3],des[4],des[5],des[6],des[7],des[8]);
		break;
	case 10:
		ret=(*(u32(*)())shell_table[uc_shellCmdNum].func)(des[0],des[1],des[2],des[3],des[4],des[5],des[6],des[7],des[8],des[9]);
		break;
	default:
		break;
	}
	
	if((shell_table[uc_shellCmdNum].cmdhasreturn != 0) && (ret != 0))
		printsh("\r\nError0x%08x:\r\n",ret);
}


