#include<bits/stdc++.h> 
using namespace std;


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
	set<unsigned int> id_set;
	set<unsigned int>::iterator it;
//	vector<unordered_map<int,vector<int>>> Graph3;

	void get_input(string &input_file){
		//读取文件内容
		FILE* file=fopen(input_file.c_str(),"r");
		unsigned int i,j,k;
		record_num = 0;
	    while(fscanf(file,"%u,%u,%u",&i,&j,&k)!=EOF){
			inputs.push_back(i);
			inputs.push_back(j);
			id_set.insert(i);
			id_set.insert(j);
	        ++record_num;
	    }
//		cout<<"total record nums : "<<record_num<<endl;
	
		//构建已排序的id
		//方法一：使用vector
//		id_arr = inputs;
//		sort(id_arr.begin(), id_arr.end());
//		id_arr.erase(unique(id_arr.begin(), id_arr.end()), id_arr.end());
//		str = new string[id_arr.size()];
//		node_num = 0;
//		for (unsigned int &x : id_arr) {
//			str[node_num] = to_string(x)+",";
//			ids[x] = node_num++;
//			
//		}
		
		//方法二：使用set
		str = new string[id_set.size()];
		node_num = 0;
		for(it=id_set.begin ();it!=id_set.end ();it++){
			str[node_num] = to_string(*it)+",";
			ids[*it] = node_num++;
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
		
//		//构建邻接表3
//		Graph3 = vector<unordered_map<int,vector<int>>>(node_num);
//		for (int i = 0; i < node_num; i++) {
//			int i_size = Graph[i].size();
//			for(int j = 0; j < i_size; j++){
//				int graph_ij = Graph[i][j];
//				int j_size = Graph[graph_ij].size();
//				for(int k = 0; k < j_size; k++){
//					Graph3[i][Graph[graph_ij][k]].push_back(graph_ij);
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
	    path.push_back(cur);
	    for(unsigned int &v:Graph[cur]){
	        if(v==head && depth>=3){
//	            vector<unsigned int> tmp;
//	            for(int &x:path)
//	                tmp.push_back(id_arr[x]);
//	            res.emplace_back(Path(depth,tmp));
				res[depth].push_back(path);
				continue;
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
	auto time = 0;
	int test_nums = 10;
	for(int i=0;i<test_nums;i++){
		auto begintime=clock();
	Mysolution ms;
	ms.get_input(InputFileName);
	ms.dfs_method();
	ms.save_output(OutputFileName);
//	cout<<"result is :"<<compare_files(ResultFileName,OutputFileName)<<endl;
	auto endtime=clock();
	auto using_time = (double)1000.0*(endtime-begintime)/CLOCKS_PER_SEC;
	cout<<"used times : "<<using_time<<endl;
	time += using_time;
	}
	
	cout<<"Mysolution used time: "<<(double)1.0*time/test_nums<<endl;
	
	return 0;
}
