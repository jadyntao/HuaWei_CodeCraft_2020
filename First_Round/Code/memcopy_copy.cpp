#include<bits/stdc++.h> 
#include <vector>  
#include <iostream>  
using namespace std;  
 
int main()  
{   
	unsigned int a[7];
	a[0]=1;a[1] = 2;a[2] = 3;a[3] = 4;a[4] = 2;a[5] = 3;a[6] = 4;
	unsigned int b[2][3][7];
	auto endtime=clock();
	auto begintime=clock();
	int test_num = 20000000;
	
	begintime=clock();
	for(int i=0;i<test_num;i++) 
		copy(a+1, a+8, b[0][0]);
	endtime=clock();
	cout<<"1: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
//	for (int i = 0; i < 7; i++) cout<<b[0][0][i]; cout<<endl; 
	
	begintime=clock();
	for(int i=0;i<test_num;i++) 
		memcpy(b[0][0],a+1,7*sizeof(int));
	endtime=clock();
	cout<<"2: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
//	for (int i = 0; i < 7; i++) cout<<b[0][0][i]; cout<<endl; 
	
	begintime=clock();
	for(int i=0;i<test_num;i++) 
		memmove(b[0][0],a+1,7*sizeof(int));
	endtime=clock();
	cout<<"3: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
//	for (int i = 0; i < 7; i++) cout<<b[0][0][i]; cout<<endl; 
	
//	unsigned int path[8] = {1,2,3,4,5,6,7,8};
//	unsigned int res[6][3000001][7];
//	int test_num = 20000000;
//	for(int i=0;i<test_num;i++) {
//		for(int j=1;j<6;j++){
//			path[depth+1] = v;
//			memcpy(res[depth][res_size[depth]++],path,(depth+1)*sizeof(int));
//		}
//
//	}

    
    
 	return 0;
} 
