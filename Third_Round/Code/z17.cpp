#include<bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <omp.h>
#include <arm_neon.h>
#include <stdarg.h>
#include <stddef.h>

#include <stdint.h>
using namespace std;
#define maxn 5000001
// #define outMAXS 400000000
#define Tnum 8
pthread_mutex_t mt;
struct Timer
{
    /*
        计时函数，可以用于多线程内精准计时
    */
    timeval tic, toc;

    Timer()
    {
        gettimeofday(&tic,NULL);
    }

    void stop(const char* name)
    {
        gettimeofday(&toc,NULL);
        printf("%s: %f(s)\n", name, float(toc.tv_sec-tic.tv_sec) + float(toc.tv_usec-tic.tv_usec)/1000000);
    }
};
Timer timer0;
struct node
{
    long long w;
    unsigned int now;
    inline bool operator <(const node &x)const
    //重载运算符把最小的元素放在堆顶（大根堆）
    {
        return w>x.w;//这里注意符号要为'>'
    }
};

vector<pair<unsigned int,unsigned int>> Graph[maxn];
unsigned int arrive_num[maxn];
unsigned int indegree[maxn];
unsigned cut_num[maxn];
unsigned firstid[maxn];
string str[maxn];

vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<unsigned int> inputs_menoy;
vector<unsigned int> id_arr;
unordered_map<unsigned int, unsigned int> ids;
unsigned int node_num=0,record_num=0,len_double,len_l;
// const long long INF = 0x3f;
long long dis[Tnum][maxn];
// vector<vector<int> >A[Tnum]; 
double A[Tnum][maxn];
priority_queue<node>q[Tnum];

