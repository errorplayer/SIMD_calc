/****************************************************************************
* author          errorplayer
* document name   dot_acc.cpp
* description     Simple float calculation(dot product and accumulation)  and Open for world
* email           opendev_xiaoze@aliyun.com
*****************************************************************************/
#include "time.h"
#include "stdafx.h"
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <iomanip>
#include <sys/timeb.h>
using namespace std;











//A : Single precision pointer
//B : another Single precision pointer
//cnt : Array length (equal)
//operation : dot product But no Accumulation
//the result exists in Array A
void sse_mul_float(float *A, float *B, int  cnt)
{

	//MOV EAX,1               ;request CPU feature flags
	//CPUID                   ;0Fh, 0A2h CPUID instruction
	//TEST EDX,4000000h       ;test bit 26 (SSE2)
	//JNZ >L18                ;SSE2 available

	int cnt1;
	int cnt2;
	int cnt3;

	//we process  the majority by using SSE instructions
	if (((int)A % 16) || ((int)B % 16))      //如果内存不对齐
	{

		cnt1 = cnt / 16;                         //该loop一轮处理16个float*float
		cnt2 = (cnt - (16 * cnt1)) / 4;          //该loop一轮处理4个float*float
		cnt3 = (cnt - (16 * cnt1) - (4 * cnt2)); //该loop一轮处理1个float*float

		_asm
		{

			mov edi, A;	                  //先将内存地址放入指针寄存器
			mov esi, B;	
			mov ecx, cnt1;	              //循环寄存器置值
			jecxz ZERO;                   //如果数据量不超过16个，则跳过L1

		L1:

						                  
			//xmm 寄存器有128bit
			//movups  XMM,XMM/m128
			//传128bit数据,不必对齐内存16字节.
			movups xmm0, [edi];
			movups xmm1, [edi + 16];		
			movups xmm2, [edi + 32];		
			movups xmm3, [edi + 48];	
			//为什么只载入4*4个float?  到上面看看这一轮需要处理多少数据

			movups xmm4, [esi];			
			movups xmm5, [esi + 16];		
			movups xmm6, [esi + 32];		
			movups xmm7, [esi + 48];		

			//mulps XMM,XMM/m128
			//寄存器按双字对齐, 
			//共4个单精度浮点数与目的寄存器里的4个对应相乘, 
			//结果送入目的寄存器, 内存变量必须对齐内存16字节.
			mulps xmm0, xmm4;			
			mulps xmm1, xmm5;			
			mulps xmm2, xmm6;			
			mulps xmm3, xmm7;			

			//（一个float占4字节，也就是32bit）
			//到这里，xmm0-3寄存器里都有了4个float的乘积结果
			//然后回载到相应内存
			movups[edi], xmm0;		
			movups[edi + 16], xmm1;		
			movups[edi + 32], xmm2;		
			movups[edi + 48], xmm3;		

			//记得给指针移位
			//64=16 * 4 
			//每一轮处理了16次float * float,每一个float占4字节
			//所以移位应该加64
			add edi, 64;
			add esi, 64;

			loop L1;							

		ZERO:
			mov ecx, cnt2;
			jecxz ZERO1;

		L2:

			movups xmm0, [edi];		   //对于4个float，一个xmm寄存器正好够用
			movups xmm1, [esi];		
			mulps xmm0, xmm1;		   //对应相乘，结果在xmm0
			movups[edi], xmm0;		   //由xmm0回载内存
			add edi, 16;               //指针移位
			add esi, 16;

			loop L2;

		ZERO1:

			mov ecx, cnt3;
			jecxz ZERO2;

			mov eax, 0;

		L3:								

			movd eax, [edi];            //对于单个float * float，无需sse指令
			imul eax, [esi];
			movd[edi], eax;
			add esi, 4;
			add edi, 4;

			loop L3;

		ZERO2:

			EMMS;                       //清空

		}

	}
	else
	{

		cnt1 = cnt / 28;                          //该loop一轮处理28个float*float
		cnt2 = (cnt - (28 * cnt1)) / 4;           //该loop一轮处理4个float*float
		cnt3 = (cnt - (28 * cnt1) - (4 * cnt2));  //该loop一轮处理1个float*float

		_asm
		{

			
			mov edi, A;	
			mov esi, B;	
			mov ecx, cnt1;	
			jecxz AZERO;

		AL1:

			//movaps XMM, XMM / m128 
			//把源存储器内容值送入目的寄存器, 当有m128时, 必须对齐内存16字节, 也就是内存地址低4位为0.
			movaps xmm0, [edi];		
			movaps xmm1, [edi + 16];		
			movaps xmm2, [edi + 32];		
			movaps xmm3, [edi + 48];		
			movaps xmm4, [edi + 64];		
			movaps xmm5, [edi + 80];		
			movaps xmm6, [edi + 96];
			//7*4=28，处理28个float*float

			mulps xmm0, [esi];			//对应点乘
			mulps xmm1, [esi + 16];		
			mulps xmm2, [esi + 32];		
			mulps xmm3, [esi + 48];		
			mulps xmm4, [esi + 64];		
			mulps xmm5, [esi + 80];		
			mulps xmm6, [esi + 96];		

			movaps[edi], xmm0;		    //回载
			movaps[edi + 16], xmm1;		
			movaps[edi + 32], xmm2;		
			movaps[edi + 48], xmm3;		
			movaps[edi + 64], xmm4;		
			movaps[edi + 80], xmm5;		
			movaps[edi + 96], xmm6;		

			add edi, 112;
			add esi, 112;

			loop AL1;							

		AZERO:
			mov ecx, cnt2;
			jecxz AZERO1;

		AL2:

			movaps xmm0, [edi];		
			mulps xmm0, [esi];		
			movaps[edi], xmm0;		
			add edi, 16;
			add esi, 16;

			loop AL2;

		AZERO1:

			mov ecx, cnt3;
			jecxz AZERO2;

			mov eax, 0;

		AL3:								

			movd eax, [edi];
			imul eax, [esi];
			movd[edi], eax;
			add esi, 4;
			add edi, 4;

			loop AL3;

		AZERO2:

			EMMS;

		}

	}
	
	//we process  remainder by using C++ lines
	int start;
	start = cnt - (cnt % 4);
	for (int i = start; i < cnt; i++)
	{
		A[i] *= B[i];
	}
	
}

