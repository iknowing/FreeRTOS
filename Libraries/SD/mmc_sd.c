#include "mmc_sd.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stdio.h"
#include "diskio.h"
#include "ff.h"
#include "string.h"

#if 1
#define SD_Printf   printf
#else
#define SD_Printf   Noprintf
#endif


#define SD_SPI_CS          GPIOB, GPIO_Pin_12
#define SD_SPI_CS_EN()     GPIO_ResetBits(SD_SPI_CS)
#define SD_SPI_CS_DIS()    GPIO_SetBits(SD_SPI_CS)

#define BLOCK_SIZE  512
uint8_t SD_Type;

/** 
  * @brief  Card Identification Data: CID Register   
  */
typedef struct
{
  uint8_t  ManufacturerID;       //生产标识ID
  uint16_t OEM_AppliID;          //OEM/应用ID
  uint32_t ProdName1;            //产品名称1
  uint8_t  ProdName2;            //产品名称2
  uint8_t  ProdRev;              //产品版本
  uint32_t ProdSN;               //产品序号
  uint8_t  Reserved1;            //保留 
  uint16_t ManufactDate;         //生产日期
  uint8_t  CID_CRC;              //CID CRC
  uint8_t  Reserved2;            //always 1
} SD_CID;

typedef struct
{
  uint8_t  CSDStruct;            // CSD结构
  uint8_t  SysSpecVersion;       // 系统规范版本
  uint8_t  Reserved1;            // 保留
  uint8_t  TAAC;                 // 读取时间
  uint8_t  NSAC;                 // 数据在CLK周期内读取时间
  uint8_t  MaxBusClkFrec;        // 最大总线速度
  uint16_t CardComdClasses;      // 卡命令集合
  uint8_t  RdBlockLen;           // 最大读取数据块长
  uint8_t  PartBlockRead;        // 允许读的部分块
  uint8_t  WrBlockMisalign;      // 非线写块
  uint8_t  RdBlockMisalign;      // 非线读块
  uint8_t  DSRImpl;              // DSR条件
  uint8_t  Reserved2;            // 保留
  uint32_t DeviceSize;           // 设备容量
  uint8_t  MaxRdCurrentVDDMin;   // 最小读取电流 @ VDD min
  uint8_t  MaxRdCurrentVDDMax;   // 最大读取电流 @ VDD max
  uint8_t  MaxWrCurrentVDDMin;   // 最小写入电流 @ VDD min
  uint8_t  MaxWrCurrentVDDMax;   // 最大写入电流 @ VDD max
  uint8_t  DeviceSizeMul;        // 设备容量乘积因子
  uint8_t  EraseGrSize;          // 擦除块大小
  uint8_t  EraseGrMul;           // 擦除扇区大小
  uint8_t  WrProtectGrSize;      // 写保护群大小
  uint8_t  WrProtectGrEnable;    // 写保护群使能
  uint8_t  ManDeflECC;           // Manufacturer default ECC
  uint8_t  WrSpeedFact;          // 写速度因子
  uint8_t  MaxWrBlockLen;        // 最大写数据块长度
  uint8_t  WriteBlockPaPartial;  // 允许写的部分
  uint8_t  Reserved3;            // 保留
  uint8_t  ContentProtectAppli;  // Content protection application
  uint8_t  FileFormatGrouop;     // 文件系统群
  uint8_t  CopyFlag;             // 拷贝标志
  uint8_t  PermWrProtect;        // 永久写保护
  uint8_t  TempWrProtect;        // 暂时写保护
  uint8_t  FileFormat;           // 文件系统
  uint8_t  ECC;                  // ECC code
  uint8_t  CSD_CRC;              // CSD CRC
  uint8_t  Reserved4;            // always 1
} SD_CSD;

void Noprintf(char* str,...){}

void SD_SPI_Configuration(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
        
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
     
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
        
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);
    
    return;   
}
#define SPI_SPEED_LOW   1
#define SPI_SPEED_HIGH  2
void SD_SPI_SetSpeed(u8 SpeedSet)
{
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    if(SpeedSet==SPI_SPEED_LOW)
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }
    else
    {
        SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    }
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
    return;
}
u8 SD_SPI_ReadWriteByte(u8 TxData)
{
    u8 RxData = 0;
   
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, TxData);
    
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    RxData = SPI_I2S_ReceiveData(SPI2);
    
    return (u8)RxData;
}

