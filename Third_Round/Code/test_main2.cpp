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
#include<sys/time.h>

using namespace std;
#define maxn 2500000
// #define outMAXS 400000000
#define Tnum 4
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
    int w,now;
    inline bool operator <(const node &x)const
    //重载运算符把最小的元素放在堆顶（大根堆）
    {
        return w>x.w;//这里注意符号要为'>'
    }
};

vector<pair<unsigned int,unsigned int>> Graph[maxn];
bool vis[Tnum][maxn];
string str[maxn];
vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<unsigned int> inputs_menoy;
vector<unsigned int> id_arr;
unordered_map<unsigned int, int> ids;
int node_num=0,record_num=0;
const int INF = 0x3f3f3f3f;
int dis[Tnum][maxn];
float64_t A[Tnum][maxn];
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
	int in_num=0,out_num=0,menoy=0;
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
		inputs_in_num.push_back(in_num);
		inputs_out_num.push_back(out_num);
		inputs_menoy.push_back(menoy);
		id_arr.emplace_back(in_num);
		id_arr.emplace_back(out_num);
		in_num = 0;
		out_num = 0;
		menoy = 0;
		
	}
	//删重
	// id_arr = inputs_in_num;
	sort(id_arr.begin(), id_arr.end());
	id_arr.erase(unique(id_arr.begin(), id_arr.end()), id_arr.end());
	
	node_num = 1;
	for (unsigned int &x : id_arr) {
		str[node_num] = myto_string(x)+",";
		ids[x] = node_num++;
	}
	record_num = inputs_in_num.size();
	for(int i=0;i<record_num;i++){
		int ids_in_num = ids[inputs_in_num[i]];
		int ids_out_num = ids[inputs_out_num[i]];
		unsigned int menoy = inputs_menoy[i];
		Graph[ids_in_num].push_back({ids_out_num,menoy});
	}
}
bool cmpsum(pair<double,int>p1,pair<float64_t,int>p2){
	if(p1.first==p2.first)return p1.second<p2.second;
	return p1.first>p2.first;
}

