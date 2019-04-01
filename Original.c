// cangku.cpp : 定义控制台应用程序的入口点。


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mysql.h"
#include "quary.h"
MYSQL conn;
MYSQL userse;
int res;
MYSQL_RES * result;
MYSQL_ROW row;
int choose;
char str1[20],str2[20],str3[20],str4[20];

char strname[2048];
char insert_query[2048];
char delete_query[2048];
char select_query[2048];
char check_query[2048];



//以下是加密算法需要的声明
const static char IP_Table[64] =//初始置换
{
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

const static char IPR_Table[64] =    //初始逆置换表
{
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};
static const char Extension_Table[48]=  //扩展置换表
{
    32, 1, 2, 3, 4, 5,4,5, 6, 7, 8, 9,
    8,9,10,11,12,13,12,13,14,15,16,17,
    16,17,18,19,20,21,20,21,22,23,24,25,
    24,25,26,27,28,29,28,29,30,31,32,1
};

const static char P_Table[32]=    //P盒置换表
{
    16, 7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26,5, 18, 31, 10,
    2, 8, 24, 14,32, 27, 3, 9,
    19, 13, 30, 6,22, 11, 4, 25
};

const static char PCK_Table[56] =//密钥置换（原64位去掉奇偶校验位后）
{
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};
const static char LOOP_Table[16] =//左移
{
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};
const static char PCC_Table[48] =//压缩置换表
{
    14, 17, 11, 24,   1,   5,
    3,  28, 15,  6,  21,  10,
    23, 19, 12,  4,  26,   8,
    16,  7, 27, 20,  13,   2,
    41, 52, 31, 37,  47,  55,
    30, 40, 51, 45,  33,  48,
    44, 49, 39, 56,  34,  53,
    46, 42, 50, 36,  29,  32
};
// 48 / 6 * 4 = 32
const static char S_Box[8][4][16] =//8个S盒
{
    {
        // S1
        {  14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7   },
        {   0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8   },
        {   4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0   },
        {  15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13   }
    },
    {
        // S2
        {  15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10   },
        {   3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5   },
        {   0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15   },
        {  13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9   }
    },
    {
        // S3
        {  10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8   },
        {  13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1   },
        {  13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7   },
        {   1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12   }
    },
    {
        // S4
        {   7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15   },
        {  13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9   },
        {  10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4   },
        {   3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14   }
    },
    {
        // S5
        {   2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9   },
        {  14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6   },
        {   4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14   },
        {  11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3   }
    },
    {
        // S6
        {  12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11   },
        {  10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8   },
        {   9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6   },
        {   4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13   }
    },
    {
        // S7
        {   4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1   },
        {  13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6   },
        {   1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2   },
        {   6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12   }
    },
    {
        // S8
        {  13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7   },
        {   1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2   },
        {   7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8   },
        {   2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11   }
    }
};


typedef bool (*PSubKey)[16][48];
enum{ENCRYPT,DECRYPT};
static bool SubKey[2][16][48];
static bool Is3DES;
static char Tmp[256],deskey[16];

static void SetKey(const char *Key,int len);
static void SetSubKey(PSubKey pSubKey,const char Key[8]);
static void F_func(bool In[32],const bool Ki[48]);
static void S_func(bool Out[32],const bool In[48]);
static void Transform(bool *Out[32],const bool In[48],int len);
static void Xor(bool *InA,const bool *InB,int len);
static void RotateL(bool *In,int len,int loop);
static void ByteToBit(bool *Out,const char *In,int bits);
static void BitToByte(char *Out,const bool *In,int bits);
static void myDES(char Out[8],char In[8],const PSubKey pSubKey,bool Type);
static void Setrealencrypte(char *s , int length);
static void Getrealencrypte(char *s , char *tar ,int length);
static void ItoA(int a, char s[],int pos);
static int Strtoint(int recive[1000] , char saveencrypt[1000] , int length);

bool DES_Act(char *Out,char *In,long datalen,const char *Key,int keylen,bool Type);

	char key[]={0,2,1,0,9,4,5,1,7,8,5,0,7,2,8};
    char plain_text[128];
    char encrypt_text[1024];
    char decrypt_text[1024];
    char tests[255];

void Setrealencrypte(char *encrypte , int length){
    /*
        这个函数用来生成一个类似",14,25,-5,1," 这样的字符串  也就是往数据库里存储的东西
        encrypt是指针 直接把结果赋值给你想要的地方
        最需要注意的地方是 不要把原本的encrypt_text传进来会出指针错误
        把原来的encrype_text    strcpy一份给别的数组 然后把那个数组传进来  需要存储的字符串也会被放到这个数组里
    */
     char saveencrypt[1000];
     memset(saveencrypt,0,sizeof(saveencrypt));
     int tt[1000];
     for (int i = 0 ; i < length; ++i) {
            tt[i] = encrypte[i] - '\0';

            strcat(saveencrypt , ",");


        ItoA(tt[i] , saveencrypt, strlen(saveencrypt));

        if(i == strlen(encrypte)) {
            strcat(saveencrypt , ",");
        }
	}
	strcat(saveencrypt , ",");
	int pos = 0;
	for(int i = 0 ; i < strlen(saveencrypt) ; ++i) {
	    *(encrypte + pos) = saveencrypt[i];
	    pos++;
	}*(encrypte + pos) = '\0';
}

int Atoi(char str[]){
    int length = strlen(str);
    int num = 0;
    if (str[0] == '-') {
        int time = 0;
        for (int i = 1 ; i < length ; i ++) {
            num = num * 10 + (str[i] - '0');
        }
        return num * -1;
    }
    else {
        int time = 0;
        for (int i = 0 ; i < length ; i ++) {
            num = num * 10 + (str[i] - '0');
        }
        return num;
    }
}

void ItoA(int a, char s[],int pos){
    if(a < 0) {
        *(s + pos) = '-';
        pos++;
        a *= -1;
    }
    char temp[10];
    int k = 0;
    while (a) {

        temp[k++] = '0' + (a % 10);

        a /= 10;
    }
    for (int i = k - 1 ; i >= 0 ; i --) {
        *(s + pos) = temp[i];
        pos ++;
    }
}
/*
一个密文生成函数
一个密文解密函数
*/
void Getrealencrypte(char *encrypte , char * tar ,int length){
    /*
        这个函数用来返回一个真正的密文
        把数据库里读出来的字符串传到第一个参数 第二个参数固定传encrype_text
        第三个传数据库里读出来的密文长度
    */
    int recive[1000];
    char testforencry[1000];
    int nums = 0;
    memset(recive,0,sizeof(recive));
    memset(testforencry,0,sizeof(testforencry));
    memset(recive,0,sizeof(recive));
    nums = Strtoint(recive , encrypte, length);
    for (int i = 0 ; i < nums; ++i) {
        testforencry[i] = '\0' + recive[i];

    }
    int pos = 0;
    for(int i = 0 ; i < nums ; i ++) {
        encrypte[pos++] = testforencry[i];
    }
    encrypte[pos] = '\0';
    for(int i = 0 ; i < pos ; ++i) {

        *(tar + i) = encrypte[i];
    }
}

void putin_this_report(char *str1,char* str3,char* str4,char *mothod){
	
	memset(insert_query, 0, sizeof(insert_query));
	strcat(insert_query, "insert into intreport values('");

	//存入物品名称的加密
    memset(tests, 0, sizeof(tests));
    memset(encrypt_text,0,sizeof(encrypt_text));
    memset(decrypt_text,0,sizeof(decrypt_text));
    memset(plain_text,0,sizeof(plain_text));

	strcpy(plain_text,str1);
    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

 			strcpy(tests , encrypt_text);//加密后的密文
            Setrealencrypte(tests, strlen(tests));
            printf("\nDES加密前的明文:\n");
            printf("%s\n\n",plain_text);

    printf("\nDES加密后的密文:\n");
    printf("%s\n\n",tests);

	strcat(insert_query, tests);
	strcat(insert_query, "','");

	//存入数量的加密
	memset(tests, 0, sizeof(tests));
	memset(plain_text,0,sizeof(plain_text));
	memset(encrypt_text,0,sizeof(encrypt_text));
    memset(decrypt_text,0,sizeof(decrypt_text));

	strcpy(plain_text,str3);

    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

			strcpy(tests , encrypt_text);//加密后的密文

            Setrealencrypte(tests, strlen(tests));
  
            printf("\nDES加密前的明文:\n");
            printf("%s\n\n",plain_text);


    printf("\nDES加密后的密文:\n");
    printf("%s\n\n",tests);
 
	strcat(insert_query, tests);
	strcat(insert_query, "','");

	//存入时间的加密
	memset(tests, 0, sizeof(tests));
	memset(plain_text,0,sizeof(plain_text));
	memset(encrypt_text,0,sizeof(encrypt_text));
    memset(decrypt_text,0,sizeof(decrypt_text));

	strcpy(plain_text,str4);

    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

			strcpy(tests , encrypt_text);//加密后的密文

            Setrealencrypte(tests, strlen(tests));
 
            printf("\nDES加密前的明文:\n");
            printf("%s\n\n",plain_text);




    printf("\nDES加密后的密文:\n");
    printf("%s\n\n",tests);
  
	strcat(insert_query, tests);
	strcat(insert_query, "','");




	//存入种类的加密
	memset(tests, 0, sizeof(tests));
	memset(plain_text,0,sizeof(plain_text));
	memset(encrypt_text,0,sizeof(encrypt_text));
    memset(decrypt_text,0,sizeof(decrypt_text));

	strcpy(plain_text,mothod);


    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

			strcpy(tests , encrypt_text);//加密后的密文

            Setrealencrypte(tests, strlen(tests));
     
            printf("\nDES加密前的明文:\n");
            printf("%s\n\n",plain_text);



    printf("\nDES加密后的密文:\n");
    printf("%s\n\n",tests);
 
	strcat(insert_query, tests);
	strcat(insert_query, "'");
	strcat(insert_query, ")");

	printf("SQL语句: %s\n", insert_query);
	res = mysql_query(&conn, insert_query);
	if (!res) {
		printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		printf("insert error\n");
	}//向putin报表录入//向putin报表录入//录入入库信息的加密
}

void putout_this_reprot(char* row1,char *row2,char* row3,char* mothod){

					memset(insert_query, 0, sizeof(insert_query));
					strcat(insert_query, "insert into outreport values('");


						//出库物品名称的加密
						memset(tests, 0, sizeof(tests));
						memset(plain_text,0,sizeof(plain_text));
						memset(encrypt_text,0,sizeof(encrypt_text));
					    memset(decrypt_text,0,sizeof(decrypt_text));

						strcpy(plain_text,row1);


					    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

								strcpy(tests , encrypt_text);//加密后的密文

					            Setrealencrypte(tests, strlen(tests));
					       
					            
					            printf("\nDES加密前的明文:\n");
					            printf("%s\n\n",plain_text);



					    printf("\nDES加密后的密文:\n");
					    printf("%s\n\n",tests);

					strcat(insert_query, tests);//物品名称
					strcat(insert_query, "','");


						//出库物品数量的加密
						memset(tests, 0, sizeof(tests));
						memset(plain_text,0,sizeof(plain_text));
						memset(encrypt_text,0,sizeof(encrypt_text));
					    memset(decrypt_text,0,sizeof(decrypt_text));

						strcpy(plain_text,row2);

			
					    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

								strcpy(tests , encrypt_text);//加密后的密文

					            Setrealencrypte(tests, strlen(tests));
					       
					            
					            printf("\nDES加密前的明文:\n");
					            printf("%s\n\n",plain_text);



					    printf("\nDES加密后的密文:\n");
					    printf("%s\n\n",tests);

					strcat(insert_query, tests);//物品数量
					strcat(insert_query, "','");



						//出库物品时间的加密
						memset(tests, 0, sizeof(tests));
						memset(plain_text,0,sizeof(plain_text));
						memset(encrypt_text,0,sizeof(encrypt_text));
					    memset(decrypt_text,0,sizeof(decrypt_text));

						strcpy(plain_text,row3);

					
					    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

								strcpy(tests , encrypt_text);//加密后的密文

					            Setrealencrypte(tests, strlen(tests));
					     
					            
					            printf("\nDES加密前的明文:\n");
					            printf("%s\n\n",plain_text);



					    printf("\nDES加密后的密文:\n");
					    printf("%s\n\n",tests);


					strcat(insert_query, tests);//存储时间
					strcat(insert_query, "','");


						//物品种类的加密
						memset(tests, 0, sizeof(tests));
						memset(plain_text,0,sizeof(plain_text));
						memset(encrypt_text,0,sizeof(encrypt_text));
					    memset(decrypt_text,0,sizeof(decrypt_text));

						strcpy(plain_text,mothod);

				
					    DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

								strcpy(tests , encrypt_text);//加密后的密文

					            Setrealencrypte(tests, strlen(tests));
					          
					            
					            printf("\nDES加密前的明文:\n");
					            printf("%s\n\n",plain_text);



					    printf("\nDES加密后的密文:\n");
					    printf("%s\n\n",tests);


					strcat(insert_query, tests);//物品种类
					strcat(insert_query, "'");
					strcat(insert_query, ")");
					printf("SQL语句: %s\n", insert_query);
					res = mysql_query(&conn, insert_query);
					if (!res) {
						printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
					}
					else {
						printf("insert error\n");
					}//向putout报表录入//录入出库信息的加密
}


//以下是仓库管理系统的基本应用函数
void report();//报表

void KuCunInterface(){//仓库界面
	printf("*****************************************\n");
	printf("<<<<仓库管理系统————库存管理权限>>>>\n");
	printf("1.入库管理\n");
	printf("2.库存管理\n");
	printf("3.出库管理\n");
	printf("4.出入库信息报表查询\n");
	printf("5.退出系统\n");
	printf("———请选择需要执行的库存管理权限———\n");
	printf("请输入选项编号：");
}

void PutinDaily(){//入库日用品物品
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	//43501
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;
	int overdue = 0;
	printf("您已经进入日用品入库管理界面....\n");
	printf("请录入需要储存的日用品名称\n");
	memset(insert_query, 0, sizeof(insert_query));

	printf("请输入物品名称：");
	scanf("%s", str1);

	printf("请输入物品的保质期：");
	scanf("%d", &overdue);
	getchar();
	itoa(overdue , str2 , 10);
	
	printf("请输入物品的数量：");
	scanf("%s",str3);

	printf("intime is %d\n",intime);
	itoa(intime , str4, 10);


	
	//insert 到相应的仓库中

	strcat(insert_query, "insert into daily values('");
	strcat(insert_query, str1);
	strcat(insert_query, "','");
	strcat(insert_query, str2);
	strcat(insert_query, "','");
	strcat(insert_query, str3);
	strcat(insert_query, "','");
	strcat(insert_query, str4);
	strcat(insert_query, "'");
	strcat(insert_query, ")");
	printf("SQL语句: %s\n", insert_query);
	res = mysql_query(&conn, insert_query);
	if (!res) {
		printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		printf("insert error\n");
	}


	//insert 到report表中
	char mothod[20] = ("日用品");
	putin_this_report(str1,str3,str4,mothod);

}

void PutinFood(){//入库食品物品
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	//43501
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;
	int overdue = 0;
	printf("您已经进入日用品入库管理界面....\n");
	printf("请录入需要储存的日用品名称\n");
	memset(insert_query, 0, sizeof(insert_query));

	printf("请输入物品名称：");
	scanf("%s", str1);

	printf("请输入物品的保质期：");
	scanf("%d", &overdue);
	getchar();
	itoa(overdue , str2 , 10);
	overdue += (now_time.tm_year - 1900) * 365;
	printf("请输入物品的数量：");
	scanf("%s",str3);

	//printf("请输入物品的存入日期");
	//scanf("%s",n);
	printf("intime is %d\n",intime);
	itoa(intime , str4, 10);
	//insert
	strcat(insert_query, "insert into food values('");
	strcat(insert_query, str1);
	strcat(insert_query, "','");
	strcat(insert_query, str2);
	strcat(insert_query, "','");
	strcat(insert_query, str3);
	strcat(insert_query, "','");
	strcat(insert_query, str4);
	strcat(insert_query, "'");
	strcat(insert_query, ")");
	printf("SQL语句: %s\n", insert_query);
	res = mysql_query(&conn, insert_query);
	if (!res) {
		printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		printf("insert error\n");
	}

	//insert 到report表中
	char mothod[20] = ("食物");

	putin_this_report(str1,str3,str4,mothod);

}

void PutinFurniture(){//入库家具物品
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;
	int overdue = 0;
	printf("您已经进入日用品入库管理界面....\n");
	printf("请录入需要储存的日用品名称\n");
	memset(insert_query, 0, sizeof(insert_query));

	printf("请输入物品名称：");
	scanf("%s", str1);

	printf("请输入物品的保质期：");
	scanf("%d", &overdue);
	getchar();
	itoa(overdue , str2 , 10);
	overdue += (now_time.tm_year - 1900) * 365;
	printf("请输入物品的数量：");
	scanf("%s",str3);

	//printf("请输入物品的存入日期");
	//scanf("%s",n);
	printf("intime is %d\n",intime);
	itoa(intime , str4, 10);
	//insert
	strcat(insert_query, "insert into furniture values('");
	strcat(insert_query, str1);
	strcat(insert_query, "','");
	strcat(insert_query, str2);
	strcat(insert_query, "','");
	strcat(insert_query, str3);
	strcat(insert_query, "','");
	strcat(insert_query, str4);
	strcat(insert_query, "'");
	strcat(insert_query, ")");
	printf("SQL语句: %s\n", insert_query);
	res = mysql_query(&conn, insert_query);
	if (!res) {
		printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		printf("insert error\n");
	}

	//insert 到report表中
	char mothod[20] = ("家具");

	putin_this_report(str1,str3,str4,mothod);
}

void PutinElectric(){//入库电器物品
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	//43501
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;
	int overdue = 0;
	printf("您已经进入日用品入库管理界面....\n");
	printf("请录入需要储存的日用品名称\n");
	memset(insert_query, 0, sizeof(insert_query));

	printf("请输入物品名称：");
	scanf("%s", str1);

	printf("请输入物品的保质期：");
	scanf("%d", &overdue);
	getchar();
	itoa(overdue , str2 , 10);
	overdue += (now_time.tm_year - 1900) * 365;
	printf("请输入物品的数量：");
	scanf("%s",str3);

	//printf("请输入物品的存入日期");
	//scanf("%s",n);
	printf("intime is %d\n",intime);
	itoa(intime , str4, 10);
	//insert
	strcat(insert_query, "insert into electric values('");
	strcat(insert_query, str1);
	strcat(insert_query, "','");
	strcat(insert_query, str2);
	strcat(insert_query, "','");
	strcat(insert_query, str3);
	strcat(insert_query, "','");
	strcat(insert_query, str4);
	strcat(insert_query, "'");
	strcat(insert_query, ")");
	printf("SQL语句: %s\n", insert_query);
	res = mysql_query(&conn, insert_query);
	if (!res) {
		printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		printf("insert error\n");
	}
	//insert 到report表中
	char mothod[20] = ("电器");

	putin_this_report(str1,str3,str4,mothod);
}

void PutoutDaliy(){//出库日用品物品
	memset(delete_query, 0, sizeof(delete_query));
	printf("请输入需要出库的物品名称");
	scanf("%s", str1);
	

		//先进行对出库的物品进行查询并且保存信息
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from daily where name ='");
		strcat(select_query, str1);
		strcat(select_query, "'");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					
					//将出库的信息添加到outreport
					char mothod[20] = ("日用品");

					putout_this_reprot(row[0],row[2],row[3],mothod);
				}
			}
		}

	strcat(delete_query, "delete from daily where name='");
	strcat(delete_query, str1);
	strcat(delete_query, "'");
	//delete
	printf("SQL语句: %s\n", delete_query);
	res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
	if (!res) {
		printf("delete successful\n");
	}
	else {
		printf("delete error\n");
	}
		//select	
}

