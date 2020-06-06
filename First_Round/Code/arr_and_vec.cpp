#include<iostream>
#include<ctime>
#include<vector>
using namespace std;
int main() {
	int const ACount = 100000;
	int Count = ACount;
	double a[ACount];
	double b[ACount];
	vector<double>va(Count);
	vector<double>vb(Count);
 
	//开始计时
	for(int t=0;t<10;t++){
		clock_t bgn = clock();
		for (int i = 0;i < ACount;i++)
		{
			for (int j = 0;j < 100;j++)
			{
				a[i] = 2 * i;
				b[i] = 3 * i;
				b[i] = a[i] * b[i];
			}
		}
		double	ct = (double)(clock() - bgn);
		cout << "数列用时长为" << ct << endl;
	}
	//开始计时
	for(int t=0;t<10;t++){
		clock_t bgn2 = clock();
		for (int i = 0;i < ACount;i++)
		{
			for (int j = 0;j < 100;j++)
			{
				va[i] = 2 * i;
				vb[i] = 3 * i;
				vb[i] = va[i] * vb[i];
			}
		}
		double	ct2 = (double)(clock() - bgn2);
		cout << "向量用时长为" << ct2 << endl;
	} 
	

}