//A : Single precision pointer 
//cnt : Array length 
//returns Accumulation of the array
float sse_acc_float(float *A, int cnt)
{

	float temp = 0;

	int cnt1;
	int cnt2;
	int cnt3;
	int select = 0;

	//we process  the majority by using SSE instructions
	if (((int)A % 16))	  //unaligned 如果这次调用，内存数据不对齐
	{
		select = 1;
		
		cnt1 = cnt / 24;
		cnt2 = (cnt - (24 * cnt1)) / 8;
		cnt3 = (cnt - (24 * cnt1) - (8 * cnt2));

		__asm
		{
			
			mov edi, A;			
			mov ecx, cnt1;		
			pxor xmm0, xmm0;	
			jecxz ZERO;

		L1:

			movups xmm1, [edi];
			movups xmm2, [edi + 16];
			movups xmm3, [edi + 32];
			movups xmm4, [edi + 48];
			movups xmm5, [edi + 64];
			movups xmm6, [edi + 80];

			//addps 对应相加
			//结果返回目的寄存器
			addps xmm1, xmm2;
			addps xmm3, xmm4;
			addps xmm5, xmm6;

			addps xmm1, xmm5;
			addps xmm0, xmm3;

			addps xmm0, xmm1;
			//至此，xmm0内4个float的和就是24个float的和

			add edi, 96;

			loop L1;						

		ZERO:


			movd ebx, xmm0;      //低4个字节(第一个float)传入ebx
			psrldq xmm0, 4;      //xmm0右移4字节
			movd eax, xmm0;      //右移后，低4个字节(第二个float)传入eax

			movd xmm1, eax;      //第一个float传入xmm1低32bit
			movd xmm2, ebx;      //第二个float传入xmm2低32bit
			addps xmm1, xmm2;    //两个寄存器内4个float对应相加
			movd eax, xmm1;      //只取我们要的低位float,传入eax
			movd xmm3, eax;      //第一和第二个float的和存在xmm3低32位
			psrldq xmm0, 4;      //又截掉一个float
			

			movd ebx, xmm0;      //第三个float进ebx
			psrldq xmm0, 4;      //截掉第三个float
			movd eax, xmm0;      //第四个float进eax 

			movd xmm1, eax;      
			movd xmm2, ebx;
			addps xmm1, xmm2;    //第三和第四个float的和存在xmm1低32位
			movd eax, xmm1;
			movd xmm4, eax;
			addps xmm3, xmm4;    //4个float的和存在xmm3低32位


			movd eax, xmm3;
			mov temp, eax;       //这部分求和存在temp地址区



			EMMS;							

		}
	}
	else	          // aligned   如果这次调用，内存数据对齐
	{
		select = 2;
		
		cnt1 = cnt / 56;                     
		cnt2 = (cnt - (56 * cnt1)) / 8;        
		cnt3 = (cnt - (56 * cnt1) - (8 * cnt2)); 

		__asm
		{
			
			mov edi, A;			
			mov ecx, cnt1;		
			pxor xmm0, xmm0;	
			jecxz ZZERO;

		LL1:

			movups xmm1, [edi];
			movups xmm2, [edi + 16];
			movups xmm3, [edi + 32];
			movups xmm4, [edi + 48];
			movups xmm5, [edi + 64];
			movups xmm6, [edi + 80];

			addps xmm1, xmm2;
			addps xmm3, xmm4;
			addps xmm5, xmm6;
			addps xmm1, xmm5;
			addps xmm0, xmm3;
			addps xmm0, xmm1;

			add edi, 96;

			movups xmm1, [edi];
			movups xmm2, [edi + 16];
			movups xmm3, [edi + 32];
			movups xmm4, [edi + 48];
			movups xmm5, [edi + 64];
			movups xmm6, [edi + 80];

			addps xmm1, xmm2;
			addps xmm3, xmm4;
			addps xmm5, xmm6;
			addps xmm1, xmm5;
			addps xmm0, xmm3;
			addps xmm0, xmm1;

			add edi, 96;

			movups xmm1, [edi];
			movups xmm2, [edi + 16];

			addps xmm1, xmm2;
			addps xmm0, xmm1;

			add edi, 32;

			loop LL1;						

		ZZERO:


			movd ebx, xmm0;
			psrldq xmm0, 4;
			movd eax, xmm0;

			movd xmm1, eax;
			movd xmm2, ebx;
			addps xmm1, xmm2;
			movd eax, xmm1;
			movd xmm3, eax;
			psrldq xmm0, 4;
			

			movd ebx, xmm0;
			psrldq xmm0, 4;
			movd eax, xmm0;

			movd xmm1, eax;
			movd xmm2, ebx;
			addps xmm1, xmm2;
			movd eax, xmm1;
			movd xmm4, eax;
			addps xmm3, xmm4;


			movd eax, xmm3;
			mov temp, eax;

			EMMS;							

		}
	}

	//we process  remainder by using C++ lines

	//上面的select记录本次调用sse_acc_float时，数据是否对齐内存
	//下面分情况把剩余的和累加
	int start;
	float c = 0.0f;
	if (select == 1)
	{
		
		start = cnt - (cnt % 24);
		for (int i = start; i < cnt; i++)
		{
			c += A[i];
		}
		
	}
	else
	{
		start = cnt - (cnt % 56);
		for (int i = start; i < cnt; i++)
		{
			c += A[i];
		}
		
	}

	//temp 是用指令计算 ，大部分数据的和
	//c    是用C++语句计算， 所有数据模24或者56剩余部分数据的和
	return(temp + c);
}