u8 SD_GetResponse(u8 Response) 
{  
    u16 Count=0xFFF;            
    while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;
    if (Count==0)
        return MSD_RESPONSE_FAILURE; 
    else 
        return MSD_RESPONSE_NO_ERROR;
} 

u8 SD_WaitDataReady(void) 
{      
    u8 r1=MSD_DATA_OTHER_ERROR; 
    u32 retry = 0;         
    do     
    {          
        r1=SD_SPI_ReadWriteByte(0xFF)&0X1F;      
        if(retry==0xfffe)
            return 1;    
        retry++;   
        switch (r1)   
        {            
            case MSD_DATA_OK:
                r1=MSD_DATA_OK;     
                break;      
            case MSD_DATA_CRC_ERROR:  
                return MSD_DATA_CRC_ERROR;      
            case MSD_DATA_WRITE_ERROR:
                return MSD_DATA_WRITE_ERROR;      
            default:        
                r1=MSD_DATA_OTHER_ERROR;     
                break;     
        }        
    }while(r1==MSD_DATA_OTHER_ERROR); 
    retry=0;  
    while(SD_SPI_ReadWriteByte(0XFF)==0)
    {   
        retry++;   
        //delay_us(10);   
        if(retry>=0XFFFFFFFE)
            return 0XFF;
    };           
    return 0;
}
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc) 
{      
    u8 r1;    
    u8 Retry=0;  
    
    SD_SPI_CS_DIS();     
    SD_SPI_ReadWriteByte(0xff);
    SD_SPI_ReadWriteByte(0xff);        
    SD_SPI_ReadWriteByte(0xff); 
    
    SD_SPI_CS_EN();     
    SD_SPI_ReadWriteByte(cmd | 0x40);   
    SD_SPI_ReadWriteByte(arg >> 24);     
    SD_SPI_ReadWriteByte(arg >> 16);    
    SD_SPI_ReadWriteByte(arg >> 8);     
    SD_SPI_ReadWriteByte(arg);     
    SD_SPI_ReadWriteByte(crc);          
    while((r1=SD_SPI_ReadWriteByte(0xFF))==0xFF)     
    {          
        Retry++;              
        if(Retry>200)
            break;      
    }           
    SD_SPI_CS_DIS(); 
    
    SD_SPI_ReadWriteByte(0xFF);     
    return r1; 
}
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg,u8 crc)
{
    unsigned char r1;
    unsigned int Retry = 0;
 
    SD_SPI_CS_DIS();
    SD_SPI_ReadWriteByte(0xff);
    
    
    SD_SPI_CS_EN();
    SD_SPI_ReadWriteByte(cmd | 0x40);   
    SD_SPI_ReadWriteByte(arg >> 24);     
    SD_SPI_ReadWriteByte(arg >> 16);    
    SD_SPI_ReadWriteByte(arg >> 8);     
    SD_SPI_ReadWriteByte(arg);     
    SD_SPI_ReadWriteByte(crc);  
 
    while((r1 = SD_SPI_ReadWriteByte(0xff))==0xff)
    {
 	   Retry++;
	   if(Retry>200)
           break;
     }
    
	 return r1;
}

u8 SD_Idle_Sta(void) 
{  
    u16 i;   
    u8 retry;             
    for(i=0;i<0xf00;i++);
    for(i=0;i<10;i++)
        SD_SPI_ReadWriteByte(0xFF);       
      
    retry = 0;     
    do      
    {                     
        i = SD_SendCommand(CMD0, 0, 0x95);         
        retry++;      
    }while((i!=0x01)&&(retry<200));   
    
    if(retry==200)
        return 1;
        
    return 0;          
} 

