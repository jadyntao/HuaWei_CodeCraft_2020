#include<bits/stdc++.h> 
using namespace std;
const int MAXS_input_str = 10*1024*1024;
char buf[MAXS_input_str];
const int MAXS_node_num = 280000;
int record_num = 0,node_num =0;
unordered_map<unsigned int, int> ids;
unsigned int Graph[MAXS_node_num][50];
unsigned int Graph2[MAXS_node_num][50];
unsigned int Graph_size[MAXS_node_num];
unsigned int Graph2_size[MAXS_node_num];
//int *Graph_size;
//int *Graph2_size;
vector<vector<vector<unsigned int>>> res = vector<vector<vector<unsigned int>>>(8);
int res_size[8];
bool visited[MAXS_node_num];
unsigned int three_border[MAXS_node_num];
vector<unsigned int> path;
string *str;
unsigned int inputs[560000];
unsigned int id_arr[560000];
int num = 0;
bool compare_files(string &file1,string &file2){
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

void get_input(string &input_file){
	//读取文件内容
//	FILE* file=fopen(input_file.c_str(),"r");
//	unsigned int i,j,k;
//	record_num = 0;
//    while(fscanf(file,"%u,%u,%u",&i,&j,&k)!=EOF){
//		inputs[record_num++] = i;
//		inputs[record_num++] = j;
//
//    }
    
    FILE* file=fopen(input_file.c_str(),"r");
		int len = fread(buf, 1, MAXS_input_str, file);
		buf[len] = '\0';
		for(char *p = buf; *p && p-buf<len; p++) {
			while(*p && *p != ',')
				num = num*10 + (*(p++) - '0');
			p++;
			inputs[record_num++] =num;
			num = 0;
			while(*p && *p != ',')
				num = num*10 + (*(p++) - '0');
			inputs[record_num++] =num;
			num = 0;
			while(*p && *p != '\n')
				p++;
		}
//	cout<<"total record nums : "<<record_num<<endl;

	//构建已排序的id数组
	memcpy(id_arr,inputs,record_num*sizeof(unsigned int));
	sort(id_arr, id_arr+record_num);
	node_num = unique(id_arr, id_arr+record_num)-id_arr;
	str = new string[node_num];
	for(int i=0;i<node_num;i++){
		str[i] = to_string(id_arr[i])+",";
		ids[id_arr[i]] = i;
	}
//	cout << "total node nums : " << node_num << endl;

	//构建邻接表
//	Graph_size = new int[node_num];
//	Graph2_size = new int[node_num];
//	for(int i=0;i<node_num;i++){
//		Graph_size[i]=0;
//		Graph2_size[i]=0;
//	}
	for (int i = 0; i < record_num; i += 2) {	
		Graph[ids[inputs[i]]][Graph_size[ids[inputs[i]]]++]=ids[inputs[i + 1]];
		Graph2[ids[inputs[i + 1]]][Graph2_size[ids[inputs[i + 1]]]++]=ids[inputs[i]];
	}
	
	//保存邻接表到txt文件 
	string file1 = "test_Graph_10.txt";
	char huanhang[1] = {'\n'};
	FILE *file2 = fopen(file1.c_str(),"w");
	for (int i = 0; i < node_num; i++){
		for (int j = 0; j < Graph_size[i]; j++){
			fwrite(str[Graph[i][j]].c_str(),sizeof(char),str[Graph[i][j]].size(),file2);
		}
		fwrite(huanhang,sizeof(char),1,file2);
	}
	fclose(file2);
}

void save_output(string &output_file){
	FILE *file = fopen(output_file.c_str(),"w");
	char huanhang[1] = {'\n'};
	int total_res_size = 0;
	for(int i=3;i<8;++i){
		res_size[i] = res[i].size();
		total_res_size += res_size[i];
		sort(res[i].begin(),res[i].end());
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
    int i_size = Graph_size[cur];
    for(int i=0;i<i_size;i++){
		unsigned int &v = Graph[cur][i];
        if(v==head && depth>=3){
			res[depth].emplace_back(path);
        }
        if(depth<7 && !visited[v] && v>head){
			if(depth>3 && three_border[v]!=head)
				continue;
            dfs(head,v,depth+1,path);
        }
    }
    visited[cur]=false;
    path.pop_back();
}


void dfs_cut_backward(unsigned int head,unsigned int cur,int depth){
	visited[cur] = true;
	int i_size = Graph2_size[cur];
	for(int i=0;i<i_size;i++){
		unsigned int &v = Graph2[cur][i];
		if(v>head && !visited[v] && depth<4){
			three_border[v] = head;
			dfs_cut_backward(head,v,depth+1);	
		}
	}
	visited[cur] = false;
} 

void dfs_method(){
//	visited = vector<bool>(node_num,false);
//	three_border = vector<unsigned int>(node_num,-1);
	for(unsigned int i=0;i<node_num;i++){
		if(Graph_size[i]!=0){
			dfs_cut_backward(i,i,1);
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
	cout<<"used times : "<<using_time<<endl;
	exit(0);
	return 0;
}
