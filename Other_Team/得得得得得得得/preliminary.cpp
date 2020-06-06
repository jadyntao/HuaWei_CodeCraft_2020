#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cassert>
#include <list>
#include <unordered_map>
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
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
using namespace std;
typedef unsigned short us;

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
#define THREADNUM 8
// 控制struct大小在2的幂次大小
#define NODELISTSIZE 31
typedef struct Node {
    char isInit = false;
    char num;
    us node[NODELISTSIZE];
} Node;

typedef union Path {
    struct{
        us stp2;
        us stp1;
    };
    unsigned int num;
} Path;

extern uint32_t const NUMTOSTR1[];
extern uint32_t const NUMTOSTR2[];
extern uint32_t const NUMTOSTR3[];

inline void writeNum(char *output, int &p, us num) {
    if(num < 100){
        *(int *) (output + p) = NUMTOSTR2[num];
        p += 2;
        return;
    }else if(num < 1000){
        *(int *) (output + p) = NUMTOSTR3[num];
        p += 3;
        return;
    }else if(num < 10000){
        us x = num / 10;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR1[num - x * 10];
        p += 4;
        return;
    }else{
        us x = num / 100;
        *(int *) (output + p) = NUMTOSTR3[x];
        *(int *) (output + p + 3) = NUMTOSTR2[num - x * 100];
        p += 5;
        return;
    }
}

#define MAXDATANUM 50000
Node nodeSetHashList[MAXDATANUM];
Node inNodeList[MAXDATANUM];
us nodeIDList[MAXDATANUM]; //保存结点
int nodeSetSize = 0;
int edgeTotalNum = 0;//边数累加
char gapId = 0;
int nodeCnt = 0;
int threadGap[THREADNUM];
int resultLen[THREADNUM];

char *inputStr;
int inputStrLen;

int totalDataNum = 0;

inline void nodeAdd(Node &node, us num) {
    node.node[node.num] = num;
    node.num++;
}

inline us nodeGet(Node &node, int order) {
    return node.node[order];
}

#define CAL ((totalDataNum / (THREADNUM * (THREADNUM - gapId + 1))) * gapId)  //计算线程分布情形

inline void nodeDivideTask(Node &node) {
    //计算线程分配
    //cout << nodeIDList[nodeCnt] << endl;
    edgeTotalNum += node.num;
    if (edgeTotalNum > CAL || nodeCnt > (nodeSetSize / THREADNUM * gapId)) {
        if (gapId >= THREADNUM) {
            //cout << "error:" << nodeCnt << endl;
        } else {
            threadGap[gapId] = nodeCnt;
            //cout << threadGap[gapId] << endl;
            //cout << "gap" << gapId << ":" <<nodeCnt << endl;
            //cout << CAL << endl;
            gapId++;
        }

    }
    nodeCnt++;
}

inline void nodeSort(Node &node) {
    sort(node.node, node.node + node.num);
}

struct Result {
#define RESULTSTRSIZE 11*7*3000000/THREADNUM
    int strLen = 0;
    //struct Result *next = nullptr;
    char * str;
    inline void insert3(us *start, Path &path) {
        writeNum(str, strLen, start[0]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[1]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[2]);
        str[strLen++] = '\n';
    }

    inline void insert4(us *start, Path &path) {
        writeNum(str, strLen, start[0]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[1]);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp1);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp2);
        str[strLen++] = '\n';
    }

    inline void insert5(us *start, Path &path) {
        writeNum(str, strLen, start[0]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[1]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[2]);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp1);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp2);
        str[strLen++] = '\n';
    }

    inline void insert6(us *start, Path &path) {
        writeNum(str, strLen, start[0]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[1]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[2]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[3]);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp1);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp2);
        str[strLen++] = '\n';
    }

    inline void insert7(us *start, Path &path) {
        writeNum(str, strLen, start[0]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[1]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[2]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[3]);
        str[strLen++] = ',';
        writeNum(str, strLen, start[4]);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp1);
        str[strLen++] = ',';
        writeNum(str, strLen, path.stp2);
        str[strLen++] = '\n';
    }

    inline void outputResult(char *output, int &p) {
        memcpy(output + p, str, strLen);
        p += strLen;
    }
} result[THREADNUM][5];

int threadNum = 0;
# define SIXTYTHOUSAND (280000 / THREADNUM + 10000)
us inputTemp[THREADNUM][SIXTYTHOUSAND][2];
int dataNum[THREADNUM]; //每个线程数组储存的数量
int readStart[THREADNUM]; // 输入时读的开头
int dataSum[THREADNUM]; //从第一个线程开始的累加和
inline us * readInputData(int i) {
    if (i < dataSum[0])
        return inputTemp[0][i];
    for (int x = 1; x < THREADNUM; x++)
        if (i < dataSum[x])
            return inputTemp[x][i - dataSum[x - 1]];
}