void TestFunc(int len)
{
	
	

	cout << "Array Length：" << len << endl;
	float *a, *b, *c, *d;
	
	a = (float *)malloc(len * sizeof(float));
	b = (float *)malloc(len * sizeof(float));
	c = (float *)malloc(len * sizeof(float));
	d = (float *)malloc(len * sizeof(float));


	//Initialize numbers
	srand(time(NULL));
	for (int i = 0; i < len; i++)
	{
		a[i] = (float)(rand() % 1) - 0.6231f;
		d[i] = a[i];
	}
	for (int i = 0; i < len; i++)
	{
		if (i % 5) b[i] = -1.000f;
		else if (i % 3) b[i] = 3.000f;
		else if (i % 7) b[i] = 1.000f;
		else b[i] = -3.000f;

	}
	float test = 0.000f;

	LARGE_INTEGER t1, t2, tc;
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);
	
	sse_mul_float(a, b, len);
	test = sse_acc_float(a, len);
	
	QueryPerformanceCounter(&t2);
	cout << "SIMD result = " << test;
	printf("   Use Time:%f s\n", (t2.QuadPart - t1.QuadPart)*1.0 / tc.QuadPart);
	
	
	
	

	float result = 0.000f;

	LARGE_INTEGER t3, t4, td;
	QueryPerformanceFrequency(&td);
	QueryPerformanceCounter(&t3);

	for (int i = 0; i < len; i++)
	{
		c[i] = d[i] * b[i];
	}
	for (int i = 0; i < len; i++)
	{
		result += c[i];
	}

	QueryPerformanceCounter(&t4);
	cout << "C++  result = " << result;
	printf("   Use Time:%f s\n", (t4.QuadPart - t3.QuadPart)*1.0 / td.QuadPart);
	
	
	

	cout << "error: " << result - test << endl;
	free(a);
	free(b);
	free(c);
	free(d);



	return;
}


void main()
{
	for (int i =900000; i < 1000000; i = i + 30000)
	{
		TestFunc(i);
		cout << endl;
		cout << endl;
	}
	return;
}