uint8_t SD_Init(void)
{
    uint8_t r1;
    uint16_t retry;
    uint8_t buff[6];
    
    SD_SPI_Configuration();
    SD_SPI_CS_DIS();
    if(SD_Idle_Sta())
        return 1;
        
    SD_SPI_CS_EN();  
    r1 = SD_SendCommand_NoDeassert(8,0x1AA,0x87);    
    if(r1 == 0x05)
    {
        SD_Type = SD_TYPE_V1;
        SD_SPI_CS_DIS();
        SD_SPI_ReadWriteByte(0xFF);
        
        retry = 0;
        do
        {
            r1 = SD_SendCommand(CMD55,0,0);
            if(r1 == 0xFF)
                return r1;
            r1 = SD_SendCommand(ACMD41,0,0);
            retry++;
        }while((r1 != 0x00) && (retry < 400 ));
        
        if(retry == 400)
        {
            retry = 0;
            do
            {
                r1 = SD_SendCommand(CMD1,0,0);
                retry++;
            }while((r1 != 0x00) && (retry < 400 ));
            if(retry == 400)
            {
                return 1;
            }
            SD_Type = SD_TYPE_MMC;
        }
        SD_SPI_SetSpeed(SPI_SPEED_HIGH);
        SD_SPI_ReadWriteByte(0xFF);
        r1 = SD_SendCommand(CMD59,0,0x95); 
        if(r1 != 0x00)
            return r1;
        r1 = SD_SendCommand(CMD16,512,0x95);
        if(r1 != 0x00)
            return r1;                       
    }
    else if(r1 == 0x01)    
    {
        buff[0] = SD_SPI_ReadWriteByte(0xFF);
        buff[1] = SD_SPI_ReadWriteByte(0xFF);
        buff[2] = SD_SPI_ReadWriteByte(0xFF);
        buff[3] = SD_SPI_ReadWriteByte(0xFF);
        
        SD_SPI_CS_DIS();
        SD_SPI_ReadWriteByte(0xFF);
        
        if(buff[2] == 0x01 && buff[3] == 0xAA)
        {
            retry = 0;
            do
            {
                r1 = SD_SendCommand(CMD55,0,0);
                if(r1 != 0x01)
                    return r1;
                r1 = SD_SendCommand(ACMD41,0x40000000,0);
                if(retry > 200 )
                    return r1;
                retry++;
            }while(r1 != 0x00);
            
            r1 = SD_SendCommand_NoDeassert(CMD58,0,0);
            if(r1 != 0x00)
            {
                SD_SPI_CS_DIS();
                return r1;
            }
            buff[0] = SD_SPI_ReadWriteByte(0xFF);
            buff[1] = SD_SPI_ReadWriteByte(0xFF);
            buff[2] = SD_SPI_ReadWriteByte(0xFF);
            buff[3] = SD_SPI_ReadWriteByte(0xFF);
            
            SD_SPI_CS_DIS();
            SD_SPI_ReadWriteByte(0xFF);
            
            if(buff[0]&0x40)
            {
                SD_Type = SD_TYPE_V2HC;
            }
            else
            {
                SD_Type = SD_TYPE_V2;
            }
            SD_SPI_SetSpeed(SPI_SPEED_HIGH);
        }
        
    }
    return r1;    
}




uint8_t SD_ReceiveData(uint8_t* data,uint16_t len,uint8_t release)
{
    SD_SPI_CS_EN();           
    if(SD_GetResponse(0xFE))
    {      
        SD_SPI_CS_DIS();   
        return 1;  
    }      
    while(len--)    
    {          
        *data=SD_SPI_ReadWriteByte(0xFF);         
        data++;     
    }      
    SD_SPI_ReadWriteByte(0xFF);     
    SD_SPI_ReadWriteByte(0xFF);      
    if(release == RELEASE)   
    {          
        SD_SPI_CS_DIS();         
        SD_SPI_ReadWriteByte(0xFF);     
    }                           
    return 0; 
}

u8 SD_GetCID(u8 *cid_data) 
{      
    u8 r1;              
    r1 = SD_SendCommand(CMD10,0,0xFF);      
    if(r1 != 0x00)
        return r1;       
    SD_ReceiveData(cid_data,16,RELEASE);  
    return 0; 
}

u8 SD_GetCSD(u8 *csd_data) 
{     
    u8 r1;       
    r1=SD_SendCommand(CMD9,0,0xFF);   
    if(r1)
        return r1;        
    SD_ReceiveData(csd_data, 16, RELEASE);    
    return 0; 
} 
u32 SD_GetCapacity(void) 
{      
    u8 csd[16];     
    u32 Capacity;     
    u8 r1;     
    u16 i;  
    u16 temp;               
    if(SD_GetCSD(csd)!=0) 
        return 0;             
    if((csd[0]&0xC0)==0x40)     
    {                
        Capacity = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
        Capacity*=512; 
    }
    else  
    {           
        i = csd[6]&0x03;      
        i<<=8;     
        i += csd[7];      
        i<<=2;     
        i += ((csd[8]&0xc0)>>6);         //C_SIZE_MULT     
        r1 = csd[9]&0x03;      
        r1<<=1;      
        r1 += ((csd[10]&0x80)>>7);        
        r1+=2;//BLOCKNR      
        temp = 1;     
        while(r1)
        {        
            temp*=2;       
            r1--;      
        }      
        Capacity = ((u32)(i+1))*((u32)temp);           // READ_BL_LEN      
        i = csd[5]&0x0f;         //BLOCK_LEN      
        temp = 1;      
        while(i)      
        {        
            temp*=2;      
            i--;      
        }          //The final result      
        Capacity *= (u32)temp;  

        Capacity = Capacity/1024;  //Kbytes
    }      
    return (u32)Capacity; 
} 