void PutoutFood(){//出库食品物品
	memset(delete_query, 0, sizeof(delete_query));
	printf("请输入需要出库的物品名称");
	scanf("%s", str1);


		//先进行对出库的物品进行查询并且保存信息
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from food where name ='");
		strcat(select_query, str1);
		strcat(select_query, "'");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					
					//将出库的信息添加到outreport
					char mothod[20] = ("食物");

					putout_this_reprot(row[0],row[2],row[3],mothod);
				}
			}
		}





	strcat(delete_query, "delete from food where name='");
	strcat(delete_query, str1);
	strcat(delete_query, "'");
	//delete
	printf("SQL语句: %s\n", delete_query);
	res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
	if (!res) {
		printf("delete successful\n");
	}
	else {
		printf("delete error\n");
	}
}

void PutoutFurniture(){//出库家具物品
	memset(delete_query, 0, sizeof(delete_query));
	printf("请输入需要出库的物品名称");
	scanf("%s", str1);


		//先进行对出库的物品进行查询并且保存信息
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from furniture where name ='");
		strcat(select_query, str1);
		strcat(select_query, "'");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					
					//将出库的信息添加到outreport
					char mothod[20] = ("家具");
					putout_this_reprot(row[0],row[2],row[3],mothod);
				}
			}
		}


	strcat(delete_query, "delete from furniture where name='");
	strcat(delete_query, str1);
	strcat(delete_query, "'");
	//delete
	printf("SQL语句: %s\n", delete_query);
	res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
	if (!res) {
		printf("delete successful\n");
	}
	else {
		printf("delete error\n");
	}
}

