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
#define outMAXS 480000000
#define Tnum 4
#define ll unsigned long long
pthread_mutex_t mt;
char outbuf[Tnum][7][outMAXS];
int offset[Tnum][7];
int hdbst[7][maxn][3];

typedef struct {
	ll  menoy1;
	ll  menoy2;
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
    ll money;
};
note Graph[maxn];
note Graph2[maxn];
bool note_cmp(note a,note b){//排序逻辑
    // if(a.from==b.from&&a.to==b.to)return a.w<b.w;
    if(a.from==b.from)return a.to<b.to;
    return a.from<b.from;
}
void add_g_g2(unsigned int u,unsigned int v,ll w){
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
unsigned int path[Tnum][9];
string str[maxn];
// pair<unsigned int,unsigned int> lastone[Tnum][maxn];

vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<ll> inputs_menoy;
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
inline char* mmapread(string &filepath,ll &len)
{
	ll fd=open(filepath.c_str(),O_RDONLY);
	if(fd==-1){
        fd=open(filepath.c_str(),O_RDWR|O_CREAT,0666);
        if(fd==-1){
            exit(-1);
        }
    }
	struct stat st;
	ll r=fstat(fd,&st);
	len=st.st_size;
    char* buf=(char*)mmap(NULL,len,PROT_READ,MAP_PRIVATE,fd,0);
	return buf;
}

inline void get_input(string &input_file){
	ll len = 0;
	char *buf = mmapread(input_file,len);
	int in_num=0,out_num=0;//,menoy_xioahsu=0;
	ll menoy=0;
	for(char *p = buf; *p && p-buf<len; p++) {
		int dian_cnt=0,dian_flag=0;
		while(*p && (*p == '\n'||*p=='\r'))
			p++;		
		while(*p && *p != ',')
			in_num = in_num*10 + (*(p++) - '0');
		p++;

		while(*p && *p != ',')
			out_num = out_num*10 + (*(p++) - '0');
		p++;
		
		while(*p && *p != '\n'&&*p!='\r'){
			if(*p=='.'){
				dian_flag=1;
				p++;
			}
			if(dian_flag){
				menoy = menoy*10 + (*(p++) - '0');
				dian_cnt++;
			}
			else{
				menoy = menoy*10 + (*(p++) - '0');
			}
		}
		switch(dian_cnt){
			case 0:
				menoy*=100;break;
			case 1:
				menoy*=10;break;
		}
		// if(*p=='.'){
		// 	p++;
		// 	while(*p && *p != '\n'&&*p!='\r')
		// 		menoy_xioahsu = menoy_xioahsu*10 + (*(p++) - '0');
		// }
		// menoy = menoy*100+menoy_xioahsu;
		inputs_in_num.emplace_back(in_num);
		inputs_out_num.emplace_back(out_num);
		inputs_menoy.emplace_back(menoy);
		// if(in_num==6050||in_num==6051||in_num==6052)
		// cout<<in_num<<" "<<out_num<<" "<<menoy<<endl;
		// id_size=max(in_num,id_size);
		// Graph[in_num][size1[in_num]].first=out_num;
		// Graph[in_num][size1[in_num]++].second=menoy;
		// Graph2[out_num][size2[out_num]].first=in_num;
		// Graph2[out_num][size2[out_num]++].second=menoy;

		in_num = 0;
		out_num = 0;
		menoy = 0;
		// menoy_xioahsu = 0;
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
		// if(x==6050||x==6051||x==6052) cout<<x<<":"<<node_num-1<<endl;
	}

	record_num = inputs_in_num.size();
	// for(int i=record_num-1;i>=0;i--){
	for(int i=0;i<record_num;i++){
		int ids_in_num = ids[inputs_in_num[i]];
		int ids_out_num = ids[inputs_out_num[i]];
		ll menoy = inputs_menoy[i];
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

	ll total_offset = 0;
	int total_res_size = 0;
	for(int tid=0;tid<Tnum;tid++){
		total_res_size += res_size[tid];
		for(int i=0;i<6;++i){
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
	ll cur_offset = out.size();
	for(int i=0;i<6;++i){
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

void IsLoop(unsigned int tid,unsigned int depth,int head, unsigned int v, ll head_menoy,ll cur_menoy,ll next_menoy){

		ll r1 = head_menoy * 5ll;
		ll r2 = next_menoy * 3ll;
		if(r1>=next_menoy&&head_menoy<=r2){
			r1 = next_menoy * 5ll;
			r2 = cur_menoy * 3ll;
			if(r1>=cur_menoy&&next_menoy<=r2){
			// if(head_menoy>=0.2*next_menoy&&head_menoy<=3.0*next_menoy
			// &&next_menoy>=0.2*cur_menoy&&next_menoy<=3.0*cur_menoy){

				if(!hdbst[depth][head][1]){
					hdbst[depth][head][0]=offset[tid][depth];
					hdbst[depth][head][2]=tid;
				}
				path[tid][depth+1]=v;
				for(int j=0;j<depth+2;j++){
					memcpy(outbuf[tid][depth]+offset[tid][depth],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
					offset[tid][depth] += str[path[tid][j]].size();
				}
				hdbst[depth][head][1]=offset[tid][depth]-hdbst[depth][head][0];
				res_size[tid]++;
				outbuf[tid][depth][offset[tid][depth]-1]='\n';
			}
		}
	
}


void dfs2(unsigned int &head,unsigned int &cur,int depth,unsigned int tid,ll &last_menoy,ll &head_menoy){
    // cnt[tid]++;
	visited[tid][cur]=true;
    path[tid][depth] = cur;
	ll r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		if(v<head||visited[tid][v]) 
			continue;
		ll  cur_menoy = Graph[i].money;

		ll r1 = cur_menoy * 5ll;
		ll r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		if(two_border_head[tid][v] == head ){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = head_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
				ll next_menoy = two_border[tid][v][j].menoy2;;
				ll end_menoy = two_border[tid][v][j].menoy1;
				ll next_node = two_border[tid][v][j].v;
				if(!visited[tid][next_node]){
					r1 = next_menoy * 5ll;
					r4 = end_menoy * 3ll;
					// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
					// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
					if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && head_menoy <= r4){
						path[tid][depth+1] = v;
						path[tid][depth+2] = next_node;
						// if(path[tid][0]==1360&&path[tid][1]==2392&&path[tid][2]==1764&&path[tid][3]==2113)
						// 	cout<<last_menoy<<" "<<cur_menoy<<" "<<next_menoy<<" "<<end_menoy<<"!!!!"<<endl;
						if(!hdbst[depth][head][1]){
							hdbst[depth][head][0]=offset[tid][depth];
							hdbst[depth][head][2]=tid;
						}
						for(int j=0;j<depth+3;j++){
							memcpy(outbuf[tid][depth]+offset[tid][depth],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
							offset[tid][depth] += str[path[tid][j]].size();
						}
						hdbst[depth][head][1]=offset[tid][depth]-hdbst[depth][head][0];
						res_size[tid]++;
						outbuf[tid][depth][offset[tid][depth]-1]='\n';
					}
				}
			}
		}

        if(depth<4){
			if(depth>2 && three_border[tid][v]!=head)
				continue;
            dfs2(head,v,depth+1,tid,cur_menoy,head_menoy);
        }
    }
    visited[tid][cur]=false;
}

void dfs(unsigned int &cur ,unsigned int tid){
    visited[tid][cur]=true;
    path[tid][0] = cur;
	ll r1,r2,r3,r4;
    // unsigned int size = Graph[cur].size();
	// for(unsigned int i=0;i<size;i++){
	for(unsigned int  i=fir[cur];Graph[i].from==cur;i++){
		unsigned int v = Graph[i].to;
		// unsigned int v = Graph[cur][i].first;
		if(v<cur || visited[tid][v]) continue;
		ll  cur_menoy = Graph[i].money;

		if(two_border_head[tid][v] == cur){
			int v_size = two_border[tid][v].size();
			r2 = cur_menoy * 3ll;
			r3 = cur_menoy * 5ll;
			// sort(two_border[tid][v].begin(),two_border[tid][v].end(),comp);
			for(int j=0;j<v_size;j++){
			// for(int j=0;j<two_border[tid][v].size();j++){
				ll  next_menoy = two_border[tid][v][j].menoy2;
				ll end_menoy = two_border[tid][v][j].menoy1;
				r1 = next_menoy * 5ll;
				r4 = end_menoy * 3ll;
				// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
				// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
				if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && cur_menoy <= r4){
					path[tid][1] = v;
					path[tid][2] = two_border[tid][v][j].v;
					// if(path[tid][0]==1360&&path[tid][1]==2392&&path[tid][2]==1764&&path[tid][3]==2113)
					// 	cout<<last_menoy<<" "<<cur_menoy<<" "<<next_menoy<<" "<<end_menoy<<"!!!!"<<endl;
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
		}

        dfs2(cur,v,1,tid,cur_menoy,cur_menoy);
    }
    visited[tid][cur]=false;
}

void dfs_diedai(unsigned int &head ,unsigned int tid){

	//************depth = 0**************// 
	// cnt[tid]++;
	visited[tid][head]=true;
    path[tid][0] = head;
	ll r1,r2,r3,r4;
	ll next_menoy, end_menoy,next_node;
    // unsigned int size_0 = Graph[head].size();
	// for(int a=0;a<size_0;a++){
	for(unsigned int  a=fir[head];Graph[a].from==head;a++){
		// unsigned int v0 = Graph[head][a].first;
		unsigned int v0 = Graph[a].to;
		// cout<<v0<<endl;
		if(v0<head || visited[tid][v0]) continue;
		ll v0_menoy = Graph[a].money;
		// cout<<v0<<" "<<v0_menoy<<endl;
		if(two_border_head[tid][v0] == head){
			int v_size = two_border[tid][v0].size();
			r2 = v0_menoy * 3ll;
			r3 = v0_menoy * 5ll;
			// sort(two_border[tid][v0].begin(),two_border[tid][v0].end(),comp);
			for(int j=0;j<v_size;j++){
				next_menoy = two_border[tid][v0][j].menoy2;
				end_menoy = two_border[tid][v0][j].menoy1;
				r1 = next_menoy * 5ll;
				r4 = end_menoy * 3ll;
				if(r1 >= v0_menoy && next_menoy <= r2 && r3 >= end_menoy && v0_menoy <= r4){
					path[tid][1] = v0;
					path[tid][2] = two_border[tid][v0][j].v;
					if(!hdbst[0][head][1]){
						hdbst[0][head][0]=offset[tid][0];
						hdbst[0][head][2]=tid;
					}
					for(int j=0;j<3;j++){
						memcpy(outbuf[tid][0]+offset[tid][0],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
						offset[tid][0] += str[path[tid][j]].size();
					}
					hdbst[0][head][1]=offset[tid][0]-hdbst[0][head][0];
					res_size[tid]++;
					outbuf[tid][0][offset[tid][0]-1]='\n';
				}
			}
		}

		//************depth = 1**************// 
		// cnt[tid]++;
		visited[tid][v0]=true;
		path[tid][1] = v0;
		// unsigned int size_1 = Graph[v0].size();
		// for(int b=0;b<size_1;b++){
		for(unsigned int b=fir[v0];Graph[b].from==v0;b++){
			unsigned int v1 = Graph[b].to;
			if(v1<head||visited[tid][v1]) 
				continue;
			ll  v1_menoy = Graph[b].money;

			r1 = v1_menoy * 5ll;
			r2 = v0_menoy * 3ll;
			if(r1<v0_menoy||v1_menoy>r2) continue;
			if(two_border_head[tid][v1] == head ){
				int v_size = two_border[tid][v1].size();
				r2 = v1_menoy * 3ll;
				r3 = v0_menoy * 5ll;
				// sort(two_border[tid][v1].begin(),two_border[tid][v1].end(),comp);
				for(int j=0;j<v_size;j++){
					next_menoy = two_border[tid][v1][j].menoy2;;
					end_menoy = two_border[tid][v1][j].menoy1;
					next_node = two_border[tid][v1][j].v;
					if(!visited[tid][next_node]){
						r1 = next_menoy * 5ll;
						r4 = end_menoy * 3ll;
						if(r1 >= v1_menoy && next_menoy <= r2&& r3 >= end_menoy && v0_menoy <= r4){
							path[tid][2] = v1;
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
			}

			//************depth = 2**************// 
			// cnt[tid]++;
			visited[tid][v1]=true;
			path[tid][2] = v1;
			// unsigned int size_2 = Graph[v1].size();
			// for(int c=0;c<size_2;c++){
			for(unsigned int  c=fir[v1];Graph[c].from==v1;c++){
				unsigned int v2 = Graph[c].to;
				if(v2<head||visited[tid][v2]) 
					continue;
				ll v2_menoy = Graph[c].money;

				r1 = v2_menoy * 5ll;
				r2 = v1_menoy * 3ll;
				if(r1<v1_menoy||v2_menoy>r2) continue;
				if(two_border_head[tid][v2] == head ){
					int v_size = two_border[tid][v2].size();
					r2 = v2_menoy * 3ll;
					r3 = v0_menoy * 5ll;
					// sort(two_border[tid][v2].begin(),two_border[tid][v2].end(),comp);
					for(int j=0;j<v_size;j++){
						next_menoy = two_border[tid][v2][j].menoy2;;
						end_menoy = two_border[tid][v2][j].menoy1;
						next_node = two_border[tid][v2][j].v;
						if(!visited[tid][next_node]){
							r1 = next_menoy * 5ll;
							r4 = end_menoy * 3ll;
							if(r1 >= v2_menoy && next_menoy <= r2&& r3 >= end_menoy && v0_menoy <= r4){
								path[tid][3] = v2;
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
							}
						}
					}
				}

				//************depth = 3**************// 
				// cnt[tid]++;
				visited[tid][v2]=true;
				path[tid][3] = v2;
				// unsigned int size_3 = Graph[v2].size();
				// for(int d=0;d<size_3;d++){
				for(unsigned int  d=fir[v2];Graph[d].from==v2;d++){
					unsigned int v3 = Graph[d].to;
					if(v3<head||visited[tid][v3]) 
						continue;
					ll v3_menoy = Graph[d].money;

					r1 = v3_menoy * 5ll;
					r2 = v2_menoy * 3ll;
					if(r1<v2_menoy||v3_menoy>r2) continue;
					if(two_border_head[tid][v3] == head ){
						int v_size = two_border[tid][v3].size();
						r2 = v3_menoy * 3ll;
						r3 = v0_menoy * 5ll;
						// sort(two_border[tid][v3].begin(),two_border[tid][v3].end(),comp);
						for(int j=0;j<v_size;j++){
							next_menoy = two_border[tid][v3][j].menoy2;;
							end_menoy = two_border[tid][v3][j].menoy1;
							next_node = two_border[tid][v3][j].v;
							if(!visited[tid][next_node]){
								r1 = next_menoy * 5ll;
								r4 = end_menoy * 3ll;
								if(r1 >= v3_menoy && next_menoy <= r2&& r3 >= end_menoy && v0_menoy <= r4){
									path[tid][4] = v3;
									path[tid][5] = next_node;
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
								}
							}
						}
					}

					//************depth = 4**************// 
					// if(three_border[tid][v3]!=head) continue;
					// cnt[tid]++;
					visited[tid][v3]=true;
					path[tid][4] = v3;
					// unsigned int size_4 = Graph[v3].size();
					// for(int e=0;e<size_4;e++){
					for(unsigned int  e=fir[v3];Graph[e].from==v3;e++){
						unsigned int v4 = Graph[e].to;
						if(v4<head||visited[tid][v4]) 
							continue;
						ll v4_menoy = Graph[e].money;

						r1 = v4_menoy * 5ll;
						r2 = v3_menoy * 3ll;
						if(r1<v3_menoy||v4_menoy>r2) continue;
						if(two_border_head[tid][v4] == head ){
							int v_size = two_border[tid][v4].size();
							r2 = v4_menoy * 3ll;
							r3 = v0_menoy * 5ll;
							// sort(two_border[tid][v4].begin(),two_border[tid][v4].end(),comp);
							for(int j=0;j<v_size;j++){
								next_menoy = two_border[tid][v4][j].menoy2;;
								end_menoy = two_border[tid][v4][j].menoy1;
								next_node = two_border[tid][v4][j].v;
								if(!visited[tid][next_node]){
									r1 = next_menoy * 5ll;
									r4 = end_menoy * 3ll;
									if(r1 >= v4_menoy && next_menoy <= r2&& r3 >= end_menoy && v0_menoy <= r4){
										path[tid][5] = v4;
										path[tid][6] = next_node;
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
									}
								}
							}
						}

						//************depth = 5**************// 
						if(three_border[tid][v4]!=head) continue;
						// cnt[tid]++;
						visited[tid][v4]=true;
						path[tid][5] = v4;
						// unsigned int size_4 = Graph[v3].size();
						// for(int e=0;e<size_4;e++){
						for(unsigned int  f=fir[v4];Graph[f].from==v4;f++){
							unsigned int v5 = Graph[f].to;
							if(v5<head||visited[tid][v5]) 
								continue;
							ll v5_menoy = Graph[f].money;

							r1 = v5_menoy * 5ll;
							r2 = v4_menoy * 3ll;
							if(r1<v4_menoy||v5_menoy>r2) continue;
							if(two_border_head[tid][v5] == head ){
								int v_size = two_border[tid][v5].size();
								r2 = v5_menoy * 3ll;
								r3 = v0_menoy * 5ll;
								// sort(two_border[tid][v4].begin(),two_border[tid][v4].end(),comp);
								for(int j=0;j<v_size;j++){
									ll next_menoy = two_border[tid][v5][j].menoy2;;
									ll end_menoy = two_border[tid][v5][j].menoy1;
									unsigned int next_node = two_border[tid][v5][j].v;
									if(!visited[tid][next_node]){
										r1 = next_menoy * 5ll;
										r4 = end_menoy * 3ll;
										if(r1 >= v5_menoy && next_menoy <= r2&& r3 >= end_menoy && v0_menoy <= r4){
											path[tid][6] = v5;
											path[tid][7] = next_node;
											if(!hdbst[5][head][1]){
												hdbst[5][head][0]=offset[tid][5];
												hdbst[5][head][2]=tid;
											}
											for(int j=0;j<8;j++){
												memcpy(outbuf[tid][5]+offset[tid][5],str[path[tid][j]].c_str(),str[path[tid][j]].size());//str[path[tid][j]].size()
												offset[tid][5] += str[path[tid][j]].size();
											}
											hdbst[5][head][1]=offset[tid][5]-hdbst[5][head][0];
											res_size[tid]++;
											outbuf[tid][5][offset[tid][5]-1]='\n';
										}
									}
								}
							}
						}
						visited[tid][v4]=false;
					}
					visited[tid][v3]=false;
				}
				visited[tid][v2]=false;
			}
			visited[tid][v1]=false;
		}
		visited[tid][v0]=false;
    }
    visited[tid][head]=false;
}
// ll v0[node_num][3];
void dfs_cut_backward(unsigned int &head,unsigned int tid){

	// unsigned int flen1 = Graph[head].size();
	
	// for(int fi=0;fi<flen1;fi++){
	// for(unsigned int  fi=fir[head];Graph[fi].from==head;fi++){
	// 	v0[fi][0] = Graph[fi].to;
	// 	v0[fi][1] = Graph[fi].money;
	// 	v0[fi][2] = Graph[fi].money * 5ll;
	// }
	//depth = 0  -> head
	// cnt2[tid]++;
	// unsigned int len1 = Graph2[head].size();
    // for(int i=0;i<len1;i++){
	for(unsigned int  i=fir2[head];Graph2[i].from==head;i++){
    	unsigned int v1=Graph2[i].to;
		if(v1<=head)continue;
		ll v1_money=Graph2[i].money;

     
		// //forward
		// ll r1 = v1_money * 3ll;
		// unsigned int flen1 = Graph[head].size();
		// bool flag = false;
		// for(int fi=0;fi<flen1;fi++){
		// 	unsigned int &fv1=Graph[head][fi].first;
		// 	if(fv1<=head)continue;
		// 	unsigned int &fv1_money=Graph[head][fi].second;
		// 	ll fr1 = fv1_money * 5ll;
		// 	if(fr1<v1_money||fv1_money>r1) continue;
		// 	flag = true; break;
		// 	// three_border[tid][fv1] = head;
		// }
		// if(flag==true) three_border[tid][v1] = head;

		// three_border[tid][v1] = head;
		ll r1 = v1_money * 5ll;
		//depth = 1  -> v1
		// cnt2[tid]++;
		// unsigned int len2 = Graph2[v1].size();
		// for(int j=0;j<len2;j++){
		for(unsigned int  j=fir2[v1];Graph2[j].from==v1;j++){
			unsigned int v2=Graph2[j].to;
			// unsigned int v2=Graph2[v1][j].first;
			if(v2<=head)continue;
			// unsigned int v2_money=Graph2[v1][j].second;
			ll v2_money=Graph2[j].money;
            ll r2 = v2_money*3ll;
            if(r1<v2_money||v1_money>r2) continue;
			// three_border[tid][v2] = head;

			if(two_border_head[tid][v2] != head){
				two_border_head[tid][v2] = head;
				// two_border_head[tid][v2][1] = 1;
				two_border[tid][v2].clear();
			}
			// int cur_size = two_border_head[tid][v2][1]-1;
			// two_border[tid][v2][cur_size--] = v1;
			// two_border[tid][v2][cur_size--] = v1_money;
			// two_border[tid][v2][cur_size] = v2_money;
			two_border[tid][v2].push_back(node{v1_money,v2_money,v1});

			//depth = 2  -> v2
			// cnt2[tid]++;
			r2 = v2_money*5ll;
			// unsigned int len3 = Graph2[v2].size();
			// for(int k=0;k<len3;k++){
				// unsigned int v3=Graph2[v2][k].first;
			for(unsigned int  k=fir2[v2];Graph2[k].from==v2;k++){
				unsigned int v3=Graph2[k].to;
				if(v3<=head||v3==v1)continue;
				// unsigned int &v3_money=Graph2[v2][k].second;
				ll v3_money=Graph2[k].money;
                ll r3 = v3_money*3ll;
                if(r2<v3_money||v2_money>r3) continue;
				three_border[tid][v3] = head;
				// third_border[tid][v3] = head;
				// second_border[tid][v2] = head;
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
        dfs_diedai(cur,tid);
        // dfs(cur,tid);
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

	// ll len1 = 0,len2 = 0;
	// char *buf1 = mmapread(file1,len1);
	// char *buf2 = mmapread(file2,len2);
	// if(len1!=len2){
	// 	cout<<"len1 != len2 "<<endl;
	// 	// return false;
	// }

	// unsigned int num1,num2;
	// unsigned int line = 0;
	// for(char *p1 = buf1,*p2 = buf2; *p1 && p1-buf1<len1&&*p2&&p2-buf2<len2; p1++,p2++) {
	// 	while(*p1 && (*p1 != ','||*p1 != '\n'||*p1!='\r'))
	// 		num1 = num1*10 + (*(p1++) - '0');
	// 	while(*p2 &&  (*p2 != ','||*p2 != '\n'||*p2!='\r'))
	// 		num2 = num2*10 + (*(p2++) - '0');
	// 	// if(line>0)
	// 	if(num1!=num2) {
	// 		cout<<"Line is "<<line<<endl;
	// 		return false;
	// 	}
	// 	while(*p1 && (*p1 == '\n'||*p1=='\r')){
	// 		p1++;
	// 		line++;
	// 	}
	// 	while(*p2 && (*p2 == '\n'||*p2=='\r')){
	// 		p2++;
	// 	}
	// }
	// return true;
}




int main(){
	  string InputFileName = "/data/test_data.txt";
	  string OutputFileName = "/projects/student/result.txt";
//	string num = "1004812";
//	//  string ResultFileName = num + "/result.txt";
//	 string InputFileName = num + "/test_data.txt";
//	 string OutputFileName = num + "/wen_output.txt";
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
