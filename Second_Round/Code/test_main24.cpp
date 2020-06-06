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
#define maxn 2000001
#define outMAXS 400000000
#define Tnum 4
pthread_mutex_t mt;
char outbuf[Tnum][6][outMAXS];
int offset[Tnum][6];
int hdbst[6][maxn][3];

typedef struct {
	unsigned int menoy1;
	unsigned int menoy2;
	unsigned int v;
}node;

bool comp(node A,node B){
	return A.v<B.v;
}
// vector<pair<unsigned int,unsigned int>> Graph[maxn];
// vector<pair<unsigned int,unsigned int>> Graph2[maxn];
int tot=0,tot2=0;
int fir[maxn],fir2[maxn];//存储起点为Vi的第一条边的位置
struct note{
    unsigned int from;
    unsigned int to;
    unsigned int money;
};
note Graph[maxn];
note Graph2[maxn];
bool note_cmp(note a,note b){//排序逻辑
    // if(a.from==b.from&&a.to==b.to)return a.w<b.w;
    if(a.from==b.from)return a.to<b.to;
    return a.from<b.from;
}
void add_g_g2(unsigned int u,unsigned int v,unsigned int w){
    Graph[tot].from=u;
    Graph[tot].to=v;
    Graph[tot++].money=w;

    Graph2[tot2].from=v;
    Graph2[tot2].to=u;
    Graph2[tot2++].money=w;
}
// unordered_map<int,unordered_map<unsigned int,vector<unsigned int>>> Three_Graph[maxn];
// vector<vector<vector<unsigned int>>> two_border(Tnum,vector<vector<unsigned int>>(maxn));

// vector<vector<unsigned int>> two_border_head(Tnum,vector<unsigned int>(maxn));
unsigned int two_border_head[Tnum][maxn];
unordered_map<int,unordered_map<unsigned int,vector<node>>> two_border;
// unsigned int size1[maxn];
// unsigned int size2[maxn];

int res_size[Tnum];
bool visited[Tnum][maxn];
unsigned int three_border[Tnum][maxn];
// unsigned int first_one[Tnum][maxn];
// unsigned int third_border[Tnum][maxn];
// unsigned int second_border[Tnum][maxn];
unsigned int path[Tnum][8];
string str[maxn];
// pair<unsigned int,unsigned int> lastone[Tnum][maxn];

vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<unsigned int> inputs_menoy;
vector<unsigned int> id_arr;
unordered_map<unsigned int, int> ids;
int node_num=0,record_num=0;
// int cnt[Tnum];
// int cnt2[Tnum];
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
		inputs_in_num.emplace_back(in_num);
		inputs_out_num.emplace_back(out_num);
		inputs_menoy.emplace_back(menoy);
		// id_size=max(in_num,id_size);
		// Graph[in_num][size1[in_num]].first=out_num;
		// Graph[in_num][size1[in_num]++].second=menoy;
		// Graph2[out_num][size2[out_num]].first=in_num;
		// Graph2[out_num][size2[out_num]++].second=menoy;

		in_num = 0;
		out_num = 0;
		menoy = 0;
	}

	//删重
	id_arr = inputs_in_num;
	sort(id_arr.begin(), id_arr.end());
	id_arr.erase(unique(id_arr.begin(), id_arr.end()), id_arr.end());

	node_num = 1;
	for (unsigned int &x : id_arr) {

		// if(x==976||x==991||x==980||x==983) cout<<node_num<<": "<<x<<endl;
		str[node_num] = myto_string(x)+",";
		// if(node_num==203||node_num==204) cout<<node_num<<":"<<x<<endl;
		ids[x] = node_num++;
		// if(x==84||x==119||x==147) cout<<x<<":"<<node_num-1<<endl;
	}

	record_num = inputs_in_num.size();
	// for(int i=record_num-1;i>=0;i--){
	for(int i=0;i<record_num;i++){
		int ids_in_num = ids[inputs_in_num[i]];
		int ids_out_num = ids[inputs_out_num[i]];
		unsigned int menoy = inputs_menoy[i];
		if(ids_out_num==0) continue;
		add_g_g2(ids_in_num,ids_out_num,menoy);
	}
	sort(Graph,Graph+record_num,note_cmp);
	sort(Graph2,Graph2+record_num,note_cmp);
	fir[Graph[0].from]=0;
	fir2[Graph2[0].from]=0;
	for(int i=1;i<record_num;i++){
        if(Graph[i].from!=Graph[i-1].from)//确定起点为Vi的第一条边的位置
            fir[Graph[i].from]=i;
		if(Graph2[i].from!=Graph2[i-1].from)//确定起点为Vi的第一条边的位置
            fir2[Graph2[i].from]=i;
	}
	// for (int i = 1; i <= node_num; i++){
	// 	sort(Graph[i].begin(),Graph[i].end());
	// 	// sort(Graph2[i],Graph2[i]+size2[i]);
	// }
}