void PutoutElectric(){//出库电器物品
	memset(delete_query, 0, sizeof(delete_query));
	printf("请输入需要出库的物品名称");
	scanf("%s", str1);


		//先进行对出库的物品进行查询并且保存信息
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from electric where name ='");
		strcat(select_query, str1);
		strcat(select_query, "'");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					
					//将出库的信息添加到outreport
					char mothod[20] = ("电器");

					putout_this_reprot(row[0],row[2],row[3],mothod);
				}
			}
		}


	strcat(delete_query, "delete from electric where name='");
	strcat(delete_query, str1);
	strcat(delete_query, "'");
	//delete
	printf("SQL语句: %s\n", delete_query);
	res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
	if (!res) {
		printf("delete successful\n");
	}
	else {
		printf("delete error\n");
	}
}

void Gdaily(){//查找日用品仓库物品
		//select
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from daily");
		
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void Gfood(){//查找食品仓库物品
		//select
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from food");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void Gfurniture(){//查找家具仓库物品
	//select
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from furniture");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void Gelectric(){//查找电器仓库物品
		//select
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from electric");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void selcdaily(){//搜索日用品仓库
	printf("已经进入日用品仓库搜索\n");
	memset(select_query, 0, sizeof(select_query));
	strcat(select_query, "SELECT * FROM daily WHERE name = '");
	printf("请输入需要查询物品的名称\n");
	scanf("%s",strname);
	strcat(select_query, strname);
	strcat(select_query, "'");

	printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void selcfood(){//搜索食品仓库
	printf("已经进入日食品仓库搜索\n");
	memset(select_query, 0, sizeof(select_query));
	strcat(select_query, "SELECT * FROM food WHERE name = '");
	printf("请输入需要查询物品的名称\n");
	scanf("%s",strname);
	strcat(select_query, strname);
	strcat(select_query, "'");

	printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void selcfurniture(){//搜索家具仓库
	printf("已经进入家具仓库搜索\n");
	memset(select_query, 0, sizeof(select_query));
	strcat(select_query, "SELECT * FROM furniture WHERE name = '");
	printf("请输入需要查询物品的名称\n");
	scanf("%s",strname);
	strcat(select_query, strname);
	strcat(select_query, "'");

	printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void selcelectric(){//搜索电器仓库
	printf("已经进入电器仓库搜索\n");
	memset(select_query, 0, sizeof(select_query));
	strcat(select_query, "SELECT * FROM electric WHERE name = '");
	printf("请输入需要查询物品的名称\n");
	scanf("%s",strname);
	strcat(select_query, strname);
	strcat(select_query, "'");

	printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {

					printf("name is %s , ", row[0]);
					printf("overdue is %s , ", row[1]);
					printf("num is %s ", row[2]);
					printf("time is %s\n", row[3]);
				}
			}
		}
}

void check_daily(){//检测日用品仓库
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;
	//printf("intime is %d\n", intime );

	char now_name[50];
	int it_time,it_overdue;
	memset(check_query, 0, sizeof(check_query));
	strcat(check_query, "SELECT time,overdue FROM daily WHERE name = '");
	printf("进入检测日用品仓库....\n");
	printf("请输入需要检测的物品名称\n");
	scanf("%s",now_name);
	strcat(check_query, now_name);
	strcat(check_query, "'");
	printf("SQL语句: %s\n", check_query);
	if (mysql_query(&conn, check_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
					while ((row = mysql_fetch_row(result)) != NULL) {
						printf("time is %s\n", row[0]);
						it_time = atoi(row[0]);
						it_overdue = atoi(row[1]);
						if(it_time - intime < it_overdue){
							printf("该商品处于保质期期间.....安全\n");

						}
						else{
									printf("该商品超过保质期...请及时销毁或回收....\n");
									memset(delete_query, 0, sizeof(delete_query));
									strcat(delete_query, "DELETE FROM daily WHERE time = '");
									strcat(delete_query,row[0]);
									strcat(delete_query,"'");
									printf("SQL语句: %s\n", delete_query);
									res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
									if (!res) {

										printf("回收/销毁成功\n");
									}
									else {

										printf("回收/销毁失败\n");
									}
							}
					}
				}
					
			}
}

void check_food(){//检测食物仓库
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;

	char now_name[50];
	int it_time,it_overdue;
	memset(check_query, 0, sizeof(check_query));
	strcat(check_query, "SELECT time,overdue FROM food WHERE name = '");
	printf("进入检测食品仓库....\n");
	printf("请输入需要检测的物品名称\n");
	scanf("%s",now_name);
	strcat(check_query, now_name);
	strcat(check_query, "'");
	printf("SQL语句: %s\n", check_query);
	if (mysql_query(&conn, check_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
					while ((row = mysql_fetch_row(result)) != NULL) {
						printf("time is %s\n", row[0]);
						it_time = atoi(row[0]);
						it_overdue = atoi(row[1]);
						if(it_time - intime < it_overdue){
							printf("该商品处于保质期期间.....安全\n");

						}
						else{
									printf("该商品超过保质期...请及时销毁或回收....\n");
									memset(delete_query, 0, sizeof(delete_query));
									strcat(delete_query, "DELETE FROM food WHERE time = '");
									strcat(delete_query,row[0]);
									strcat(delete_query,"'");
									printf("SQL语句: %s\n", delete_query);
									res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
									if (!res) {

										printf("回收/销毁成功\n");
									}
									else {

										printf("回收/销毁失败\n");
									}
							}
					}
				}
					
			}
}

void check_furniture(){//检测家具仓库
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;

	char now_name[50];
	int it_time,it_overdue;
	memset(check_query, 0, sizeof(check_query));
	strcat(check_query, "SELECT time,overdue FROM furniture WHERE name = '");
	printf("进入检测家具仓库....\n");
	printf("请输入需要检测的物品名称\n");
	scanf("%s",now_name);
	strcat(check_query, now_name);
	strcat(check_query, "'");
	printf("SQL语句: %s\n", check_query);
	if (mysql_query(&conn, check_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
					while ((row = mysql_fetch_row(result)) != NULL) {
						printf("time is %s\n", row[0]);
						it_time = atoi(row[0]);
						it_overdue = atoi(row[1]);
						if(it_time - intime < it_overdue){
							printf("该商品处于保质期期间.....安全\n");

						}
						else{
									printf("该商品超过保质期...请及时销毁或回收....\n");
									memset(delete_query, 0, sizeof(delete_query));
									strcat(delete_query, "DELETE FROM furniture WHERE time = '");
									strcat(delete_query,row[0]);
									strcat(delete_query,"'");
									printf("SQL语句: %s\n", delete_query);
									res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
									if (!res) {

										printf("回收/销毁成功\n");
									}
									else {

										printf("回收/销毁失败\n");
									}
							}
					}
				}
					
			}
}

void check_electric(){//检测电器仓库
	// 这个now_time 用来查找到一个unix时间戳
	time_t time_seconds = time(0);
	struct tm now_time;
	localtime_s(&now_time,&time_seconds);
	
	int intime;
	intime = (now_time.tm_year) * 365 + now_time.tm_yday;


	char now_name[50];
	int it_time,it_overdue;
	memset(check_query, 0, sizeof(check_query));
	strcat(check_query, "SELECT time,overdue FROM electric WHERE name = '");
	printf("进入检测电器仓库....\n");
	printf("请输入需要检测的物品名称\n");
	scanf("%s",now_name);
	strcat(check_query, now_name);
	strcat(check_query, "'");
	printf("SQL语句: %s\n", check_query);
	if (mysql_query(&conn, check_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
					while ((row = mysql_fetch_row(result)) != NULL) {
						printf("time is %s\n", row[0]);
						it_time = atoi(row[0]);
						it_overdue = atoi(row[1]);
						if(it_time - intime < it_overdue){
							printf("该商品处于保质期期间.....安全\n");

						}
						else{
									printf("该商品超过保质期...请及时销毁或回收....\n");
									memset(delete_query, 0, sizeof(delete_query));
									strcat(delete_query, "DELETE FROM electric WHERE time = '");
									strcat(delete_query,row[0]);
									strcat(delete_query,"'");
									printf("SQL语句: %s\n", delete_query);
									res = mysql_real_query(&conn, delete_query, (unsigned int)strlen(delete_query));
									if (!res) {

										printf("回收/销毁成功\n");
									}
									else {

										printf("回收/销毁失败\n");
									}
							}
					}
				}
					
			}
}

void Putin(){//入库界面
	printf("请选择需要存放的类别\n");
	printf("1.日用品\n");
	printf("2.食品\n");
	printf("3.家具\n");
	printf("4.电器\n");
	int num ;
	printf("请输入指令：");
	scanf("%d",&num);
	printf("\n");
	switch(num){
	case 1:
		PutinDaily();
		break;
	case 2:
		PutinFood();
		break;
	case 3:
		PutinFurniture();
		break;
	case 4:
		PutinElectric();
		break;
	}	
}

void Putout(){//出库界面


	printf("请选择需要出库物品的类别\n");
	printf("1.日用品\n");
	printf("2.食品\n");
	printf("3.家具\n");
	printf("4.电器\n");
	int num ;
	scanf("%d",&num);
	switch(num){
	case 1:
		PutoutDaliy();
		break;
	case 2:
		PutoutFood();
		break;
	case 3:
		PutoutFurniture();
		break;
	case 4:
		PutoutElectric();
		break;
	}
}

void Administration(){//仓库管理界面
	printf("您已经进入库存管理界面.....\n");
	printf("请选择需要执行的权限功能...\n");
	printf("1.搜索仓库库存物品.........\n");
	printf("2.列出仓库库存物品信息.....\n");
	printf("3.检测库存物品质量.........\n");
	int num;
	scanf("%d",&num);
	switch(num){
	case 1:
		printf("请选择需要搜索的仓库\n");
		printf("1.日用品仓库\n");
		printf("2.食品仓库\n");
		printf("3.家具仓库\n");
		printf("4.电器仓库\n");
		int num1;
		scanf("%d",&num1);
		switch(num1){
			case 1:
				selcdaily();
				break;
			case 2:
				selcfood();
				break;
			case 3:
				selcfurniture();
				break;
			case 4:
				selcelectric();
				break;
		}
		break;
	case 2:
		printf("请选择需要进入的仓库\n");
		printf("1.日用品仓库\n");
		printf("2.食品仓库\n");
		printf("3.家具仓库\n");
		printf("4.电器仓库\n");
		int num;
		scanf("%d",&num);
		switch(num){
			case 1:
				Gdaily();
				break;
			case 2:
				Gfood();
				break;
			case 3:
				Gfurniture();
				break;
			case 4:
				Gelectric();
				break;
		}
		break;
	case 3:
		printf("请选择需要检测的仓库\n");
		printf("1.日用品仓库\n");
		printf("2.食品仓库\n");
		printf("3.家具仓库\n");
		printf("4.电器仓库\n");
		int num2;
		scanf("%d",&num2);
		switch(num2){
			case 1:
				check_daily();
				break;
			case 2:
				check_food();
				break;
			case 3:
				check_furniture();
				break;
			case 4:
				check_electric();
				break;
		}
		break;
	}
}

void link_cangku(){//登录后 链接仓库界面
	mysql_init(&conn);
			if (mysql_real_connect(&conn, "127.0.0.1", "root", "qwer751206", "kucunguanli", 3306, NULL, 0)) {
				printf("coneect kucunguanli_mysql successful\n");
				while(1){
					KuCunInterface();
					while (scanf("%d", &choose)){
						
						switch (choose) {
						case 1:
							Putin();
							printf("已完成入库，即将返回主页面....请继续操作\n\n");
							KuCunInterface();
							break;
						case 2:
							Administration();
							printf("已完成库存管理，即将返回主页面....请继续操作\n\n");
							KuCunInterface();
							break;
						case 3:
							Putout();
							printf("已完成出库，即将返回主页面....请继续操作\n\n");
							KuCunInterface();
							break;
						case 4:
							report();
							printf("已完成查询出入库报表信息，即将返回主页面....请继续操作\n\n");
							KuCunInterface();
							break;
						case 5:
							printf("退出系统！");
							exit(0);
							break;
						default:
							printf("您输入的不正确请重新输入:");
						}
					}
				}
				mysql_close(&conn);
			}
}

int user_regist(){//用户注册界面

	char user_account[50];
	char user_password[50];
	printf("您已经进入注册界面\n");
	printf("请输入您要注册的账号名：");
	scanf("%s",user_account);

		memset(check_query, 0, sizeof(check_query));
		strcat(check_query, "SELECT account FROM  users;");
		//printf("SQL语句: %s\n", check_query);
		if (mysql_query(&userse, check_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&userse)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					//printf("account is %s , ", row[0]);
					if(strcmp(row[0],user_account)==0){
						printf("账号已被注册....正在返回上一级界面\n");
						return 0;
					}
				}
			}
		}

	printf("账号尚未被注册\n");
	printf("请输入密码:");
	scanf("%s",user_password);
	memset(insert_query, 0, sizeof(insert_query));
	//insert
	strcat(insert_query, "insert into users values('");
	strcat(insert_query, user_account);
	strcat(insert_query, "','");

	

		//加密密码
			memset(tests, 0, sizeof(tests));
		    memset(encrypt_text,0,sizeof(encrypt_text));
		    memset(decrypt_text,0,sizeof(decrypt_text));
		    memset(plain_text,0,sizeof(plain_text));
			strcpy(plain_text,user_password);

	    	DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

				strcpy(tests , encrypt_text);//加密后的密文

	            Setrealencrypte(tests, strlen(tests));
	 
	            printf("\nDES加密前的明文:\n");
	            printf("%s\n\n",plain_text);




	    printf("\nDES加密后的密文:\n");
	    printf("%s\n\n",tests);
	  
		strcat(insert_query, tests);

	
	

	//strcat(insert_query, user_password);

	strcat(insert_query, "'");
	strcat(insert_query, ")");

	printf("SQL语句: %s\n", insert_query);
	res = mysql_query(&userse, insert_query);
	if (!res) {
		printf("insert %lu rows\n", (unsigned long)mysql_affected_rows(&userse));
	}
	else {
		printf("insert error\n");
	}
	printf("账号已注册成功\n\n");
	return 0;
}

