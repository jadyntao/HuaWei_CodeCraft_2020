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
#define maxn 2000000
#define outMAXS 100000000
#define Tnum 1
pthread_mutex_t mt;
char outbuf[Tnum][6][outMAXS];
int offset[Tnum][6];
int hdbst[5][maxn][3]; //起始offset size tid


pair<unsigned int,unsigned int> Graph[maxn][120];
pair<unsigned int,unsigned int> Graph2[maxn][120];
unsigned int size1[maxn];
unsigned int size2[maxn];

int res_size[Tnum];
bool visited[Tnum][maxn];
unsigned int three_border[Tnum][maxn][2];//head size
unsigned int three_border2[Tnum][maxn][200]; //menoy menoy node 
unsigned int path[Tnum][8];
string str[maxn];
pair<unsigned int,unsigned int> lastone[Tnum][maxn];

vector<unsigned int> inputs_in_num;
vector<unsigned int> inputs_out_num;
vector<unsigned int> inputs_menoy;
vector<unsigned int> id_arr;
unordered_map<unsigned int, int> ids;
int node_num=0,record_num=0;
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
		inputs_in_num.push_back(in_num);
		inputs_out_num.push_back(out_num);
		inputs_menoy.push_back(menoy);
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

		// if(x==1359||x==2391||x==1763||x==2112||x==1655||x==1627) cout<<node_num<<": "<<x<<endl;
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
		int menoy = inputs_menoy[i];
		// if(ids_in_num==2&&ids_out_num==30) cout<<ids_in_num<<" "<<ids_out_num<<" "<<size2[ids_out_num]<<" "<<menoy<<endl;
		// if(ids_in_num==30&&ids_out_num==55) cout<<ids_in_num<<" "<<ids_out_num<<" "<<size2[ids_out_num]<<" "<<menoy<<endl;
		// if(ids_out_num==2) cout<<ids_in_num<<" "<<ids_out_num<<" "<<size2[ids_out_num]<<" "<<menoy<<endl;
		if(ids_out_num==0) continue;
		Graph[ids_in_num][size1[ids_in_num]].first=ids_out_num;
		Graph[ids_in_num][size1[ids_in_num]++].second = menoy;
		Graph2[ids_out_num][size2[ids_out_num]].first=ids_in_num;
		Graph2[ids_out_num][size2[ids_out_num]++].second=menoy;
		// if(ids_out_num==976){
		// 	cout<<" G2["<<ids_out_num<<"]["<<size2[ids_out_num]-1<<"]:"
		// 	<<Graph2[ids_out_num][size2[ids_out_num]-1].first<<" "<<Graph2[ids_out_num][size2[ids_out_num]-1].second<<endl;
		// 	// for(int i=0;i<size2[2];i++)
		// 	// 	cout<<"203 2 G2[2]["<<i<<"]:"<<Graph2[2][i].first<<" "<<Graph2[2][i].second<<endl;
		// }
		// if(ids_out_num==2) 
		// 	cout<<"G2["<<ids_out_num<<"]["<<size2[ids_out_num]-1<<"]:"
		// 	<<Graph2[ids_out_num][size2[ids_out_num]-1].first<<" "<<Graph2[ids_out_num][size2[ids_out_num]-1].second<<endl;
	}
	// for(int i=0;i<size2[976];i++)
	// 	cout<<"G2[976]["<<i<<"]:"<<Graph2[976][i].first<<" "<<Graph2[976][i].second<<endl;

	for (int i = 1; i < node_num; i++){
		sort(Graph[i],Graph[i]+size1[i]);
		sort(Graph2[i],Graph2[i]+size2[i]);
	}
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
	munmap(p,total_offset);
	close(fd);
}