inline void save_output(string &output_file){

	long long total_offset = 0;
	int total_res_size = 0;
	for(int tid=0;tid<Tnum;tid++){
		total_res_size += res_size[tid];
		for(int i=0;i<5;++i){
			total_offset += offset[tid][i];
		}
	}

	string out=myto_string(total_res_size)+"\n";
	cout<<"total_res_size: "<<total_res_size<<endl;
	total_offset += out.size();

	int fd=open(output_file.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,total_offset);
	char*p=(char*)mmap(NULL,total_offset,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	memcpy(p,out.c_str(),out.size());
	long long cur_offset = out.size();
	for(int i=0;i<5;++i){
		for(int head=1;head<node_num;head++){
			if(hdbst[i][head][1]){
				memcpy(p + cur_offset,outbuf[hdbst[i][head][2]][i]+hdbst[i][head][0],hdbst[i][head][1]);
				cur_offset += hdbst[i][head][1];
				// cout<<outbuf[hdbst[i][head][2]][i]<<endl;
			}
		}
	}
	// msync(p,total_offset,MS_SYNC);
	// munmap(p,total_offset);
	close(fd);
}


void dfs4(unsigned int &head,unsigned int &cur,unsigned int tid,unsigned int&last_menoy,unsigned int&head_menoy){
    // cnt[tid]++;
	visited[tid][cur]=true;
    path[tid][4] = cur;
	long long r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		if(v<head||visited[tid][v]) 
			continue;
		unsigned int cur_menoy = Graph[i].money;

		long long r1 = cur_menoy * 5ll;
		long long r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		if(two_border_head[tid][v] == head ){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = head_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
				unsigned int next_node = two_border[tid][v][j].v;
				if(!visited[tid][next_node]){
					unsigned int next_menoy = two_border[tid][v][j].menoy2;;
					unsigned int end_menoy = two_border[tid][v][j].menoy1;

					r1 = next_menoy * 5ll;
					r4 = end_menoy * 3ll;
					// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
					// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
					// if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && head_menoy <= r4){
					if(r1 < cur_menoy || next_menoy > r2 || r3 < end_menoy || head_menoy > r4)
						continue;
					path[tid][5] = v;
					path[tid][6] = next_node;
					// if(path[tid][0]==1360&&path[tid][1]==2392&&path[tid][2]==1764&&path[tid][3]==2113)
					// 	cout<<last_menoy<<" "<<cur_menoy<<" "<<next_menoy<<" "<<end_menoy<<"!!!!"<<endl;
					if(!hdbst[4][head][1]){
						hdbst[4][head][0]=offset[tid][4];
						hdbst[4][head][2]=tid;
					}
					for(int j=0;j<7;j++){
						memcpy(outbuf[tid][4]+offset[tid][4],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
						offset[tid][4] += str[path[tid][j]].size();
					}
					hdbst[4][head][1]=offset[tid][4]-hdbst[4][head][0];
					res_size[tid]++;
					outbuf[tid][4][offset[tid][4]-1]='\n';
					// }
				}
			}
		}
    }
    visited[tid][cur]=false;
}


void dfs3(unsigned int &head,unsigned int &cur,unsigned int tid,unsigned int&last_menoy,unsigned int&head_menoy){
    // cnt[tid]++;
	visited[tid][cur]=true;
    path[tid][3] = cur;
	long long r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		if(v<head||visited[tid][v]) 
			continue;
		unsigned int cur_menoy = Graph[i].money;

		long long r1 = cur_menoy * 5ll;
		long long r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		if(two_border_head[tid][v] == head ){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = head_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
				unsigned int next_node = two_border[tid][v][j].v;	
				if(!visited[tid][next_node]){
					unsigned int next_menoy = two_border[tid][v][j].menoy2;;
					unsigned int end_menoy = two_border[tid][v][j].menoy1;

					r1 = next_menoy * 5ll;
					r4 = end_menoy * 3ll;
					// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
					// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
					// if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && head_menoy <= r4){
					if(r1 < cur_menoy || next_menoy > r2 || r3 < end_menoy || head_menoy > r4)
						continue;
					path[tid][4] = v;
					path[tid][5] = next_node;
					// if(path[tid][0]==1360&&path[tid][1]==2392&&path[tid][2]==1764&&path[tid][3]==2113)
					// 	cout<<last_menoy<<" "<<cur_menoy<<" "<<next_menoy<<" "<<end_menoy<<"!!!!"<<endl;
					if(!hdbst[3][head][1]){
						hdbst[3][head][0]=offset[tid][3];
						hdbst[3][head][2]=tid;
					}
					for(int j=0;j<6;j++){
						memcpy(outbuf[tid][3]+offset[tid][3],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
						offset[tid][3] += str[path[tid][j]].size();
					}
					hdbst[3][head][1]=offset[tid][3]-hdbst[3][head][0];
					res_size[tid]++;
					outbuf[tid][3][offset[tid][3]-1]='\n';
					// }
				}
			}
		}

		if(three_border[tid][v]==head)
			dfs4(head,v,tid,cur_menoy,head_menoy);
    }
    visited[tid][cur]=false;
}



