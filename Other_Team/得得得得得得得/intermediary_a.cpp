#ifndef TEST
#pragma check_stack(off)
#pragma runtime_checks( "scu", off )
#pragma strict_gs_check(off)
#endif

#pragma float_control(except, off)
#pragma fenv_access(off)
#pragma float_control(precise, off)

#pragma auto_inline on
//#pragma optimize("agtw", on)

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cassert>
#include <list>
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>
#include <set>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <cmath>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
using namespace std;
typedef unsigned int ui;

#ifdef TEST

#include <sys/time.h>

timeval starttime, endtime;
#define CURTIME(x) \
    gettimeofday(&endtime,0); \
    cout << #x << ": " << (double)(1000000*(endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec) / 1000000 << endl
#else
#define CURTIME(x)
#endif

//线程数量
#define THREADNUM 4
#define GROUPSIZE 32
typedef struct Edge {
    ui vertice;
    ui weight;

    bool operator<(const Edge &a) const { return vertice < a.vertice; }

    bool operator>(const Edge &a) const { return vertice > a.vertice; }

    Edge() {
    }

    Edge(ui v, ui w) {
        vertice = v;
        weight = w;
    }
} Edge;

#define NODENEXTLISTLISTSIZE 2046
typedef struct NodeNextList {
    int num;
    struct NodeNextList *next;
    Edge node[NODENEXTLISTLISTSIZE];
    
    inline void add(struct NodeNextList **s, const Edge &num){
        if(unlikely(this->num == NODENEXTLISTLISTSIZE)){
            *s = next = (struct NodeNextList*)malloc(sizeof(struct NodeNextList));
            *next = {.num=1, .next=nullptr, .node={num,}};
        }
        else{
            node[this->num++] = num;
        }
    }
} NodeNextList;

// 控制struct大小在2的幂次大小
#define NODELISTSIZE 29
typedef struct Node {
    char num = 0;
    NodeNextList *nextList = nullptr, *endList = nullptr;
    Edge node[NODELISTSIZE];

    inline void add(const Edge &num) {
        if(this->num == NODELISTSIZE){
            if(!endList){
                nextList = endList = (NodeNextList*)malloc(sizeof(NodeNextList));
                endList->num = 0;
                endList->next = NULL;
            }
            endList->add(&endList, num);
        }
        else{
            node[this->num++] = num;
        }
    }
} Node;

typedef struct NodeIter{
    Edge *cur, *end;
    NodeNextList *nextList;

    NodeIter(Node *node){
        cur = node->node-1;
        end = node->node+node->num;
        nextList = node->nextList;
        __builtin_prefetch(cur+1, 0);
    }

    inline Edge *next() {
        cur++;
        if(unlikely(cur == end)){
            if(nextList){
                cur = nextList->node;
                end = nextList->node + nextList->num;
                nextList = nextList->next;
            }
            else{
                return NULL;
            }
        }
        __builtin_prefetch(cur+1, 0);
        return cur;
    }
} NodeIter;

typedef union Path {
    struct {
        ui stp2;
        ui stp1;
        ui wout;
        ui win;
    };
    unsigned long num;
} Path;

inline bool limit(ui w1, ui w2) {
    return (unsigned long)w2 * 5ul >= w1 && w2 <= (unsigned long)w1 * 3ul;
    /*
    ui max = w1 <= 3 * w1 ? 3 * w1 : 4294967295;
    return ((w2 >= 0.2 * w1) && (w2 <= max));
    // */
}

extern uint32_t const NUMTOSTR1[];
extern uint32_t const NUMTOSTR2[];
extern uint32_t const NUMTOSTR3[];

inline void writeNum(char *output, unsigned int &p, const ui num) {
    if (num < 10) {
        *(int *) (output + p) = NUMTOSTR1[num];
        p += 1+1;
        return;
    } else if (num < 100) {
        *(int *) (output + p) = NUMTOSTR2[num];
        p += 2+1;
        return;
    } else if (num < 1000) {
        *(int *) (output + p) = NUMTOSTR3[num];
        p += 3+1;
        return;
    } else if (num < 10000) {
        ui x = num / 10;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR1[num - x * 10];
        p += 4+1;
        return;
    } else if (num < 100000) {
        ui x = num / 100;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR2[num - x * 100];
        p += 5+1;
        return;
    } else if (num < 1000000) {
        ui x = num / 1000;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR3[num - x * 1000];
        p += 6+1;
        return;
    } else if (num < 10000000) {
        ui x = num / 10000;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR3[(num - x * 10000) / 10];
        *(int *) (output + p + 6) = NUMTOSTR1[num % 10];
        p += 7+1;
        return;
    } else if (num < 100000000) {
        ui x = num / 100000;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR3[(num - x * 100000) / 100];
        *(int *) (output + p + 6) = NUMTOSTR2[num % 100];
        p += 8+1;
        return;
    } else if (num < 1000000000) {
        ui x = num / 1000000;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR3[(num - x * 1000000) / 1000];
        *(int *) (output + p + 6) = NUMTOSTR3[num % 1000];
        p += 9+1;
        return;
    } else {
        ui x = num / 10000000;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR3[(num - x * 10000000) / 10000];
        *(int *) (output + p + 6) = NUMTOSTR3[num % 10000 / 10];
        *(int *) (output + p + 9) = NUMTOSTR1[num % 10];
        p += 10 + 1;
        return;
    }
}
struct str{
    char str[19];
    char add = 0;
    unsigned int len = 0;
};
#define MAXDATANUM (1<<21u)
Node nodeSetHashList[MAXDATANUM];
Node inNodeList[MAXDATANUM];
pthread_spinlock_t nodeLock[MAXDATANUM][2];
//ui nodeIDList[MAXDATANUM]; //保存结点
ui nodePreList[MAXDATANUM]; //保存结点
str nodePreListStr[MAXDATANUM]; //保存节点字符串
int nodeSizeCount[MAXDATANUM];
//int nodeLessInSize[MAXDATANUM];
//double totalCountSize = 0;
//ui nodeSetSize = 0;
ui nodeIDNum = 0;
//int gapId = 1;
//ui threadGap[THREADNUM] = {0, };
ui resultLen[THREADNUM];

char *inputStr;
int inputStrLen;

int totalDataNum = 0;

inline Node *nodeSet(const ui i, Node *list) {
    return &list[i];
}

inline Node *getNodeSet(const ui i) {
    return &nodeSetHashList[i];
}

inline Node *getInNodeSet(const ui i) {
    return &inNodeList[i];
}

/*
#define COUNTSIZE 1000000
inline void nodeDivideTask(ui id, ui nodeCnt) {
    if(gapId >= THREADNUM){
        return;
    }
    //计算线程分配
    static unsigned long curTotalNum = 0;
    unsigned long n1 = nodeLessInSize[id], n2 = nodeLargerOutSize[id];
    unsigned long cur = n1*n1*n1 + n2*n2*n2*n2;
    cur = cur > COUNTSIZE ? COUNTSIZE : cur;
    if(curTotalNum + cur >= totalCountSize / THREADNUM){
        threadGap[gapId++] = id-1;
        curTotalNum = cur;
        //cout << "gap" << gapId - 1 << ":" <<nodeCnt << endl;
    }
    else{
        curTotalNum += cur;
    }
}
*/
inline void nodeSort(Node &node) {
    //sort(node.node.begin(), node.node.end());
    if ((node.num < NODELISTSIZE)) {
        sort(node.node, node.node + node.num);
    } else {
        // 使用优先队列排序
        NodeIter inIter(&node);
        priority_queue<Edge, vector<Edge>, greater<Edge>> q;
        for(Edge *edge = inIter.next(); edge; edge = inIter.next()){
            q.push(*edge);
        }

        NodeIter outIter(&node);
        for(Edge *edge = outIter.next(); edge; edge = outIter.next()){
            *edge = q.top();
            q.pop();
        }
    }
}

