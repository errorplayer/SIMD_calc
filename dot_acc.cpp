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
	if (((int)A % 16) || ((int)B % 16)) 
	{

		cnt1 = cnt / 16;
		cnt2 = (cnt - (16 * cnt1)) / 4;
		cnt3 = (cnt - (16 * cnt1) - (4 * cnt2));

		_asm
		{

			
			mov edi, A;	
			mov esi, B;	
			mov ecx, cnt1;	
			jecxz ZERO;

		L1:

			movups xmm0, [edi];			
			movups xmm1, [edi + 16];		
			movups xmm2, [edi + 32];		
			movups xmm3, [edi + 48];		

			movups xmm4, [esi];			
			movups xmm5, [esi + 16];		
			movups xmm6, [esi + 32];		
			movups xmm7, [esi + 48];		

			mulps xmm0, xmm4;			
			mulps xmm1, xmm5;			
			mulps xmm2, xmm6;			
			mulps xmm3, xmm7;			

			movups[edi], xmm0;		
			movups[edi + 16], xmm1;		
			movups[edi + 32], xmm2;		
			movups[edi + 48], xmm3;		

			add edi, 64;
			add esi, 64;

			loop L1;							

		ZERO:
			mov ecx, cnt2;
			jecxz ZERO1;

		L2:

			movups xmm0, [edi];		
			movups xmm1, [esi];		
			mulps xmm0, xmm1;		
			movups[edi], xmm0;		
			add edi, 16;
			add esi, 16;

			loop L2;

		ZERO1:

			mov ecx, cnt3;
			jecxz ZERO2;

			mov eax, 0;

		L3:								

			movd eax, [edi];
			imul eax, [esi];
			movd[edi], eax;
			add esi, 4;
			add edi, 4;

			loop L3;

		ZERO2:

			EMMS;

		}

	}
	else
	{

		cnt1 = cnt / 28;
		cnt2 = (cnt - (28 * cnt1)) / 4;
		cnt3 = (cnt - (28 * cnt1) - (4 * cnt2));

		_asm
		{

			
			mov edi, A;	
			mov esi, B;	
			mov ecx, cnt1;	
			jecxz AZERO;

		AL1:

			movaps xmm0, [edi];		
			movaps xmm1, [edi + 16];		
			movaps xmm2, [edi + 32];		
			movaps xmm3, [edi + 48];		
			movaps xmm4, [edi + 64];		
			movaps xmm5, [edi + 80];		
			movaps xmm6, [edi + 96];		

			mulps xmm0, [esi];			
			mulps xmm1, [esi + 16];		
			mulps xmm2, [esi + 32];		
			mulps xmm3, [esi + 48];		
			mulps xmm4, [esi + 64];		
			mulps xmm5, [esi + 80];		
			mulps xmm6, [esi + 96];		

			movaps[edi], xmm0;		
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
	if (((int)A % 16))	  //unaligned 
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

			addps xmm1, xmm2;
			addps xmm3, xmm4;
			addps xmm5, xmm6;

			addps xmm1, xmm5;
			addps xmm0, xmm3;

			addps xmm0, xmm1;

			add edi, 96;

			loop L1;						

		ZERO:


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
	else	          // aligned
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
	return(temp + c);
}




void TestFunc(int len)
{
	
	

	cout << "Array Length£º" << len << endl;
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



