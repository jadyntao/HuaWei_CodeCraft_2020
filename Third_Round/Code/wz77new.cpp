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
#define INF 0x3f3f3f3f3f3f3f
#define Tnum 8
pthread_mutex_t mt;
// struct Timer
// {
//     /*
//         计时函数，可以用于多线程内精准计时
//     */
//     timeval tic, toc;

//     Timer()
//     {
//         gettimeofday(&tic,NULL);
//     }

//     void stop(const char* name)
//     {
//         gettimeofday(&toc,NULL);
//         printf("%s: %f(s)\n", name, float(toc.tv_sec-tic.tv_sec) + float(toc.tv_usec-tic.tv_usec)/1000000);
//     }
// };
// Timer timer0;
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
// vector<pair<unsigned int,unsigned int>> Graph2[maxn];
// unsigned int size1[maxn];
// unsigned int size2[maxn];

// int res_size[Tnum];
// int vis[Tnum][maxn];
// unsigned int three_border[Tnum][maxn];
// unsigned int path[Tnum][8];
string str[maxn];
// pair<unsigned int,unsigned int> lastone[Tnum][maxn];

vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<unsigned int> inputs_menoy;
vector<unsigned int> id_arr;
unordered_map<unsigned int, unsigned int> ids;
unsigned int node_num=0,record_num=0,len_double,len_l;
// const long long INF = 0x3f;

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
		// id_arr.emplace_back(in_num);
		// id_arr.emplace_back(out_num);
		// cout<<in_num<<","<<out_num<<","<<menoy<<endl;
		in_num = 0;
		out_num = 0;
		menoy = 0;
		
	}

	//删重
	// id_arr = inputs_in_num;
    id_arr.reserve(inputs_in_num.size()+inputs_out_num.size()); 
    id_arr.assign(inputs_in_num.begin(),inputs_in_num.end());
    // id_arr.insert(id_arr.end(),inputs_in_num.begin(),inputs_in_num.end());
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
		// cout<<ids_in_num<<","<<ids_out_num<<","<<menoy<<endl;
		// A[ids_in_num][ids_out_num]=1;
		// Graph2[ids_out_num].push_back({ids_in_num,menoy});
	}
    
    // len_double=node_num*sizeof(double);
	// len_l=node_num*sizeof(long long);
	// for(int tid=0;tid<Tnum;tid++) memset(dis[tid],0x3f,len_l);
	// memset(A[tid],0,len_double);
	// memset(vis[tid],0,len_int);
	
	// for (int i = 1; i <= node_num; i++){
	// 	sort(Graph[i].begin(),Graph[i].end());
	// }
}
bool cmpsum(pair<double,unsigned int>p1,pair<double,unsigned int>p2){
	if(abs(p1.first-p2.first)<=0.0001)return p1.second<p2.second;
	return p1.first>p2.first;
}
pair<double,unsigned int> sum[Tnum][maxn];
inline void save_output(string &output_file){
	// cout<<3<<endl;
	// int sfir=0;
	for(int i=1;i<node_num;i++){
		sum[0][i].second=i;
		for(int tid=1;tid<Tnum;tid++){
			sum[0][i].first+=sum[tid][i].first;
		}
		// sum[0][i].first = floor(sum[0][i].first * 1000.000f + 0.5) / 1000.000f;
	}
	sort(sum[0]+1,sum[0]+node_num,cmpsum);

	
	// int total_res_size = 0;
	// for(int tid=0;tid<Tnum;tid++){
	// 	total_res_size += res_size[tid];
	// 	for(int i=1;i<6;++i){
	// 		total_offset += offset[tid][i];
	// 	}
	// }
	string mys;
	
	for(int i=1;i<101;i++){
        char buf[30];
        sprintf(buf,  "%.3f", sum[0][i].first);
		mys+=str[sum[0][i].second]+buf+'\n';
	}
	
	long long total_offset = mys.size();
	// string out=myto_string(total_res_size)+"\n";
	// cout<<"total_res_size: "<<total_res_size<<endl;
	// total_offset += out.size();

	int fd=open(output_file.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,total_offset);
	char*p=(char*)mmap(NULL,total_offset,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	// memcpy(p,out.c_str(),out.size());
	// long long cur_offset = 0;//out.size();
	// for(int i=0;i<100;i++){
	// string mys=str[sum[i].second]+','+myto_string(sum[i].first)+'\n';
	memcpy(p,mys.c_str(),total_offset);
		// cur_offset+=mys.size();
	// }

	// for(int head=1;head<node_num;head++){
		
	// 		memcpy(p + cur_offset,outbuf[hdbst[i][head][2]][i]+hdbst[i][head][0],hdbst[i][head][1]);
	// 		cur_offset += ;
	// 		// cout<<outbuf[hdbst[i][head][2]][i]<<endl;
		
	// }

	// msync(p,total_offset,MS_SYNC);
	// munmap(p,total_offset);
	close(fd);
	// cout<<4<<endl;
}

// vector<unsigned int> prv[Tnum][maxn];
// // queue<int>quecnt[Tnum];
// double delta[Tnum][maxn];
// // bool viscnt[Tnum][maxn];
// unsigned int Stk[Tnum][maxn];
// unsigned int sizeStk[Tnum];

// long long dis[Tnum][maxn];
// double A[Tnum][maxn];
struct NodeInfo {
    // int shortest_paths = 0;
    long long dis = INF;
	unsigned int A = 0;
    double delta = 0;
	vector<unsigned int> prv;
    unsigned int Stk;
};
// NodeInfo **nodesInfo;
NodeInfo nodesInfo[Tnum][maxn];
priority_queue<node>q[Tnum];
// unsigned long long dji_num[Tnum];
// unsigned long long cal_num[Tnum];
void dji(unsigned int s,int tid)
{
	unsigned int sizeStk=0;
	nodesInfo[tid][s].dis=0;
	nodesInfo[tid][s].A=1;
	nodesInfo[tid][s].prv.clear();
    q[tid].push((node){0,s});
    while(!q[tid].empty())//堆为空即为所有点都更新
    {
		// dji_num[tid]++;
        node x=q[tid].top();
        q[tid].pop();
        unsigned int u=x.now;
        if(nodesInfo[tid][u].dis<x.w) continue; 
        nodesInfo[tid][sizeStk++].Stk = u;
		// Stk.push(u);
        //记录堆顶（堆内最小的边）并将其弹出
        
        //没有遍历过才需要遍历
		
        // vis[tid][u]=1;
		int len=Graph[u].size();
		for(int i=0;i<len;i++)
        {
			// for(int i=head[u];i;i=e[i].next)
        	//搜索堆顶所有连边
            unsigned int v=Graph[u][i].first;
            // if(vis[tid][v])continue;
			// int w=Graph[u][i].second;
            long long newDist=nodesInfo[tid][u].dis+Graph[u][i].second;
            if(nodesInfo[tid][v].dis>newDist)
            {
                nodesInfo[tid][v].dis=newDist;
				// printf("dis[%d][%d]=dis[%d][%d]+%d=%d\n",s,v,s,u,w,dis[tid][v]);
                nodesInfo[tid][v].prv.clear();
                // vis[tid][v]=1;
                // prv[tid][v].emplace_back(u);
				// A[tid][v]=A[tid][u];
                nodesInfo[tid][v].A=0;
				// cout<<"更新:"<<"A["<<s<<"]"<<"["<<v<<"] = "<<A[tid][v]<<endl;
                //松弛操作
                
                q[tid].push((node){newDist,v});
                //把新遍历到的点加入堆中
            }
			// else if(dis[tid][v]==newDist){
			// 	A[tid][v]+=A[tid][u];
            //     prv[tid][v].emplace_back(u);
			// 	// cout<<"add:"<<"A["<<s<<"]"<<"["<<v<<"] = "<<A[tid][v]<<endl;
			// 	// A[tid][u]++;
			// }
            if(newDist==nodesInfo[tid][v].dis){
                nodesInfo[tid][v].prv.emplace_back(u);
                nodesInfo[tid][v].A+=nodesInfo[tid][u].A;
            }
        }
    }
	unsigned int lenStk=sizeStk;
	int cutn = cut_num[s];
	if(cutn==0)
		cutn=1;
    while(sizeStk){
		// cal_num[tid]++;
        // unsigned int t=Stk[tid][--sizeStk[tid]];
        unsigned int t=nodesInfo[tid][--sizeStk].Stk;
	// while(!Stk.empty()){
	// 	unsigned int t=Stk.top();
    //     Stk.pop();
        // if(!A[tid][t])continue;
    // for(int t=node_num-1;t>=1;t--){
        // if(!A[tid][t])continue;//跳过：起始点、无法到达的t点
		// cout<<"start at "<<s<<" end at"<<t<<" cnt="<<A[tid][t]<<endl;
        double coeff=(1.0+nodesInfo[tid][t].delta)/nodesInfo[tid][t].A;
        for(unsigned int v:nodesInfo[tid][t].prv){
			// if(Graph[v].size()==1){

			// }
            // Gdis[tid][v].emplace_back(t);

            nodesInfo[tid][v].delta += nodesInfo[tid][v].A*coeff;
        }
		sum[tid][t].first += cutn*nodesInfo[tid][t].delta;
        
    }
	sum[tid][s].first -= cutn*nodesInfo[tid][s].delta;
    // for(int i=0;i<Gdis[tid][s].size();i++){
	for(int i=0;i<lenStk;i++){
		unsigned int vi=nodesInfo[tid][i].Stk;
		nodesInfo[tid][vi].dis=INF;
		nodesInfo[tid][vi].delta=0;
		// nodesInfo[tid][vi].A=0;
		// Stk[i]=0;
	}
	// delete[] nodesInfo;
	// delete[] Stk;
	// delete nodesInfo;
	// memset(dis[tid],0x3f,len_l);
	// // memset(A[tid],0,len_double);
	// // memset(vis[tid],0,len_int);
	// memset(delta[tid],0,len_double);
    // }
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
			// cut_num[v]++;
			// firstid[i]=1;

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
	
	string num = "juesai2";
	string ResultFileName = num + "/result.txt";
	string InputFileName = num + "/test_data.txt";
	string OutputFileName = num + "/w77_output.txt";
	// timer0.stop("init");
	// Timer timer1;
	get_input(InputFileName);
    Graph_cut();
	
	

	// timer1.stop("get and cut");

	// Timer timer2;
	unsigned int cs[Tnum];
	pthread_t Td[Tnum];
	// nodesInfo= new NodeInfo *[Tnum];


	for(unsigned int i=0;i<Tnum;i++)
    {
        cs[i]=i;
        // nodesInfo[i] = new NodeInfo[node_num];
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
	
	// timer2.stop("solve");
	// Timer timer3;
	save_output(OutputFileName);
	// timer3.stop("save");
	// Timer timer4;
	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;
	// timer4.stop("compare");
	// cout<<"node_num : "<<node_num<<endl;
	// cout<<"dji_num : "<<dnum<<endl;
	// cout<<"cal_num :　"<<cnum<<endl;
	exit(0);
	return 0;
}
