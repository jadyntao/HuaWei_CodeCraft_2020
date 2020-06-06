#include<bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <omp.h>  
using namespace std;
#define maxn 280000
#define MAXS 10000000
#define outMAXS 1200000000
#define Tnum 3
auto using_time=0.0;
auto endtime=clock();
auto begintime=clock();
unsigned int inputs[MAXS];
unsigned int inputs_size=0;
char outbuf[outMAXS];
int id_size=-1;
unsigned int Graph[maxn][50];
unsigned int Graph2[maxn][50];
unsigned int size1[maxn];
unsigned int size2[maxn];
unsigned int res[Tnum][6][3000001][7];
int res_size[Tnum][8];
bool visited[Tnum][maxn];
unsigned int three_border[Tnum][maxn];
unsigned int path[Tnum][8];
string str[maxn];
unsigned int lastone[Tnum][maxn];
string myto_string(int value) {
 
	static const char digits[19] = {
		'9','8','7','6','5','4','3','2','1','0',
		'1','2','3','4','5','6','7','8','9'
	};
	static const char* zero = digits + 9;//zero->'0'
 
	char localbuf[24];      //不考虑线程安全的情况时，可以改成静态变量
	int i = value;
	char *p = localbuf + 24;
	*--p = '\0' ;
	do {
		int lsd = i % 10;
		i /= 10;
		*--p = zero[lsd];
	} while (i != 0);
	if (value <0)
		*--p = '-';
	return string(p);
}
char* mmapread(string filepath,long long &len)
{
	long long fd=open(filepath.c_str(),O_RDONLY);
	if(fd==-1){
        fd=open(filepath.c_str(),O_RDWR|O_CREAT,0666);
        if(fd==-1){
            exit(-1);
        }
    }
	struct stat st;
	long long r=fstat(fd,&st);
	len=st.st_size;
    char* buf=(char*)mmap(NULL,len,PROT_READ,MAP_PRIVATE,fd,0);
	return buf;
}

void get_input(string &input_file){
	long long len = 0;
	char *buf = mmapread(input_file,len);
	int in_num=0,out_num=0;
	for(char *p = buf; *p && p-buf<len; p++) {
		while(*p && *p != ',')
			in_num = in_num*10 + (*(p++) - '0');
		p++;
		id_size=max(in_num,id_size);
		while(*p && *p != ',')
			out_num = out_num*10 + (*(p++) - '0');
	
		while(*p && *p != '\n')
			p++;
			
		Graph[in_num][size1[in_num]++]=out_num;
		Graph2[out_num][size2[out_num]++]=in_num;
		in_num = 0;
		out_num = 0;
	}
	for (int i = 0; i <= id_size; i++){
		if(size1[i]){
			str[i]=myto_string(i)+",";
			sort(Graph[i],Graph[i]+size1[i]);
		}
	}
	id_size++;
}
void mmapwrite(string filepath,const char* ans,long long length)
{
	int fd=open(filepath.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,length);
	long long offset=0,inp_len=0;
	char*p=(char*)mmap(NULL,length,PROT_WRITE|PROT_READ,MAP_SHARED,fd,offset);
	memcpy(p,ans,length);
	msync(p,length,MS_SYNC);
	munmap(p,length);
	close(fd);
}
void save_output(string &output_file){
	int total_res_size = 0;

	for(int i=1;i<6;++i){
		for(int tid=0;tid<Tnum;tid++){
			total_res_size += res_size[tid][i];
		}
	}
	cout<<total_res_size<<endl;
	string out=myto_string(total_res_size)+"\n";
	memcpy(outbuf,out.c_str(),sizeof(out));
	int offset=out.size();
	for(int index=1;index<6;++index){
		for(int tid=0;tid<Tnum;tid++){
			int i_size = res_size[tid][index];
			for(int k=0;k<i_size;k++){
				unsigned int *i = res[tid][index][k];
				for(int j=0;j<index+2;j++){
					memcpy(outbuf+offset,str[i[j]].c_str(),sizeof(str[i[j]]));
					offset+=str[i[j]].size();
				}
				outbuf[offset-1]='\n';
			}
		}
	}
	mmapwrite(output_file,outbuf,offset);
}

void dfs(unsigned int head,unsigned int cur,int depth,int tid){
    visited[tid][cur]=true;
    path[tid][depth] = cur;
    unsigned int *it=lower_bound(Graph[cur],Graph[cur]+size1[cur],head);
	for(;it!=Graph[cur]+size1[cur];++it){
    	unsigned int &v=*it;
		if(visited[tid][v]||size1[v]==0){
			continue;
		}

    	if( lastone[tid][v] == head+1 && depth){
			path[tid][depth+1]=v;
			memcpy(res[tid][depth][res_size[tid][depth]++],path[tid],(depth+2)*sizeof(int));
		}
        if(depth<5){
			if(depth>2 && three_border[tid][v]!=head+1)
				continue;
            dfs(head,v,depth+1,tid);
        }
    }
    visited[tid][cur]=false;
}

void dfs_cut_backward(unsigned int head,unsigned int cur,int depth,int tid){
	visited[tid][cur] = true;
    for(int i=0;i<size2[cur];i++){
    	unsigned int &v=Graph2[cur][i];
		if(v>head && !visited[tid][v] && depth<3){
			if(!depth){
				lastone[tid][v]=head+1;
			}
			three_border[tid][v] = head+1;
			dfs_cut_backward(head,v,depth+1,tid);
		}

	}
	visited[tid][cur] = false;
}
double split[]={0,0.2,0.7,1};
void dfs_method(int tid){
	for(unsigned int i=int(id_size*split[tid]);i<int(id_size*split[tid+1]);i++){
		if(size1[i]){
			dfs_cut_backward(i,i,0,tid);
			dfs(i,i,0,tid);
		}
	}
}

int main(){
	// string InputFileName = "test_data1004812.txt";
	// string OuputFileName = "output.txt";
	// auto begintime=clock();
	// get_input(InputFileName);
	// dfs_method();
	// save_output(OuputFileName);
	// auto endtime=clock();
	// auto using_time = (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	// cout<<"used times : "<<using_time<<endl;
	// exit(0);
	// return 0;
	string InputFileName = "test_data56.txt";
	string OutputFileName = "our_output.txt";
	string ResultFileName = "our_result.txt";
		endtime = clock();
		cout<<"1: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
		
		begintime = clock();		
	get_input(InputFileName);
		endtime = clock();
		cout<<"2: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
		
		begintime = clock();
	thread Td[Tnum];
	for(unsigned int i=0;i<Tnum;i++){
		Td[i] = thread(dfs_method,i);
	}
	for(unsigned int i=0;i<Tnum;i++){
		Td[i].join();
	}
		endtime = clock();
		cout<<"3: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
		
		begintime = clock();		
	save_output(OutputFileName);
		endtime = clock();
		cout<<"4: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
//	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;
//	cout<<"cnt :"<<cnt<<endl;
	cout<<"total using times : "<<using_time<<endl;
	exit(0);
	return 0;
}