struct Result {
    unsigned int strLen = 0;
    char *str;
    int groupLen[MAXDATANUM / GROUPSIZE] = {0,}; // 前面组号，后面位置
    int groupPointer = 1;
    int outputPointer[THREADNUM] = {1, 1, 1, 1};

    inline void insert3(const ui &id, const ui &x, const ui &y) {
        memcpy(str+strLen, nodePreListStr[id].str, nodePreListStr[id].len);
        strLen += nodePreListStr[id].len;
        memcpy(str+strLen, nodePreListStr[x].str, nodePreListStr[x].len);
        strLen += nodePreListStr[x].len;
        memcpy(str+strLen, nodePreListStr[y].str, nodePreListStr[y].len);
        strLen += nodePreListStr[y].len;
        /*writeNum(str, strLen, nodePreList[id]);
        writeNum(str, strLen, nodePreList[x]);
        writeNum(str, strLen, nodePreList[y]);*/
        str[strLen - 1] = '\n';
    }

    inline void insert4(char *start, const int len, const Path &path) {
        memcpy(str+strLen, start, len);
        strLen += len;
        memcpy(str+strLen, nodePreListStr[path.stp1].str, nodePreListStr[path.stp1].len);
        strLen += nodePreListStr[path.stp1].len;
        memcpy(str+strLen, nodePreListStr[path.stp2].str, nodePreListStr[path.stp2].len);
        strLen += nodePreListStr[path.stp2].len;
        /*writeNum(str, strLen, nodePreList[path.stp1]);
        writeNum(str, strLen, nodePreList[path.stp2]);*/
        str[strLen-1] = '\n';
    }

    inline void newGroup(){
        groupLen[groupPointer++] = strLen;
    }

    inline void outputResult(char *output, unsigned int &p, int threadId) {
        int len = groupLen[outputPointer[threadId]] - groupLen[outputPointer[threadId]-1];
        memcpy(output + p, str+groupLen[outputPointer[threadId]-1], len);
        p += len;
        outputPointer[threadId]++;
    }

    inline void moveForward(unsigned int &p, int threadId){
        int len = groupLen[outputPointer[threadId]] - groupLen[outputPointer[threadId]-1];
        p += len;
        outputPointer[threadId]++;
    }
} result[THREADNUM][5];

ui *inputTemp[THREADNUM];
int dataNum[THREADNUM]; //每个线程数组储存的数量
int readStart[THREADNUM]; // 输入时读的开头
int dataSum[THREADNUM]; //从第一个线程开始的累加和
//int nodeIDListEnd[THREADNUM];
# define SIXTYTHOUSAND (2800000 / THREADNUM + 10000)

inline ui *readInputData(const ui i) {
    if (i < dataSum[0])
        return inputTemp[0] + i * 3;
    for (int x = 1; x < THREADNUM; x++)
        if (i < dataSum[x])
            return inputTemp[x] + (i - dataSum[x - 1]) * 3;
}

void *readLineThread(void *threadId) {
    const int id = *((int *) threadId);
    int p = readStart[id];
    //cout << id;
    int stop;
    //stop = inputStrLen;
    dataNum[id] = 0;
    inputTemp[id] = (ui *) (malloc(sizeof(ui) * SIXTYTHOUSAND * 3));
    if (id == THREADNUM - 1)
        stop = inputStrLen;
    else
        stop = readStart[id + 1];
    for (int i = 0; p < stop; i++) {
        for (int j = 0; j < 2; j++) {
            while (inputStr[p] != ',') {
                *(inputTemp[id] + i * 3 + j) = *(inputTemp[id] + i * 3 + j) * 10 + (inputStr[p] - '0');
                p++;
            }
            //nodeIDMap[*(inputTemp[id] + i * 3 + j)] = 1;
            //cout <<i <<":"<< *(inputTemp[id] + i*2 + j) << ",";
            p++;
        }
        //cout << endl;
        while (inputStr[p] != '\n' && inputStr[p] != '\r') {
            *(inputTemp[id] + i * 3 + 2) = *(inputTemp[id] + i * 3 + 2) * 10 + (inputStr[p] - '0');
            p++;
        }
        while (inputStr[p] == '\n' || inputStr[p] == '\r')
            p++;
        if(*(inputTemp[id] + i * 3 + 2) == 0){
            *(inputTemp[id] + i * 3 + 0) = 0;
            *(inputTemp[id] + i * 3 + 1) = 0;
            i--;
            continue;
        }
        dataNum[id]++;
    }
    pthread_exit(nullptr);
}

inline void readInput(const char *inputPath) {
    const int fd = open(inputPath, O_RDONLY, 0666);
    assert(fd >= 0);
    struct stat st{};
    fstat(fd, &st);
    inputStrLen = st.st_size;
    const int readBitNum = inputStrLen / THREADNUM;
    //cout << inputStrLen << endl;
    inputStr = (char *) mmap(nullptr, inputStrLen, PROT_READ, MAP_PRIVATE, fd, 0);
    assert(inputStr != MAP_FAILED);

    readStart[0] = 0;
    for (int i = 1; i < THREADNUM; i++) {
        int p;
        p = max(readBitNum * i, readStart[i - 1] + 1);
        while (inputStr[p] != '\n' && p < inputStrLen) {
            p++;
        }
        p++;
        readStart[i] = min(p, inputStrLen);
        //cout << p << endl;
    }
    //cout << "read begin" <<endl;
    pthread_t threads[THREADNUM];
    for (int i = 0; i < THREADNUM; i++) {
        const int *j = new int(i);
        //cout << *j << endl;
        int rc = pthread_create(&threads[*j], nullptr, readLineThread, (void *) j);
#ifdef TEST
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
#endif
    }

    for (int i = 0; i < THREADNUM; i++) {
        pthread_join(threads[i], nullptr);
        totalDataNum = totalDataNum + dataNum[i];
        dataSum[i] = totalDataNum;
    }
}

typedef struct IDMap{
    bool isInit = false;
    ui val;
    ui mapedVal;
}IDMap;
IDMap nodeIDMap[MAXDATANUM];
bool InitMap(IDMap* list, const ui i){
    ui id = i & (MAXDATANUM - 1);
    if (!list[id].isInit) {
	list[id].isInit = true;
	list[id].val = i;
        return true;
    } else {
        if (list[id].val == i)
            return false;
        else{
            do{
                id++;
                if (list[id].val == i)
                    return false;
            } while (list[id].isInit);
	    list[id].isInit = true;
	    list[id].val = i;
            return true;
        }
    }
}
void MapSet(IDMap* list,ui i, const ui k){
    ui id = i & (MAXDATANUM - 1);
    while(list[id].val != i)
        id++;
    list[id].mapedVal = k;
}
ui MapGet(IDMap* list, const ui i){
    ui id = i & (MAXDATANUM - 1);
    while(list[id].val != i)
        id++;
    return list[id].mapedVal;
}