void dfs2(unsigned int &head,unsigned int &cur,unsigned int tid,unsigned int&last_menoy,unsigned int&head_menoy){
    // cnt[tid]++;
	visited[tid][cur]=true;
    path[tid][2] = cur;
	long long r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		if(v<head||visited[tid][v]) 
			continue;
		unsigned int cur_menoy = Graph[i].money;

		long long r1 = cur_menoy * 5ll;
		long long r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		if(two_border_head[tid][v] == head ){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = head_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
				unsigned int next_node = two_border[tid][v][j].v;
				if(!visited[tid][next_node]){
					unsigned int next_menoy = two_border[tid][v][j].menoy2;;
					unsigned int end_menoy = two_border[tid][v][j].menoy1;

					r1 = next_menoy * 5ll;
					r4 = end_menoy * 3ll;
					// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
					// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
					// if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && head_menoy <= r4){
					if(r1 < cur_menoy || next_menoy > r2 || r3 < end_menoy || head_menoy > r4)
						continue;
					path[tid][3] = v;
					path[tid][4] = next_node;
					if(!hdbst[2][head][1]){
						hdbst[2][head][0]=offset[tid][2];
						hdbst[2][head][2]=tid;
					}
					for(int j=0;j<5;j++){
						memcpy(outbuf[tid][2]+offset[tid][2],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
						offset[tid][2] += str[path[tid][j]].size();
					}
					hdbst[2][head][1]=offset[tid][2]-hdbst[2][head][0];
					res_size[tid]++;
					outbuf[tid][2][offset[tid][2]-1]='\n';
					// }
				}
			}
		}

        dfs3(head,v,tid,cur_menoy,head_menoy);
    }
    visited[tid][cur]=false;
}


void dfs1(unsigned int &head,unsigned int &cur,unsigned int tid,unsigned int&last_menoy,unsigned int&head_menoy){
    // cnt[tid]++;
	visited[tid][cur]=true;
    path[tid][1] = cur;
	long long r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		if(v<head||visited[tid][v]) 
			continue;
		unsigned int cur_menoy = Graph[i].money;

		long long r1 = cur_menoy * 5ll;
		long long r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		if(two_border_head[tid][v] == head ){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = head_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
				unsigned int next_node = two_border[tid][v][j].v;
				if(!visited[tid][next_node]){
					unsigned int next_menoy = two_border[tid][v][j].menoy2;;
					unsigned int end_menoy = two_border[tid][v][j].menoy1;

					r1 = next_menoy * 5ll;
					r4 = end_menoy * 3ll;
					// if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && head_menoy <= r4){
					if(r1 < cur_menoy || next_menoy > r2 || r3 < end_menoy || head_menoy > r4)
						continue;
					path[tid][2] = v;
					path[tid][3] = next_node;
					if(!hdbst[1][head][1]){
						hdbst[1][head][0]=offset[tid][1];
						hdbst[1][head][2]=tid;
					}
					for(int j=0;j<4;j++){
						memcpy(outbuf[tid][1]+offset[tid][1],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
						offset[tid][1] += str[path[tid][j]].size();
					}
					hdbst[1][head][1]=offset[tid][1]-hdbst[1][head][0];
					res_size[tid]++;
					outbuf[tid][1][offset[tid][1]-1]='\n';
					
				}
			}
		}
        dfs2(head,v,tid,cur_menoy,head_menoy);
    }
    visited[tid][cur]=false;
}