u8 SD_ReadSingleBlock(u32 sector, u8 *buffer)
{ 
    u8 r1;              
    SD_SPI_SetSpeed(SPI_SPEED_HIGH);            
    if(SD_Type!=SD_TYPE_V2HC)     
    {         
        sector = sector<<9;     
    }   
    r1 = SD_SendCommand(CMD17, sector, 0);       
    if(r1 != 0x00)
        return r1;                 
    r1 = SD_ReceiveData(buffer, 512, RELEASE);     
    if(r1 != 0)
        return r1;   
    else 
        return 0;  
} 

u8 MSD_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumByteToWrite) 
{  
    u32 i,NbrOfBlock = 0, Offset = 0;  
    u32 sector;  
    u8 r1;      
    NbrOfBlock = NumByteToWrite / BLOCK_SIZE;         
    SD_SPI_CS_EN();          
    while (NbrOfBlock--) 
    {   
        sector=WriteAddr+Offset;   
        if(SD_Type==SD_TYPE_V2HC)
            sector>>=9;            
        r1=SD_SendCommand_NoDeassert(CMD24,sector,0xff);     
        if(r1)   
        {    
            SD_SPI_CS_DIS();    
            return 1;           
        }      
        SD_SPI_ReadWriteByte(0xFE);     
        for(i=0;i<512;i++)
            SD_SPI_ReadWriteByte(*pBuffer++);            
        SD_SPI_ReadWriteByte(0xff);      
        SD_SPI_ReadWriteByte(0xff);     
        if(SD_WaitDataReady())  
        {    
            SD_SPI_CS_DIS();   
            return 2;       
        }   
        Offset += 512;      
    }              
    SD_SPI_CS_DIS();     
    SD_SPI_ReadWriteByte(0xff);       
    return 0;
}

u8 MSD_ReadBuffer(u8* pBuffer, u32 ReadAddr, u32 NumByteToRead) 
{  
    u32 NbrOfBlock=0,Offset=0;  
    u32 sector=0;  
    u8 r1=0;          
    NbrOfBlock=NumByteToRead/BLOCK_SIZE;        
    SD_SPI_CS_EN();  
    while (NbrOfBlock --)  
    {    
        sector=ReadAddr+Offset;   
        if(SD_Type==SD_TYPE_V2HC)
            sector>>=9;            
        r1=SD_SendCommand_NoDeassert(CMD17,sector,0xff);         
        if(r1) 
        {        
            SD_SPI_CS_DIS();   
            return r1;   
        }                
        r1=SD_ReceiveData(pBuffer,512,RELEASE);      
        if(r1) 
        {        
            SD_SPI_CS_DIS();   
            return r1;   
        }   
        pBuffer+=512;                 
        Offset+=512;         
    }         
    SD_SPI_CS_DIS();     
    SD_SPI_ReadWriteByte(0xff);      
    return 0; 
}

u8 SD_WriteSingleBlock(u32 sector, const u8 *data) 
{     
    u8 r1;    
    u16 i;    
    u16 retry;             
    //SPIx_SetSpeed(SPI_SPEED_HIGH);            
    if(SD_Type!=SD_TYPE_V2HC)   
    {     
        sector = sector<<9;    
    }         
    r1 = SD_SendCommand(CMD24, sector, 0x00);    
    if(r1 != 0x00)     
    {          
        return r1;      
    }            
    SD_SPI_CS_EN();          
    SD_SPI_ReadWriteByte(0xff);     
    SD_SPI_ReadWriteByte(0xff);     
    SD_SPI_ReadWriteByte(0xff);         
    SD_SPI_ReadWriteByte(0xFE);         
    for(i=0;i<512;i++)     
    {         
        SD_SPI_ReadWriteByte(*data++);     
    }         
    SD_SPI_ReadWriteByte(0xff);     
    SD_SPI_ReadWriteByte(0xff);               
    r1 = SD_SPI_ReadWriteByte(0xff); 
    if((r1&0x1F)!=0x05)     
    {          
        SD_SPI_CS_DIS();         
        return r1;     
    }             
    retry = 0;     
    while(!SD_SPI_ReadWriteByte(0xff))     
    {         
        retry++;          
        if(retry>0xfffe)               
        {             
            SD_SPI_CS_DIS();              
            return 1;                
        }     
    }              
    SD_SPI_CS_DIS();      
    SD_SPI_ReadWriteByte(0xff);       
    return 0; 
} 