void *treat_thread(void *arg){
    const int threadId = *((int *) arg);
    const int part = totalDataNum / THREADNUM;
    const int front = threadId * part;
    const int back = threadId == THREADNUM-1 ? totalDataNum : (threadId + 1) * part;
    for (int i = front; i < back; i++) {
        //auto iter1 = nodeSet.find(readInputData(i)[0]);
        ui *edge = readInputData(i);
        ui start = MapGet(nodeIDMap,edge[0]);
        ui end = MapGet(nodeIDMap,edge[1]);
        ui weight = edge[2];
        ///*
        if(start < end){
            nodeSizeCount[start] += 16;
        }
        else{
            nodeSizeCount[end]++;
        }
        // */
        pthread_spin_lock(&nodeLock[start][0]);
        nodeSet(start, nodeSetHashList)->add(Edge(end, weight));
        pthread_spin_unlock(&nodeLock[start][0]);
        pthread_spin_lock(&nodeLock[end][1]);
        nodeSet(end, inNodeList)->add(Edge(start, weight));
        pthread_spin_unlock(&nodeLock[end][1]);
    }
}

inline void treatData() {
    nodeIDNum = 0;
    for (int i = 0; i < totalDataNum; i++) {
        const ui *edge = readInputData(i);
        const ui start = edge[0];
        const ui end = edge[1];
        if(InitMap(nodeIDMap,start)){
            nodePreList[nodeIDNum] = start;
            pthread_spin_init(&nodeLock[nodeIDNum][0], PTHREAD_PROCESS_PRIVATE);
            pthread_spin_init(&nodeLock[nodeIDNum][1], PTHREAD_PROCESS_PRIVATE);
            nodeIDNum++;
        }
        if(InitMap(nodeIDMap,end)){
            nodePreList[nodeIDNum] = end;
            pthread_spin_init(&nodeLock[nodeIDNum][0], PTHREAD_PROCESS_PRIVATE);
            pthread_spin_init(&nodeLock[nodeIDNum][1], PTHREAD_PROCESS_PRIVATE);
            nodeIDNum++;
        }
    }
    CURTIME(点统计结束);
    //memset(nodeSetHashList, 0, sizeof(Node) * nodeIDNum);
    //memset(inNodeList, 0, sizeof(Node) * nodeIDNum);
    sort(nodePreList,nodePreList + nodeIDNum);
    for(int i = 0;i < nodeIDNum;i++){
        MapSet(nodeIDMap,nodePreList[i],i);
        writeNum(nodePreListStr[i].str,nodePreListStr[i].len, nodePreList[i]);
        nodePreListStr[i].add = nodePreListStr[i].len & 7;
    }
    CURTIME(映射完成);
    pthread_t threads[THREADNUM];
    //cout << nodeSet.size()<<endl;
    for (int j = 1; j < THREADNUM; j++) {
        const int *x = new int(j);
        pthread_create(&threads[j], nullptr, treat_thread, (void *) x);
    }
    treat_thread(new int(0));
    for (int i = 1; i < THREADNUM; i++) {
        pthread_join(threads[i], nullptr);
    }
    CURTIME(建图完成);
    //cout << "treatdata end" <<endl;
    for (int i = 0; i < THREADNUM; i++) {
        free(inputTemp[i]);
    }
}

struct PathList {
    Path p;
    int next;
};
pthread_spinlock_t lock;
int curGroup = 0;
int groupThread[MAXDATANUM / GROUPSIZE];
int groupDividing[1u<<17];
int groupNum = 1;
#define GEOUP_DIVIDING_THRESH 4096
int totalLen = 0;
#define LIMIT_INIT(x) \
    const ui x##02 = (x->weight+4) / 5, \
        x##3 = x->weight > 0xffffffff/3 ? 0xffffffff : x->weight*3
#define LIMIT_INIT_R(x) \
    const ui x##r3 = (x->weight+2) / 3, \
        x##r02 = x->weight > 0xffffffff/5 ? 0xffffffff : x->weight*5
