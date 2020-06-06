#include<bits/stdc++.h> 
using namespace std;

auto using_time=0.0;
auto endtime=clock();
auto begintime=clock();
long long cnt = 0;
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

class Mysolution{
public:
	int record_num = 0,node_num =0;
	vector<unsigned int> id_arr;
	unordered_map<unsigned int, int> ids;
	vector<vector<unsigned int> > Graph;
	vector<vector<unsigned int> > Graph2;
	vector<vector<vector<unsigned int>>> res = vector<vector<vector<unsigned int>>>(8);
	vector<int> res_size = vector<int>(8);
	vector<bool> visited;
	vector<unsigned int> three_border;
	vector<unsigned int> path;
	string *str;
	vector<unsigned int> inputs;
	
//	vector<unordered_map<int,vector<int>>> Graph3;
	void get_input(string &input_file){
		//读取文件内容
		FILE* file=fopen(input_file.c_str(),"r");
		unsigned int i,j,k;
		record_num = 0;
	    while(fscanf(file,"%u,%u,%u",&i,&j,&k)!=EOF){
			inputs.push_back(i);
			inputs.push_back(j);
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
		Graph = vector<vector<unsigned int> > (node_num);
		Graph2 = vector<vector<unsigned int> > (node_num);
		int inputs_size = 2*record_num;
		for (int i = 0; i < inputs_size; i += 2) {
			Graph[ids[inputs[i]]].push_back(ids[inputs[i + 1]]);
			Graph2[ids[inputs[i + 1]]].push_back(ids[inputs[i]]);	
		}
		
		string file1 = "test_Graph_7.txt";
		char huanhang[1] = {'\n'};
		FILE *file2 = fopen(file1.c_str(),"w");
		for (int i = 0; i < node_num; i++){
			for (int j = 0; j < Graph[i].size(); j++){
				fwrite(str[Graph[i][j]].c_str(),sizeof(char),str[Graph[i][j]].size(),file2);
			}
			fwrite(huanhang,sizeof(char),1,file2);
		}
		fclose(file2);

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
	    path.push_back(cur);
	    for(unsigned int &v:Graph[cur]){
	        if(v==head && depth>=3){
				res[depth].push_back(path);
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
	
	void dfs_cut_forward(unsigned int head,unsigned int cur,int depth){
		visited[cur] = true;
		
		for(unsigned int &v:Graph[cur]){
			if(v>head && !visited[v] && depth<4){
				three_border[v] = head;
				dfs_cut_forward(head,v,depth+1);	
			}
				
		}
		visited[cur] = false;
	} 
	
	void dfs_cut_backward(unsigned int head,unsigned int cur,int depth){
		visited[cur] = true;
		
		for(unsigned int &v:Graph2[cur]){
			if(v>head && !visited[v] && depth<4){
				three_border[v] = head;
				dfs_cut_backward(head,v,depth+1);	
			}
				
		}
		visited[cur] = false;
	} 
	
	void dfs_method(){
		visited = vector<bool>(node_num,false);
		three_border = vector<unsigned int>(node_num,-1);
		for(unsigned int i=0;i<node_num;i++){
			if(!Graph[i].empty()){
//				dfs_cut_forward(i,i,1);
				dfs_cut_backward(i,i,1);
				dfs(i,i,1,path);
			}
		}
//		sort(res.begin(),res.end());
	}
};


int main(){
	string InputFileName = "test_data1004812.txt";
	string OutputFileName = "output1004812.txt";
	string ResultFileName = "result1004812.txt";
	
		endtime = clock();
		cout<<"0: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
		
		begintime=clock();
	Mysolution ms;
		endtime = clock();
		cout<<"1: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	
		begintime = clock();
	ms.get_input(InputFileName);
		endtime = clock();
		cout<<"2: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	
		begintime = clock();
	ms.dfs_method();
		endtime = clock();
		cout<<"3: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	
		begintime = clock();
	ms.save_output(OutputFileName);
		endtime = clock();
		cout<<"4: "<<(double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC<<endl;
		using_time += (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	cout<<"cnt :"<<cnt<<endl;	
	cout<<"total using times : "<<using_time<<endl;
	exit(0);
	return 0;
}
