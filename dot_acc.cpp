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
	if (((int)A % 16) || ((int)B % 16))      //����ڴ治����
	{

		cnt1 = cnt / 16;                         //��loopһ�ִ���16��float*float
		cnt2 = (cnt - (16 * cnt1)) / 4;          //��loopһ�ִ���4��float*float
		cnt3 = (cnt - (16 * cnt1) - (4 * cnt2)); //��loopһ�ִ���1��float*float

		_asm
		{

			mov edi, A;	                  //�Ƚ��ڴ��ַ����ָ��Ĵ���
			mov esi, B;	
			mov ecx, cnt1;	              //ѭ���Ĵ�����ֵ
			jecxz ZERO;                   //���������������16����������L1

		L1:

						                  
			//xmm �Ĵ�����128bit
			//movups  XMM,XMM/m128
			//��128bit����,���ض����ڴ�16�ֽ�.
			movups xmm0, [edi];
			movups xmm1, [edi + 16];		
			movups xmm2, [edi + 32];		
			movups xmm3, [edi + 48];	
			//Ϊʲôֻ����4*4��float?  �����濴����һ����Ҫ�����������

			movups xmm4, [esi];			
			movups xmm5, [esi + 16];		
			movups xmm6, [esi + 32];		
			movups xmm7, [esi + 48];		

			//mulps XMM,XMM/m128
			//�Ĵ�����˫�ֶ���, 
			//��4�������ȸ�������Ŀ�ļĴ������4����Ӧ���, 
			//�������Ŀ�ļĴ���, �ڴ������������ڴ�16�ֽ�.
			mulps xmm0, xmm4;			
			mulps xmm1, xmm5;			
			mulps xmm2, xmm6;			
			mulps xmm3, xmm7;			

			//��һ��floatռ4�ֽڣ�Ҳ����32bit��
			//�����xmm0-3�Ĵ����ﶼ����4��float�ĳ˻����
			//Ȼ����ص���Ӧ�ڴ�
			movups[edi], xmm0;		
			movups[edi + 16], xmm1;		
			movups[edi + 32], xmm2;		
			movups[edi + 48], xmm3;		

			//�ǵø�ָ����λ
			//64=16 * 4 
			//ÿһ�ִ�����16��float * float,ÿһ��floatռ4�ֽ�
			//������λӦ�ü�64
			add edi, 64;
			add esi, 64;

			loop L1;							

		ZERO:
			mov ecx, cnt2;
			jecxz ZERO1;

		L2:

			movups xmm0, [edi];		   //����4��float��һ��xmm�Ĵ������ù���
			movups xmm1, [esi];		
			mulps xmm0, xmm1;		   //��Ӧ��ˣ������xmm0
			movups[edi], xmm0;		   //��xmm0�����ڴ�
			add edi, 16;               //ָ����λ
			add esi, 16;

			loop L2;

		ZERO1:

			mov ecx, cnt3;
			jecxz ZERO2;

			mov eax, 0;

		L3:								

			movd eax, [edi];            //���ڵ���float * float������sseָ��
			imul eax, [esi];
			movd[edi], eax;
			add esi, 4;
			add edi, 4;

			loop L3;

		ZERO2:

			EMMS;                       //���

		}

	}
	else
	{

		cnt1 = cnt / 28;                          //��loopһ�ִ���28��float*float
		cnt2 = (cnt - (28 * cnt1)) / 4;           //��loopһ�ִ���4��float*float
		cnt3 = (cnt - (28 * cnt1) - (4 * cnt2));  //��loopһ�ִ���1��float*float

		_asm
		{

			
			mov edi, A;	
			mov esi, B;	
			mov ecx, cnt1;	
			jecxz AZERO;

		AL1:

			//movaps XMM, XMM / m128 
			//��Դ�洢������ֵ����Ŀ�ļĴ���, ����m128ʱ, ��������ڴ�16�ֽ�, Ҳ�����ڴ��ַ��4λΪ0.
			movaps xmm0, [edi];		
			movaps xmm1, [edi + 16];		
			movaps xmm2, [edi + 32];		
			movaps xmm3, [edi + 48];		
			movaps xmm4, [edi + 64];		
			movaps xmm5, [edi + 80];		
			movaps xmm6, [edi + 96];
			//7*4=28������28��float*float

			mulps xmm0, [esi];			//��Ӧ���
			mulps xmm1, [esi + 16];		
			mulps xmm2, [esi + 32];		
			mulps xmm3, [esi + 48];		
			mulps xmm4, [esi + 64];		
			mulps xmm5, [esi + 80];		
			mulps xmm6, [esi + 96];		

			movaps[edi], xmm0;		    //����
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
	if (((int)A % 16))	  //unaligned �����ε��ã��ڴ����ݲ�����
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

			//addps ��Ӧ���
			//�������Ŀ�ļĴ���
			addps xmm1, xmm2;
			addps xmm3, xmm4;
			addps xmm5, xmm6;

			addps xmm1, xmm5;
			addps xmm0, xmm3;

			addps xmm0, xmm1;
			//���ˣ�xmm0��4��float�ĺ;���24��float�ĺ�

			add edi, 96;

			loop L1;						

		ZERO:


			movd ebx, xmm0;      //��4���ֽ�(��һ��float)����ebx
			psrldq xmm0, 4;      //xmm0����4�ֽ�
			movd eax, xmm0;      //���ƺ󣬵�4���ֽ�(�ڶ���float)����eax

			movd xmm1, eax;      //��һ��float����xmm1��32bit
			movd xmm2, ebx;      //�ڶ���float����xmm2��32bit
			addps xmm1, xmm2;    //�����Ĵ�����4��float��Ӧ���
			movd eax, xmm1;      //ֻȡ����Ҫ�ĵ�λfloat,����eax
			movd xmm3, eax;      //��һ�͵ڶ���float�ĺʹ���xmm3��32λ
			psrldq xmm0, 4;      //�ֽص�һ��float
			

			movd ebx, xmm0;      //������float��ebx
			psrldq xmm0, 4;      //�ص�������float
			movd eax, xmm0;      //���ĸ�float��eax 

			movd xmm1, eax;      
			movd xmm2, ebx;
			addps xmm1, xmm2;    //�����͵��ĸ�float�ĺʹ���xmm1��32λ
			movd eax, xmm1;
			movd xmm4, eax;
			addps xmm3, xmm4;    //4��float�ĺʹ���xmm3��32λ


			movd eax, xmm3;
			mov temp, eax;       //�ⲿ����ʹ���temp��ַ��



			EMMS;							

		}
	}
	else	          // aligned   �����ε��ã��ڴ����ݶ���
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

	//�����select��¼���ε���sse_acc_floatʱ�������Ƿ�����ڴ�
	//����������ʣ��ĺ��ۼ�
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

	//temp ����ָ����� ���󲿷����ݵĺ�
	//c    ����C++�����㣬 ��������ģ24����56ʣ�ಿ�����ݵĺ�
	return(temp + c);
}


void TestFunc(int len)
{
	
	

	cout << "Array Length��" << len << endl;
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