void user_logining(){//登录界面
	char user_account[1024];
	char user_password[1024];
	int judge;
	printf("已经进入登录界面....\n");
	printf("请输入账号：");
	scanf("%s",user_account);
	printf("请输入密码：");
	scanf("%s",user_password);

	/*
			//加密密码
				memset(tests, 0, sizeof(tests));
			    memset(encrypt_text,0,sizeof(encrypt_text));
			    memset(decrypt_text,0,sizeof(decrypt_text));
			    memset(plain_text,0,sizeof(plain_text));
				strcpy(plain_text,user_password);

		    	DES_Act(encrypt_text,plain_text,sizeof(plain_text),key,sizeof(key),ENCRYPT);

					strcpy(tests , encrypt_text);//加密后的密文

		            Setrealencrypte(tests, strlen(tests));
		 
		            printf("\nDES加密前的明文:\n");
		            printf("%s\n\n",plain_text);

		            printf("\nDES加密后的密文:\n");
		    		printf("%s\n\n",tests);

	*/

		memset(check_query, 0, sizeof(check_query));
		strcat(check_query, "SELECT account,password FROM users;");
		//printf("SQL语句: %s\n", check_query);
		if (mysql_query(&userse, check_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&userse)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					if(strcmp(row[0],user_account) == 0){
						//printf("账号正确\n");
	
						//密码的解密
					    memset(tests, 0, sizeof(tests));
					    memset(encrypt_text,0,sizeof(encrypt_text));
					    memset(decrypt_text,0,sizeof(decrypt_text));
					    memset(plain_text,0,sizeof(plain_text));
					    strcpy(tests,row[1]);

						Getrealencrypte(tests, encrypt_text, strlen(tests));

						DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

						printf("num is %s , ", decrypt_text);
	
		
						if (strcmp(decrypt_text,user_password) == 0)
						{
							printf("密码正确\n");
							printf("登录成功\n\n");
							judge = 1;
							link_cangku();
						}
					}
				}
				if (judge != 1)
				{
					printf("密码或账号不正确...返回上一界面\n\n");
				}
				
			}
		}
}