inline string myto_string(int value) {

	static const char digits[19] = {
		'9','8','7','6','5','4','3','2','1','0',
		'1','2','3','4','5','6','7','8','9'
	};
	static const char* zero = digits + 9;

	char localbuf[32];
	int i = value;
	char *p = localbuf + 32;
	*--p = '\0' ;
	do {
		int lsd = i % 10;
		i /= 10;
		*--p = zero[lsd];
	} while (i != 0);
	return string(p);
}
inline char* mmapread(string &filepath,long long &len)
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
inline void get_input(string &input_file){
	long long len = 0;
	char *buf = mmapread(input_file,len);
	unsigned int in_num=0,out_num=0,menoy=0;
   
	for(char *p = buf; *p && p-buf<len; p++) {
		while(*p && (*p == '\n'||*p=='\r'))
			p++;		
		while(*p && *p != ',')
			in_num = in_num*10 + (*(p++) - '0');
		p++;

		while(*p && *p != ',')
			out_num = out_num*10 + (*(p++) - '0');
		p++;
		
		while(*p && *p != '\n'&&*p!='\r'){
			menoy = menoy*10 + (*(p++) - '0');
		}
        if(menoy==0){
            in_num = 0;
		    out_num = 0;
            continue;
        }
		inputs_in_num.emplace_back(in_num);
		inputs_out_num.emplace_back(out_num);
		inputs_menoy.emplace_back(menoy);
		in_num = 0;
		out_num = 0;
		menoy = 0;
		
	}

	//删重
    id_arr.reserve(inputs_in_num.size()+inputs_out_num.size()); 
    id_arr.assign(inputs_in_num.begin(),inputs_in_num.end());
    id_arr.insert(id_arr.end(),inputs_out_num.begin(),inputs_out_num.end());

	sort(id_arr.begin(), id_arr.end());
	id_arr.erase(unique(id_arr.begin(), id_arr.end()), id_arr.end());
    

	node_num = 1;
	for (unsigned int &x : id_arr) {
		str[node_num] = myto_string(x)+",";
		ids[x] = node_num++;
	}
	// cout<<"node_num"<<node_num<<endl;
	record_num = inputs_in_num.size();
	// for(int i=record_num-1;i>=0;i--){
	for(unsigned int i=0;i<record_num;i++){
		unsigned int ids_in_num = ids[inputs_in_num[i]];
		unsigned int ids_out_num = ids[inputs_out_num[i]];
		unsigned int menoy = inputs_menoy[i];
		// if(ids_out_num==0) continue;
		Graph[ids_in_num].push_back({ids_out_num,menoy});
		indegree[ids_out_num]++;
	}
    len_double=node_num*sizeof(double);
	len_l=node_num*sizeof(long long);
}
bool cmpsum(pair<double,unsigned int>p1,pair<double,unsigned int>p2){
	if(abs(p1.first-p2.first)<=0.0001)return p1.second<p2.second;
	return p1.first>p2.first;
}
pair<double,unsigned int> sum[Tnum][maxn];
inline void save_output(string &output_file){
	for(int i=1;i<node_num;i++){
		sum[0][i].second=i;
		for(int tid=1;tid<Tnum;tid++){
			sum[0][i].first+=sum[tid][i].first;
		}
		// sum[0][i].first = floor(sum[0][i].first * 1000.000f + 0.5) / 1000.000f;
	}
	sort(sum[0]+1,sum[0]+node_num,cmpsum);

	string mys;
	
	for(int i=1;i<101;i++){
        char buf[30];
        sprintf(buf,  "%.3f", sum[0][i].first);
		mys+=str[sum[0][i].second]+buf+'\n';
	}
	
	long long total_offset = mys.size();

	int fd=open(output_file.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,total_offset);
	char*p=(char*)mmap(NULL,total_offset,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	memcpy(p,mys.c_str(),total_offset);

	close(fd);

}

vector<unsigned int> prv[Tnum][maxn];
// queue<int>quecnt[Tnum];
double delta[Tnum][maxn];
// bool viscnt[Tnum][maxn];
unsigned int Stk[Tnum][maxn];
unsigned int sizeStk[Tnum];

void dji(unsigned int s,int tid)
{
	memset(dis[tid],0x3f,len_l);
	memset(delta[tid],0,len_double);
	dis[tid][s]=0;
	A[tid][s]=1;
	prv[tid][s].clear();
    q[tid].push((node){0,s});
    while(!q[tid].empty())
    {
        node x=q[tid].top();
        q[tid].pop();
        unsigned int u=x.now;
        if(dis[tid][u]<x.w) continue; 
        Stk[tid][sizeStk[tid]++] = u;
		int len=Graph[u].size();
		for(int i=0;i<len;i++)
        {
            unsigned int v=Graph[u][i].first;
            long long newDist=dis[tid][u]+Graph[u][i].second;
            if(dis[tid][v]>newDist)
            {
                dis[tid][v]=newDist;
                prv[tid][v].clear();
                A[tid][v]=0;
                q[tid].push((node){dis[tid][v],v});
            }
            if(newDist==dis[tid][v]){
                prv[tid][v].emplace_back(u);
                A[tid][v]+=A[tid][u];
            }
        }
    }
	int cutn = cut_num[s];
	if(cutn==0)
		cutn=1;
    while(sizeStk[tid]){
        unsigned int t=Stk[tid][--sizeStk[tid]];
        double coeff=(1.0+delta[tid][t])/A[tid][t];
        for(unsigned int v:prv[tid][t]){
            delta[tid][v] += A[tid][v]*coeff;
        }
		sum[tid][t].first += cutn*delta[tid][t];
        
    }
	sum[tid][s].first -= cutn*delta[tid][s];
}


unsigned int curhead=1;
static void* dji_method(void* c){
	int tid =*(int*)c;
	
	while(1){
		pthread_mutex_lock(&mt);
		unsigned int cur=curhead;
		
		++curhead;
		pthread_mutex_unlock(&mt);
		if(cur>=node_num)break;
		if(Graph[cur].size()){
            
			dji(cur,tid);
		}
		// if(Graph[cur].size()){
		// 	unsigned int label=cur+1;
		// 	dfs_cut_backward(cur,tid);
		// 	dfs(cur,cur,0,tid,label);
		// }
	}

	// curhead=1;
	// while(1){
	// 	pthread_mutex_lock(&mt);
	// 	unsigned int cur=curhead;
	// 	++curhead;
	// 	pthread_mutex_unlock(&mt);
	// 	if(cur>=node_num)break;
	// 	if(Graph[cur].size()){
	// 		cal(cur,tid);
	// 	}
	// }
	

}
bool compare_files(string &file1, string &file2){
	//?????1
	ifstream file1_stream;
    file1_stream.open(file1);
    assert(file1_stream.is_open());
	//?????2
	ifstream file2_stream;
    file2_stream.open(file2);
    assert(file2_stream.is_open());

    char c1,c2;
    bool same_flg = true;
    int t = 0;
	int i = 0;
    while (!file1_stream.eof()&&!file2_stream.eof())
    {
        file1_stream>>c1;
        file2_stream>>c2;
		i++;
        if(c1!=c2){
        	same_flg = false;
        	break;
		}
		// if(same_flg == false && t++<20){
		// 	cout<<i<<endl;
		// 		cout<<c1<<" "<<c2<<endl;
		// }

    }
    if(file1_stream.eof()!=file2_stream.eof())
    	same_flg = false;

    file1_stream.close();
    file2_stream.close();
    return same_flg;
}

void Graph_cut() {
	for(int i=1; i<node_num; i++) {
		if(indegree[i]==0 && Graph[i].size()==1) {
			int v = Graph[i][0].first;
			if(firstid[v]==0) {
				firstid[v] = i;
				cut_num[i]++;
			}
			else {
				Graph[i].clear();
				cut_num[firstid[v]]++;
			}
		}
	}
}

int main(){
	// string InputFileName = "/data/test_data.txt";
	// string OutputFileName = "/projects/student/result.txt";
	
	string num = "juesai1";
	string ResultFileName = num + "/result.txt";
	string InputFileName = num + "/test_data.txt";
	string OutputFileName = num + "/z_output.txt";
	timer0.stop("init");
	Timer timer1;
	get_input(InputFileName);
    
	Graph_cut();

	timer1.stop("get and cut");

	Timer timer2;
	unsigned int cs[Tnum];
	pthread_t Td[Tnum];
	
	for(unsigned int i=0;i<Tnum;i++)
    {
        cs[i]=i;
		pthread_create(&Td[i],NULL,dji_method,(void*)(cs+i));
    }
	// unsigned long long dnum,cnum;
	// dnum=0;
	// cnum=0;
	for(unsigned int i=0;i<Tnum;i++){
		// Td[i].join();
		pthread_join(Td[i],NULL);
		// dnum+=dji_num[i];
		// cnum+=cal_num[i];
	}
	
	timer2.stop("solve");
	Timer timer3;
	save_output(OutputFileName);
	timer3.stop("save");
	Timer timer4;
	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;
	timer4.stop("compare");
	// cout<<"node_num : "<<node_num<<endl;
	// cout<<"dji_num : "<<dnum<<endl;
	// cout<<"cal_num :　"<<cnum<<endl;
	exit(0);
	return 0;
}
