#include<bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
using namespace std;
#define maxn 280000
#define MAXS 10000000
auto using_time=0.0;
auto endtime=clock();
auto begintime=clock();
char buf[MAXS];
int id_size=-1;
unsigned int Graph[maxn][50];
unsigned int Graph2[maxn][50];
unsigned int size1[maxn];
unsigned int size2[maxn];
unsigned int res[5][3000001][7];
int res_size[8];
bool visited[maxn];
unsigned int three_border[maxn];
unsigned int path[8];
string str[maxn];
unsigned int lastone[maxn];
char* mmapread(string path,long long &len)
{
	long long fd=open(path.c_str(),O_RDONLY);
	if(fd==-1){
        fd=open(path.c_str(),O_RDWR|O_CREAT,0666);
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
			str[i]=to_string(i)+",";
			sort(Graph[i],Graph[i]+size1[i]);
		}


	}
}
void mmapwrite(string path,const char* inp,long long length)
{
	int fd=open(path.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,length*sizeof(char));
	long long offset=0,inp_len=0;
	char*p=(char*)mmap(NULL,length*sizeof(char),PROT_WRITE|PROT_READ,MAP_SHARED,fd,offset);
	memcpy(p,inp,length*sizeof(char));
	if(-1==msync(p,length*sizeof(char),MS_SYNC))
	{
		cout<<offset<<"error"<<endl;
	}
	if(-1==munmap(p,length*sizeof(char)))
	{
		cout<<offset<<endl;
	}
	close(fd);
}
void save_output(string &output_file){
	int total_res_size = 0;
	for(int i=0;i<5;++i){
		total_res_size += res_size[i];
	}
	string out=to_string(total_res_size)+"\n";
	for(int index=0;index<5;++index){
		int i_size = res_size[index];
		for(int k=0;k<i_size;k++){
			auto &i = res[index][k];
	        for(int j=0;j<index+3;j++){
	        	out += str[i[j]];
			}
			*(out.end()-1) = '\n';
	    }
	}
	mmapwrite(output_file,out.c_str(),out.size());
}

void dfs(unsigned int head,unsigned int cur,int depth){
    visited[cur]=true;
    path[depth] = cur;
    auto it=lower_bound(Graph[cur],Graph[cur]+size1[cur],head);
	for(;it!=Graph[cur]+size1[cur];++it){
    	unsigned int &v=*it;
		if(size1[v]==0||visited[v]||v<=head){
			continue;
		}

    	if( lastone[v] == head+1 && depth>1){
			memcpy(res[depth-2][res_size[depth-2]],path+1,depth*sizeof(int));
			res[depth-2][res_size[depth-2]++][depth] = v;
		}
        if(depth<6){
			if(depth>3 && three_border[v]!=head+1)
				continue;
            dfs(head,v,depth+1);
        }
    }
    visited[cur]=false;
}

void dfs_cut_backward(unsigned int head,unsigned int cur,int depth){
	visited[cur] = true;
    for(int i=0;i<size2[cur];i++){
    	unsigned int &v=Graph2[cur][i];
		if(v>head && !visited[v] && depth<4){
			if(depth==1){
				lastone[v]=head+1;
			}
			three_border[v] = head+1;
			dfs_cut_backward(head,v,depth+1);
		}

	}
	visited[cur] = false;
}

void dfs_method(){
	for(int i=0;i<=id_size;i++){
		if(size1[i]!=0){
			dfs_cut_backward(i,i,1);
			dfs(i,i,1);
		}
	}
}
int main(){
	string InputFileName = "test_data1004812.txt";
	string OutputFileName = "output1004812.txt";
	string ResultFileName = "result1004812.txt";
		endtime = clock();
		cout<<"1: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
		
		begintime = clock();		
	get_input(InputFileName);
		endtime = clock();
		cout<<"2: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
		
		begintime = clock();
	dfs_method();
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
