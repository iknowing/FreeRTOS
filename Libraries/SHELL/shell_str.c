#include "shell.h"

//#define SHELL_DEBUG

extern u8 uc_shellCmdName[SHELLCMDSTRMAX];
extern s16 uc_shellCmdNum;//ָ����б��
extern u8 uc_shellCmdParamName[10][10];//������
extern u8 uc_shellCmdParamType[10];//��������
extern u8 uc_shellCmdParamValue[10][20];//����ֵ
extern s16 uc_shellCmdParamNum;
extern _shellCmdTable shell_table[];//ָ���б�
extern s16 shell_table_length;
u8 uc_shellSysCmdName[SHELLCMDSTRMAX];
s16 uc_shellSysCmdNum;//ָ����б��
u8 uc_shellSysCmdParamName[10][10];//������
u8 uc_shellSysCmdParamType[10];//��������
u8 uc_shellSysCmdParamValue[10][20];//����ֵ
s16 uc_shellSysCmdParamNum;
enum SHELLCMDPARAMTYPE
{
	VOID,
	UCHAR,
	UCHARP,
	USHORT,
	SSHORT,
	FLOAT,
	DOUBLE,
	ULONG,
	SLONG
};
/****************************ϵͳָ���****************************/
void shell_str_listsys(void);
void shell_str_list(void);
void shell_str_help(u8 *funcname);
/****************************ϵͳָ���б�****************************/
_shellCmdTable shell_sys_table[]=
{
	(void *)shell_str_listsys,"?","VOID",0,0
	,"?: ?\r\n���ڻ�ȡϵͳ������Ϣ.\r\n",
	(void *)shell_str_list,"list","VOID",0,0
	,"list: list\r\n���ڻ�ȡ�����û�������Ϣ.\r\n",
	(void *)shell_str_help,"help funcname","UCHARP",0,1
	,"help: help [funcname]\r\n���ڻ�ȡ�����İ�����Ϣ.\r\n",
};
/************************ϵͳָ���б���*****************************/
s16 shell_sys_table_length = sizeof(shell_sys_table)/sizeof(_shellCmdTable);
/******************************************************
 *��������shell_str_gettablefuncname
 *��  ����num,�����б����,fname,�õ��ĺ�����
 *����ֵ����
 *��  �������б��л��������
 ******************************************************/
void shell_str_gettablefuncname(s16 num,u8 *fname)
{
	const u8 *tablestr;
	tablestr=shell_table[num].cmdstr;

	for(;(*tablestr!=' ')&&(*tablestr!=0);tablestr++,fname++)
	{
		*fname=*tablestr;
	}
	*fname=0;
}
/******************************************************
 *��������shell_str_getsystablefuncname
 *��  ����num,�����б����,fname,�õ��ĺ�����
 *����ֵ����
 *��  �������б��л��ϵͳ������
 ******************************************************/
void shell_str_getsystablefuncname(s16 num,u8 *fname)
{
	const u8 *tablestr;
	tablestr=shell_sys_table[num].cmdstr;

	for(;(*tablestr!=' ')&&(*tablestr!=0);tablestr++,fname++)
	{
		*fname=*tablestr;
	}
	*fname=0;
}
/******************************************************
 *��������shell_str_compare
 *��  ����str1,str2 �Ա��ַ���
 *����ֵ��0,��ͬ,-1��ͬ
 *��  ����
 ******************************************************/
s16 shell_str_compare(u8 *str1, u8 *str2)
{
	for(;*str1!=0;str1++,str2++)
	{
		if(*str1!=*str2)
		{
			return (-1);
		}
	}
	if(*str1!=*str2)
	{
		return (-1);
	}
	return 0;
}
/******************************************************
 *��������shell_str_copy
 *��  ����src,Դ�ַ���
 *        des,Ŀ���ַ���
 *        length,��������
 *����ֵ��0,�ɹ�,1ʧ��
 *��  ����
 ******************************************************/
