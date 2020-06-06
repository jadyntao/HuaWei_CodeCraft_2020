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
#define maxn 2500000
// #define outMAXS 400000000
#define Tnum 1
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
// vector<pair<unsigned int,unsigned int>> Graph2[maxn];
// unsigned int size1[maxn];
// unsigned int size2[maxn];

int res_size[Tnum];
bool vis[Tnum][maxn];
// unsigned int three_border[Tnum][maxn];
// unsigned int path[Tnum][8];
string str[maxn];
// pair<unsigned int,unsigned int> lastone[Tnum][maxn];

vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<unsigned int> inputs_menoy;
vector<unsigned int> id_arr;
unordered_map<unsigned int, int> ids;
int node_num=0,record_num=0;
const int INF = 1<<30;
int dis[Tnum][maxn];
// vector<vector<int> >A[Tnum]; 
// long double A[Tnum][maxn];
long double sigma[Tnum][maxn];
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
		// cout<<in_num<<","<<out_num<<","<<menoy<<endl;
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
		if(x==111357) cout<<ids[x]<<endl;
	}
	// cout<<"node_num"<<node_num<<endl;
	record_num = inputs_in_num.size();
	// for(int i=record_num-1;i>=0;i--){
	for(int i=0;i<record_num;i++){
		int ids_in_num = ids[inputs_in_num[i]];
		int ids_out_num = ids[inputs_out_num[i]];
		unsigned int menoy = inputs_menoy[i];
		// if(ids_out_num==0) continue;
		Graph[ids_in_num].push_back({ids_out_num,menoy});
		// cout<<ids_in_num<<","<<ids_out_num<<","<<menoy<<endl;
		// A[ids_in_num][ids_out_num]=1;
		
		// Graph2[ids_out_num].push_back({ids_in_num,menoy});
	}
	// for (int i = 1; i <= node_num; i++){
	// 	sort(Graph[i].begin(),Graph[i].end());
	// }
}
bool cmpsum(pair<double,int>p1,pair<long double,int>p2){
	if(abs(p1.first-p2.first)<=0.0001)return p1.second<p2.second;
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
		// sum[0][i].first = floor(sum[0][i].first * 1000.000f + 0.5) / 1000.000f;
	}
	sort(sum[0]+1,sum[0]+node_num,cmpsum);

	string mys;
	
	for(int i=1;i<101;i++){
        char buf[30];
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
	// cout<<4<<endl;
}