void *readLineThread(void *threadId) {
    int id = *((int *) threadId);
    int p = readStart[id];
    //cout << id;
    int stop = id == threadNum - 1 ? inputStrLen : readStart[id + 1];
    dataNum[id] = 0;

    for (int i = 0; p < stop; i++) {
        int input[2];
        input[0] = 0;
        while (inputStr[p] != ',') {
            input[0] = input[0] * 10 + (inputStr[p] - '0');
            p++;
        }
        p++;
        input[1] = 0;
        while (inputStr[p] != ',') {
            input[1] = input[1] * 10 + (inputStr[p] - '0');
            p++;
        }
        p++;
        while (inputStr[p] != '\n')
            p++;
        p++;
        int start = input[0], end = input[1];
        if(start > 50000 || end > 50000){
            i--;
        }else{
            //cout << start << "," << end << endl;
            inputTemp[id][i][0] = start;
            inputTemp[id][i][1] = end;
            if(!nodeSetHashList[start].isInit){
                nodeSetHashList[start] ={true,0, {}};
            }
            if(!nodeSetHashList[end].isInit){
                inNodeList[end] ={true,0, {}};
            }
            dataNum[id]++;
        }

    }
    pthread_exit(NULL);
}

inline void readInput(char * inputPath) {
    int fd = open(inputPath, O_RDONLY, 0666);
#ifdef TEST
    assert(fd >= 0);
#endif
    struct stat st{};
    fstat(fd, &st);
    inputStrLen = st.st_size;
    int readBitNum = inputStrLen / THREADNUM;
    //cout << inputStrLen << endl;
    inputStr = (char *) mmap(nullptr, inputStrLen, PROT_READ, MAP_PRIVATE, fd, 0);
#ifdef TEST
    assert(inputStr != MAP_FAILED);
#endif
    threadNum = inputStrLen < 150000 ? 1 : THREADNUM;
    //smemset(nodeIDList, -1, sizeof(nodeIDList));
    for (int i = 0; i < threadNum; i++) {
        if (i == 0) {
            readStart[i] = 0;
            continue;
        }
        int p;
        p = max(readBitNum * i, readStart[i - 1] + 1);
        while (inputStr[p] != '\n') {
            p++;
        }
        p++;
        readStart[i] = p;
    }

    pthread_t threads[THREADNUM];
    for (int i = 0; i < threadNum; i++) {
        int *j = new int(i);
        int rc = pthread_create(&threads[*j], NULL, readLineThread, (void *) j);
#ifdef TEST
        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
#endif
    }

    for (int i = 0; i < threadNum; i++){
        pthread_join(threads[i], NULL);
        totalDataNum = totalDataNum + dataNum[i];
        dataSum[i] = totalDataNum;
    }
}

inline void treatData() {
    // 获得排序（从头到尾扫一遍）
    for(int i=0; i < MAXDATANUM; i++){
        if(nodeSetHashList[i].isInit){
            nodeIDList[nodeSetSize++] = i;
        }
    }
    CURTIME(获得顺序结束);

    for (int i = 0; i < totalDataNum; i++) {
        //auto iter1 = nodeSet.find(readInputData(i)[0]);
        us *edge = readInputData(i);
        nodeAdd(nodeSetHashList[edge[0]], edge[1]);
        nodeAdd(inNodeList[edge[1]], edge[0]);
    }
}

struct PathList{
    Path p;
    short next;
};
struct FrontNeed{
    short * pathListHead;
    us * isThirdNode;
    struct PathList *pathList;
    short * pointer;
};
void dfs_front(Node *node,struct FrontNeed *frontNeed, us *route, int layer){
    for(us a=0; a < node->num; a++){
        us x = nodeGet(*node, a);
        if(x <= route[0]) continue;
        switch(layer){
            case 1:
            case 2:
                route[layer] = x;
                dfs_front(&inNodeList[x], frontNeed, route, layer+1);
                break;
            case 3:
                if(x == route[1]) continue;
                short *i = &frontNeed->pathListHead[x];
                Path p = {route[1], route[2]};
                if(frontNeed->isThirdNode[x] != route[0]){
                    frontNeed->isThirdNode[x] = route[0];
                    frontNeed->pathList[*frontNeed->pointer].next = -1;
                }
                else{
                    for (; *i != -1; i = &frontNeed->pathList[*i].next) {
                        if (p.num < frontNeed->pathList[*i].p.num) {
                            break;
                        }
                    }
                    frontNeed->pathList[*frontNeed->pointer].next = *i;
                }
                frontNeed->pathList[*frontNeed->pointer].p = p;
                *i = *frontNeed->pointer;
                (*frontNeed->pointer)++;
                break;
        }
    }
}