s16 shell_str_copy(u8 *src, u8 *des,u16 length)
{
	u16 len;
	
	for(len=0;*(src+len)!=0;len++);
	if(length>len)
	{
		return 1;
	}
	for(;*src;src++,des++)
	{
		*des=*src;
	}
	return 0;
}
/*******************************************************
 *��������shell_str_funcandparam
 *��  ����cmdstr��ָ���ַ���
 *����ֵ��-1��Ч����������
 *��  ������ȡ����������������������
 *        �������10���������20���ַ�
 ******************************************************/
s16 shell_str_funcandparam(u8 *cmdstr)
{
	u8 i,shellcmdparamlength=0;//�����ַ�����
	char paramstate=0;
	u8 shellcmdnamelength=0;//����������
	uc_shellCmdParamNum=0;//��������

	for(i=0;(*cmdstr!=0)&&(i<SHELLCMDSTRMAX-2);i++,cmdstr++)//get func name
	{
		if(*cmdstr==' ') {
			if(shellcmdnamelength!=0) {
				uc_shellCmdName[shellcmdnamelength] = 0;
				break;//��������ȡ���
			}
			continue;//������ͷ�ո�
		}
		else if(((*cmdstr>='0')&&(*cmdstr<='9'))||((*cmdstr>='a')&&(*cmdstr<='z'))
			||((*cmdstr>='A')&&(*cmdstr<='Z'))||(*cmdstr=='_'))
		{
			uc_shellCmdName[shellcmdnamelength++] = *cmdstr;//��ȡ������
		} else 
		{
			if((*cmdstr=='?')&&(shellcmdnamelength==0)) {
				uc_shellCmdName[0]='?';
				uc_shellCmdName[1]=0;
				return 0;
			} else
			{
				return -1;//����������
			}
		}
	}
	if(*cmdstr == 0) {
		uc_shellCmdName[shellcmdnamelength] = 0;
		return 0;
	}
	for(paramstate=0;(*cmdstr!=0)&&(i<SHELLCMDSTRMAX-2);i++,cmdstr++)//get func param
	{
		if(*cmdstr==' ')
		{
			if((paramstate==2)&&(shellcmdparamlength))//һ�������������
			{
				uc_shellCmdParamValue[uc_shellCmdParamNum-1][shellcmdparamlength]=0;
				shellcmdparamlength=0;
				paramstate=0;
			}
			else if(paramstate&0x80)
			{
				uc_shellCmdParamValue[uc_shellCmdParamNum-1][shellcmdparamlength++]=*cmdstr;
			}
			continue;
		}
		else if(*cmdstr=='=')
		{
			if((paramstate&0x03)==1)
			{
				paramstate+=1;
				uc_shellCmdParamName[uc_shellCmdParamNum-1][shellcmdparamlength]=0;
				shellcmdparamlength=0;
				continue;
			}
			else
			{
				return -1;//������ʽ����
			}
		}
		else if(*cmdstr=='"')
		{
			if(paramstate&0x80)
			{
				paramstate &= ~0x80;
			}
			else
			{
				paramstate |= 0x80;
			}
			continue;
		}
		else if(paramstate==0)
		{
			paramstate=1;
			uc_shellCmdParamNum++;
		}
		else
		{
			
		}
		if((paramstate&0x03)==1)//���ղ�����
		{
			if(((*cmdstr>='0')&&(*cmdstr<='9'))||((*cmdstr>='a')&&(*cmdstr<='z'))
				||((*cmdstr>='A')&&(*cmdstr<='Z'))||(*cmdstr=='_'))
			{
				uc_shellCmdParamName[uc_shellCmdParamNum-1][shellcmdparamlength++]=*cmdstr;
			}
			else
			{
				return -1;//����������
			}
		}
		else if((paramstate&0x03)==2)//���ղ���ֵ
		{
			uc_shellCmdParamValue[uc_shellCmdParamNum-1][shellcmdparamlength++]=*cmdstr;
		}
	}
	
	if(((paramstate&0x03)==2)&&(shellcmdparamlength))//һ�������������
	{
		uc_shellCmdParamValue[uc_shellCmdParamNum-1][shellcmdparamlength]=0;
		paramstate=0;
	}else if((paramstate&0x03)==1)
	{
		uc_shellCmdParamName[uc_shellCmdParamNum-1][shellcmdparamlength]=0;
	}
	if(paramstate)
	{
		return -1;//�������մ���
	}
	return uc_shellCmdParamNum;
}
/********************************************************
 *��������shell_str_str2hex
 *��  ����num,��������,des,��Ŵ�
 *����ֵ��0,�ɹ�,-1,����
 *��  ������ȡ������ʵ��ֵ���ַ����򲻱䣬��ֵ��ʮ�����Ʊ�ʾ
 ********************************************************/