void dfs2(unsigned int &head,unsigned int &cur,int depth,unsigned int tid,unsigned int&label,unsigned int&last_menoy,unsigned int&head_menoy){
    visited[tid][cur]=true;
    path[tid][depth] = cur;
    unsigned int size = size1[cur];
	long long r1,r2,r3,r4;
	for(int i=0;i<size;i++){
		unsigned int v = Graph[cur][i].first;
		if(v<head||visited[tid][v]) 
			continue;
		unsigned int cur_menoy = Graph[cur][i].second;

		r1 = cur_menoy * 5ll;
		r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		// if(cur_menoy<0.2*last_menoy||cur_menoy>3.0*last_menoy) 
		// 	continue;
		if(three_border[tid][v][0] == head){
			int v_size = three_border[tid][v][1];
			r2 = cur_menoy * 3ll;
			r3 = head_menoy * 5ll;
			for(int j=0;j<v_size;j+=2){
				unsigned int next_menoy = three_border2[tid][v][j++];
				unsigned int end_menoy = three_border2[tid][v][j++];
				r1 = next_menoy * 5ll;
				r4 = end_menoy * 3ll;
				// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
				// if(head_menoy<0.2*end_menoy||head_menoy>3.0*end_menoy)   continue;
				if(r1 >= cur_menoy && next_menoy <= r2&& r3 >= end_menoy && head_menoy <= r4){
					path[tid][depth+1] = v;
					path[tid][depth+2] = three_border2[tid][v][j];
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

        if(depth<4){
            dfs2(head,v,depth+1,tid,head,cur_menoy,head_menoy);
        }
    }
    visited[tid][cur]=false;
}

void dfs(unsigned int &head,unsigned int &cur,int depth,unsigned int tid,unsigned int&label){
    visited[tid][cur]=true;
    path[tid][depth] = cur;
    unsigned int size = size1[cur];
	long long r1,r2;
	for(int i=0;i<size;i++){
		unsigned int v = Graph[cur][i].first;
		if(v<head || visited[tid][v]) continue;
		unsigned int cur_menoy = Graph[cur][i].second;
		
		if(three_border[tid][v][0] == head){
			int v_size = three_border[tid][v][1];
			r1 = cur_menoy * 5ll;
			for(int j=0;j<v_size;j++){
				unsigned int next_menoy = three_border2[tid][v][j++];
				r2 = next_menoy * 3ll;
				// if(next_menoy<0.2*cur_menoy||next_menoy>3.0*cur_menoy)	continue;
				if(r1 >= next_menoy && cur_menoy <= r2 ){
					// if(head==2&&v==30) cout<<"2 30 "<<three_border2[tid][v][j+1]<<endl;
					path[tid][1] = v;
					path[tid][2] = three_border2[tid][v][++j];
					// cout<<str[path[tid][0]]<<" "<<str[path[tid][1]]<<" "<<str[path[tid][2]]<<endl;
					if(!hdbst[depth][head][1]){
						hdbst[depth][head][0]=offset[tid][depth];
						hdbst[depth][head][2]=tid;
					}
					for(int k=0;k<3;k++){
						// if(head==2&&v==30&&three_border2[tid][v][j+1]==55) cout<<path[tid][k]<<endl;
						memcpy(outbuf[tid][depth]+offset[tid][depth],str[path[tid][k]].c_str(),str[path[tid][k]].size());//str[path[tid][j]].size()
						offset[tid][depth] += str[path[tid][k]].size();
					}
					hdbst[depth][head][1]=offset[tid][depth]-hdbst[depth][head][0];
					res_size[tid]++;
					outbuf[tid][depth][offset[tid][depth]-1]='\n';
				}
			}
		}
		else
        	dfs2(head,v,depth+1,tid,label,cur_menoy,cur_menoy);
    }
    visited[tid][cur]=false;
}
/*
//4+3剪枝
void dfs_cut_backward(unsigned int &head,unsigned int tid){
	//depth = 0  -> head
	visited[tid][head] = true;
	unsigned int len1 = size2[head];
	long long r1,r2,r3,r4;
    for(int i=0;i<len1;i++){
		//depth = 1  -> v1
    	unsigned int &v1=Graph2[head][i].first;
		if(visited[tid][v1]||v1<=head )continue;
		unsigned int menoy1 = Graph2[head][i].second;
		r1 = menoy1 * 5ll;

		visited[tid][v1] = true;
		unsigned int len2 = size2[v1];
		for(int j=0;j<len2;j++){
			//depth = 2  -> v2
			unsigned int &v2=Graph2[v1][j].first;
			if(visited[tid][v2]||v2<=head )continue;
			unsigned int menoy2 = Graph2[head][j].second;
			
			r2 = menoy2 * 3ll;
			if(r1<menoy2||menoy1>r2) continue;
			// if(menoy1<0.2*menoy2||menoy1>3.0*menoy2) continue;
			r3 = menoy2 * 5ll;

			visited[tid][v2] = true;
			unsigned int len3 = size2[v2];
			for(int k=0;k<len3;k++){
				//depth = 3  -> v3
				unsigned int &v3=Graph2[v2][k].first;
				if(visited[tid][v3]||v3<=head )continue;
				unsigned int menoy3 = Graph2[head][k].second;

				r4 = menoy3 * 3ll;
				if(r3<menoy3||menoy2>r4) continue;
				// if(menoy2<0.2*menoy3||menoy2>3.0*menoy3) continue;
				if(three_border[tid][v3][0] == head){
					three_border[tid][v3][1] += 3;
				}
				else{
					three_border[tid][v3][0] = head;
					three_border[tid][v3][1] = 3;
				}
				int cur_size = three_border[tid][v3][1]-1;
				three_border2[tid][v3][cur_size--] = v1;
				three_border2[tid][v3][cur_size--] = v2;
				three_border2[tid][v3][cur_size] = menoy3;

			}
			visited[tid][v2] = false;
		}
		visited[tid][v1] = false;
	}
	visited[tid][head] = false;

	// visited[tid][cur] = true;
    // for(int i=0;i<size2[cur];i++){
    // 	unsigned int &v=Graph2[cur][i].first;
	// 	if(visited[tid][v]||v<=head )continue;
	// 	three_border[tid][v] = head;
	// 	if(depth<2){
	// 		if(!depth){
	// 			lastone[tid][v].first=head;
	// 			lastone[tid][v].second=Graph2[cur][i].second;
	// 		}
	// 		dfs_cut_backward(head,v,depth+1,tid,head);
	// 	}
	// }
	// visited[tid][cur] = false;
}
*/

//5+2剪枝
void dfs_cut_backward(unsigned int &head,unsigned int tid){
	//depth = 0  -> head
	visited[tid][head] = true;
	unsigned int len1 = size2[head];
	long long r1,r2,r3,r4;
    for(int i=0;i<len1;i++){
		//depth = 1  -> v1
    	unsigned int &v1=Graph2[head][i].first;
		if(visited[tid][v1]||v1<=head )continue;
		unsigned int menoy1 = Graph2[head][i].second;
		r1 = menoy1 * 5ll;

		visited[tid][v1] = true;
		unsigned int len2 = size2[v1];
		for(int j=0;j<len2;j++){
			//depth = 2  -> v2
			unsigned int &v2=Graph2[v1][j].first;
			
			if(visited[tid][v2]||v2<=head )continue;
			unsigned int menoy2 = Graph2[v1][j].second;
			r2 = menoy2 * 3ll;
			// if(head==2&&v1==55&&v2==30) {
			// 	cout<<"2 55 30!"<<endl;
			// 	cout<<menoy1<<" "<<menoy2<<endl;
			// }
			if(r1<menoy2||menoy1>r2) continue;
			// if(head==2&&v1==55&&v2==30) cout<<"2 55 30!!"<<endl;
			// if(menoy1<0.2*menoy2||menoy1>3.0*menoy2) continue;
			
			if(three_border[tid][v2][0] == head){
				three_border[tid][v2][1] += 3;
			}
			else{
				three_border[tid][v2][0] = head;
				three_border[tid][v2][1] = 3;
			}
			int cur_size = three_border[tid][v2][1]-1;
			three_border2[tid][v2][cur_size--] = v1;
			three_border2[tid][v2][cur_size--] = menoy1;
			three_border2[tid][v2][cur_size] = menoy2;
			
		}
		visited[tid][v1] = false;
	}
	visited[tid][head] = false;

	// visited[tid][cur] = true;
    // for(int i=0;i<size2[cur];i++){
    // 	unsigned int &v=Graph2[cur][i].first;
	// 	if(visited[tid][v]||v<=head )continue;
	// 	three_border[tid][v] = head;
	// 	if(depth<2){
	// 		if(!depth){
	// 			lastone[tid][v].first=head;
	// 			lastone[tid][v].second=Graph2[cur][i].second;
	// 		}
	// 		dfs_cut_backward(head,v,depth+1,tid,head);
	// 	}
	// }
	// visited[tid][cur] = false;
}

unsigned int curhead=1;
static void* dfs_method(void* c){
	int tid =*(int*)c;

	while(1){
		pthread_mutex_lock(&mt);
		unsigned int cur=curhead;
		++curhead;
		pthread_mutex_unlock(&mt);
		if(cur>=node_num)break;
		if(size1[cur]){
			unsigned int label=cur+1;
			dfs_cut_backward(cur,tid);
			dfs(cur,cur,0,tid,label);
		}
	}

}
bool compare_files(const string &file1, const string &file2){
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
    while (!file1_stream.eof()&&!file2_stream.eof())
    {
        file1_stream>>c1;
        file2_stream>>c2;
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
	string num = "9153";
	 string ResultFileName = num + "/result.txt";
	 string InputFileName = num + "/test_data.txt";
	 string OutputFileName = num + "/tao_output.txt";
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
	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;

	exit(0);
	return 0;
}