void dfs(unsigned int &cur ,unsigned int tid){
    visited[tid][cur]=true;
    path[tid][0] = cur;
	long long r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(unsigned int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		// unsigned int v = Graph[cur][i].first;
		if(v<cur || visited[tid][v]) continue;
		unsigned int cur_menoy = Graph[i].money;

		if(two_border_head[tid][v] == cur){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = cur_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
			// for(int j=0;j<two_border[tid][v].size();j++){
				unsigned int next_menoy = two_border[tid][v][j].menoy2;
				unsigned int end_menoy = two_border[tid][v][j].menoy1;
				r1 = next_menoy * 5ll;
				r4 = end_menoy * 3ll;
				// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
				// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
				// if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && cur_menoy <= r4){
				if(r1 < cur_menoy || next_menoy > r2 || r3 < end_menoy || cur_menoy > r4)
					continue;
				path[tid][1] = v;
				path[tid][2] = two_border[tid][v][j].v;
				if(!hdbst[0][cur][1]){
					hdbst[0][cur][0]=offset[tid][0];
					hdbst[0][cur][2]=tid;
				}
				for(int j=0;j<3;j++){
					memcpy(outbuf[tid][0]+offset[tid][0],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
					offset[tid][0] += str[path[tid][j]].size();
				}
				hdbst[0][cur][1]=offset[tid][0]-hdbst[0][cur][0];
				res_size[tid]++;
				outbuf[tid][0][offset[tid][0]-1]='\n';
				
			}
		}

        dfs1(cur,v,tid,cur_menoy,cur_menoy);
    }
    visited[tid][cur]=false;
}


void dfs_cut_backward(unsigned int &head,unsigned int tid){
	for(unsigned int  i=fir2[head];Graph2[i].from==head;i++){
    	unsigned int v1=Graph2[i].to;
		if(v1<=head)continue;
		unsigned int v1_money=Graph2[i].money;

		three_border[tid][v1] = head;
		long long r1 = v1_money * 3ll;
		
        r1 = v1_money * 5ll;
		//depth = 1  -> v1
		
		for(unsigned int  j=fir2[v1];Graph2[j].from==v1;j++){
			unsigned int v2=Graph2[j].to;
			// unsigned int v2=Graph2[v1][j].first;
			if(v2<=head)continue;
			// unsigned int v2_money=Graph2[v1][j].second;
			unsigned int v2_money=Graph2[j].money;
            long long r2 = v2_money*3ll;
            if(r1<v2_money||v1_money>r2) continue;
			three_border[tid][v2] = head;

			if(two_border_head[tid][v2] != head){
				two_border_head[tid][v2] = head;
				// two_border_head[tid][v2][1] = 1;
				two_border[tid][v2].clear();
			}
			two_border[tid][v2].push_back(node{v1_money,v2_money,v1});

			//depth = 2  -> v2
			// cnt2[tid]++;
			r2 = v2_money*5ll;
			for(unsigned int  k=fir2[v2];Graph2[k].from==v2;k++){
				unsigned int v3=Graph2[k].to;
				if(v3<=head||v3==v1)continue;
				// unsigned int &v3_money=Graph2[v2][k].second;
				unsigned int v3_money=Graph2[k].money;
                long long r3 = v3_money*3ll;
                if(r2<v3_money||v2_money>r3) continue;
				three_border[tid][v3] = head;
				
			}
		}
	}
}



unsigned int curhead=1;
static void* dfs_method(void* c){
	int tid =*(int*)c;

	while(1){
		pthread_mutex_lock(&mt);
		unsigned int cur=curhead++;
		pthread_mutex_unlock(&mt);
		if(cur>=node_num)break;
		// if(Graph[cur].size()){
        // if(fir[cur]||cur==Graph[0].from){
			dfs_cut_backward(cur,tid);
			// dfs_diedai(cur,tid);
			dfs(cur,tid);
		// }
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
	//  string InputFileName = "/data/test_data.txt";
	//  string OutputFileName = "/projects/student/result.txt";
	string num = "19630345";
	 string ResultFileName = num + "/result.txt";
	 string InputFileName = num + "/test_data.txt";
	 string OutputFileName = num + "/wen_output.txt";
	get_input(InputFileName);
	unsigned int cs[Tnum];
	pthread_t Td[Tnum];
	for(unsigned int i=0;i<Tnum;i++)
    {
        cs[i]=i;
		pthread_create(&Td[i],NULL,dfs_method,(void*)(cs+i));
    }
	for(unsigned int i=0;i<Tnum;i++){
		// Td[i].join();
		pthread_join(Td[i],NULL);
	}
	save_output(OutputFileName);
	// cout<<"dfs2 cnt: "<<cnt[0]+cnt[1]+cnt[2]+cnt[3]<<endl;
	// cout<<"dfs_cut cnt: "<<cnt2[0]+cnt2[1]+cnt2[2]+cnt2[3]<<endl;
	// cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;

	exit(0);
	return 0;
}