s32 shell_str_str2hex(u8 *str,s32 *des)
{
	u8 state=10;
	s16 i=0,cnt,b=1;
	s32 value=0;
	if(str[0]=='-')
	{
		b=-1;
		str++;
	}
	if((str[0]=='0')&&(str[1]!=0))//�����ж�
	{
		if((str[1]<='d')||(str[1]<='D'))
		{
			state=11;
		}
		else if((str[1]=='x')||(str[1]=='x'))
		{
			state=16;
		}
		else if((str[1]=='o')||(str[1]=='O'))
		{
			state=8;
		}
		else if((str[1]=='b')||(str[1]=='B'))
		{
			state=2;
		}
		else if((str[1]<='0')||(str[1]>='9'))
		{
			return (-1);
		}
	}

	if(state==16)
	{
		for(cnt=2;str[cnt]!=0;cnt++)
		{
			if( ((str[cnt]>='0')&&(str[cnt]<='9')) || ((str[cnt]>='a')&&(str[cnt]<='f')) || ((str[cnt]>='A')&&(str[cnt]<='F')) )//�淶���
			{
				continue;
			}
			else
			{
				return (-1);
			}
		}
		for(value=0,i=0;i<cnt;i++)
		{
			value *=16;
			if((str[i]>='a')&&(str[i]<='f'))
			{
				value+=str[i]-'a'+10;
			}
			else if((str[i]>='A')&&(str[i]<='F'))
			{
				value+=str[i]-'A'+10;
			}
			else
			{
				value+=str[i]-'0';
			}
		}
	}
	else if(state==8)
	{
		for(cnt=2;str[cnt]!=0;cnt++)
		{
			if((str[cnt]>='0')&&(str[cnt]<='7'))//�淶���
			{
				continue;
			}
			else
			{
				return (-1);
			}
		}
		for(value=0,i=2;i<cnt;i++)
		{
			value *=8;
			value+=str[i]-'0';
		}
	}
	else if(state==2)
	{
		for(cnt=2;str[cnt]!=0;cnt++)
		{
			if((str[cnt]>='0')&&(str[cnt]<='1'))//�淶���
			{
				continue;
			}
			else
			{
				return (-1);
			}
		}
		for(value=0,i=2;i<cnt;i++)
		{
			value *=2;
			value+=str[i]-'0';
		}
	}
	else
	{
		for(cnt=((state==10)?0:2);str[cnt]!=0;cnt++)//�淶���
		{
			if((str[cnt]>='0')&&(str[cnt]<='9'))
			{
				continue;
			}
			else
			{
				return (-1);
			}
		}
		if(state==10)
		{
			for(value=0,i=0;i<cnt;i++)
			{
				value *=10;
				value+=str[i]-'0';
			}
		}
		else
		{
			for(value=0,i=2;i<cnt;i++)
			{
				value *=10;
				value+=str[i]-'0';
			}
		}
	}
	*des = b*value;
	return (0);
}
/********************************************************
 *��������str_getparamtype
 *��  ����num,��Ӧ�Ĳ������
 *����ֵ��-1,��,��������
 *��  ����
 *********************************************************/