pair<long double,int> sum[Tnum][maxn];
inline void save_output(string &output_file){
	// cout<<3<<endl;
	int sfir=0;
	for(int i=1;i<node_num;i++){
		sum[0][i].second=i;
		for(int tid=1;tid<Tnum;tid++){
			sum[0][i].first+=sum[tid][i].first;
		}
	}
	sort(sum[0]+1,sum[0]+node_num,cmpsum);
	string mys;
	
	for(int i=1;i<101;i++){
        char buf[20];
        sprintf(buf,  "%.3Lf", sum[0][i].first);
		mys+=str[sum[0][i].second]+buf+'\n';
	}
	
	long long total_offset = mys.size();
	int fd=open(output_file.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,total_offset);
	char*p=(char*)mmap(NULL,total_offset,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	memcpy(p,mys.c_str(),total_offset);
	munmap(p,total_offset);
	close(fd);
}

vector<int> prv[Tnum][maxn];
int cal_num = 0;
void cal_vcnt(int s,int t,int tid,float64_t sigma_st){
	cal_num++;
	if(t==s)return;
    for(int p:prv[tid][t]){	
		if(p!=s)sum[tid][p].first+=1.0/sigma_st;
			cal_vcnt(s,p,tid,sigma_st);
    }
}

int dji_num = 0;
void dji(int s,int tid)
{
	// memset(dis,-1,sizeof(dis));
    // dis[s][s]=0;
    //赋初值
	// cout<<1<<endl;
    // init(s,tid);
	//初始化
	for(int j=1;j<node_num;j++){
		prv[tid][j].clear();
		dis[tid][j]=INF;
		A[tid][j]=0;
		vis[tid][j]=0;
	}
	dis[tid][s]=0;
	A[tid][s]=1;

    q[tid].push((node){0,s});
    while(!q[tid].empty())//堆为空即为所有点都更新
    {
		dji_num++;
        node x=q[tid].top();
        q[tid].pop();
        int u=x.now;
        //记录堆顶（堆内最小的边）并将其弹出
        if(vis[tid][u]) continue; 
        //没有遍历过才需要遍历
		// if(dis[tid][u]<x.w)continue;
        vis[tid][u]=1;
		for(int i=0;i<Graph[u].size();i++)
        {
			// for(int i=head[u];i;i=e[i].next)
        	//搜索堆顶所有连边
            int v=Graph[u][i].first;
			int w=Graph[u][i].second;
            if(dis[tid][v]>dis[tid][u]+w)
            {
                dis[tid][v]=dis[tid][u]+w;
				// printf("dis[%d][%d]=dis[%d][%d]+%d=%d\n",s,v,s,u,w,dis[tid][v]);
                prv[tid][v].clear();
                prv[tid][v].emplace_back(u);
                // path_vcnt[v][u]++;
				A[tid][v]=A[tid][u];
				// cout<<"更新:"<<"A["<<s<<"]"<<"["<<v<<"] = "<<A[tid][v]<<endl;
                //松弛操作
                q[tid].push((node){dis[tid][v],v});
                //把新遍历到的点加入堆中
            }
			else if(dis[tid][v]==dis[tid][u]+w){
				A[tid][v]+=A[tid][u];
                prv[tid][v].emplace_back(u);
				// cout<<"add:"<<"A["<<s<<"]"<<"["<<v<<"] = "<<A[tid][v]<<endl;
                // path_vcnt[v][u]++;
				// A[tid][u]++;
				// cout<<s<<" "<<v<<" "<<u<<" "<<w<<endl;
			}
        }
    }
    // for(int v=1;v<node_num;v++){
    //     sum[v].second=v;
    // cout<<"dji_while_done! "<<tid<<endl;
    // cout<<s<<endl;
    for(int t=1;t<node_num;t++){
		int flag = 0;
        if(t==s||!A[tid][t])continue;//跳过：起始点、无法到达的t点
		// if(s==11471&&t==11477)
		// 	flag = 1;
		// cout<<"start at "<<s<<" end at "<<t<<" cnt="<<A[tid][t]<<endl;
        cal_vcnt(s,t,tid,A[tid][t]);
    }
	// cout<<"cal_done! "<<tid<<endl;
    // }
}


unsigned int curhead=1;
static void* dji_method(void* c){
	int tid =*(int*)c;
	// cout<<1<<endl;
	while(1){
		pthread_mutex_lock(&mt);
		unsigned int cur=curhead;
		
		++curhead;
		pthread_mutex_unlock(&mt);
		if(cur>=node_num)break;
            
			dji(cur,tid);
	}
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

    }
    if(file1_stream.eof()!=file2_stream.eof())
    	same_flg = false;

    file1_stream.close();
    file2_stream.close();
    return same_flg;
}

int main(){
	// string InputFileName = "/data/test_data.txt";
	// string OutputFileName = "/projects/student/result.txt";
	
	string num = "juesai";
	string ResultFileName = num + "/result.txt";
	string InputFileName = num + "/test_data.txt";
	string OutputFileName = num + "/tao_output.txt";
	timer0.stop("init");
	Timer timer1;
	get_input(InputFileName);
	timer1.stop("get");
	// init();

	Timer timer2;
	unsigned int cs[Tnum];
	pthread_t Td[Tnum];
	
	for(unsigned int i=0;i<Tnum;i++)
    {
        cs[i]=i;
		pthread_create(&Td[i],NULL,dji_method,(void*)(cs+i));
    }
	for(unsigned int i=0;i<Tnum;i++){
		// Td[i].join();
		pthread_join(Td[i],NULL);
	}
	
	timer2.stop("solve");
	Timer timer3;
	save_output(OutputFileName);
	timer3.stop("save");
	Timer timer4;
	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;
	timer4.stop("compare");
	cout<<"node_num : "<<node_num<<endl;
	cout<<"dji_num : "<<dji_num<<endl;
	cout<<"cal_num :　"<<cal_num<<endl;
	exit(0);
	return 0;
	
}
