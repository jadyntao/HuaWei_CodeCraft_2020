#include<bits/stdc++.h>
using namespace std;
auto using_time=0.0;
auto endtime=clock();
auto begintime=clock();
#define maxn 280000
#define MAXS 10000000
unsigned int inputs[MAXS];
unsigned int inputs_size=0;
char buf[MAXS];
int num = 0;
int record_num = 0;
int node_num =0;
unsigned int id_arr[MAXS];
int id_size=-1;
unordered_map<unsigned int, int> ids;
unsigned int Graph[maxn][50];
unsigned int Graph2[maxn][50];
unsigned int size1[maxn];
unsigned int size2[maxn];
vector<vector<vector<unsigned int>>> res = vector<vector<vector<unsigned int>>>(8);
int res_size[8];
bool visited[maxn];
unsigned int three_border[maxn];
vector<unsigned int> path;
string str[maxn];
unsigned int lastone[maxn];

void get_input(string &input_file){
	FILE* file=fopen(input_file.c_str(),"r");
	int len = fread(buf, 1, MAXS, file);
	buf[len] = '\0';
	for(char *p = buf; *p && p-buf<len; p++) {
		while(*p && *p != ',')
			num = num*10 + (*(p++) - '0');
		p++;
		inputs[inputs_size++]=num;
		id_arr[num]++;
		id_size=max(num,id_size);
		num = 0;
		while(*p && *p != ',')
			num = num*10 + (*(p++) - '0');
		inputs[inputs_size++]=num;
		id_arr[num]++;
		id_size=max(num,id_size);
		num = 0;
		while(*p && *p != '\n')
			p++;
		++record_num;
	}

	node_num = 0;
	for(int i=0;i<=id_size;i++){
		if(id_arr[i]){
			str[node_num]=to_string(i)+",";
			ids[i] = node_num++;
		}
	}

	int inputs_size = 2*record_num;
	for (int i = 0; i < inputs_size; i += 2) {

		Graph[ids[inputs[i]]][size1[ids[inputs[i]]]++]=ids[inputs[i + 1]];
		Graph2[ids[inputs[i + 1]]][size2[ids[inputs[i + 1]]]++]=ids[inputs[i]];
	}
	for (int i = 0; i < node_num; i++){
		sort(Graph[i],Graph[i]+size1[i]);

	}
}

void save_output(string &output_file){
	FILE *file = fopen(output_file.c_str(),"w");
	char huanhang[1] = {'\n'};
	int total_res_size = 0;
	for(int i=3;i<8;++i){
		res_size[i] = res[i].size();
		total_res_size += res_size[i];
	}
	
	fprintf(file,"%d\n",total_res_size);
	for(int index=3;index<8;++index){
		for(auto &i:res[index]){
	        for(int j=0;j<index-1;j++){
	        	fwrite(str[i[j]].c_str(),sizeof(char),str[i[j]].size(),file);
			}
			fwrite(str[i[index-1]].c_str(),sizeof(char),str[i[index-1]].size()-1,file);
			fwrite(huanhang,sizeof(char),1,file);
	    }
	}
	fclose(file);


}

void dfs(unsigned int head,unsigned int cur,int depth,vector<unsigned int> &path){
    visited[cur]=true;
    path.emplace_back(cur);
    auto it=lower_bound(Graph[cur],Graph[cur]+size1[cur],head);
	for(;it!=Graph[cur]+size1[cur];++it){
    	unsigned int &v=*it;
		if(size1[v]==0||visited[v]||v<=head){
			continue;
		}

    	if( lastone[v] == head+1 && depth>1){
        	path.push_back(v);
        	res[depth+1].push_back(path);
        	path.pop_back();

		}
        if(depth<6){
			if(depth>3 && three_border[v]!=head+1)
				continue;
            dfs(head,v,depth+1,path);
        }
    }
    visited[cur]=false;
    path.pop_back();
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
	for(unsigned int i=0;i<node_num;i++){
		if(size1[i]!=0){
			dfs_cut_backward(i,i,1);
			dfs(i,i,1,path);
		}
	}
}

//bool compare_files(const string &file1, const string &file2){
//	//���ļ�1 
//	ifstream file1_stream; 
//    file1_stream.open(file1);   
//    assert(file1_stream.is_open()); 
//	//���ļ�2  
//	ifstream file2_stream; 
//    file2_stream.open(file2);   
//    assert(file2_stream.is_open());   
//    
//    char c1,c2;
//    bool same_flg = true;
//    while (!file1_stream.eof()&&!file2_stream.eof())
//    {
//        file1_stream>>c1;
//        file2_stream>>c2;
//        if(c1!=c2){
//        	same_flg = false;
//        	break;
//		}
//
//    }
//    if(file1_stream.eof()!=file2_stream.eof())
//    	same_flg = false;
//    
//    file1_stream.close();
//    file2_stream.close();
//    return same_flg;
//    
//}
int main(){
//	string InputFileName = "/data/test_data.txt";
//	string OuputFileName = "/projects/student/result.txt";
//	get_input(InputFileName);
//	dfs_method();
//	save_output(OuputFileName);
//	exit(0);
//	return 0;

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