#define LIMIT(x, y) ((x##02) <= (y) && (y) <= (x##3))
#define LIMIT_R(x, y) ((y##r3) <= (x) && (x) <= (y##r02))
void *search_thread(void *arg) {
    const int threadId = *((int *) arg);
    resultLen[threadId] = 0;
    //int begin = threadGap[threadId];
    //int end = threadId == THREADNUM - 1 ? nodeIDNum : threadGap[threadId + 1];
    //cout << "thread "<< threadId << " working from" << begin << " to " << end << endl;
    //ui route[7];//路径
    char routeStr[7 * (20)];
    int *isThirdNode = (int *) (malloc(sizeof(int) * nodeIDNum));
    memset(isThirdNode, -1, nodeIDNum * sizeof(int));
    int *pathListHead = (int *) (malloc(sizeof(int) * nodeIDNum));
    //unordered_map<ui,int> pathListHead;
    struct PathList pathList[30000];
    Result *r = result[threadId];
    r[0].str = (char *) (malloc(sizeof(char) * 11u * (0u + 1u) * (40000000u * (0u + 1u))));
    r[1].str = (char *) (malloc(sizeof(char) * 11u * (1u + 1u) * (40000000u * (1u + 1u))));
    r[2].str = (char *) (malloc(sizeof(char) * 11u * (2u + 1u) * (40000000u * (2u + 1u))));
    r[3].str = (char *) (malloc(sizeof(char) * 11u * (3u + 1u) * (40000000u * (3u + 1u))));
    r[4].str = (char *) (malloc(sizeof(char) * 11u * (4u + 1u) * (40000000u * (4u + 1u))));
    //int groupTotalNum = (nodeIDNum + GROUPSIZE - 1) / GROUPSIZE;
    //int oldResultLen = 0;
    while(true){
        int t;
        pthread_spin_lock(&lock);
        //oldResultLen = resultLen[threadId];
        if(curGroup >= groupNum)
            break;
        t = curGroup;
        groupThread[curGroup] = threadId;
        curGroup++;
        pthread_spin_unlock(&lock);
#ifdef TEST
        assert(t < groupNum);
#endif
        const int begin = groupDividing[t];
        const int end = t == groupNum-1 ? nodeIDNum : groupDividing[t+1];

        for (int id = begin; id < end; id++) {
            //route[0] = id;
            Node *root = getNodeSet(id);
            Node *inRoot = getInNodeSet(id);
            if(!root->num || !inRoot->num){
                continue;
            }
            //cout << id <<endl;
            int pointer = 0;
            //Node *inRoot = &inNodeList[id];
            NodeIter xIter(inRoot);
            for (Edge *ix = xIter.next(); ix; ix = xIter.next()) {
                const Edge *x = ix;
                if (x->vertice <= id)
                    continue;
                Node *pre = getInNodeSet(x->vertice);
                NodeIter yIter(pre);
                for (Edge *iy = yIter.next(); iy; iy = yIter.next()) {
                    const Edge *y = iy;
                    if (y->vertice <= id || !limit(y->weight, x->weight))
                        continue;
                    Node *pre2 = getInNodeSet(y->vertice);
                    NodeIter zIter(pre2);
                    for (Edge *iz = zIter.next(); iz; iz = zIter.next()) {
                        Edge *z = iz;
                        if (z->vertice <= id|| z->vertice == x->vertice || !limit(z->weight, y->weight) )
                            continue;
                        int *head = &pathListHead[z->vertice];
                        //cout << z->vertice << " " << y->vertice << " " << x->vertice <<endl;
                        Path p = {x->vertice, y->vertice, x->weight, z->weight};
                        if (isThirdNode[z->vertice] != id) {
                            isThirdNode[z->vertice] = id;
                            pathList[pointer].next = -1;
                            //cout << z <<endl;
                        } else {
                            for (; *head != -1; head = &pathList[*head].next) {
                                __builtin_prefetch(&pathList[pathList[*head].next], 0);
                                if (p.num < pathList[*head].p.num) {
                                    break;
                                }
                            }
                            pathList[pointer].next = *head;
                        }
                        pathList[pointer].p = p;
                        *head = pointer;
                        pointer++;
                        __builtin_prefetch(&pathList[pointer], 0);
                        //cout << pointer <<endl;
                    }
                }
            }

            NodeIter x_iter(root);
            for (Edge *ix = x_iter.next(); ix; ix = x_iter.next()) {
                const Edge *x = ix;
                if (x->vertice < id) continue;
                //route[1] = x->vertice;
                // 检查是否有4环
                if (isThirdNode[x->vertice] == id) {
                    ui p = 0;
                    memcpy(routeStr + p, nodePreListStr[id].str, nodePreListStr[id].len +  nodePreListStr[id].add);
                    p += nodePreListStr[id].len;
                    memcpy(routeStr + p, nodePreListStr[x->vertice].str, nodePreListStr[x->vertice].len + nodePreListStr[x->vertice].add);
                    p += nodePreListStr[x->vertice].len;
                    __builtin_prefetch(&r[1], 1);
                    for (int I = pathListHead[x->vertice]; I != -1; I = pathList[I].next) {
                        __builtin_prefetch(&pathList[pathList[I].next], 0);
                        const Path *path = &pathList[I].p;
                        if (limit(x->weight, path->win) && limit(path->wout, x->weight)) {
                            __builtin_prefetch(&nodePreListStr[path->stp2], 0);
                            __builtin_prefetch(&nodePreListStr[path->stp1], 0);
                            r[1].insert4(routeStr, p, *path);
                            resultLen[threadId]++;
                        }
                    }
                }
                NodeIter yIter(getNodeSet(x->vertice));
                for (Edge *iy = yIter.next(); iy; iy = yIter.next()) {
                    const Edge *y = iy;
                    if (y->vertice <= id || !limit(x->weight, y->weight)) continue;
                    //route[2] = y->vertice;
                    // 检查是否有5环
                    if (isThirdNode[y->vertice] == id) {
                        ui p = 0;
                        memcpy(routeStr + p, nodePreListStr[id].str, nodePreListStr[id].len + nodePreListStr[id].add);
                        p += nodePreListStr[id].len;
                        memcpy(routeStr + p, nodePreListStr[x->vertice].str, nodePreListStr[x->vertice].len + nodePreListStr[x->vertice].add);
                        p += nodePreListStr[x->vertice].len;
                        memcpy(routeStr + p, nodePreListStr[y->vertice].str, nodePreListStr[y->vertice].len + nodePreListStr[y->vertice].add);
                        p += nodePreListStr[y->vertice].len;
                        __builtin_prefetch(&r[2], 1);
                        for (int I = pathListHead[y->vertice]; I != -1; I = pathList[I].next) {
                            __builtin_prefetch(&pathList[pathList[I].next], 0);
                            const Path *path = &pathList[I].p;
                            if (x->vertice != path->stp1 &&
                                x->vertice != path->stp2 &&
                                limit(y->weight, path->win) &&
                                limit(path->wout, x->weight)) {
                                __builtin_prefetch(&nodePreListStr[path->stp2], 0);
                                __builtin_prefetch(&nodePreListStr[path->stp1], 0);
                                r[2].insert4(routeStr, p, *path);
                                resultLen[threadId]++;
                            }
                        }
                    }
                    NodeIter zIter(getNodeSet(y->vertice));
                    for (Edge *iz = zIter.next(); iz; iz = zIter.next()) {
                        const Edge *z = iz;
                        // 检查是是否有3环
                        if (id == z->vertice && limit(z->weight, x->weight) && limit(y->weight, z->weight)) {
                            r[0].insert3(id, x->vertice, y->vertice);
                            resultLen[threadId]++;
                            continue;
                        }
                        if (z->vertice <= id || x->vertice == z->vertice || !limit(y->weight, z->weight) || pointer == 0) continue;
                        //route[3] = z->vertice;
                        // 检查是否有6环
                        if (isThirdNode[z->vertice] == id ) {
                            __builtin_prefetch(&r[3], 1);
                            ui p = 0;
                            memcpy(routeStr + p, nodePreListStr[id].str, nodePreListStr[id].len + nodePreListStr[id].add);
                            p += nodePreListStr[id].len;
                            memcpy(routeStr + p, nodePreListStr[x->vertice].str, nodePreListStr[x->vertice].len + nodePreListStr[x->vertice].add);
                            p += nodePreListStr[x->vertice].len;
                            memcpy(routeStr + p, nodePreListStr[y->vertice].str, nodePreListStr[y->vertice].len + nodePreListStr[y->vertice].add);
                            p += nodePreListStr[y->vertice].len;
                            memcpy(routeStr + p, nodePreListStr[z->vertice].str, nodePreListStr[z->vertice].len + nodePreListStr[z->vertice].add);
                            p += nodePreListStr[z->vertice].len;
                            for (int I = pathListHead[z->vertice]; I != -1; I = pathList[I].next) {
                                __builtin_prefetch(&pathList[pathList[I].next], 0);
                                const Path *path = &pathList[I].p;
                                if (x->vertice != path->stp1 && x->vertice != path->stp2 &&
                                    y->vertice != path->stp1 && y->vertice != path->stp2 &&
                                    limit(z->weight, path->win) && limit(path->wout, x->weight) ) {
                                    __builtin_prefetch(&nodePreListStr[path->stp2], 0);
                                    __builtin_prefetch(&nodePreListStr[path->stp1], 0);
                                    r[3].insert4(routeStr, p, *path);
                                    resultLen[threadId]++;
                                }
                            }
                        }
                        NodeIter DIter(getNodeSet(z->vertice));
                        for (Edge *iD = DIter.next(); iD; iD = DIter.next()) {
                            const Edge *D = iD;
                            if (D->vertice <= id|| x->vertice == D->vertice || y->vertice == D->vertice || !limit(z->weight, D->weight) )
                                continue;
                            // Node *nodeD = getNodeSet(D);
                            // 检查是否有7环
                            if (isThirdNode[D->vertice] == id) {
                                __builtin_prefetch(&r[4], 1);
                                //route[4] = D->vertice;
                                ui p = 0;
                                memcpy(routeStr + p, nodePreListStr[id].str, nodePreListStr[id].len + nodePreListStr[id].add);
                                p += nodePreListStr[id].len;
                                memcpy(routeStr + p, nodePreListStr[x->vertice].str, nodePreListStr[x->vertice].len + nodePreListStr[x->vertice].add);
                                p += nodePreListStr[x->vertice].len;
                                memcpy(routeStr + p, nodePreListStr[y->vertice].str, nodePreListStr[y->vertice].len + nodePreListStr[y->vertice].add);
                                p += nodePreListStr[y->vertice].len;
                                memcpy(routeStr + p, nodePreListStr[z->vertice].str, nodePreListStr[z->vertice].len + nodePreListStr[z->vertice].add);
                                p += nodePreListStr[z->vertice].len;
                                memcpy(routeStr + p, nodePreListStr[D->vertice].str, nodePreListStr[D->vertice].len + nodePreListStr[D->vertice].add);
                                p += nodePreListStr[D->vertice].len;
                                for (int I = pathListHead[D->vertice]; I != -1; I = pathList[I].next) {
                                    __builtin_prefetch(&pathList[pathList[I].next], 0);
                                    const Path *path = &pathList[I].p;
                                    if (x->vertice != path->stp1 && x->vertice != path->stp2 &&
                                        y->vertice != path->stp1 && y->vertice != path->stp2 &&
                                        z->vertice != path->stp1 && z->vertice != path->stp2 &&
                                        limit(D->weight, path->win) && limit(path->wout, x->weight)) {
                                        __builtin_prefetch(&nodePreListStr[path->stp2], 0);
                                        __builtin_prefetch(&nodePreListStr[path->stp1], 0);
                                        r[4].insert4(routeStr, p, *path);
                                        resultLen[threadId]++;
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }

        r[0].newGroup();
        r[1].newGroup();
        r[2].newGroup();
        r[3].newGroup();
        r[4].newGroup();
    }
    pthread_spin_unlock(&lock);

#ifdef TEST
    cout << threadId;
    CURTIME(线程结束);
#endif
    if (threadId != 0)
        pthread_exit(nullptr);
}

char *output;
unsigned int startLoc;
unsigned int pFinalNum;
void *outputResult_thread(void *arg){
    int threadId = *((int *) arg);
    unsigned int p = startLoc;

    for (int i = 0; i < 5; i++){
        for(int j=0; j < curGroup; j++){
            if(threadId == groupThread[j])
                result[threadId][i].outputResult(output, p, threadId);
            else
                result[groupThread[j]][i].moveForward(p, threadId);
        }
    }
    pFinalNum = p;
#ifdef TEST
    cout << threadId;
    CURTIME(线程结束);
#endif
}

# define OUTPUTSIZE (sizeof(char) * 20000000 * ((10+1)*7) + 10)
inline void search(char * outputPath) {
#ifdef TEST
    cout <<"nodeNum :" << nodeIDNum << endl;
    cout <<"edgeNum :" << totalDataNum << endl;
#endif
    /*
    for(int i=0; i < nodeIDNum; i++){
        ui id = i;
        Node *node = getNodeSet(id);
        if(!node->isInit){
            continue;
        }
        unsigned long n1 = nodeLessInSize[id], n2 = nodeLargerOutSize[id];
        unsigned long res = n1*n1*n1 + n2*n2*n2*n2;
        totalCountSize += res > COUNTSIZE ? COUNTSIZE : res;
    }
    //sort(nodeIDList, nodeIDList+nodeSetSize);
     */
    int curAddNum = 0;
    for (int id = 0; id < nodeIDNum; id++) {
        Node *node = getNodeSet(id), *inNode = getInNodeSet(id);
        __builtin_prefetch(&nodeSetHashList[id+1], 0);
        __builtin_prefetch(&inNodeList[id+1], 0);

        if(!node->num){
            inNode->num = 0;
            inNode->nextList = nullptr;
            continue;
        }
        if(!inNode->num){
            node->num = 0;
            node->nextList = nullptr;
            continue;
        }

        if(curAddNum > GEOUP_DIVIDING_THRESH){
            groupDividing[groupNum++] = id;
            curAddNum = 0;
        }
        curAddNum += nodeSizeCount[id];

        //nodeDivideTask(id, i);
        nodeSort(*node);
    }
    /*
    for(; gapId < THREADNUM; gapId++){
        threadGap[gapId] = nodeIDNum;
    }
     */
    const int fd = open(outputPath, O_RDWR | O_CREAT, 0666);
#ifdef TEST
    assert(fd >= 0);
#endif

    CURTIME(排序结束);
    const int ret = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    assert(ret == 0);
    pthread_t threads[THREADNUM];
    //cout << nodeSet.size()<<endl;
    for (int j = 1; j < THREADNUM; j++) {
        const int *x = new int(j);
        pthread_create(&threads[j], nullptr, search_thread, (void *) x);
    }
    search_thread(new int(0));
    totalLen = resultLen[0];
    int term = ftruncate(fd, OUTPUTSIZE);
#ifdef TEST
    assert(term == 0);
#endif
    output = (char*) mmap(nullptr, OUTPUTSIZE, PROT_WRITE, MAP_SHARED, fd, 0);
#ifdef  TEST
    assert(MAP_FAILED != output);
#endif

    for (int i = 1; i < THREADNUM; i++) {
        pthread_join(threads[i], nullptr);
        totalLen += resultLen[i];
    }
#ifdef TEST
    cout << totalLen << endl;
#endif
    CURTIME(search);

    unsigned int p = 0;
    writeNum(output, p, totalLen);
    output[p-1] = '\n';
    startLoc = p;

    //pthread_t threads[THREADNUM];
    for (int j = 1; j < THREADNUM; j++) {
        int *x = new int(j);
        pthread_create(&threads[j], nullptr, outputResult_thread, (void *) x);
    }
    outputResult_thread(new int(0));
    for (int i = 1; i < THREADNUM; i++) {
        pthread_join(threads[i], nullptr);
    }
    p = pFinalNum;

    CURTIME(output解析完成);
    int t = munmap(output, OUTPUTSIZE);
#ifdef TEST
    assert(t >= 0);
#endif
    term = ftruncate(fd, p);
#ifdef TEST
    assert(term == 0);
#endif
    //int write_result = write(fd, output, p);
    //assert(write_result == p);
    close(fd);
    CURTIME(outputResult);
}

int main() {

#ifdef TEST
    gettimeofday(&starttime, 0);
    //*
    char inputPath[] = "../data/intermediary/test6/test_data.txt";
    char outputPath[] = "../output/result.txt";
    char answerPath[] = "../data/intermediary/test6/result.txt";
    // */
    /*
    char inputPath[] = "../data/TestData/3512444/test_data.txt";
    char outputPath[] = "../output/result.txt";
    char answerPath[] = "../data/TestData/3512444/result.txt";
    // */
    /*
    char inputPath[] = "../data/test17/test_data.txt";
    char outputPath[] = "../output/result.txt";
    char answerPath[] = "../data/test17/result.txt";
    // */
#else
    //sleep(5);
    char inputPath[] = "/data/test_data.txt";
    char outputPath[] = "/projects/student/result.txt";
#endif
    //memset(nodeSetHashList, 0, sizeof(nodeSetHashList));
    //memset(inNodeList, 0, sizeof(inNodeList));
    //memset(nodeLargerOutSize, 0, sizeof(nodeLargerOutSize));
    //memset(nodeLessInSize, 0, sizeof(nodeLessInSize));
    CURTIME(开始);
    readInput(inputPath);
    CURTIME(readInput);
    treatData();
    CURTIME(treatData);
    search(outputPath);
#ifdef TEST
    ifstream answer(answerPath);
    ifstream output(outputPath);
    int count = 0;
    for (int i = 0; !answer.eof(); i++) {
        string s1, s2;
        getline(answer, s1);
        getline(output, s2);

        if (s1 == "" && s2 == "") {
            break;
        } else if (s1 == "" || s2 == "") {
            cout << "长度不一致" << endl;
            break;
        }

        int len1 = s1.size(), len2 = s2.size();
        bool isSame = true;
        int j;
        for (j = 0; j < len1 && j < len2; j++) {
            bool b1 = (s1[j] >= '0' && s1[j] <= '9') || s1[j] == ',';
            bool b2 = (s2[j] >= '0' && s2[j] <= '9') || s2[j] == ',';
            if (!b1 && !b2) {
                break;
            } else if (!b1 || !b2 || s1[j] != s2[j]) {
                isSame = false;
                break;
            }
        }
        if (!isSame) {
            count++;
            cout << "在第" << i << "行有不同" << endl
                 << "答案：" << s1 << endl
                 << "结果：" << s2 << endl << "###" << endl;
        }
        if (count == 100)
            return 0;
    }
#endif
    exit(0);
}

uint32_t const NUMTOSTR1[] = {
        0x30302c30,0x30302c31,0x30302c32,0x30302c33,0x30302c34,0x30302c35,0x30302c36,0x30302c37,0x30302c38,0x30302c39,
};

uint32_t const NUMTOSTR2[] = {
        0x302c3030,0x302c3130,0x302c3230,0x302c3330,0x302c3430,0x302c3530,0x302c3630,0x302c3730,0x302c3830,0x302c3930,
        0x302c3031,0x302c3131,0x302c3231,0x302c3331,0x302c3431,0x302c3531,0x302c3631,0x302c3731,0x302c3831,0x302c3931,
        0x302c3032,0x302c3132,0x302c3232,0x302c3332,0x302c3432,0x302c3532,0x302c3632,0x302c3732,0x302c3832,0x302c3932,
        0x302c3033,0x302c3133,0x302c3233,0x302c3333,0x302c3433,0x302c3533,0x302c3633,0x302c3733,0x302c3833,0x302c3933,
        0x302c3034,0x302c3134,0x302c3234,0x302c3334,0x302c3434,0x302c3534,0x302c3634,0x302c3734,0x302c3834,0x302c3934,
        0x302c3035,0x302c3135,0x302c3235,0x302c3335,0x302c3435,0x302c3535,0x302c3635,0x302c3735,0x302c3835,0x302c3935,
        0x302c3036,0x302c3136,0x302c3236,0x302c3336,0x302c3436,0x302c3536,0x302c3636,0x302c3736,0x302c3836,0x302c3936,
        0x302c3037,0x302c3137,0x302c3237,0x302c3337,0x302c3437,0x302c3537,0x302c3637,0x302c3737,0x302c3837,0x302c3937,
        0x302c3038,0x302c3138,0x302c3238,0x302c3338,0x302c3438,0x302c3538,0x302c3638,0x302c3738,0x302c3838,0x302c3938,
        0x302c3039,0x302c3139,0x302c3239,0x302c3339,0x302c3439,0x302c3539,0x302c3639,0x302c3739,0x302c3839,0x302c3939,
};

uint32_t const NUMTOSTR3[] = {
        0x2c303030,0x2c313030,0x2c323030,0x2c333030,0x2c343030,0x2c353030,0x2c363030,0x2c373030,0x2c383030,0x2c393030,
        0x2c303130,0x2c313130,0x2c323130,0x2c333130,0x2c343130,0x2c353130,0x2c363130,0x2c373130,0x2c383130,0x2c393130,
        0x2c303230,0x2c313230,0x2c323230,0x2c333230,0x2c343230,0x2c353230,0x2c363230,0x2c373230,0x2c383230,0x2c393230,
        0x2c303330,0x2c313330,0x2c323330,0x2c333330,0x2c343330,0x2c353330,0x2c363330,0x2c373330,0x2c383330,0x2c393330,
        0x2c303430,0x2c313430,0x2c323430,0x2c333430,0x2c343430,0x2c353430,0x2c363430,0x2c373430,0x2c383430,0x2c393430,
        0x2c303530,0x2c313530,0x2c323530,0x2c333530,0x2c343530,0x2c353530,0x2c363530,0x2c373530,0x2c383530,0x2c393530,
        0x2c303630,0x2c313630,0x2c323630,0x2c333630,0x2c343630,0x2c353630,0x2c363630,0x2c373630,0x2c383630,0x2c393630,
        0x2c303730,0x2c313730,0x2c323730,0x2c333730,0x2c343730,0x2c353730,0x2c363730,0x2c373730,0x2c383730,0x2c393730,
        0x2c303830,0x2c313830,0x2c323830,0x2c333830,0x2c343830,0x2c353830,0x2c363830,0x2c373830,0x2c383830,0x2c393830,
        0x2c303930,0x2c313930,0x2c323930,0x2c333930,0x2c343930,0x2c353930,0x2c363930,0x2c373930,0x2c383930,0x2c393930,
        0x2c303031,0x2c313031,0x2c323031,0x2c333031,0x2c343031,0x2c353031,0x2c363031,0x2c373031,0x2c383031,0x2c393031,
        0x2c303131,0x2c313131,0x2c323131,0x2c333131,0x2c343131,0x2c353131,0x2c363131,0x2c373131,0x2c383131,0x2c393131,
        0x2c303231,0x2c313231,0x2c323231,0x2c333231,0x2c343231,0x2c353231,0x2c363231,0x2c373231,0x2c383231,0x2c393231,
        0x2c303331,0x2c313331,0x2c323331,0x2c333331,0x2c343331,0x2c353331,0x2c363331,0x2c373331,0x2c383331,0x2c393331,
        0x2c303431,0x2c313431,0x2c323431,0x2c333431,0x2c343431,0x2c353431,0x2c363431,0x2c373431,0x2c383431,0x2c393431,
        0x2c303531,0x2c313531,0x2c323531,0x2c333531,0x2c343531,0x2c353531,0x2c363531,0x2c373531,0x2c383531,0x2c393531,
        0x2c303631,0x2c313631,0x2c323631,0x2c333631,0x2c343631,0x2c353631,0x2c363631,0x2c373631,0x2c383631,0x2c393631,
        0x2c303731,0x2c313731,0x2c323731,0x2c333731,0x2c343731,0x2c353731,0x2c363731,0x2c373731,0x2c383731,0x2c393731,
        0x2c303831,0x2c313831,0x2c323831,0x2c333831,0x2c343831,0x2c353831,0x2c363831,0x2c373831,0x2c383831,0x2c393831,
        0x2c303931,0x2c313931,0x2c323931,0x2c333931,0x2c343931,0x2c353931,0x2c363931,0x2c373931,0x2c383931,0x2c393931,
        0x2c303032,0x2c313032,0x2c323032,0x2c333032,0x2c343032,0x2c353032,0x2c363032,0x2c373032,0x2c383032,0x2c393032,
        0x2c303132,0x2c313132,0x2c323132,0x2c333132,0x2c343132,0x2c353132,0x2c363132,0x2c373132,0x2c383132,0x2c393132,
        0x2c303232,0x2c313232,0x2c323232,0x2c333232,0x2c343232,0x2c353232,0x2c363232,0x2c373232,0x2c383232,0x2c393232,
        0x2c303332,0x2c313332,0x2c323332,0x2c333332,0x2c343332,0x2c353332,0x2c363332,0x2c373332,0x2c383332,0x2c393332,
        0x2c303432,0x2c313432,0x2c323432,0x2c333432,0x2c343432,0x2c353432,0x2c363432,0x2c373432,0x2c383432,0x2c393432,
        0x2c303532,0x2c313532,0x2c323532,0x2c333532,0x2c343532,0x2c353532,0x2c363532,0x2c373532,0x2c383532,0x2c393532,
        0x2c303632,0x2c313632,0x2c323632,0x2c333632,0x2c343632,0x2c353632,0x2c363632,0x2c373632,0x2c383632,0x2c393632,
        0x2c303732,0x2c313732,0x2c323732,0x2c333732,0x2c343732,0x2c353732,0x2c363732,0x2c373732,0x2c383732,0x2c393732,
        0x2c303832,0x2c313832,0x2c323832,0x2c333832,0x2c343832,0x2c353832,0x2c363832,0x2c373832,0x2c383832,0x2c393832,
        0x2c303932,0x2c313932,0x2c323932,0x2c333932,0x2c343932,0x2c353932,0x2c363932,0x2c373932,0x2c383932,0x2c393932,
        0x2c303033,0x2c313033,0x2c323033,0x2c333033,0x2c343033,0x2c353033,0x2c363033,0x2c373033,0x2c383033,0x2c393033,
        0x2c303133,0x2c313133,0x2c323133,0x2c333133,0x2c343133,0x2c353133,0x2c363133,0x2c373133,0x2c383133,0x2c393133,
        0x2c303233,0x2c313233,0x2c323233,0x2c333233,0x2c343233,0x2c353233,0x2c363233,0x2c373233,0x2c383233,0x2c393233,
        0x2c303333,0x2c313333,0x2c323333,0x2c333333,0x2c343333,0x2c353333,0x2c363333,0x2c373333,0x2c383333,0x2c393333,
        0x2c303433,0x2c313433,0x2c323433,0x2c333433,0x2c343433,0x2c353433,0x2c363433,0x2c373433,0x2c383433,0x2c393433,
        0x2c303533,0x2c313533,0x2c323533,0x2c333533,0x2c343533,0x2c353533,0x2c363533,0x2c373533,0x2c383533,0x2c393533,
        0x2c303633,0x2c313633,0x2c323633,0x2c333633,0x2c343633,0x2c353633,0x2c363633,0x2c373633,0x2c383633,0x2c393633,
        0x2c303733,0x2c313733,0x2c323733,0x2c333733,0x2c343733,0x2c353733,0x2c363733,0x2c373733,0x2c383733,0x2c393733,
        0x2c303833,0x2c313833,0x2c323833,0x2c333833,0x2c343833,0x2c353833,0x2c363833,0x2c373833,0x2c383833,0x2c393833,
        0x2c303933,0x2c313933,0x2c323933,0x2c333933,0x2c343933,0x2c353933,0x2c363933,0x2c373933,0x2c383933,0x2c393933,
        0x2c303034,0x2c313034,0x2c323034,0x2c333034,0x2c343034,0x2c353034,0x2c363034,0x2c373034,0x2c383034,0x2c393034,
        0x2c303134,0x2c313134,0x2c323134,0x2c333134,0x2c343134,0x2c353134,0x2c363134,0x2c373134,0x2c383134,0x2c393134,
        0x2c303234,0x2c313234,0x2c323234,0x2c333234,0x2c343234,0x2c353234,0x2c363234,0x2c373234,0x2c383234,0x2c393234,
        0x2c303334,0x2c313334,0x2c323334,0x2c333334,0x2c343334,0x2c353334,0x2c363334,0x2c373334,0x2c383334,0x2c393334,
        0x2c303434,0x2c313434,0x2c323434,0x2c333434,0x2c343434,0x2c353434,0x2c363434,0x2c373434,0x2c383434,0x2c393434,
        0x2c303534,0x2c313534,0x2c323534,0x2c333534,0x2c343534,0x2c353534,0x2c363534,0x2c373534,0x2c383534,0x2c393534,
        0x2c303634,0x2c313634,0x2c323634,0x2c333634,0x2c343634,0x2c353634,0x2c363634,0x2c373634,0x2c383634,0x2c393634,
        0x2c303734,0x2c313734,0x2c323734,0x2c333734,0x2c343734,0x2c353734,0x2c363734,0x2c373734,0x2c383734,0x2c393734,
        0x2c303834,0x2c313834,0x2c323834,0x2c333834,0x2c343834,0x2c353834,0x2c363834,0x2c373834,0x2c383834,0x2c393834,
        0x2c303934,0x2c313934,0x2c323934,0x2c333934,0x2c343934,0x2c353934,0x2c363934,0x2c373934,0x2c383934,0x2c393934,
        0x2c303035,0x2c313035,0x2c323035,0x2c333035,0x2c343035,0x2c353035,0x2c363035,0x2c373035,0x2c383035,0x2c393035,
        0x2c303135,0x2c313135,0x2c323135,0x2c333135,0x2c343135,0x2c353135,0x2c363135,0x2c373135,0x2c383135,0x2c393135,
        0x2c303235,0x2c313235,0x2c323235,0x2c333235,0x2c343235,0x2c353235,0x2c363235,0x2c373235,0x2c383235,0x2c393235,
        0x2c303335,0x2c313335,0x2c323335,0x2c333335,0x2c343335,0x2c353335,0x2c363335,0x2c373335,0x2c383335,0x2c393335,
        0x2c303435,0x2c313435,0x2c323435,0x2c333435,0x2c343435,0x2c353435,0x2c363435,0x2c373435,0x2c383435,0x2c393435,
        0x2c303535,0x2c313535,0x2c323535,0x2c333535,0x2c343535,0x2c353535,0x2c363535,0x2c373535,0x2c383535,0x2c393535,
        0x2c303635,0x2c313635,0x2c323635,0x2c333635,0x2c343635,0x2c353635,0x2c363635,0x2c373635,0x2c383635,0x2c393635,
        0x2c303735,0x2c313735,0x2c323735,0x2c333735,0x2c343735,0x2c353735,0x2c363735,0x2c373735,0x2c383735,0x2c393735,
        0x2c303835,0x2c313835,0x2c323835,0x2c333835,0x2c343835,0x2c353835,0x2c363835,0x2c373835,0x2c383835,0x2c393835,
        0x2c303935,0x2c313935,0x2c323935,0x2c333935,0x2c343935,0x2c353935,0x2c363935,0x2c373935,0x2c383935,0x2c393935,
        0x2c303036,0x2c313036,0x2c323036,0x2c333036,0x2c343036,0x2c353036,0x2c363036,0x2c373036,0x2c383036,0x2c393036,
        0x2c303136,0x2c313136,0x2c323136,0x2c333136,0x2c343136,0x2c353136,0x2c363136,0x2c373136,0x2c383136,0x2c393136,
        0x2c303236,0x2c313236,0x2c323236,0x2c333236,0x2c343236,0x2c353236,0x2c363236,0x2c373236,0x2c383236,0x2c393236,
        0x2c303336,0x2c313336,0x2c323336,0x2c333336,0x2c343336,0x2c353336,0x2c363336,0x2c373336,0x2c383336,0x2c393336,
        0x2c303436,0x2c313436,0x2c323436,0x2c333436,0x2c343436,0x2c353436,0x2c363436,0x2c373436,0x2c383436,0x2c393436,
        0x2c303536,0x2c313536,0x2c323536,0x2c333536,0x2c343536,0x2c353536,0x2c363536,0x2c373536,0x2c383536,0x2c393536,
        0x2c303636,0x2c313636,0x2c323636,0x2c333636,0x2c343636,0x2c353636,0x2c363636,0x2c373636,0x2c383636,0x2c393636,
        0x2c303736,0x2c313736,0x2c323736,0x2c333736,0x2c343736,0x2c353736,0x2c363736,0x2c373736,0x2c383736,0x2c393736,
        0x2c303836,0x2c313836,0x2c323836,0x2c333836,0x2c343836,0x2c353836,0x2c363836,0x2c373836,0x2c383836,0x2c393836,
        0x2c303936,0x2c313936,0x2c323936,0x2c333936,0x2c343936,0x2c353936,0x2c363936,0x2c373936,0x2c383936,0x2c393936,
        0x2c303037,0x2c313037,0x2c323037,0x2c333037,0x2c343037,0x2c353037,0x2c363037,0x2c373037,0x2c383037,0x2c393037,
        0x2c303137,0x2c313137,0x2c323137,0x2c333137,0x2c343137,0x2c353137,0x2c363137,0x2c373137,0x2c383137,0x2c393137,
        0x2c303237,0x2c313237,0x2c323237,0x2c333237,0x2c343237,0x2c353237,0x2c363237,0x2c373237,0x2c383237,0x2c393237,
        0x2c303337,0x2c313337,0x2c323337,0x2c333337,0x2c343337,0x2c353337,0x2c363337,0x2c373337,0x2c383337,0x2c393337,
        0x2c303437,0x2c313437,0x2c323437,0x2c333437,0x2c343437,0x2c353437,0x2c363437,0x2c373437,0x2c383437,0x2c393437,
        0x2c303537,0x2c313537,0x2c323537,0x2c333537,0x2c343537,0x2c353537,0x2c363537,0x2c373537,0x2c383537,0x2c393537,
        0x2c303637,0x2c313637,0x2c323637,0x2c333637,0x2c343637,0x2c353637,0x2c363637,0x2c373637,0x2c383637,0x2c393637,
        0x2c303737,0x2c313737,0x2c323737,0x2c333737,0x2c343737,0x2c353737,0x2c363737,0x2c373737,0x2c383737,0x2c393737,
        0x2c303837,0x2c313837,0x2c323837,0x2c333837,0x2c343837,0x2c353837,0x2c363837,0x2c373837,0x2c383837,0x2c393837,
        0x2c303937,0x2c313937,0x2c323937,0x2c333937,0x2c343937,0x2c353937,0x2c363937,0x2c373937,0x2c383937,0x2c393937,
        0x2c303038,0x2c313038,0x2c323038,0x2c333038,0x2c343038,0x2c353038,0x2c363038,0x2c373038,0x2c383038,0x2c393038,
        0x2c303138,0x2c313138,0x2c323138,0x2c333138,0x2c343138,0x2c353138,0x2c363138,0x2c373138,0x2c383138,0x2c393138,
        0x2c303238,0x2c313238,0x2c323238,0x2c333238,0x2c343238,0x2c353238,0x2c363238,0x2c373238,0x2c383238,0x2c393238,
        0x2c303338,0x2c313338,0x2c323338,0x2c333338,0x2c343338,0x2c353338,0x2c363338,0x2c373338,0x2c383338,0x2c393338,
        0x2c303438,0x2c313438,0x2c323438,0x2c333438,0x2c343438,0x2c353438,0x2c363438,0x2c373438,0x2c383438,0x2c393438,
        0x2c303538,0x2c313538,0x2c323538,0x2c333538,0x2c343538,0x2c353538,0x2c363538,0x2c373538,0x2c383538,0x2c393538,
        0x2c303638,0x2c313638,0x2c323638,0x2c333638,0x2c343638,0x2c353638,0x2c363638,0x2c373638,0x2c383638,0x2c393638,
        0x2c303738,0x2c313738,0x2c323738,0x2c333738,0x2c343738,0x2c353738,0x2c363738,0x2c373738,0x2c383738,0x2c393738,
        0x2c303838,0x2c313838,0x2c323838,0x2c333838,0x2c343838,0x2c353838,0x2c363838,0x2c373838,0x2c383838,0x2c393838,
        0x2c303938,0x2c313938,0x2c323938,0x2c333938,0x2c343938,0x2c353938,0x2c363938,0x2c373938,0x2c383938,0x2c393938,
        0x2c303039,0x2c313039,0x2c323039,0x2c333039,0x2c343039,0x2c353039,0x2c363039,0x2c373039,0x2c383039,0x2c393039,
        0x2c303139,0x2c313139,0x2c323139,0x2c333139,0x2c343139,0x2c353139,0x2c363139,0x2c373139,0x2c383139,0x2c393139,
        0x2c303239,0x2c313239,0x2c323239,0x2c333239,0x2c343239,0x2c353239,0x2c363239,0x2c373239,0x2c383239,0x2c393239,
        0x2c303339,0x2c313339,0x2c323339,0x2c333339,0x2c343339,0x2c353339,0x2c363339,0x2c373339,0x2c383339,0x2c393339,
        0x2c303439,0x2c313439,0x2c323439,0x2c333439,0x2c343439,0x2c353439,0x2c363439,0x2c373439,0x2c383439,0x2c393439,
        0x2c303539,0x2c313539,0x2c323539,0x2c333539,0x2c343539,0x2c353539,0x2c363539,0x2c373539,0x2c383539,0x2c393539,
        0x2c303639,0x2c313639,0x2c323639,0x2c333639,0x2c343639,0x2c353639,0x2c363639,0x2c373639,0x2c383639,0x2c393639,
        0x2c303739,0x2c313739,0x2c323739,0x2c333739,0x2c343739,0x2c353739,0x2c363739,0x2c373739,0x2c383739,0x2c393739,
        0x2c303839,0x2c313839,0x2c323839,0x2c333839,0x2c343839,0x2c353839,0x2c363839,0x2c373839,0x2c383839,0x2c393839,
        0x2c303939,0x2c313939,0x2c323939,0x2c333939,0x2c343939,0x2c353939,0x2c363939,0x2c373939,0x2c383939,0x2c393939,
};