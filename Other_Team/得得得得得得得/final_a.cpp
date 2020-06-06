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
typedef uint32_t uEdge;
#define E_MAXVALUE ((uEdge)-1) //uEdge最大值宏定义
//#define E_MAXVALUE 0xffffffff
#ifdef TEST

#include <sys/time.h>

timeval starttime, endtime;
#define CURTIME(x) \
    gettimeofday(&endtime,0); \
    cout << #x << ": " << (double)(1000000*(endtime.tv_sec - starttime.tv_sec) + endtime.tv_usec - starttime.tv_usec) / 1000000 << endl
#define Assert(...) assert(__VA_ARGS__)
#else
#define CURTIME(x)
#define Assert(...)
#endif

#define THREADNUM 8
#define SIXTYTHOUSAND (2800000 / THREADNUM + 10000)
#define MAXDATANUM (1u<<21u)

ui *inputTemp[THREADNUM]; //每个线程输入读取后存放的地方
int dataNum[THREADNUM]; //每个线程数组储存的数量
int readStart[THREADNUM]; // 输入时读的开头
int dataSum[THREADNUM]; //从第一个线程开始的累加和

char *inputStr; //输入时mmap映射的地址
int inputStrLen; //输入字符的长度
int totalDataNum = 0; //输入的边数总和

//计算边权值并进行动态的策略调整
uint64_t totalEdgeWeightThread[THREADNUM];
enum{CommonHeap, DistHeap} curMethod = CommonHeap;
#define COMMON_DIST_THREAD 200

//边的结构体定义
typedef struct Edge {
    ui start;
    ui end;
    uEdge weight;

    bool operator<(const Edge &a) const { return start < a.start; }

    bool operator>(const Edge &a) const { return start > a.start; }
} Edge;
typedef struct NewEdge{
    ui end;
    uEdge weight;
} NewEdge;
/**
 * 在treatData函数调用，读取输入的所有线程所读取的边
 * @param i 输入的index
 * @return 返回对应index的边地址
 * **/
inline ui *readInputData(const ui i) {
    if (i < dataSum[0])
        return inputTemp[0] + i * 3;
    for (int x = 1; x < THREADNUM; x++)
        if (i < dataSum[x])
            return inputTemp[x] + (i - dataSum[x - 1]) * 3;
}

/**
 * 读取输入的线程
 * @param threadId 线程ID
 * **/
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
        if (*(inputTemp[id] + i * 3 + 2) == 0) {
            *(inputTemp[id] + i * 3 + 0) = 0;
            *(inputTemp[id] + i * 3 + 1) = 0;
            i--;
            continue;
        }
        totalEdgeWeightThread[id] += inputTemp[id][2];
        dataNum[id]++;
    }
    if(id)
        pthread_exit(nullptr);
}

/**
 * 读取输入并进行线程分配的函数
 * @param inputPath 读取输入的路径
 * **/
