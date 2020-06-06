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
#define outMAXS 400000000
#define Tnum 4
pthread_mutex_t mt;
char outbuf[Tnum][6][outMAXS];
int offset[Tnum][6];
int hdbst[6][maxn][3];


vector<pair<unsigned int,unsigned int>> Graph[maxn];
vector<pair<unsigned int,unsigned int>> Graph2[maxn];
// unsigned int size1[maxn];
// unsigned int size2[maxn];

int res_size[Tnum];
bool visited[Tnum][maxn];
unsigned int three_border[Tnum][maxn];
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
		// Graph[ids_in_num][size1[ids_in_num]].first=ids_out_num;
		// Graph[ids_in_num][size1[ids_in_num]++].second = menoy;
		// Graph2[ids_out_num][size2[ids_out_num]].first=ids_in_num;
		// Graph2[ids_out_num][size2[ids_out_num]++].second=menoy;
		Graph[ids_in_num].push_back({ids_out_num,menoy});
		Graph2[ids_out_num].push_back({ids_in_num,menoy});
	}
	for (int i = 1; i <= node_num; i++){
		sort(Graph[i].begin(),Graph[i].end());
		// sort(Graph2[i],Graph2[i]+size2[i]);
	}
}

inline void save_output(string &output_file){

	long long total_offset = 0;
	int total_res_size = 0;
	for(int tid=0;tid<Tnum;tid++){
		total_res_size += res_size[tid];
		for(int i=1;i<6;++i){
			total_offset += offset[tid][i];
		}
	}

	string out=myto_string(total_res_size)+"\n";
	// cout<<"total_res_size: "<<total_res_size<<endl;
	total_offset += out.size();

	int fd=open(output_file.c_str(),O_CREAT|O_RDWR,0666);
	1==ftruncate(fd,total_offset);
	char*p=(char*)mmap(NULL,total_offset,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
	memcpy(p,out.c_str(),out.size());
	long long cur_offset = out.size();
	for(int i=1;i<6;++i){
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

void dfs2(unsigned int &head,unsigned int &cur,int depth,unsigned int tid,unsigned int&last_menoy,unsigned int&head_menoy){
    visited[tid][cur]=true;
    path[tid][depth] = cur;
    unsigned int size = Graph[cur].size();
	for(int i=0;i<size;i++){
		unsigned int v = Graph[cur][i].first;
		if(v<head||visited[tid][v]) 
			continue;
		unsigned int cur_menoy = Graph[cur][i].second;

		long long r1 = cur_menoy * 5ll;
		long long r2 = last_menoy * 3ll;
		if(r1<last_menoy||cur_menoy>r2) continue;
		// if(cur_menoy<0.2*last_menoy||cur_menoy>3.0*last_menoy) 
		// 	continue;
    	if( lastone[tid][v].first == head && depth ){
			unsigned int next_menoy = lastone[tid][v].second;
			r1 = head_menoy * 5ll;
			r2 = next_menoy * 3ll;
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

        if(depth<5){
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
    unsigned int size = Graph[cur].size();
	for(int i=0;i<size;i++){
		unsigned int v = Graph[cur][i].first;
		if(v<cur || visited[tid][v]) continue;
		unsigned int cur_menoy = Graph[cur][i].second;
		// if(head==2) cout<<"v:"<<v<<"   cur_menoy:"<<cur_menoy<<endl;
        dfs2(cur,v,1,tid,cur_menoy,cur_menoy);
    }
    visited[tid][cur]=false;
}


void dfs_cut_backward(unsigned int &head,unsigned int tid){
	//depth = 0  -> head
	unsigned int len1 = Graph2[head].size();
    for(int i=0;i<len1;i++){
    	unsigned int &v1=Graph2[head][i].first;
		if(v1<=head)continue;
		unsigned int &v1_money=Graph2[head][i].second;
		three_border[tid][v1] = head;
		lastone[tid][v1].first=head;
		lastone[tid][v1].second=v1_money;
        long long r1 = v1_money * 5ll;
		//depth = 1  -> v1
		unsigned int len2 = Graph2[v1].size();
		for(int j=0;j<len2;j++){
			unsigned int &v2=Graph2[v1][j].first;
			if(v2<=head)continue;
			unsigned int &v2_money=Graph2[v1][j].second;
            long long r2 = v2_money*3ll;
            if(r1<v2_money||v1_money>r2) continue;
            r2 = v2_money*5ll;
			three_border[tid][v2] = head;
			//depth = 2  -> v2
			// visited[tid][v2] = true;
			unsigned int len3 = Graph2[v2].size();
			for(int k=0;k<len3;k++){
				unsigned int &v3=Graph2[v2][k].first;
				if(v3<=head||v3==v1)continue;
				unsigned int &v3_money=Graph2[v2][k].second;
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
		unsigned int cur=curhead;
		++curhead;
		pthread_mutex_unlock(&mt);
		if(cur>=node_num)break;
		if(Graph[cur].size()){
			// unsigned int label=cur+1;
			dfs_cut_backward(cur,tid);
			dfs(cur,tid);
		}
	}

}
// bool compare_files(const string &file1, const string &file2){
// 	//?????1
// 	ifstream file1_stream;
//     file1_stream.open(file1);
//     assert(file1_stream.is_open());
// 	//?????2
// 	ifstream file2_stream;
//     file2_stream.open(file2);
//     assert(file2_stream.is_open());

//     char c1,c2;
//     bool same_flg = true;
//     int t = 0;
//     while (!file1_stream.eof()&&!file2_stream.eof())
//     {
//         file1_stream>>c1;
//         file2_stream>>c2;
//         if(c1!=c2){
//         	same_flg = false;
//         	break;
// 		}
//     }
//     if(file1_stream.eof()!=file2_stream.eof())
//     	same_flg = false;

//     file1_stream.close();
//     file2_stream.close();
//     return same_flg;

// }
int main(){
	 string InputFileName = "/data/test_data.txt";
	 string OutputFileName = "/projects/student/result.txt";
//	string num = "19630345";
//	 string ResultFileName = num + "/result.txt";
//	 string InputFileName = num + "/test_data.txt";
//	 string OutputFileName = num + "/tao_output.txt";
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
	// cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;

	exit(0);
	return 0;
}