u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count) 
{      
    u8 r1;            
    //SPIx_SetSpeed(SPI_SPEED_HIGH);        
    if(SD_Type!=SD_TYPE_V2HC)
        sector = sector<<9;       
    //SD_WaitDataReady();      
    r1 = SD_SendCommand(CMD18, sector, 0); 
    if(r1 != 0x00)
        return r1;       
    do 
    {          
        if(SD_ReceiveData(buffer, 512, NO_RELEASE) != 0x00)
            break;  
        buffer += 512;     
    } while(--count);           
    SD_SendCommand(CMD12, 0, 0);     
    SD_SPI_CS_DIS();      
    SD_SPI_ReadWriteByte(0xFF);         
    if(count != 0)
        return count;  
    else 
        return 0;  
}
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count) 
{      
    u8 r1;     
    u16 i;           
    //SPIx_SetSpeed(SPI_SPEED_HIGH);     
    if(SD_Type != SD_TYPE_V2HC)
        sector = sector<<9;      
    if(SD_Type != SD_TYPE_MMC) 
        r1 = SD_SendCommand(ACMD23, count, 0x00);         
    r1 = SD_SendCommand(CMD25, sector, 0x00);   
    if(r1 != 0x00)
        return r1;       
    SD_SPI_CS_EN();      
    SD_SPI_ReadWriteByte(0xff);
    SD_SPI_ReadWriteByte(0xff);          
    do     
    {                 
        SD_SPI_ReadWriteByte(0xFC);                 
        for(i=0;i<512;i++)         
        {              
            SD_SPI_ReadWriteByte(*data++);         
        }                 
        SD_SPI_ReadWriteByte(0xff); 
        SD_SPI_ReadWriteByte(0xff);                            
        r1 = SD_SPI_ReadWriteByte(0xff);         
        if((r1&0x1F)!=0x05)         
        {             
            SD_SPI_CS_DIS();               
            return r1;        
        }                     
        if(SD_WaitDataReady()==1)         
        {             
            SD_SPI_CS_DIS();             
            return 1;         
        }          
    }while(--count);  
    r1 = SD_SPI_ReadWriteByte(0xFD);     
    if(r1==0x00)     
    {          
        count =  0xfe;    
    }          
    if(SD_WaitDataReady()) 
    {   
        SD_SPI_CS_DIS();   
        return 1;   
    }          
    SD_SPI_CS_DIS();      
    SD_SPI_ReadWriteByte(0xff);        
    return count;   
} 
uint8_t SD_ReadBytes(uint32_t address,uint8_t *buf,uint16_t offset,uint16_t count)  
{ 
    u8 r1;u16 i=0;        
    r1=SD_SendCommand(CMD17,address<<9,0);        
    if(r1)return r1; 
    SD_SPI_CS_EN();
    if(SD_GetResponse(0xFE))
    {   
        SD_SPI_CS_DIS(); 
        return 1; 
    }    
    for(i=0;i<offset;i++)
        SD_SPI_ReadWriteByte(0xff);      
    for(;i<offset+count;i++)
        *buf++=SD_SPI_ReadWriteByte(0xff);     
    for(;i<512;i++) 
        SD_SPI_ReadWriteByte(0xff);   
    SD_SPI_ReadWriteByte(0xff);  
    SD_SPI_ReadWriteByte(0xff);      
    SD_SPI_CS_DIS();
    return 0; 
}