inline void readInput(const char *inputPath) {
    const int fd = open(inputPath, O_RDONLY, 0666);
    Assert(fd >= 0);
    struct stat st{};
    fstat(fd, &st);
    inputStrLen = st.st_size;
    const int readBitNum = inputStrLen / THREADNUM;
    //cout << inputStrLen << endl;
    inputStr = (char *) mmap(nullptr, inputStrLen, PROT_READ, MAP_PRIVATE, fd, 0);
    Assert(inputStr != MAP_FAILED);

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
    for (int i = 1; i < THREADNUM; i++) {
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
    readLineThread(new int(0));
    totalDataNum = totalDataNum + dataNum[0];
    dataSum[0] = totalDataNum;
    uint64_t totalEdgeWeight = totalEdgeWeightThread[0];

    for (int i = 1; i < THREADNUM; i++) {
        pthread_join(threads[i], nullptr);
        totalDataNum = totalDataNum + dataNum[i];
        dataSum[i] = totalDataNum;
        totalEdgeWeight += totalEdgeWeightThread[i];
    }
    // 判断使用那种方法
    if(totalEdgeWeight / totalDataNum < COMMON_DIST_THREAD){
        curMethod = DistHeap;
#ifdef TEST
        cout << "使用DisDarytHeap" << endl;
#endif
    }
#ifdef TEST
    else{
        cout << "使用DistHeap" << endl;
    }
    //cout << "决策数值：" << totalEdgeWeight << " " << totalDataNum << " " << COMMON_DIST_THREAD << endl;
#endif
}

//id映射使用的结构体
typedef struct IDMap {
    bool isInit = false;
    ui val;
    ui mapedVal;
} IDMap;
IDMap nodeIDMap[MAXDATANUM];

/**
 * 检查映射前的ID是不是被存放在映射表里面
 * @param list 映射的表(哈希实现)
 * @param i 映射前ID
 * @return 是否被初始化
 * **/
bool InitMap(IDMap *list, const ui i) {
    ui id = i & (MAXDATANUM - 1);
    if (!list[id].isInit) {
        list[id].isInit = true;
        list[id].val = i;
        return true;
    } else {
        if (list[id].val == i)
            return false;
        else {
            do {
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

/**
 * 进行映射
 * @param i 映射前ID
 * @param list 映射的表(哈希实现)
 * @param k 映射后ID
 * **/
void MapSet(IDMap *list, ui i, const ui k) {
    ui id = i & (MAXDATANUM - 1);
    while (list[id].val != i)
        id++;
    list[id].mapedVal = k;
}

/**
 * 取得映射后ID
 * @param i 映射前ID
 * @param list 映射的表(哈希实现)
 * @return 映射后ID
 * **/
ui MapGet(IDMap *list, const ui i) {
    ui id = i & (MAXDATANUM - 1);
    while (list[id].val != i)
        id++;
    return list[id].mapedVal;
}

Edge edgeData[MAXDATANUM]; //存放所有边
NewEdge newEdge[MAXDATANUM]; //进行dijk时用的edge
ui edgeDataNum[MAXDATANUM + 1]; //存放每个点的边的起始位置
uint16_t preNode[MAXDATANUM]; //存放每个点的入度为0出度为1的前向点
ui nodeIDNum = 0; //映射后点的数量
ui nodePreList[MAXDATANUM]; //保存结点
ui calculateID[MAXDATANUM]; //需要计算的ID
//ui inNodeNum[MAXDATANUM];
ui calculateIDNum = 0; //需要计算的ID数量

inline void treatData() {
    nodeIDNum = 0;
    for (int i = 0; i < totalDataNum; i++) {
        const ui *edge = readInputData(i);
        const ui start = edge[0];
        const ui end = edge[1];
        if (InitMap(nodeIDMap, start)) {
            nodePreList[nodeIDNum] = start;
            nodeIDNum++;
        }
        if (InitMap(nodeIDMap, end)) {
            nodePreList[nodeIDNum] = end;
            nodeIDNum++;
        }
    }
    CURTIME(点统计结束);
    sort(nodePreList, nodePreList + nodeIDNum);
    for (int i = 0; i < nodeIDNum; i++) {
        MapSet(nodeIDMap, nodePreList[i], i);
        preNode[i] = 1;
    }
    CURTIME(映射完成);
    ui *inNodeNum = (ui *) malloc(sizeof(ui) * nodeIDNum);
    memset(inNodeNum, 0u, sizeof(ui) * nodeIDNum);
    for (int i = 0; i < totalDataNum; i++) {
        ui *edge = readInputData(i);
        ui start = MapGet(nodeIDMap, edge[0]);
        ui end = MapGet(nodeIDMap, edge[1]);
        ui weight = edge[2];
        edgeData[i] = {start, end, weight};
        *(inNodeNum + end) = *(inNodeNum + end) + 1;
    }

    sort(edgeData, edgeData + totalDataNum);
    int x = 0;
    edgeDataNum[x] = 0;
    for (int i = 0; i < totalDataNum; i++) {
        newEdge[i] = {edgeData[i].end, edgeData[i].weight};
        while (edgeData[i].start != x) {
            edgeDataNum[++x] = i;
        }
    }
    while (x != nodeIDNum) {
        edgeDataNum[++x] = totalDataNum;
    }

    //将入度为0出度为1的点与下一个点合并
    calculateIDNum = 0;
    for (int i = 0; i < nodeIDNum; i++) {
        if ((edgeDataNum[i + 1] - edgeDataNum[i]) == 1 && inNodeNum[i] == 0) {
            //Node *node1 = getNodeSet(node->node[0].end);
            preNode[edgeData[edgeDataNum[i]].end]++;
        } else {
            calculateID[calculateIDNum++] = i;
        }
    }
    CURTIME(建图完成);
    //cout << "treatdata end" <<endl;
    free(inNodeNum);
    for (int i = 0; i < THREADNUM; i++) {
        free(inputTemp[i]);
    }
#ifdef TEST
    cout << "nodeNum:" << nodeIDNum << endl;
    cout << "edgeNum:" << totalDataNum << endl;
#endif
}

//进行dijk需要保存的东西
typedef struct RoadNode {
    uint16_t roadNum[MAXDATANUM]; // 初始点到当前点的路径条数
    uEdge dist[MAXDATANUM]; //初始点到当前点的距离
    double weight[MAXDATANUM]; //初始点到当前点的权值
    ui preIter[MAXDATANUM]; //路径前向迭代器

    inline void init() {
        memset(weight, 0, sizeof(weight));
        memset(dist, -1, sizeof(dist));
    }

    inline void clear(ui id) {
        //roadNum[id] = 0;
        weight[id] = 0.0;
        dist[id] = E_MAXVALUE;
        //preIter[id] = -1;
    }
} RoadNode;
RoadNode roadNodeThread[THREADNUM];

#define PATHNUM 1000000
//存放路径
typedef struct Path {
    ui id;
    ui next;
} Path;
Path pathListThreads[THREADNUM][PATHNUM];

double centralityThread[THREADNUM][MAXDATANUM];
ui centralityID[MAXDATANUM];
//最后对中心性排序时用的比较函数
bool cmp(ui &a, ui &b) {
    double diff = centralityThread[0][a] - centralityThread[0][b];
    return diff > 0.0001 || (diff >= -0.0001 && a < b);
}

//堆里面的结构
typedef struct QueueNode {
    ui id;
    uEdge dist;

    bool operator<(const struct QueueNode &a) const {
        return dist < a.dist;
    }
    /*
    inline QueueNode() : dist(E_MAXVALUE) {}
    explicit inline QueueNode(ui id, uEdge dist) : id(id), dist(dist) {}
     */
} QueueNode;

/**
 * 多叉堆
 * **/
#define DARYHEAPNUMOFFSET 3u
#define DARYHEAPNUM (1u<<(DARYHEAPNUMOFFSET))
typedef struct DaryHeap {
    QueueNode heap[MAXDATANUM];
    int size;

    inline void init() {
        memset(heap, -1, sizeof(heap));
        size = 0;
    }

    inline int parentIndex(int loc) {
        return (loc - 1) >> DARYHEAPNUMOFFSET;
    }

    inline int childIndex(int index, int order) {
        return (index << DARYHEAPNUMOFFSET) + order;
    }

    inline void push(const ui id, const uEdge dist) {
        const QueueNode q = {id, dist};
        int i = size, j = parentIndex(size);
        for (; i > 0 && q.dist < heap[j].dist; i = j, j = parentIndex(j)) {
            heap[i] = heap[j];
        }
        heap[i] = q;
        size++;
    }

    inline QueueNode pop() {
        size--;
        const QueueNode q = heap[size];
        int i = 0, j = 0;
        const QueueNode ret = heap[0];
        for (; i < size; heap[i] = heap[j], i = j) {
            // 查找当前位置的出节点和q谁是最小的
            uEdge minDist = q.dist;
            for (int c = 1; c <= DARYHEAPNUM; c++) {
                int ci = childIndex(i, c);
                if (ci >= size) break;
                if (heap[ci].dist < minDist) {
                    minDist = heap[ci].dist;
                    j = ci;
                }
            }

            if (i == j) {
                break;
            }
        }

        heap[i] = q;
        return ret;
    }

    inline bool empty() {
        return size == 0;
    }
} DaryHeap;
DaryHeap daryHeapThread[THREADNUM];

/**
 * 数组 + 多叉堆
 * **/
#define DISTDARYHEAPNUMOFFSET 3u
#define DISTDARYHEAPNUM (1u<<(DISTDARYHEAPNUMOFFSET))
#define MAXDISTDISTNUM 1000
#define DISTDARYHEAPLISTSIZE 10000
typedef struct DistDaryHeap {
    //int size;
    short lQueue[MAXDISTDISTNUM];
    short min;
    ui node[MAXDISTDISTNUM][DISTDARYHEAPLISTSIZE];
    DaryHeap daryHeap;
    inline void init() {
        //size = 0;
        memset(lQueue, 0, sizeof(lQueue));
        daryHeap.init();
        reset();
    }

    inline void reset() {
        //Assert(size == 0);
        min = MAXDISTDISTNUM;
    }
    inline void push(const ui qid, const uEdge qdist) {
        if(qdist < MAXDISTDISTNUM){
            //Assert(size);
            if(qdist < min){
                min = qdist;
            }
            node[qdist][lQueue[qdist]] = qid;
            ++lQueue[qdist];
            //++size;
        }
        else{
            daryHeap.push(qid, qdist);
        }
        //Assert(size);
    }
    ui* popArray(short &n,uEdge &dist){
        dist = min;
        ui *res = node[min];
        n = lQueue[min];
        //size -= lQueue[min];
        lQueue[min] = 0;
        for(;min < MAXDISTDISTNUM;++min){
            if(lQueue[min] > 0)
		        break;
        }
        return res;
    }
    inline QueueNode pop() {
        return daryHeap.pop();
    }
    inline bool empty() {
        return min == MAXDISTDISTNUM && daryHeap.empty();
    }
} DistDaryHeap;
DistDaryHeap distDaryHeapThread[THREADNUM];

pthread_spinlock_t lock;
int calculatePointer = 0;
#define GROUPSIZE 64

/**
 * 计算中心性的线程
 * @param arg 线程ID
 * **/
void *calculate_thread(void *arg) {
    int threadId = *((int *) arg);
    double *centrality = centralityThread[threadId];
    Path *pathList = pathListThreads[threadId];
    RoadNode *roadNode = &roadNodeThread[threadId];
    //cout << "begin thread " << threadId <<endl;
    uint16_t *roadNum = roadNode->roadNum; // 初始点到当前点的路径条数
    uEdge *dist = roadNode->dist;
    double *weight = roadNode->weight;
    ui *preIter = roadNode->preIter;

    auto *record = (int *) (malloc(sizeof(int) * (nodeIDNum + 10000)));
    roadNode->init();
    DaryHeap *q1 = &daryHeapThread[threadId];
    q1->init();
    DistDaryHeap *q2 = &distDaryHeapThread[threadId];
    q2->init();
    const auto method = curMethod;
    const ui localCalculateIDNum = calculateIDNum;
    while (true) {
        pthread_spin_lock(&lock);
        if (calculatePointer >= localCalculateIDNum) {
            pthread_spin_unlock(&lock);
            break;
        }
        const int start = calculatePointer;
        calculatePointer += GROUPSIZE;
        pthread_spin_unlock(&lock);
        const int end = start + GROUPSIZE >= localCalculateIDNum ? localCalculateIDNum : start + GROUPSIZE;
        for (int x = start; x < end; x++) {
            int id = calculateID[x];
            //cout << "current " << nodePreList[id]  <<endl;
            int pointer = 0;
            int curpos = 1;
            dist[id] = 0;
            roadNum[id] = 1;
            preIter[id] = 0;
            double idCentrality = centrality[id];
            if(method == DistHeap){
                DistDaryHeap *q = q2;
                q->reset();
                q->push(id, 0);
                while (!q->empty()) {
                    if(q->min < MAXDISTDISTNUM){
                        short num; // = -1;
                        uEdge qdist; // = 5000;
                        ui *idList = q->popArray(num,qdist);
                        //Assert(num != -1 && qdist!= 5000);
                        for(short k = 0;k<num;k++){
                            const ui u = idList[k];
                            if (dist[u] < qdist)
                                continue;
                            record[pointer++] = u;
                            const ui limit = edgeDataNum[u + 1];
                            for (ui i = edgeDataNum[u]; i < limit; i++) {
                                NewEdge *e = &newEdge[i];
                                //RoadNode *v = &roadNode[e->end];
                                const int v = e->end;
                                const uEdge newDist = dist[u] + e->weight;
                                if (dist[v] == newDist) {
                                    roadNum[v] += roadNum[u];
                                    pathList[curpos] = {u, preIter[v]};
                                    preIter[v] = curpos;
                                    curpos++;
                                } else if (dist[v] > newDist) {
                                    dist[v] = newDist;
                                    q->push(e->end, newDist);
                                    //cout << "push " << nodePreList[e->end] << " " << newDist <<endl;
                                    roadNum[v] = roadNum[u];
                                    pathList[curpos] = {u, 0};
                                    preIter[v] = curpos;
                                    curpos++;
                                }
                            }
                        }
                    }else{
                        QueueNode queueNode = q->pop();
                        //RoadNode *u = &roadNode[queueNode.id];
                        //cout << "pop " << nodePreList[queueNode.id] << " " << queueNode.dist <<endl;
                        const ui u = queueNode.id;
                        if (dist[u] < queueNode.dist)
                            continue;
                        record[pointer++] = u;
                        ui limit = edgeDataNum[u + 1];
                        for (ui i = edgeDataNum[u]; i < limit; i++) {
                            NewEdge *e = &newEdge[i];
                            //RoadNode *v = &roadNode[e->end];
                            const ui v = e->end;
                            uEdge newDist = dist[u] + e->weight;
                            if (dist[v] == newDist) {
                                roadNum[v] += roadNum[u];
                                pathList[curpos] = {u, preIter[v]};
                                preIter[v] = curpos;
                                curpos++;
                            } else if (dist[v] > newDist) {
                                dist[v] = newDist;
                                q->push(e->end, newDist);
                                //cout << "push " << nodePreList[e->end] << " " << newDist <<endl;
                                roadNum[v] = roadNum[u];
                                pathList[curpos] = {u, 0};
                                preIter[v] = curpos;
                                curpos++;
                            }
                        }
                    }
                }
            }
            else if(method == CommonHeap){
                DaryHeap *q = q1;
                q->push(id, 0);
                while (!q->empty()) {
                    QueueNode queueNode = q->pop();
                    //RoadNode *u = &roadNode[queueNode.id];
                    //cout << "pop " << nodePreList[queueNode.id] << " " << queueNode.dist <<endl;
                    const ui u = queueNode.id;
                    if (dist[u] < queueNode.dist)
                        continue;
                    record[pointer++] = u;
                    const ui limit = edgeDataNum[u + 1];
                    for (ui i = edgeDataNum[u]; i < limit; i++) {
                        NewEdge *e = &newEdge[i];
                        //RoadNode *v = &roadNode[e->end];
                        const ui v = e->end;
                        const uEdge newDist = dist[u] + e->weight;
                        if (dist[v] == newDist) {
                            roadNum[v] += roadNum[u];
                            pathList[curpos] = {u, preIter[v]};
                            preIter[v] = curpos;
                            curpos++;
                        } else if (dist[v] > newDist) {
                            dist[v] = newDist;
                            q->push(e->end, newDist);
                            //cout << "push " << nodePreList[e->end] << " " << newDist <<endl;
                            roadNum[v] = roadNum[u];
                            pathList[curpos] = {u, 0};
                            preIter[v] = curpos;
                            curpos++;
                        }
                    }
                }
            }

            double part;
            for (int i = pointer - 1; i >= 0; i--) {
                int r = record[i];
                part = (weight[r] + preNode[id]) / roadNum[r];
                int pos = preIter[r];
                while (pos) {
                    weight[pathList[pos].id] += part * roadNum[pathList[pos].id];
                    pos = pathList[pos].next;
                }
                centrality[r] += weight[r];
                roadNode->clear(record[i]);
            }
            centrality[id] -= (centrality[id] - idCentrality) / preNode[id];
        }
    }
#ifdef TEST
    cout << threadId;
    CURTIME(线程结束);
#endif
}

/**
 * 计算中心性并分配线程的函数
 * **/
void calculate() {
    int ret = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    assert(ret == 0);
    pthread_t threads[THREADNUM];
    for (int j = 1; j < THREADNUM; j++) {
        int *x = new int(j);
        pthread_create(&threads[j], nullptr, calculate_thread, (void *) x);
    }
    calculate_thread(new int(0));
    for (int i = 1; i < THREADNUM; i++) {
        pthread_join(threads[i], nullptr);
        for (int j = 0; j < nodeIDNum; j++) {
            centralityThread[0][j] += centralityThread[i][j];
        }
    }
}

/**
 * 输出答案的函数
 * @param outputPath 输出的路径
 * **/
void outputResult(char *outputPath) {
    for(int i=0; i < nodeIDNum; i++){
        centralityID[i] = i;
    }
    sort(centralityID, centralityID + nodeIDNum, cmp);
    auto fd = fopen(outputPath, "w");
    int end = nodeIDNum < 100 ? nodeIDNum : 100;
    for (int i = 0; i < end; i++) {
        fprintf(fd, "%d,%.3f\n", nodePreList[centralityID[i]], centralityThread[0][centralityID[i]]);
    }
    fclose(fd);
}

int main() {
    //sleep(100);
    //cout << sizeof(Node);
#ifdef TEST
    gettimeofday(&starttime, 0);
    //*
    char inputPath[] = "./build/test_data.txt";
    char outputPath[] = "./output/result.txt";
    // */
    /*
    char inputPath[] = "../data/final/selfTest1/test_data.txt";
    char outputPath[] = "../output/result.txt";
    // */
    /*
    char inputPath[] = "../data/std/test_data.txt";
    char outputPath[] = "../output/result.txt";
    // */
#else
    //sleep(5);
    char inputPath[] = "/data/test_data.txt";
    char outputPath[] = "/projects/student/result.txt";
#endif
    CURTIME(开始);
    readInput(inputPath);
    CURTIME(readInput);
    treatData();
    CURTIME(treatData);
    calculate();
    CURTIME(calculate);
    outputResult(outputPath);
    CURTIME(outputResult);
}