void *search_thread(void *arg) {
    int threadId = *((int *) arg);
    int *rl = &resultLen[threadId];
    (*rl) = 0;
    int begin = threadGap[threadId];
    int end = threadId == threadNum - 1 ? MAXDATANUM : threadGap[threadId + 1];
	//cout << "thread "<< threadId << " working from" << begin << " to " << end << endl;
    us route[7];//路径
    us isThirdNode[MAXDATANUM];
    memset(isThirdNode, -1, MAXDATANUM);
    short pathListHead[MAXDATANUM];
    struct PathList pathList[5000];
    Result *r = result[threadId];

    r[0].str = (char*)(malloc(sizeof(char) * 11 * (0+1) * (500000 * (0 + 1))));
    r[1].str = (char*)(malloc(sizeof(char) * 11 * (1+1) * (500000 * (1 + 1))));
    r[2].str = (char*)(malloc(sizeof(char) * 11 * (2+1) * (500000 * (2 + 1))));
    r[3].str = (char*)(malloc(sizeof(char) * 11 * (3+1) * (500000 * (3 + 1))));
    r[4].str = (char*)(malloc(sizeof(char) * 11 * (4+1) * (500000 * (4 + 1))));

    for (us i = begin; i < end; i++) {
        //cout << "thread "<< threadId << " working from" << begin << " to " << end << " now " << i << endl;
        route[0] = nodeIDList[i];
        short pointer = 0;
        Node *root = &inNodeList[route[0]];

        struct FrontNeed frontNeed = {pathListHead, isThirdNode, pathList, &pointer};
        dfs_front(root, &frontNeed , route, 1);

        root = &nodeSetHashList[route[0]];
        Path * path;
        for(us a=0; a < root->num; a++){
            us x = nodeGet(*root, a);
            if(x < route[0]) continue;
            route[1] = x;
            // 检查是否有4环
            if (isThirdNode[x] == route[0]){
                __builtin_prefetch(r[1].str,1);
                for (short I = pathListHead[x]; I != -1; I = pathList[I].next) {
                    path = &pathList[I].p;
                    r[1].insert4(route, *path);
                    (*rl)++;
                }
            }
            Node *nodeX = &nodeSetHashList[x];
            for(us b=0; b < nodeX->num; b++){
                us y = nodeGet(*nodeX, b);
                if(y <= route[0]) continue;
                route[2] = y;
                // 检查是否有5环
                if (isThirdNode[y] == route[0]) {
                    __builtin_prefetch(r[2].str,1);
                    for (short I = pathListHead[y]; I != -1; I = pathList[I].next) {
                        path = &pathList[I].p;
                        if(x!=path->stp1 && x!=path->stp2) {
                            r[2].insert5(route, *path);
                            (*rl)++;
                        }
                    }
                }
                Node *nodeY = &nodeSetHashList[y];
                for(us c=0; c < nodeY->num; c++){
                    us z = nodeGet(*nodeY, c);
                    // 检查是是否有3环
                    if(route[0] == z){
                        r[0].insert3(route, *path);
                        (*rl)++;
                        continue;
                    }
                    if(z < route[0] || x == z) continue;
                    route[3] = z;
                    // 检查是否有6环
                    if (isThirdNode[z] == route[0]) {
                        __builtin_prefetch(r[3].str,1);
                        for (short I = pathListHead[z]; I != -1; I = pathList[I].next) {
                            path = &pathList[I].p;
                            if(x!=path->stp1 && x!=path->stp2 &&
                               y!=path->stp1 && y!=path->stp2) {
                                r[3].insert6(route, *path);
                                (*rl)++;
                            }
                        }
                    }
                    Node *nodeZ = &nodeSetHashList[z];
                    for(us d=0; d < nodeZ->num; d++){
                        us D = nodeGet(*nodeZ, d);
                        if(D <= route[0] || x == D || y == D) continue;
                        // Node *nodeD = getNodeSet(D);
                        // 检查是否有7环
                        if (isThirdNode[D] == route[0]) {
                            route[4] = D;
                            __builtin_prefetch(r[4].str,1);
                            for (short I = pathListHead[D]; I != -1; I = pathList[I].next) {
                                path = &pathList[I].p;
                                if(x!=path->stp1 && x!=path->stp2 &&
                                   y!=path->stp1 && y!=path->stp2 &&
                                   z!=path->stp1 && z!=path->stp2) {
                                    r[4].insert7(route, *path);
                                    (*rl)++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
#ifdef TEST
    cout << threadId;
    CURTIME(线程结束);
#endif
    if(threadId != 0)
        pthread_exit(NULL);
}

int totalLen = 0;
inline void search(char * outputPath) {
    for (int i=12;i < 50000;i++) {
        Node *node = &nodeSetHashList[nodeIDList[i]];
        nodeDivideTask(*node);
        nodeSort(*node);
    }
    CURTIME(排序结束);
    pthread_t threads[THREADNUM];
    //cout << nodeSet.size()<<endl;
    for (int j = 1; j < threadNum; j++) {
        int *x = new int(j);
        pthread_create(&threads[j], NULL, search_thread, (void *) x);
    }
    search_thread(new int(0));
    totalLen = resultLen[0];

# define OUTPUTSIZE (sizeof(char) * 3000000 * ((10+1)*7) + 10)
    //cout << OUTPUTSIZE << endl;
    int fd = open(outputPath, O_RDWR | O_CREAT, 0666);
#ifdef TEST
    assert(fd >= 0);
#endif
    int term = ftruncate(fd, OUTPUTSIZE);
#ifdef TEST
    assert(term == 0);
#endif
    char *output = (char *) mmap(NULL, OUTPUTSIZE, PROT_WRITE, MAP_SHARED, fd, 0);
#ifdef TEST
    assert(MAP_FAILED != output);
#endif
#define P_START 12
    int p = P_START;
    output[P_START-1] = '\n';
    result[0][0].outputResult(output, p);

    for (int i = 1; i < threadNum; i++) {
        pthread_join(threads[i], NULL);
        result[i][0].outputResult(output, p);
        totalLen += resultLen[i];
    }
#ifdef TEST
    cout << totalLen << endl;
#endif
    int term1 = 0;
    writeNum(output, term1, totalLen);
    memset(output+term1, ' ', P_START-term1-1);

    CURTIME(即将开始循环);
    for (int i = 1; i < 5; i++)
        for (auto & j : result)
            j[i].outputResult(output, p);

    CURTIME(output解析完成);
    int t = munmap(output, OUTPUTSIZE);
#ifdef TEST
    assert(t >= 0);
#endif
    term = ftruncate(fd, p);
#ifdef TEST
    assert(term == 0);
#endif
    close(fd);
}

int main() {
#ifdef TEST
    gettimeofday(&starttime, 0);
    char inputPath[] = "../data/2020HuaweiCodecraft-TestData-master/77409/test_data.txt";
    char outputPath[] = "../output/result.txt";
    char answerPath[] = "../data/2020HuaweiCodecraft-TestData-master/77409//result.txt";
#else
    char inputPath[] = "/data/test_data.txt";
    char outputPath[] = "/projects/student/result.txt";
#endif
    //memset(nodeSetHashList, 0, sizeof(nodeSetHashList));
    CURTIME(开始);
    readInput(inputPath);
    CURTIME(readInput);
    treatData();
    CURTIME(treatData);
    search(outputPath);
    CURTIME(search);
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
        0x30303030, 0x30303031, 0x30303032, 0x30303033, 0x30303034, 0x30303035, 0x30303036, 0x30303037, 0x30303038,
        0x30303039,
};

uint32_t const NUMTOSTR2[] = {
        0x30303030, 0x30303130, 0x30303230, 0x30303330, 0x30303430, 0x30303530, 0x30303630, 0x30303730, 0x30303830,
        0x30303930,
        0x30303031, 0x30303131, 0x30303231, 0x30303331, 0x30303431, 0x30303531, 0x30303631, 0x30303731, 0x30303831,
        0x30303931,
        0x30303032, 0x30303132, 0x30303232, 0x30303332, 0x30303432, 0x30303532, 0x30303632, 0x30303732, 0x30303832,
        0x30303932,
        0x30303033, 0x30303133, 0x30303233, 0x30303333, 0x30303433, 0x30303533, 0x30303633, 0x30303733, 0x30303833,
        0x30303933,
        0x30303034, 0x30303134, 0x30303234, 0x30303334, 0x30303434, 0x30303534, 0x30303634, 0x30303734, 0x30303834,
        0x30303934,
        0x30303035, 0x30303135, 0x30303235, 0x30303335, 0x30303435, 0x30303535, 0x30303635, 0x30303735, 0x30303835,
        0x30303935,
        0x30303036, 0x30303136, 0x30303236, 0x30303336, 0x30303436, 0x30303536, 0x30303636, 0x30303736, 0x30303836,
        0x30303936,
        0x30303037, 0x30303137, 0x30303237, 0x30303337, 0x30303437, 0x30303537, 0x30303637, 0x30303737, 0x30303837,
        0x30303937,
        0x30303038, 0x30303138, 0x30303238, 0x30303338, 0x30303438, 0x30303538, 0x30303638, 0x30303738, 0x30303838,
        0x30303938,
        0x30303039, 0x30303139, 0x30303239, 0x30303339, 0x30303439, 0x30303539, 0x30303639, 0x30303739, 0x30303839,
        0x30303939,
};

uint32_t const NUMTOSTR3[] = {
        0x30303030, 0x30313030, 0x30323030, 0x30333030, 0x30343030, 0x30353030, 0x30363030, 0x30373030, 0x30383030,
        0x30393030,
        0x30303130, 0x30313130, 0x30323130, 0x30333130, 0x30343130, 0x30353130, 0x30363130, 0x30373130, 0x30383130,
        0x30393130,
        0x30303230, 0x30313230, 0x30323230, 0x30333230, 0x30343230, 0x30353230, 0x30363230, 0x30373230, 0x30383230,
        0x30393230,
        0x30303330, 0x30313330, 0x30323330, 0x30333330, 0x30343330, 0x30353330, 0x30363330, 0x30373330, 0x30383330,
        0x30393330,
        0x30303430, 0x30313430, 0x30323430, 0x30333430, 0x30343430, 0x30353430, 0x30363430, 0x30373430, 0x30383430,
        0x30393430,
        0x30303530, 0x30313530, 0x30323530, 0x30333530, 0x30343530, 0x30353530, 0x30363530, 0x30373530, 0x30383530,
        0x30393530,
        0x30303630, 0x30313630, 0x30323630, 0x30333630, 0x30343630, 0x30353630, 0x30363630, 0x30373630, 0x30383630,
        0x30393630,
        0x30303730, 0x30313730, 0x30323730, 0x30333730, 0x30343730, 0x30353730, 0x30363730, 0x30373730, 0x30383730,
        0x30393730,
        0x30303830, 0x30313830, 0x30323830, 0x30333830, 0x30343830, 0x30353830, 0x30363830, 0x30373830, 0x30383830,
        0x30393830,
        0x30303930, 0x30313930, 0x30323930, 0x30333930, 0x30343930, 0x30353930, 0x30363930, 0x30373930, 0x30383930,
        0x30393930,
        0x30303031, 0x30313031, 0x30323031, 0x30333031, 0x30343031, 0x30353031, 0x30363031, 0x30373031, 0x30383031,
        0x30393031,
        0x30303131, 0x30313131, 0x30323131, 0x30333131, 0x30343131, 0x30353131, 0x30363131, 0x30373131, 0x30383131,
        0x30393131,
        0x30303231, 0x30313231, 0x30323231, 0x30333231, 0x30343231, 0x30353231, 0x30363231, 0x30373231, 0x30383231,
        0x30393231,
        0x30303331, 0x30313331, 0x30323331, 0x30333331, 0x30343331, 0x30353331, 0x30363331, 0x30373331, 0x30383331,
        0x30393331,
        0x30303431, 0x30313431, 0x30323431, 0x30333431, 0x30343431, 0x30353431, 0x30363431, 0x30373431, 0x30383431,
        0x30393431,
        0x30303531, 0x30313531, 0x30323531, 0x30333531, 0x30343531, 0x30353531, 0x30363531, 0x30373531, 0x30383531,
        0x30393531,
        0x30303631, 0x30313631, 0x30323631, 0x30333631, 0x30343631, 0x30353631, 0x30363631, 0x30373631, 0x30383631,
        0x30393631,
        0x30303731, 0x30313731, 0x30323731, 0x30333731, 0x30343731, 0x30353731, 0x30363731, 0x30373731, 0x30383731,
        0x30393731,
        0x30303831, 0x30313831, 0x30323831, 0x30333831, 0x30343831, 0x30353831, 0x30363831, 0x30373831, 0x30383831,
        0x30393831,
        0x30303931, 0x30313931, 0x30323931, 0x30333931, 0x30343931, 0x30353931, 0x30363931, 0x30373931, 0x30383931,
        0x30393931,
        0x30303032, 0x30313032, 0x30323032, 0x30333032, 0x30343032, 0x30353032, 0x30363032, 0x30373032, 0x30383032,
        0x30393032,
        0x30303132, 0x30313132, 0x30323132, 0x30333132, 0x30343132, 0x30353132, 0x30363132, 0x30373132, 0x30383132,
        0x30393132,
        0x30303232, 0x30313232, 0x30323232, 0x30333232, 0x30343232, 0x30353232, 0x30363232, 0x30373232, 0x30383232,
        0x30393232,
        0x30303332, 0x30313332, 0x30323332, 0x30333332, 0x30343332, 0x30353332, 0x30363332, 0x30373332, 0x30383332,
        0x30393332,
        0x30303432, 0x30313432, 0x30323432, 0x30333432, 0x30343432, 0x30353432, 0x30363432, 0x30373432, 0x30383432,
        0x30393432,
        0x30303532, 0x30313532, 0x30323532, 0x30333532, 0x30343532, 0x30353532, 0x30363532, 0x30373532, 0x30383532,
        0x30393532,
        0x30303632, 0x30313632, 0x30323632, 0x30333632, 0x30343632, 0x30353632, 0x30363632, 0x30373632, 0x30383632,
        0x30393632,
        0x30303732, 0x30313732, 0x30323732, 0x30333732, 0x30343732, 0x30353732, 0x30363732, 0x30373732, 0x30383732,
        0x30393732,
        0x30303832, 0x30313832, 0x30323832, 0x30333832, 0x30343832, 0x30353832, 0x30363832, 0x30373832, 0x30383832,
        0x30393832,
        0x30303932, 0x30313932, 0x30323932, 0x30333932, 0x30343932, 0x30353932, 0x30363932, 0x30373932, 0x30383932,
        0x30393932,
        0x30303033, 0x30313033, 0x30323033, 0x30333033, 0x30343033, 0x30353033, 0x30363033, 0x30373033, 0x30383033,
        0x30393033,
        0x30303133, 0x30313133, 0x30323133, 0x30333133, 0x30343133, 0x30353133, 0x30363133, 0x30373133, 0x30383133,
        0x30393133,
        0x30303233, 0x30313233, 0x30323233, 0x30333233, 0x30343233, 0x30353233, 0x30363233, 0x30373233, 0x30383233,
        0x30393233,
        0x30303333, 0x30313333, 0x30323333, 0x30333333, 0x30343333, 0x30353333, 0x30363333, 0x30373333, 0x30383333,
        0x30393333,
        0x30303433, 0x30313433, 0x30323433, 0x30333433, 0x30343433, 0x30353433, 0x30363433, 0x30373433, 0x30383433,
        0x30393433,
        0x30303533, 0x30313533, 0x30323533, 0x30333533, 0x30343533, 0x30353533, 0x30363533, 0x30373533, 0x30383533,
        0x30393533,
        0x30303633, 0x30313633, 0x30323633, 0x30333633, 0x30343633, 0x30353633, 0x30363633, 0x30373633, 0x30383633,
        0x30393633,
        0x30303733, 0x30313733, 0x30323733, 0x30333733, 0x30343733, 0x30353733, 0x30363733, 0x30373733, 0x30383733,
        0x30393733,
        0x30303833, 0x30313833, 0x30323833, 0x30333833, 0x30343833, 0x30353833, 0x30363833, 0x30373833, 0x30383833,
        0x30393833,
        0x30303933, 0x30313933, 0x30323933, 0x30333933, 0x30343933, 0x30353933, 0x30363933, 0x30373933, 0x30383933,
        0x30393933,
        0x30303034, 0x30313034, 0x30323034, 0x30333034, 0x30343034, 0x30353034, 0x30363034, 0x30373034, 0x30383034,
        0x30393034,
        0x30303134, 0x30313134, 0x30323134, 0x30333134, 0x30343134, 0x30353134, 0x30363134, 0x30373134, 0x30383134,
        0x30393134,
        0x30303234, 0x30313234, 0x30323234, 0x30333234, 0x30343234, 0x30353234, 0x30363234, 0x30373234, 0x30383234,
        0x30393234,
        0x30303334, 0x30313334, 0x30323334, 0x30333334, 0x30343334, 0x30353334, 0x30363334, 0x30373334, 0x30383334,
        0x30393334,
        0x30303434, 0x30313434, 0x30323434, 0x30333434, 0x30343434, 0x30353434, 0x30363434, 0x30373434, 0x30383434,
        0x30393434,
        0x30303534, 0x30313534, 0x30323534, 0x30333534, 0x30343534, 0x30353534, 0x30363534, 0x30373534, 0x30383534,
        0x30393534,
        0x30303634, 0x30313634, 0x30323634, 0x30333634, 0x30343634, 0x30353634, 0x30363634, 0x30373634, 0x30383634,
        0x30393634,
        0x30303734, 0x30313734, 0x30323734, 0x30333734, 0x30343734, 0x30353734, 0x30363734, 0x30373734, 0x30383734,
        0x30393734,
        0x30303834, 0x30313834, 0x30323834, 0x30333834, 0x30343834, 0x30353834, 0x30363834, 0x30373834, 0x30383834,
        0x30393834,
        0x30303934, 0x30313934, 0x30323934, 0x30333934, 0x30343934, 0x30353934, 0x30363934, 0x30373934, 0x30383934,
        0x30393934,
        0x30303035, 0x30313035, 0x30323035, 0x30333035, 0x30343035, 0x30353035, 0x30363035, 0x30373035, 0x30383035,
        0x30393035,
        0x30303135, 0x30313135, 0x30323135, 0x30333135, 0x30343135, 0x30353135, 0x30363135, 0x30373135, 0x30383135,
        0x30393135,
        0x30303235, 0x30313235, 0x30323235, 0x30333235, 0x30343235, 0x30353235, 0x30363235, 0x30373235, 0x30383235,
        0x30393235,
        0x30303335, 0x30313335, 0x30323335, 0x30333335, 0x30343335, 0x30353335, 0x30363335, 0x30373335, 0x30383335,
        0x30393335,
        0x30303435, 0x30313435, 0x30323435, 0x30333435, 0x30343435, 0x30353435, 0x30363435, 0x30373435, 0x30383435,
        0x30393435,
        0x30303535, 0x30313535, 0x30323535, 0x30333535, 0x30343535, 0x30353535, 0x30363535, 0x30373535, 0x30383535,
        0x30393535,
        0x30303635, 0x30313635, 0x30323635, 0x30333635, 0x30343635, 0x30353635, 0x30363635, 0x30373635, 0x30383635,
        0x30393635,
        0x30303735, 0x30313735, 0x30323735, 0x30333735, 0x30343735, 0x30353735, 0x30363735, 0x30373735, 0x30383735,
        0x30393735,
        0x30303835, 0x30313835, 0x30323835, 0x30333835, 0x30343835, 0x30353835, 0x30363835, 0x30373835, 0x30383835,
        0x30393835,
        0x30303935, 0x30313935, 0x30323935, 0x30333935, 0x30343935, 0x30353935, 0x30363935, 0x30373935, 0x30383935,
        0x30393935,
        0x30303036, 0x30313036, 0x30323036, 0x30333036, 0x30343036, 0x30353036, 0x30363036, 0x30373036, 0x30383036,
        0x30393036,
        0x30303136, 0x30313136, 0x30323136, 0x30333136, 0x30343136, 0x30353136, 0x30363136, 0x30373136, 0x30383136,
        0x30393136,
        0x30303236, 0x30313236, 0x30323236, 0x30333236, 0x30343236, 0x30353236, 0x30363236, 0x30373236, 0x30383236,
        0x30393236,
        0x30303336, 0x30313336, 0x30323336, 0x30333336, 0x30343336, 0x30353336, 0x30363336, 0x30373336, 0x30383336,
        0x30393336,
        0x30303436, 0x30313436, 0x30323436, 0x30333436, 0x30343436, 0x30353436, 0x30363436, 0x30373436, 0x30383436,
        0x30393436,
        0x30303536, 0x30313536, 0x30323536, 0x30333536, 0x30343536, 0x30353536, 0x30363536, 0x30373536, 0x30383536,
        0x30393536,
        0x30303636, 0x30313636, 0x30323636, 0x30333636, 0x30343636, 0x30353636, 0x30363636, 0x30373636, 0x30383636,
        0x30393636,
        0x30303736, 0x30313736, 0x30323736, 0x30333736, 0x30343736, 0x30353736, 0x30363736, 0x30373736, 0x30383736,
        0x30393736,
        0x30303836, 0x30313836, 0x30323836, 0x30333836, 0x30343836, 0x30353836, 0x30363836, 0x30373836, 0x30383836,
        0x30393836,
        0x30303936, 0x30313936, 0x30323936, 0x30333936, 0x30343936, 0x30353936, 0x30363936, 0x30373936, 0x30383936,
        0x30393936,
        0x30303037, 0x30313037, 0x30323037, 0x30333037, 0x30343037, 0x30353037, 0x30363037, 0x30373037, 0x30383037,
        0x30393037,
        0x30303137, 0x30313137, 0x30323137, 0x30333137, 0x30343137, 0x30353137, 0x30363137, 0x30373137, 0x30383137,
        0x30393137,
        0x30303237, 0x30313237, 0x30323237, 0x30333237, 0x30343237, 0x30353237, 0x30363237, 0x30373237, 0x30383237,
        0x30393237,
        0x30303337, 0x30313337, 0x30323337, 0x30333337, 0x30343337, 0x30353337, 0x30363337, 0x30373337, 0x30383337,
        0x30393337,
        0x30303437, 0x30313437, 0x30323437, 0x30333437, 0x30343437, 0x30353437, 0x30363437, 0x30373437, 0x30383437,
        0x30393437,
        0x30303537, 0x30313537, 0x30323537, 0x30333537, 0x30343537, 0x30353537, 0x30363537, 0x30373537, 0x30383537,
        0x30393537,
        0x30303637, 0x30313637, 0x30323637, 0x30333637, 0x30343637, 0x30353637, 0x30363637, 0x30373637, 0x30383637,
        0x30393637,
        0x30303737, 0x30313737, 0x30323737, 0x30333737, 0x30343737, 0x30353737, 0x30363737, 0x30373737, 0x30383737,
        0x30393737,
        0x30303837, 0x30313837, 0x30323837, 0x30333837, 0x30343837, 0x30353837, 0x30363837, 0x30373837, 0x30383837,
        0x30393837,
        0x30303937, 0x30313937, 0x30323937, 0x30333937, 0x30343937, 0x30353937, 0x30363937, 0x30373937, 0x30383937,
        0x30393937,
        0x30303038, 0x30313038, 0x30323038, 0x30333038, 0x30343038, 0x30353038, 0x30363038, 0x30373038, 0x30383038,
        0x30393038,
        0x30303138, 0x30313138, 0x30323138, 0x30333138, 0x30343138, 0x30353138, 0x30363138, 0x30373138, 0x30383138,
        0x30393138,
        0x30303238, 0x30313238, 0x30323238, 0x30333238, 0x30343238, 0x30353238, 0x30363238, 0x30373238, 0x30383238,
        0x30393238,
        0x30303338, 0x30313338, 0x30323338, 0x30333338, 0x30343338, 0x30353338, 0x30363338, 0x30373338, 0x30383338,
        0x30393338,
        0x30303438, 0x30313438, 0x30323438, 0x30333438, 0x30343438, 0x30353438, 0x30363438, 0x30373438, 0x30383438,
        0x30393438,
        0x30303538, 0x30313538, 0x30323538, 0x30333538, 0x30343538, 0x30353538, 0x30363538, 0x30373538, 0x30383538,
        0x30393538,
        0x30303638, 0x30313638, 0x30323638, 0x30333638, 0x30343638, 0x30353638, 0x30363638, 0x30373638, 0x30383638,
        0x30393638,
        0x30303738, 0x30313738, 0x30323738, 0x30333738, 0x30343738, 0x30353738, 0x30363738, 0x30373738, 0x30383738,
        0x30393738,
        0x30303838, 0x30313838, 0x30323838, 0x30333838, 0x30343838, 0x30353838, 0x30363838, 0x30373838, 0x30383838,
        0x30393838,
        0x30303938, 0x30313938, 0x30323938, 0x30333938, 0x30343938, 0x30353938, 0x30363938, 0x30373938, 0x30383938,
        0x30393938,
        0x30303039, 0x30313039, 0x30323039, 0x30333039, 0x30343039, 0x30353039, 0x30363039, 0x30373039, 0x30383039,
        0x30393039,
        0x30303139, 0x30313139, 0x30323139, 0x30333139, 0x30343139, 0x30353139, 0x30363139, 0x30373139, 0x30383139,
        0x30393139,
        0x30303239, 0x30313239, 0x30323239, 0x30333239, 0x30343239, 0x30353239, 0x30363239, 0x30373239, 0x30383239,
        0x30393239,
        0x30303339, 0x30313339, 0x30323339, 0x30333339, 0x30343339, 0x30353339, 0x30363339, 0x30373339, 0x30383339,
        0x30393339,
        0x30303439, 0x30313439, 0x30323439, 0x30333439, 0x30343439, 0x30353439, 0x30363439, 0x30373439, 0x30383439,
        0x30393439,
        0x30303539, 0x30313539, 0x30323539, 0x30333539, 0x30343539, 0x30353539, 0x30363539, 0x30373539, 0x30383539,
        0x30393539,
        0x30303639, 0x30313639, 0x30323639, 0x30333639, 0x30343639, 0x30353639, 0x30363639, 0x30373639, 0x30383639,
        0x30393639,
        0x30303739, 0x30313739, 0x30323739, 0x30333739, 0x30343739, 0x30353739, 0x30363739, 0x30373739, 0x30383739,
        0x30393739,
        0x30303839, 0x30313839, 0x30323839, 0x30333839, 0x30343839, 0x30353839, 0x30363839, 0x30373839, 0x30383839,
        0x30393839,
        0x30303939, 0x30313939, 0x30323939, 0x30333939, 0x30343939, 0x30353939, 0x30363939, 0x30373939, 0x30383939,
        0x30393939,
};
