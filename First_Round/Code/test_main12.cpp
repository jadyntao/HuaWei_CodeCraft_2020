#include<bits/stdc++.h> 
using namespace std;
#define maxn 280000

vector<unsigned int> inputs;
const int MAXS = 10*1024*1024;
char buf[MAXS];
unsigned int Graph[maxn][50];
unsigned int Graph2[maxn][50];
unsigned int size1[maxn];
unsigned int size2[maxn];
int res_size[8];
bool visited[maxn];
unsigned int three_border[maxn];
unsigned int last_one[maxn];
int num = 0;
int record_num = 0;
int node_num =0;
vector<vector<vector<unsigned int>>> res = vector<vector<vector<unsigned int>>>(8);
vector<unsigned int> path;
string *str;
vector<unsigned int> id_arr;
unordered_map<unsigned int, int> ids;
long long cnt=0;
bool compare_files(const string &file1, const string &file2){
	//打开文件1 
	ifstream file1_stream; 
    file1_stream.open(file1);   
    assert(file1_stream.is_open()); 
	//打开文件2  
	ifstream file2_stream; 
    file2_stream.open(file2);   
    assert(file2_stream.is_open());   
    
    char c1,c2;
    bool same_flg = true;
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


//	vector<unordered_map<int,vector<int>>> Graph3;
	void get_input(string &input_file){
		//读取文件内容
		FILE* file=fopen(input_file.c_str(),"r");
		int len = fread(buf, 1, MAXS, file);
		buf[len] = '\0';
		for(char *p = buf; *p && p-buf<len; p++) {
			while(*p && *p != ',')
				num = num*10 + (*(p++) - '0');
			p++;
			inputs.emplace_back(num);
			num = 0;
			while(*p && *p != ',')
				num = num*10 + (*(p++) - '0');
			inputs.emplace_back(num);
			num = 0;
			while(*p && *p != '\n')
				p++;
			++record_num;
		}

//		cout<<"total record nums : "<<record_num<<endl;
	
		//构建已排序的id数组（或者用set来删除重叠数组，再排序？）
		id_arr = inputs;
		sort(id_arr.begin(), id_arr.end());
		id_arr.erase(unique(id_arr.begin(), id_arr.end()), id_arr.end());
//		const int size = id_arr.size();
		str = new string[id_arr.size()];
		node_num = 0;
		for (unsigned int &x : id_arr) {
			str[node_num] = to_string(x)+",";
			ids[x] = node_num++;
			
		}
//		cout << "total node nums : " << node_num << endl;
	
		//构建邻接表
//		Graph = vector<vector<unsigned int> > (node_num);
//		Graph2 = vector<vector<unsigned int> > (node_num);
		int inputs_size = 2*record_num;
		for (int i = 0; i < inputs_size; i += 2) {
			
			Graph[ids[inputs[i]]][size1[ids[inputs[i]]]++]=ids[inputs[i + 1]];
			Graph2[ids[inputs[i + 1]]][size2[ids[inputs[i + 1]]]++]=ids[inputs[i]];
//			Graph[ids[inputs[i]]].emplace_back(ids[inputs[i + 1]]);
//			Graph2[ids[inputs[i + 1]]].emplace_back(ids[inputs[i]]);	
		}

//		//构建邻接表3
//		Graph3 = vector<unordered_map<int,vector<int>>>(node_num);
//		for (int i = 0; i < node_num; i++) {
//			int i_size = Graph[i].size();
//			for(int j = 0; j < i_size; j++){
//				int graph_ij = Graph[i][j];
//				int j_size = Graph[graph_ij].size();
//				for(int k = 0; k < j_size; k++){
//					Graph3[i][Graph[graph_ij][k]].emplace_back(graph_ij);
//				}
//				
//			}
//		}
	}
	
	void save_output(string &output_file){

	    //方法一：ofstream 快 
//	    ofstream outputs(output_file.c_str());
//	    int res_size = res.size();
//	    outputs<<res_size<<endl;
//		//int cnt = 1;
//	    for(auto &i:res){
//	    	auto path = i.path;
//	        int i_size = path.size();
//			//outputs<<cnt<<": ";
//	        outputs<<path[0];
//			//cnt++;
//	        for(int j=1;j<i_size;j++)
//	            outputs<<","<<path[j];
//	        outputs<<endl;
//	    }
	    
		//方法二：fprintf 慢 
//		FILE *file = fopen(output_file.c_str(),"w");
//		assert(file!=NULL); 
//		int res_size = res.size();
//		fprintf(file,"%d\n",res_size);
//		for(auto &i:res){
//	    	auto path = i.path;
//	        int i_size = path.size();
//	        fprintf(file,"%u",path[0]);
//	        for(int j=1;j<i_size;j++){
//	        	fprintf(file,",%u",path[j]);
//			}
//			fprintf(file,"\n");
//	    }
//	    fclose(file);

		//方法三：fwrite 最快但会乱码
		FILE *file = fopen(output_file.c_str(),"w");
//		assert(file!=NULL); 
		char huanhang[1] = {'\n'};
		int total_res_size = 0;
		for(int i=3;i<8;++i){
			res_size[i] = res[i].size();
			total_res_size += res_size[i];
			sort(res[i].begin(),res[i].end());
//			total_res_size += res_size[i];
//			sort(res[i],res[i]+res_size[i]);
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
		cnt++;
	    visited[cur]=true;
	    path.emplace_back(cur);
	    for(int i=0;i<size1[cur];i++){
        	unsigned int &v=Graph[cur][i];
	        if(!visited[v] && v>head){
	        	if( last_one[v] == head+1 && depth>1){
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
	        
	    }
	    visited[cur]=false;
	    path.pop_back();
	}
	
	void dfs_cut_backward(unsigned int head,unsigned int cur,int depth){
		visited[cur] = true;
        for(int i=0;i<size2[cur];i++){
        	unsigned int &v=Graph2[cur][i];
//		for(unsigned int &v:Graph2[cur]){
			if(v>head && !visited[v] && depth<4){
				three_border[v] = head+1;
				dfs_cut_backward(head,v,depth+1);	
			}
				
		}
		visited[cur] = false;
	} 
	
	void dfs_method(){
		for(int index=3;index<8;++index){
			res[index].reserve(100000);
		}
		for(unsigned int i=0;i<node_num;i++){
			if(size1[i]!=0){
				dfs_cut_backward(i,i,1);
				unsigned int i_size = size2[i];
				for(unsigned int j=0;j<i_size;j++){
					last_one[Graph2[i][j]] = i+1;
				}
				dfs(i,i,1,path);
			}
		}
	}


int main(){
	string InputFileName = "test_data1004812.txt";
	string OutputFileName = "output1004812.txt";
	string ResultFileName = "result1004812.txt";
	auto begintime=clock();
	get_input(InputFileName);
	dfs_method();
	save_output(OutputFileName);
//	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;
	auto endtime=clock();
	auto using_time = (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	cout<<cnt<<endl;
	cout<<"used times : "<<using_time<<endl;
	exit(0);
	return 0;
}
