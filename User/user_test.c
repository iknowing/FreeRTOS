#include "user_test.h"
#include "cJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "shell.h"





int user_test_cJson(void)
{
     cJSON* pRoot = cJSON_CreateObject();
     cJSON* pArray = cJSON_CreateArray();
     cJSON_AddItemToObject(pRoot, "students_info", pArray);
     char* szOut = cJSON_Print(pRoot);

     cJSON* pItem = cJSON_CreateObject();
     cJSON_AddStringToObject(pItem, "name", "chenzhongjing");
     cJSON_AddStringToObject(pItem, "sex", "male");
     cJSON_AddNumberToObject(pItem, "age", 28);
     cJSON_AddItemToArray(pArray, pItem);

     pItem = cJSON_CreateObject();
     cJSON_AddStringToObject(pItem, "name", "fengxuan");
     cJSON_AddStringToObject(pItem, "sex", "male");
     cJSON_AddNumberToObject(pItem, "age", 24);
     cJSON_AddItemToArray(pArray, pItem);

     pItem = cJSON_CreateObject();
     cJSON_AddStringToObject(pItem, "name", "tuhui");
     cJSON_AddStringToObject(pItem, "sex", "male");
     cJSON_AddNumberToObject(pItem, "age", 22);
     cJSON_AddItemToArray(pArray, pItem);

     char* szJSON = cJSON_Print(pRoot);
     cJSON_Delete(pRoot);
     //free(szJSON);
     printsh("%s",szJSON);
     
     
     pRoot = cJSON_Parse(szJSON);
     pArray = cJSON_GetObjectItem(pRoot, "students_info");
     if (NULL == pArray)
     {
         return -1;
     }
     
     int iCount = cJSON_GetArraySize(pArray);
     for (int i = 0; i < iCount; ++i)
     {
         cJSON* pItem = cJSON_GetArrayItem(pArray, i);
         if (NULL == pItem)
         {
             continue;
         }

         char *strName = cJSON_GetObjectItem(pItem, "name")->valuestring;
         char *strSex = cJSON_GetObjectItem(pItem, "sex")->valuestring;
         int iAge = cJSON_GetObjectItem(pItem, "age")->valueint;
     }

     cJSON_Delete(pRoot);
     free(szJSON);
    
     return 0;
}

