vector<int> prv[Tnum][maxn];
// queue<int>quecnt[Tnum];
float64_t delta[Tnum][maxn];
// bool viscnt[Tnum][maxn];
stack<int>Stk[Tnum];
long long dji_num[Tnum];
void dji(int s,int tid)
{

	//初始化
	for(int j=1;j<node_num;j++){
		prv[tid][j].clear();
		dis[tid][j]=INF;
		sigma[tid][j]=0;
		vis[tid][j]=0;
        delta[tid][j]=0;
       
	}
    // Stk[tid].clear();
	dis[tid][s]=0;
	sigma[tid][s]=1;
    q[tid].push((node){0,s});
    while(!q[tid].empty())//堆为空即为所有点都更新
    {
		
        node x=q[tid].top();
        q[tid].pop();
        int u=x.now;
        if(vis[tid][u]) continue; 
        Stk[tid].push(u);
        //记录堆顶（堆内最小的边）并将其弹出
        
        //没有遍历过才需要遍历
		

		dji_num[tid]++;
		// if(dji_num[tid]<20) cout<<u<<endl;

        vis[tid][u]=1;
		for(int i=0;i<Graph[u].size();i++)
        {
        	//搜索堆顶所有连边
            int v=Graph[u][i].first;
            int newDist=dis[tid][u]+Graph[u][i].second;
			// if(u==1) cout<<dis[tid][v]<<" "<<newDist<<endl;
			if(v==74296&&u==506627) cout<<"!!!!"<<newDist<<" "<<dis[tid][u]<<" "<<dis[tid][v]<<endl;
            if(dis[tid][v]>newDist)
            {
                dis[tid][v]=newDist;
				prv[tid][v].clear();
                prv[tid][v].emplace_back(u);
                sigma[tid][v]=sigma[tid][u];
                q[tid].push((node){dis[tid][v],v});
                //把新遍历到的点加入堆中
				if(v==74296&&sigma[tid][74296]==1)
					cout<<"!!"<<dis[tid][v]<<" "<<u<<endl;
            }
            else if(newDist==dis[tid][v]){
                prv[tid][v].emplace_back(u);
                sigma[tid][v]+=sigma[tid][u];
				if(v==74296&&sigma[tid][74296]==1)
					cout<<"!!!"<<dis[tid][v]<<" "<<u<<endl;
            }
        }
    }
    while(!Stk[tid].empty()){
        int w=Stk[tid].top();
        Stk[tid].pop();
        float64_t coeff=(1.0+delta[tid][w])/sigma[tid][w];
        for(int v:prv[tid][w]){
            delta[tid][v] += sigma[tid][v]*coeff;
        }
        if(w!=s){
            sum[tid][w].first += delta[tid][w];
        }
    }
		cout<<sum[tid][74296].first<<"  "<<delta[tid][74296]<<"  "<<sigma[tid][74296]<<endl;
    // for(int t=1;t<node_num;t++){
    //     if(t==s||!A[tid][t])continue;//跳过：起始点、无法到达的t点
	// 	// cout<<"start at "<<s<<" end at"<<t<<" cnt="<<A[tid][t]<<endl;
    //     quecnt[tid].push(t);
    //     while(!quecnt[tid].empty()){
    //         int top=quecnt[tid].front();
	// 		quecnt[tid].pop();
	// 		// viscnt[tid][top]=1;
	// 		for(int p:prv[tid][top]){
    //             if(p!=s){
	// 				// viscnt[tid][p]=1;
    //                 quecnt[tid].push(p);
    //                 sum[tid][p].first+=1.0/A[tid][t];
	// 				// break;
    //             }
    //         }
    //     }
    // }		
	// cout<<"cal_done! "<<tid<<endl;
    // }
}

// void dfs(int source, int vertex)
// {
//     coutSP++;
//     if(vertex == source) coutSP = 0; 
//     for(list<int>::iterator ii = edgeTo[vertex].begin(); ii != edgeTo[vertex].end(); ii++)
//     {
//         if(coutSP == 0)  path.push(vertex);
//         path.push(*ii);     
//         dfs(source, *ii);
//     }
// }

// void cal(int v,int tid){
// 	// sum[cur]+=0;
// 	// sum[v].second=v;
// 	// for(int s=1;s<node_num;s++){
// 	// 	if(s==v)continue;
// 	// 	for(int t=1;t<node_num;t++){
// 	// 		if(t==s||t==v)continue;
// 	// 		// cout<<"A["<<s<<"]"<<"["<<t<<"] = "<<A[s][t]<<endl;
// 	// 		sum[v].first+=(path_vcnt[s][t][v])*1.0/A[s][t];
// 	// 	}
// 	// }
	
// }
// void cal(int v,int tid){
// 	// sum[cur]+=0;
// 	sum[v].second=v;
// 	for(int s=1;s<node_num;s++){
// 		if(s==v)continue;
// 		for(int t=1;t<node_num;t++){
// 			if(t==s||t==v||dis[s][v]+dis[v][t]!=dis[s][t])continue;
// 			// cout<<"A["<<s<<"]"<<"["<<t<<"] = "<<A[s][t]<<endl;
// 			sum[v].first+=(A[s][v]*A[v][t])*1.0/A[s][t];
// 		}
// 	}
	
// }

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
		// if(Graph[cur].size()){
            
			dji(cur,tid);
		// }
		// if(Graph[cur].size()){
		// 	unsigned int label=cur+1;
		// 	dfs_cut_backward(cur,tid);
		// 	dfs(cur,cur,0,tid,label);
		// }
	}
	// cout<<2<<endl;
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

int main(){
	string num = "juesai1";
	string ResultFileName = num + "/result.txt";
	string InputFileName = num + "/test_data.txt";
	string OutputFileName = num + "/tao_out.txt";
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
	// cout<<"cal_num :　"<<cal_num<<endl;
	exit(0);
	return 0;
}