FRESULT scan_files (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                printf("%s/%s\r\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

void SD_Test(void)
{
    uint32_t sd_size;   
    uint8_t  buff[1024];
    uint32_t i;
    char  str[] = "This is a test file!\r\nFAFTS文件系统测试!\r\n";
    UINT fnum;
    FRESULT fr;
    FATFS fs;
    FIL fil;
    
    char rootPath[] = "1:";
    
    
    BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
 
#if 1
    SD_Printf("\r\n>>>>>>>>>SD Card File System Test<<<<<<<<<\r\n");
    
    fr = f_mount(&fs, rootPath, 1);
    if(fr == FR_NO_FILESYSTEM)
    {
        SD_Printf("SD Card has no File System,Should be formated...\r\n");
        fr = f_mkfs("", FM_FAT32, 0, work, sizeof work);
        if(fr == FR_OK)
        {
            SD_Printf("SD Card  has been successfully formatted Used for FAT32!!!\r\n");
            fr = f_mount(NULL, rootPath, 1);
            fr = f_mount(&fs, rootPath, 1);
        }
        else
        {
            SD_Printf("SD Card Failed to format file system!!!ERROR:%d\r\n",fr);
            while(1);
        }
    }
    else if(fr != FR_OK)
    {
        SD_Printf("SD failed to mount File System!!!ERROR:%d\r\n",fr);
    }
    else
    {
        SD_Printf("SD Card Mount File System Success!!!\r\n");
    }
 
    SD_Printf("\r\n>>>>>>>>>SD Card Write File Test <<<<<<<<<\r\n");
    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(&fil,"1:Logfile.txt",FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) 
    {
        SD_Printf("Logfile.txt Open Success!!!\r\n");
        fr = f_write(&fil,str,sizeof(str),&fnum);
        if(fr == FR_OK)
        {
            SD_Printf("File write Success ,Write %d bytes!!!\r\n",fnum);
            SD_Printf("The write Data:\r\n%s\r\n",str);
        }
        else
        {
            SD_Printf("File write failed!!!ERROR:%d\r\n",fr);
        }
        f_close(&fil);
    }
    else
    {
        SD_Printf("Open/Create file failed!!!ERROR:%d\r\n",fr);
    }


    SD_Printf("\r\n>>>>>>>>SD Card Read File Test<<<<<<<<<\r\n");
    fr = f_open(&fil,"1:Logfile.txt",FA_READ | FA_OPEN_ALWAYS);
    if (fr == FR_OK) 
    {
        SD_Printf("Logfile.txt Open Success!!!\r\n");
        fr = f_read(&fil,buff,sizeof(buff),&fnum);
        if(fr == FR_OK)
        {
            SD_Printf("File raad Success,read %d bytes!!!\r\n",fnum);
            SD_Printf("The read Data:\r\n%s\r\n",buff);
        }
        else
        {
            SD_Printf("File read failed!!!ERROR:%d\r\n",fr);
        }
        f_close(&fil);
    }
    else
    {
        SD_Printf("Open/Create file failed!!!ERROR:%d\r\n",fr);
    }
    
    SD_Printf("\r\n>>>>>>>>SD Card Search File Test<<<<<<<<<\r\n");
    scan_files(rootPath);


    f_mount(NULL,rootPath,1);
    
//    while(1);
#endif


#if 1

#define   ADD_STAR   500000

    SD_Printf("Enter SD Test Function...\r\n");
    while(SD_Init() != 0)
    {
        SD_Printf("SD Card Init Failed!!!\r\n");
    }
    SD_Printf("SD Card Init Success!!!\r\n");
    
    sd_size = SD_GetCapacity();
    SD_Printf("SD Card Memory Capacity:%luKBytes\r\n",sd_size);
    

    SD_Printf("SD Block %lu Read:",ADD_STAR);   
    if(SD_ReadSingleBlock(ADD_STAR,buff))
    {
        SD_Printf("ERROR!!!");
    }
    else
    {
        SD_Printf("Success!!!");
    }
    SD_Printf("\r\n");
    
    for( i=0;i<512;i++)
    {
        buff[i] = i;
    }
    
    SD_Printf("SD Block %lu Write:",ADD_STAR);
    if(SD_WriteSingleBlock(ADD_STAR,buff))
    {
        SD_Printf("ERROR!!!");
    }
    else
    {
        SD_Printf("Success!!!");
    }
    SD_Printf("\r\n");
    
    SD_Printf("SD Block %lu Check:",ADD_STAR);
    SD_ReadSingleBlock(ADD_STAR,buff+512);
    for( i=0;i<512;i++)
    {
        if(buff[i] != buff[i+512])
        {
            break;
        }
    }
    if(i != 512)
    {
        SD_Printf("ERROR!!!");
    }
    else
    {
        SD_Printf("Success!!!");
    }
    SD_Printf("\r\n");
    
//    while(1);
#endif
}










