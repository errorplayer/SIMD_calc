/****************************************************************************
* author          errorplayer
* document name   dot_acc.cpp
* description     Simple float calculation(dot product and accumulation)  and Open to the  world
* email           opendev_xiaoze@aliyun.com
*****************************************************************************/
#include "time.h"

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







void sse_mul_int(int *A, int *B, int  cnt)
{



	int cnt1;
	int cnt2;
	int cnt3;

	//we process  the majority by using SSE instructions
	if (1)//(((int)A % 16) || ((int)B % 16))     
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
	

}

int sse_acc_int(int *A, int cnt)
{

	int temp = 0;

	int cnt1;
	int cnt2;
	int cnt3;
	int select = 0;

	//we process  the majority by using SSE instructions
	if (((int)A % 16))
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
	else
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
	int c = 0;
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




//A : Single precision pointer
//B : another Single precision pointer
//cnt : Array length (equal)
//operation : dot product But no Accumulation
//the result exists in Array A
void sse_mul_float(float *A, float *B, int  cnt)
{
	int cnt1;
	int cnt2;
	int cnt3;

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
	if (1)//(((int)A % 16))	  
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
	else	         
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
	
	

	cout << "Array Length   :   " << len << endl;
	float *a, *b, *c, *d;
	
	a = (float *)malloc(len * sizeof(float));
	b = (float *)malloc(len * sizeof(float));
	c = (float *)malloc(len * sizeof(float));
	d = (float *)malloc(len * sizeof(float));

	if (a == NULL || b == NULL || c == NULL || d == NULL) {
		cout << "Memory Error! " <<endl;
		return;
	}


	//Initialize numbers
	srand(time(NULL));
	for (int i = 0; i < len; i++)
	{
		a[i] =floor( (rand() % 4));
		if (a[i] == 0) d[i] = -1.0f;
		else if (a[i] == 1) d[i] = 3.0f;
		else if (a[i] == 2) d[i] = 1.0f;
		else d[i] = -3.0f;
		a[i] = d[i];
		//a[i] = a[i] + 0.221;
		//d[i] = d[i] + 0.221;
	}
	for (int i = 0; i < len; i++)
	{
		b[i] = floor((rand() % 4));
		if (b[i] == 0) b[i] = -1.0f;
		else if (b[i] == 1) b[i] = 3.0f;
		else if (b[i] == 2) b[i] = 1.0f;
		else b[i] = -3.0f;

		//b[i] = b[i] - 0.001;
		

	}
	float test = 0.0f;

	
	
	cout << endl;


	
	
	LARGE_INTEGER t1, t2, tc;
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);
	
	sse_mul_float(a, b, len);
	test = sse_acc_float(a, len);
	
	QueryPerformanceCounter(&t2);
	cout << "SIMD result1 = " << test;
	
	
	printf("   Use Time:%f s\n", (t2.QuadPart - t1.QuadPart)*1.0 / tc.QuadPart);


	for (int i = 0; i < len; i++)
	{
		
		a[i] = d[i];
		
	}
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);

	sse_mul_float(a, b, len);
	test = sse_acc_float(a, len);

	QueryPerformanceCounter(&t2);
	cout << "SIMD result2 = " << test;


	printf("   Use Time:%f s\n", (t2.QuadPart - t1.QuadPart)*1.0 / tc.QuadPart);
	
	
	
	
	
	
	
	

	float result1 = 0.000f;

	

	LARGE_INTEGER t3, t4, td;
	QueryPerformanceFrequency(&td);
	QueryPerformanceCounter(&t3);

	for (int i = 0; i < len; i++)
	{
		c[i] = d[i] * b[i];
	}
	for (int i = 0; i < len; i++)
	{
		result1 += c[i];
	}

	QueryPerformanceCounter(&t4);
	
	cout << "C++  result1 = " << result1;
	
	
	printf("   Use Time:%f s\n", (t4.QuadPart - t3.QuadPart)*1.0 / td.QuadPart);



	float result2 = 0.000f;



	LARGE_INTEGER t5, t6, te;
	QueryPerformanceFrequency(&te);
	QueryPerformanceCounter(&t5);

	for (int i = 0; i < len; i++)
	{
		c[i] = d[i] * b[i];
	}
	for (int i = 0; i < len; i++)
	{
		result2 += c[i];
	}

	QueryPerformanceCounter(&t6);

	cout << "C++  result2 = " << result2;


	printf("   Use Time:%f s\n", (t6.QuadPart - t5.QuadPart)*1.0 / te.QuadPart);
	
	
	
	

	//cout << "error: " << result - test << endl;
	free(a);
	free(b);
	free(c);
	free(d);



	return;
}



void main()
{
	//C/A码的周期为1ms，一周期长度为1023个码片。
	//TestFunc(1023*1000); 
	cout << endl;
	cout << endl;

	//12个通道的接收机，每个通道3个相关器，每个复数型数据有2个数值。
	TestFunc(12*3*2*1023*1000);  
	cout << endl;
	cout << endl;

	system("pause");
	return;
}