void report_putin(){//入库信息表
		//select
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from intreport");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {

					//存入物品名称的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[0]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("name is %s , ", decrypt_text);

					//存入物品数量的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[1]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("num is %s , ", decrypt_text);


					//存入物品时间的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[2]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("time is %s , ", decrypt_text);



					//存入物品种类的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[3]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("kind is %s\n", decrypt_text);
				}
			}
		}
}

void report_putout(){//出库信息表
	//select
		memset(select_query, 0, sizeof(select_query));
		strcat(select_query, "select * from outreport");
		printf("SQL语句: %s\n", select_query);
		if (mysql_query(&conn, select_query) != 0) {
			fprintf(stderr, "查询失败！\n");
			exit(1);
		}
		else {
			if ((result = mysql_store_result(&conn)) == NULL) {
				fprintf(stderr, "保存结果集失败！\n");
				exit(1);
			}
			else {
				while ((row = mysql_fetch_row(result)) != NULL) {
					
					//出库物品名称的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[0]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("name is %s , ", decrypt_text);


					//出库物品数量的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[1]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("num is %s , ", decrypt_text);

					//出库时间的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[2]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);

					printf("time is %s , ", decrypt_text);



					//出库物品种类的解密
				    memset(tests, 0, sizeof(tests));
				    memset(encrypt_text,0,sizeof(encrypt_text));
				    memset(decrypt_text,0,sizeof(decrypt_text));
				    memset(plain_text,0,sizeof(plain_text));
				    strcpy(tests,row[3]);
					Getrealencrypte(tests, encrypt_text, strlen(tests));

					DES_Act(decrypt_text,encrypt_text,sizeof(plain_text),key,sizeof(key),DECRYPT);


					printf("kind is %s\n", decrypt_text);
				}
			}
		}
}