s16 shell_str_getparamtype(s16 num)
{
	s16 count;
	u8 *p;
	u8 temp[10];

	//��ȡ����������
	p = (u8 *)shell_table[uc_shellCmdNum].cmdparamtype;
	if(num==0)
	{
		for(count=0;(p[count]!=' ')&&(p[count]!=0);count++)
		{
			temp[count] = p[count];
		}
		temp[count] = 0;
	}
	else
	{
		for(count=0;(count!=num)&&(*p!=0);p++)
		{
			if(*p==' ')
			{
				count++;
			}
		}
		//p++;
		for(count=0;(p[count]!=' ')&&(p[count]!=0);count++)
		{
			temp[count] = p[count];
		}
		temp[count] = 0;
	}
	if(shell_str_compare(temp,(u8 *)"VOID")==0){
		uc_shellCmdParamType[num] = (VOID);
	}else if(shell_str_compare(temp,(u8 *)"UCHAR")==0){
		uc_shellCmdParamType[num] =  (UCHAR);
	}else if(shell_str_compare(temp,(u8 *)"UCHARP")==0){
		uc_shellCmdParamType[num] =  (UCHARP);
	}else if(shell_str_compare(temp,(u8 *)"USHORT")==0){
		uc_shellCmdParamType[num] =  (USHORT);
	}else if(shell_str_compare(temp,(u8 *)"SSHORT")==0){
		uc_shellCmdParamType[num] =  (SSHORT);
	}else if(shell_str_compare(temp,(u8 *)"ULONG")==0){
		uc_shellCmdParamType[num] =  (ULONG);
	}else if(shell_str_compare(temp,(u8 *)"SLONG")==0){
		uc_shellCmdParamType[num] =  (SLONG);
	}else if(shell_str_compare(temp,(u8 *)"FLOAT")==0){
		uc_shellCmdParamType[num] =  (FLOAT);
	}else if(shell_str_compare(temp,(u8 *)"DOUBLE")==0){
		uc_shellCmdParamType[num] =  (DOUBLE);
	}else{
		return (-1);
	}
	return (uc_shellCmdParamType[num]);
}
/********************************************************
 *��������shell_str_getparamvalue
 *��  ����num,��������,des,���ݴ�Ŵ�
 *����ֵ��0,�ɹ�,-1,����
 *��  ������ȡ������ʵ��ֵ���ַ����򲻱䣬��ֵ��ʮ�����Ʊ�ʾ
 ********************************************************/
s16 shell_str_getparamvalue(s16 num,s32 *des)
{
	for(num-=1;num>=0;num--)
	{
		if(shell_str_getparamtype(num)==UCHARP)//�ַ���
		{
			des[num]=(s32)(uc_shellCmdParamValue[num]);
		}
		else if(shell_str_str2hex(uc_shellCmdParamValue[num],des+num)==-1)//С��32λ��ֵ
		{
			return (-1);
		}
	}
	return (0);
}
//==========================ϵͳ�����=============================//
/******************************************************
 *��������shell_str_help
 *��  ����
 *����ֵ��
 *��  ����ϵͳ����
 ******************************************************/
void shell_str_help(u8 *funcname)
{
	u8 fname[SHELLCMDSTRMAX]={0};
	s16 i;
	if(*funcname==0)
	{
		printsh("��ο�������Ϣ�÷���\r\n\r\n%s",shell_sys_table[2].cmdhelp);
		return;
	}
	for(i=0;i<shell_table_length;i++)
	{
		shell_str_gettablefuncname(i,fname);
		if(shell_str_compare(funcname,fname)==0)
		{
			printsh("%s",shell_table[i].cmdhelp);
			break;
		}
	}
	if(i==shell_table_length)
	{
		printsh("δ�ҵ�����������Ϣ��");
	}
}
/******************************************************
 *��������shell_str_listsys
 *��  ����
 *����ֵ��
 *��  ����ϵͳ����
 ******************************************************/
void shell_str_listsys(void)
{
	s16 i;

	printsh("\r\nע��:�ַ�����������\"������.\r\n");
	for(i=0;i<shell_sys_table_length;i++)
	{
		printsh("#%d:%s",i+1,shell_sys_table[i].cmdhelp);
	}
}
/******************************************************
 *��������shell_str_list
 *��  ����
 *����ֵ��
 *��  �����û�����
 ******************************************************/
void shell_str_list(void)
{
	s16 i;

	printsh("\r\nע��:�ַ�����������\"������.\r\n");
	for(i=0;i<shell_table_length;i++)
	{
		printsh("#%d:%s",i+1,shell_table[i].cmdhelp);
	}
}