void report(){//报表库
	printf("进入出/入库信息查询界面....\n");
	printf("1.入库信息查询....\n");
	printf("2.出库信息查询\n");
	printf("请输入选择的查询的表:\n");
	int num4;
	scanf("%d",&num4);
	switch(num4){
		case 1:
			report_putin();
			break;
		case 2:
			report_putout();
			break;
		default:
			printf("您输入的不正确请重新输入:");
	}
}

void login(){//登录/注册界面
	int num3;
	printf("欢迎使用仓库管理系统....\n");
	printf("1.已拥有账号登录\n");
	printf("2.未拥有账号-注册\n");
	printf("请选择需要进行的选项操作：\n");
	scanf("%d",&num3);
	switch(num3){
		case 1:
			user_logining();
			break;
		case 2:
			user_regist();
			break;
		default:
			printf("进行了无效操作，请重新输入选项：");
	}//登录界面
}

int main() {//链接账号数据库
	
	mysql_init(&userse);
	if (mysql_real_connect(&userse, "127.0.0.1", "root", "qwer751206", "user_login", 3306, NULL, 0)) {
		printf("coneect user_login_mysql successful\n");
		while(1){
			login();
		}
		mysql_close(&userse);
	}
	return 0;
}

int Strtoint(int *recive , char *saveencrypt, int length){
    int num = 0;
    for (int i = 0 ; i < length ; i ++) {
        if (saveencrypt[i] == ',' && i) {
            char temp[5];
            memset(temp,0,sizeof(temp));
            int pos = i - 1;
            while (pos > 0 && saveencrypt[pos] != ',')pos --;
            pos ++;
            int tt = 0;
            for (int j = pos ; j < i; j ++) {
                temp[tt++] = saveencrypt[j];
            }
            temp[tt] = '\0';
            //printf("%s\n",temp);
            *(recive + num) = Atoi(temp);
            num++;
        }
    }
    return num;
}


//以下是加密算法的函数
void ByteToBit(bool *Out,const char *In,int bits) {
    for(int i=0;i<bits;++i)
        Out[i]=(In[i>>3]>>(i&7))&1;
}
void BitToByte(char *Out,const bool *In,int bits) {
    memset(Out,0,bits>>3);
    for(int i=0;i<bits;++i)
        Out[i>>3] |= In[i]<<(i&7);
}

void Transform(bool *Out,bool *In,const char *Table,int len) {
    for(int i=0;i<len;++i)
        Tmp[i]=In[Table[i]-1];
    memcpy(Out,Tmp,len);
}

void Xor(bool *InA,const bool *InB,int len) {
    for(int i=0;i<len;++i)
    InA[i]^=InB[i];
}

void RotateL(bool *In,int len,int loop) {
    memcpy(Tmp,In,loop);
    memcpy(In,In+loop,len-loop);
    memcpy(In+len-loop,Tmp,loop);
}

void S_func(bool Out[32],const bool In[48]) {
    for(char i=0,j,k;i<8;++i,In+=6,Out+=4) {
        j  = (In[0]<<1)+In[5];
        k = (In[1]<<3)+(In[2]<<2)+(In[3]<<1)+In[4];
        ByteToBit(Out,&S_Box[i][j][k],4);
    }
}

void F_func(bool In[32],const bool Ki[48]) {
    static bool MR[48];
    Transform(MR,In,Extension_Table,48);
    Xor(MR,Ki,48);
    S_func(In,MR);
    Transform(In,In,P_Table,32);
}

void SetSubKey(PSubKey pSubKey,const char Key[8]) {
    static bool K[64],*KL=&K[0],*KR=&K[28];
    ByteToBit(K,Key,64);
    Transform(K,K,PCK_Table,56);
    for(int i=0;i<16;i++)
    {
        RotateL(KL,28,LOOP_Table[i]);
        RotateL(KR,28,LOOP_Table[i]);
        Transform((*pSubKey)[i],K,PCC_Table,48);
    }
}

void SetKey(const char  *Key,int len) {
    memset(deskey,0,16);
    memcpy(deskey,Key,len>16?16:len);
    SetSubKey(&SubKey[0],&deskey[0]);
    Is3DES=len>8?(SetSubKey(&SubKey[1],&deskey[8]),true):false;
}

void DES(char Out[8],char In[8],const PSubKey pSubKey,bool Type) {
    static bool M[64],tmp[32],*Li=&M[0],*Ri=&M[32];
    ByteToBit(M,In,64);
    Transform(M,M,IP_Table,64);
    if(Type==ENCRYPT)
    {
        for(int i=0;i<16;i++) {
            memcpy(tmp,Ri,32);
            F_func(Ri,(*pSubKey)[i]);
            Xor(Ri,Li,32);
            memcpy(Li,tmp,32);
        }
    }
    else {
        for(int i=15;i>=0;i--) {
            memcpy(tmp,Li,32);
            F_func(Li,(*pSubKey)[i]);
            Xor(Li,Ri,32);
            memcpy(Ri,tmp,32);
        }
    }
    Transform(M,M,IPR_Table,64);
    BitToByte(Out,M,64);
}

bool DES_Act(char *Out,char *In,long datalen,const char *Key,int keylen,bool Type) {
    //参数依次为 密文保存地址 明文地址 明文大小 密钥 密钥大小  加解密类型参数
    if(!(Out&&In&&Key && (datalen=datalen+7)&0xfffffff8))
        return false;
    SetKey(Key,keylen);
    if(!Is3DES) {
        for(long i=0,j=datalen>>3;i<j;++i,Out+=8,In+=8)
            DES(Out,In,&SubKey[0],Type);
    }
    else {
        for(long i = 0,j = datalen >> 3; i < j;++i,Out += 8,In += 8) {
            DES(Out,In,&SubKey[0],Type);
            DES(Out,Out,&SubKey[1],!Type);
            DES(Out,Out,&SubKey[0],Type);
        }
    }
    return true;
}