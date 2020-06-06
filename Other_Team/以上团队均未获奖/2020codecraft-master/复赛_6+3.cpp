#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <unordered_map>
using namespace std;

#define LL long long
#define US unsigned short
#define Max_ID 10000000 //�����IDС���������������ӳ��
#define MAX_LOG 2000005 // ����������
#define MAX_circle 20000000 //��������
#define NT_F 4  // �һ��߳���
#define block_size 256 // �һ����ؾ���ʱÿһƬ��С
const uint8_t T = 30; // ���־ջ���/�Ǿջ��㳬����

const char* input_path = "/data/test_data.txt";
const char* output_path = "/projects/student/result.txt";

struct Timer
{
    /*
        ��ʱ�������������ڶ��߳��ھ�׼��ʱ
    */
    timeval tic, toc;

    Timer()
    {
        gettimeofday(&tic,NULL);
    }

    void stop(const char* name)
    {
        gettimeofday(&toc,NULL);
        printf("%s: %f(s)\n", name, float(toc.tv_sec-tic.tv_sec) + float(toc.tv_usec-tic.tv_usec)/1000000);
    }
};

struct Edge
{
    int v, w;
};

struct Edge_
{
    int u,v,w;
};
int l1[2*MAX_LOG], l1_len;

struct Node
{
    int n1, n2, wtail, whead;
};

// �������ݿ�
struct DBlock
{
    int begin, end, log_len;
    Edge_ *Log;
    int *in_du, *out_du;
};

// �������ݿ�
struct SBlock
{
    int begin, end;
};

// �һ����ݿ�
struct CBlock
{
    int n_id;                                 // �����ID��
    int *ids;                                 // �����ID
    int ns[5];                                // ÿ�ֻ��ĸ���
    int *ccs[5];                              // ���
    int *ncs[5];                              // ÿ��IDÿ�ֻ��ĸ���
    int *ics[5];                              // ÿ��IDÿ�ֻ���cc�е�λ��
};

// ת���洢��Χ
struct CRange  // i, [begin, end)
{
    int i, begin, end;
};
int ifsize;                         // �����ļ���С
int max_id, max_edge;               // ӳ��������ID�������Ŀ��,������
int* ids;                           // ԭʼID
Edge *G, *Gb;                       // ͼ���������ڽӱ�
Edge *denseG, *denseGb;            // �ջ�ͼ���������ڽӱ�
int *gidx, *gbidx;                  // ID��ͼ�е�λ������
uint8_t *ideg, *odeg;                // �����
US *denseIdeg, *denseOdeg;          // �ջ�ͼ�����
bool *isDenseG, *isDenseGb;         // �Ƿ������򣬷���ͼ�ľջ���

unordered_map<int, int>id2numMap;   // mapӳ��
int id2num[Max_ID];                 // ����ӳ��
char* node2str;                    // ӳ���ַ���
char* node2len;                     // ӳ���ַ�������

SBlock sblock[4];                   // �������ݿ�
DBlock dblock[4];                   // �������ݿ�
CBlock cblock[NT_F];                // �һ����ݿ�
CRange crange[8];                   // ת���洢��Χ
volatile bool done[8];           // �߳���ɱ�ʶ
volatile int fsize;                 // ����ļ���С
int fd;                             // ����ļ����
char *ifmap;                        // mmap

volatile int task;                  // ��ǰ����ID
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // ���ؾ�����mtx
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

int each_r[5] = {0};                // ÿ�ֻ�����
int total_r = 0;                    // ��·����
int **pos;                          // ���л�·��ָ��
int *plen;                          // ���л�·��Ӧ����

int get_file_size(const char *path)
{
    /*
        ��ȡ�ļ���С
    */
    struct stat statbuff;
    stat(path, &statbuff);
    return statbuff.st_size;
}

// ������
void *read(void* arg)
{
    /*
        ���߳�mmap����
    */
    DBlock* blk = (DBlock*)arg;
    Edge_ *Log = blk->Log;
    char c;
    int i = blk->begin, j = 0;
    int node1, node2, node3;
    while (i < blk->end)
    {
        node1 = 0, node2 = 0, node3 = 0;
        while ((c = ifmap[i++]) != ',')  node1 = (node1 * 10 + c - 48);
        while ((c = ifmap[i++]) != ',')  node2 = (node2 * 10 + c - 48);
        while ((c = ifmap[i++]) >= 48) node3 = (node3 * 10 + c - 48);
        if (ifmap[i] < 48) i++;
        Log[j].u = node1, Log[j].v = node2, Log[j++].w = node3;
    }
    blk->log_len = j;
}

void *arrayHash(void* arg)
{
    /*
        ����ӳ��
    */
    DBlock* blk = (DBlock*)arg;
    Edge_* Log = blk->Log;
    blk->in_du = (int*)calloc(max_id, sizeof(int));
    blk->out_du = (int*)calloc(max_id, sizeof(int));
    int *in_du = blk->in_du, *out_du = blk->out_du;
    int len = blk->log_len;
    for (int j = 0; j < len; j++)
    {
        Edge_ temp = Log[j];
        int u = id2num[temp.u], v = id2num[temp.v];
        in_du[v]++, out_du[u]++;
        Log[j].u = u, Log[j].v = v;
    }
}

void *mapHash(void* arg)
{
    /*
        mapӳ��
    */
    DBlock* blk = (DBlock*)arg;
    Edge_* Log = blk->Log;
    blk->in_du = (int*)calloc(max_id, sizeof(int));
    blk->out_du = (int*)calloc(max_id, sizeof(int));
    int *in_du = blk->in_du, *out_du = blk->out_du;
    int len = blk->log_len;
    for (int j = 0; j < len; j++)
    {
        Edge_ temp = Log[j];
        int u = id2numMap[temp.u], v = id2numMap[temp.v];
        in_du[v]++, out_du[u]++;
        Log[j].u = u, Log[j].v = v;
    }
}

void itoa(int d, char* c, char* l)
{
    /*
        �����תΪ�ַ���
    */
    char* s = (char*)malloc(11*sizeof(char));
    int i = 10;
    s[i--] = ',';
    while (d > 0)
    {
        s[i--] = d % 10 + 48;
        d /= 10;
    }
    *l = 10 - i;
    memcpy(c, s+i+1, 10 - i);
}

void *get_strmap(void* arg)
{
    /*
        �����תΪ�ַ���
        Ϊ�˱�֤��ַ�������������cache�����ʣ�node2str����ʹ��һάָ��
    */
    node2str = (char*)malloc(max_id * 16 * sizeof(char));
    node2len = (char*)malloc(max_id * sizeof(char));
    if (l1[0] == 0){
        node2str[0] = '0'; node2str[1] = ','; node2len[0] = 2;
        for (int i = 1; i < max_id; i++)  itoa(l1[i], node2str + i * 16, node2len + i);
    }
    else{
        for (int i = 0; i < max_id; i++)  itoa(l1[i], node2str + i * 16, node2len + i);
    }
}

bool cmp(Edge a, Edge b)
{
    return a.v < b.v;
}

void *sort_edge(void* arg)
{
    /*
        ����
    */
    SBlock* blk = (SBlock*)arg;
    Edge *p1, *p2;
    int begin = blk->begin, end = blk->end;
    for (int i = begin; i < end; i++)
    {
        if (isDenseG[i]){
            p1 = denseG + gidx[i];
            sort(p1, p1 + denseOdeg[i], cmp);
        }
        else{
            p1 = G + i * T;
            sort(p1, p1 + odeg[i], cmp);
        }
    }
}

void* create_graph(void* arg)
{
    /*
        ��ͼ
        ��Ծջ���ͷǾջ��㹹����ͬ��ͼ
        ���̶߳��������ĸ�ͼ����
    */
    // ��ʼ��
    G = (Edge*)malloc(max_id * T * sizeof(Edge));
    Gb = (Edge*)malloc(max_id * T * sizeof(Edge));
    ideg = (uint8_t*)calloc(max_id, sizeof(uint8_t));
    odeg = (uint8_t*)calloc(max_id, sizeof(uint8_t));
    isDenseGb = (bool*)calloc(max_id, sizeof(bool));
    isDenseG = (bool*)calloc(max_id, sizeof(bool));
    denseIdeg = (US*)calloc(max_id, sizeof(US));
    denseOdeg = (US*)calloc(max_id, sizeof(US));
    US *tmp_ideg = (US*)calloc(max_id, sizeof(US));
    US *tmp_odeg = (US*)calloc(max_id, sizeof(US));
    for(int i = 0; i < 4; i++){
        int *in = (dblock + i)->in_du, *out = (dblock + i)->out_du;
        for(int j = 0; j < max_id; j++) tmp_ideg[j] += in[j], tmp_odeg[j] += out[j];
    }
    // �ֱ�ջ���ͷǾջ���
    gidx = (int*)malloc((max_id + 1) * sizeof(int));
    gbidx = (int*)malloc((max_id + 1) * sizeof(int));
    gidx[0] = 0, gbidx[0] = 0;
    for(int i = 0; i < max_id; i++){
        if (tmp_odeg[i] > T) gidx[i + 1] = gidx[i] + tmp_odeg[i], isDenseG[i] = 1, odeg[i] = tmp_odeg[i], denseOdeg[i] = tmp_odeg[i];
        else gidx[i + 1] = gidx[i], odeg[i] = tmp_odeg[i];
        if (tmp_ideg[i] > T) gbidx[i + 1] = gbidx[i] + tmp_ideg[i], isDenseGb[i] = 1, denseIdeg[i] = tmp_ideg[i];
        else gbidx[i + 1] = gbidx[i], ideg[i] = tmp_ideg[i];
    }
    denseG = (Edge*)malloc(gidx[max_id] * sizeof(Edge));
    denseGb = (Edge*)malloc(gbidx[max_id] * sizeof(Edge));

    // 3. ���������ڽӱ�
    US *tmp = (US*)calloc(max_id, sizeof(US));
    US *tmpb = (US*)calloc(max_id, sizeof(US));
    int p1, p2;
    for (int i = 0; i < 4; i++)
    {
        Edge_ *Log = dblock[i].Log;
        int len = dblock[i].log_len;
        for(int j = 0; j < len; j++)
        {
            int u = Log[j].u, v = Log[j].v, w = Log[j].w;
            Edge e1 = {u, w}, e2 = {v, w};
            if (isDenseG[u]){
                p1 = gidx[u];
                denseG[p1 + (tmp[u]++)] = e2;
            }
            else{
                p1 = u * T;
                G[p1 + (tmp[u]++)] = e2;
            }
            if (isDenseGb[v]){
                p2 = gbidx[v];
                denseGb[p2 + (tmpb[v]++)] = e1;
            }
            else{
                p2 = v * T;
                Gb[p2 + (tmpb[v]++)] = e1;
            }
        }
    }
    sblock[0].begin = 0;
    for(int i = 1; i < 4; i++) sblock[i].begin = i * max_id / 4;
    for(int i = 0; i < 3; i++) sblock[i].end = sblock[i+1].begin;
    sblock[3].end = max_id;
    pthread_t thread[4];
    for (int i = 0; i < 4; i++)  pthread_create(&thread[i], NULL, sort_edge, sblock + i);
    for (int i = 0; i < 4; i++)  pthread_join(thread[i], NULL);
}

void load()
{
    /*
        �����ļ�����ͼ
        ���߳�mmap�����ļ�
        �ж�������С����С��1e7��ʹ��������hash������ʹ��unordered_map
    */
    Timer timer;
    // ȷ��ÿ���̶߳��ļ�����
    ifsize = get_file_size(input_path);
    int ifd = open(input_path, O_RDONLY, S_IRUSR);
    ifmap = (char *)mmap(NULL, ifsize, PROT_READ, MAP_SHARED, ifd, 0);
    int dsz = ifsize / 4;
    dblock[0].begin = 0;
    for (int i = 1; i < 4; i++)
    {
        int rp = dsz * i;
        while (ifmap[rp++] != '\n');
        dblock[i].begin = rp;
    }
    for (int i = 0; i < 3; i++)  dblock[i].end = dblock[i+1].begin;
    dblock[3].end = ifsize;
    // ��ʼ������
    for (int i = 0; i < 4; i++)
        dblock[i].Log = (Edge_*)malloc(MAX_LOG * sizeof(Edge_));

    // ���̶߳�����
    pthread_t thread[4];
    for (int i = 0; i < 4; i++)  pthread_create(&thread[i], NULL, read, dblock + i);
    for (int i = 0; i < 4; i++)  pthread_join(thread[i], NULL);
    max_edge = dblock[0].log_len+dblock[1].log_len+dblock[2].log_len+dblock[3].log_len;
    printf("log len: %d\n", max_edge);
    bool flag = 1;
    for (int i = 0; i < 4; i++)
    {
        Edge_* Log = dblock[i].Log;
        for (int j = 0; j < dblock[i].log_len; j++)
        {
            Edge_ e = Log[j];
            if (e.u >= Max_ID || e.v >= Max_ID){
                flag = 0;
                break;
            }
        }
        if(!flag) break;
    }

    // �ж��Ƿ������������ӳ��
    if(flag){
        // ����ӳ��
        Edge_ e;
        for (int i = 0; i < 4; i++)
        {
            Edge_* Log = dblock[i].Log;
            for (int j = 0; j < dblock[i].log_len; j++)
            {
                e = Log[j];
                id2num[e.u] = 1, id2num[e.v] = 1;
            }
        }
        for(int i = 0; i < Max_ID; i++)
            if (id2num[i]) l1[max_id] = i, id2num[i] = max_id++;

        // �Խ��hash
        for (int i = 0; i < 4; i++)  pthread_create(&thread[i], NULL, arrayHash, dblock + i);
        for (int i = 0; i < 4; i++)  pthread_join(thread[i], NULL);
    }
    else{
        // mapӳ��
        Edge_ e;
        for (int i = 0; i < 4; i++)
        {
            Edge_* Log = dblock[i].Log;
            int len = dblock[i].log_len;
            for (int j = 0; j < len; j++)
            {
                Edge_ e = Log[j];
                int u = e.u, v = e.v;
                if(id2numMap.find(u) == id2numMap.end()) id2numMap[u] = 1, l1[max_id++] = u;
                if(id2numMap.find(v) == id2numMap.end()) id2numMap[v] = 1, l1[max_id++] = v;
            }
        }
        sort(l1, l1 + max_id);
        for(int i = 0; i < max_id; i++) id2numMap[l1[i]] = i;
        // �Խ��hash
        for (int i = 0; i < 4; i++)  pthread_create(&thread[i], NULL, mapHash, dblock + i);
        for (int i = 0; i < 4; i++)  pthread_join(thread[i], NULL);
    }
    printf("max id: %d\n", max_id);

    pthread_create(&thread[0], NULL, get_strmap, NULL);
    pthread_create(&thread[1], NULL, create_graph, NULL);
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
}

inline bool checkw(LL a, LL b)
{
    return 5 * b < a || b > 3 * a;
}

void *find(void* arg)
{
    /*
        �һ��� �ȷ������������ܵ���ĵ�����·�������������㣬�������·����֦
        Ϊ�����ȡַ�ٶȣ��Ծջ���ͷǾջ��㶯̬�л����ִ洢��ʽ
        Ϊ����߳���Ч�ʣ���dfsתΪ����
        Ϊ�����Ч�ʣ�������ѹ���������͵��ֽ�����
    */
    Timer timer;
    CBlock* blk = (CBlock*)arg;
    bool *mark = (bool*)calloc(max_id, sizeof(bool));
    int *history = (int*)malloc(max_id * sizeof(int));
    uint8_t *depth = (uint8_t*)malloc(max_id * sizeof(uint8_t));
    memset(depth, 10, max_id);
    int n_hist = 0;
    LL *wxs = (LL*)malloc(max_id * sizeof(LL));
    int n3 = 0, n4 = 0, n5 = 0, n6 = 0, n7 = 0;
    int *cc3 = blk->ccs[0], *cc4 = blk->ccs[1], *cc5 = blk->ccs[2], *cc6 = blk->ccs[3], *cc7 = blk->ccs[4];
    int *nc3 = blk->ncs[0], *nc4 = blk->ncs[1], *nc5 = blk->ncs[2], *nc6 = blk->ncs[3], *nc7 = blk->ncs[4];
    int last_id1 = 0;
    int id1, id2, id3, id4, id5, id6, id7;
    Edge *j, *j2, *j3, *j4, *j5, *j6, *j7;
    LL w2, w3, w4, w5, w6, w7, w8;
    US tar2, tar3, tar4, tar5, tar6, tar7;
    while (true)
    {
        pthread_mutex_lock(&mutex);
        id1 = task;
        task += block_size;
        pthread_mutex_unlock(&mutex);
        if (id1 >= max_id) break;
        memset(mark + last_id1, 1, id1 - last_id1);
        last_id1 = min(id1 + block_size - 1, max_id);
        for (int t = 0; t < block_size && id1 < max_id; t++, id1++)
        {
            blk->ids[blk->n_id++] = id1;
            mark[id1] = 1;
            // ���·
            for (int i = 0; i < n_hist; i++) depth[history[i]] = 10;
            n_hist = 0;

            if (isDenseGb[id1]){
                j2 = denseGb + gbidx[id1];
                tar2 = denseIdeg[id1];
            }
            else{
                j2 = Gb + id1 * T;
                tar2 = ideg[id1];
            }
            for (int i2 = 0; i2 < tar2; i2++)
            {
                id2 = j2[i2].v, w2 = j2[i2].w;
                if (mark[id2] == 1) continue;
                mark[id2] = 1;
                depth[id2] = 1;
                history[n_hist++] = id2;
                if (isDenseGb[id2]){
                    j3 = denseGb + gbidx[id2];
                    tar3 = denseIdeg[id2];
                }
                else{
                    j3 = Gb + id2 * T;
                    tar3 = ideg[id2];
                }
                for (int i3 = 0; i3 < tar3; i3++)
                {
                    id3 = j3[i3].v, w3 = j3[i3].w;
                    if (mark[id3] == 1 || checkw(w3, w2)) continue;
                    if (depth[id3] > 2)
                    {
                        depth[id3] = 2;
                        history[n_hist++] = id3;
                    }

                    if (isDenseGb[id3]){
                        j4 = denseGb + gbidx[id3];
                        tar4 = denseIdeg[id3];
                    }
                    else{
                        j4 = Gb + id3 * T;
                        tar4 = ideg[id3];
                    }
                    for (int i4 = 0; i4 < tar4; i4++)
                    {
                        id4 = j4[i4].v, w4 = j4[i4].w;
                        if (mark[id4] == 1 || checkw(w4, w3)) continue;
                        if (depth[id4] > 3) {
                            depth[id4] = 3;
                            history[n_hist++] = id4;
                        }
                    }
                }
                mark[id2] = 0;
            }

            if (isDenseGb[id1]){
                j2 = denseGb + gbidx[id1];
                tar2 = denseIdeg[id1];
            }
            else{
                j2 = Gb + id1 * T;
                tar2 = ideg[id1];
            }
            for (int i = 0; i < tar2; i++)  wxs[j2[i].v] = j2[i].w;

            if (isDenseG[id1]){
                j2 = denseG + gidx[id1];
                tar2 = denseOdeg[id1];
            }
            else{
                j2 = G + id1 * T;
                tar2 = odeg[id1];
            }
            for (int i2 = 0; i2 < tar2; i2++)
            {
                id2 = j2[i2].v, w2 = j2[i2].w;
                if (mark[id2] == 1) continue;
                mark[id2] = 1;

                if (isDenseG[id2]){
                    j3 = denseG + gidx[id2];
                    tar3 = denseOdeg[id2];
                }
                else{
                    j3 = G + id2 * T;
                    tar3 = odeg[id2];
                }
                for (int i3 = 0; i3 < tar3; i3++)
                {
                    id3 = j3[i3].v, w3 = j3[i3].w;
                    if (mark[id3] == 1 || checkw(w2, w3)) continue;
                    if (depth[id3] == 1){
                        w4 = wxs[id3];
                        if(!checkw(w3, w4) && !checkw(w4, w2)){
                            int *k = cc3 + n3 * 3;
                            *(k) = id1;  *(k+1) = id2;  *(k+2) = id3;
                            n3++;  nc3[id1]++;
                        }
                    }
                    mark[id3] = 1;

                    if (isDenseG[id3]){
                        j4 = denseG + gidx[id3];
                        tar4 = denseOdeg[id3];
                    }
                    else{
                        j4 = G + id3 * T;
                        tar4 = odeg[id3];
                    }
                    for (int i4 = 0; i4 < tar4; i4++)
                    {
                        id4 = j4[i4].v, w4 = j4[i4].w;
                        if (mark[id4] == 1 || checkw(w3, w4)) continue;
                        if (depth[id4] == 1){
                            w5 = wxs[id4];
                            if(!checkw(w4, w5) && !checkw(w5, w2)){
                                int *k = cc4 + n4 * 4;
                                *(k) = id1;  *(k+1) = id2;  *(k+2) = id3; *(k+3) = id4;
                                n4++;  nc4[id1]++;
                            }
                        }
                        mark[id4] = 1;

                        if (isDenseG[id4]){
                            j5 = denseG + gidx[id4];
                            tar5 = denseOdeg[id4];
                        }
                        else{
                            j5 = G + id4 * T;
                            tar5 = odeg[id4];
                        }
                        for (int i5 = 0; i5 < tar5; i5++)
                        {
                            id5 = j5[i5].v, w5 = j5[i5].w;
                            if (depth[id5] > 3 || mark[id5] == 1 || checkw(w4, w5)) continue;
                            if (depth[id5] == 1){
                                w6 = wxs[id5];
                                if(!checkw(w5, w6) && !checkw(w6, w2)){
                                    int *k = cc5 + n5 * 5;
                                    *(k) = id1;  *(k+1) = id2;  *(k+2) = id3; *(k+3) = id4; *(k+4) = id5;
                                    n5++;  nc5[id1]++;
                                }
                            }
                            mark[id5] = 1;

                            if (isDenseG[id5]){
                                j6 = denseG + gidx[id5];
                                tar6 = denseOdeg[id5];
                            }
                            else{
                                j6 = G + id5 * T;
                                tar6 = odeg[id5];
                            }
                            for (int i6 = 0; i6 < tar6; i6++)
                            {
                                id6 = j6[i6].v, w6 = j6[i6].w;
                                if (depth[id6] > 2 || mark[id6] == 1 || checkw(w5, w6)) continue;
                                if (depth[id6] == 1){
                                    w7 = wxs[id6];
                                    if(!checkw(w6, w7) && !checkw(w7, w2)){
                                        int *k = cc6 + n6 * 6;
                                        *(k) = id1;  *(k+1) = id2;  *(k+2) = id3; *(k+3) = id4; *(k+4) = id5; *(k+5) = id6;
                                        n6++;  nc6[id1]++;
                                    }
                                }
                                mark[id6] = 1;

                                if (isDenseG[id6]){
                                    j7 = denseG + gidx[id6];
                                    tar7 = denseOdeg[id6];
                                }
                                else{
                                    j7 = G + id6 * T;
                                    tar7 = odeg[id6];
                                }
                                for (int i7 = 0; i7 < tar7; i7++)
                                {
                                    id7 = j7[i7].v, w7 = j7[i7].w;
                                    if (depth[id7] > 1 || mark[id7] == 1 || checkw(w6, w7)) continue;
                                    w8 = wxs[id7];
                                    if(!checkw(w7, w8) && !checkw(w8, w2)){
                                        int *k = cc7 + n7 * 7;
                                        *(k) = id1;  *(k+1) = id2;  *(k+2) = id3; *(k+3) = id4; *(k+4) = id5; *(k+5) = id6; *(k+6) = id7;
                                        n7++;  nc7[id1]++;
                                    }
                                }
                                mark[id6] = 0;
                            }
                            mark[id5] = 0;
                        }
                        mark[id4] = 0;
                    }
                    mark[id3] = 0;
                }
                mark[id2] = 0;
            }
        }
    }
    blk->ns[0] = n3; blk->ns[1] = n4; blk->ns[2] = n5; blk->ns[3] = n6; blk->ns[4] = n7;
    // ȷ��ID��cc�е�λ��
    for (int i = 0; i < 5; i++)
    {
        int l = i + 3, *ic = blk->ics[i], *nc = blk->ncs[i];
        ic[0] = 0;
        for (int i = 1; i < max_id; i++) ic[i] = ic[i-1] + nc[i-1] * l;
    }
    timer.stop("find(*)");
}

void find()
{
    /*
        ʹ����ռʽ���ؾ��ⷽʽ���߳��һ�
        ���껷֮��ȷ��ÿ������ָ������
    */
    Timer timer;
    // ��ʼ��
    for (int i = 0; i < NT_F; i++)
    {
        CBlock* blk = cblock + i;
        for (int j = 0; j < 5; j++)
        {
            blk->ns[j] = 0;
            blk->ccs[j] = (int*)malloc(20000000 * (j+3) * sizeof(int));
            blk->ncs[j] = (int*)calloc(max_id, sizeof(int));
            blk->ics[j] = (int*)malloc(max_id * sizeof(int));
        }
        blk->n_id = 0;
        blk->ids = (int*)malloc(max_id * sizeof(int));
    }
    // �һ�
    task = 0;
    pthread_t thread[NT_F];
    for (int i = 0; i < NT_F; i++)  pthread_create(&thread[i], NULL, find, cblock + i);
    for (int i = 0; i < NT_F; i++)  pthread_join(thread[i], NULL);
    // ȷ��ID���ĸ�block��
    uint8_t *in_blk = (uint8_t*)malloc(max_id * sizeof(uint8_t));
    memset(in_blk, 100, max_id);
    for (int i = 0; i < NT_F; i++)
    {
        CBlock* blk = cblock + i;
        for (int j = 0; j < 5; j++)
        {
            total_r += blk->ns[j];
            each_r[j] += blk->ns[j];
        }
        for (int j = 0; j < blk->n_id; j++)
        {
            in_blk[blk->ids[j]] = i;
        }
    }
    // ȷ�����л�·��ָ��
    pos = (int**)malloc(total_r * sizeof(int*));
    plen = (int*)malloc(total_r * sizeof(int));
    int ip = 0, *p, nn;
    CBlock *blk;
    for (int i = 0; i < 5; i++)
    {
        int l = i + 3;
        for (int j = 0; j < max_id; j++)
        {
            if (in_blk[j] == 100) continue;
            blk = cblock + in_blk[j];
            p = blk->ccs[i] + blk->ics[i][j];
            nn = blk->ncs[i][j];
            for (int k = 0; k < nn; k++)
            {
                plen[ip] = l;
                pos[ip++] = p;
                p += l;
            }
        }
    }
    printf("total_r: %d\neach: ", total_r);
    for (int i = 0; i < 5; i++) printf("%d ", each_r[i]);
    printf("\nip: %d\n", ip);
    timer.stop("find()");
}

void *circle2str(void* arg)
{
    /*
        �����תΪ�ַ�����д��
        ���ñ����ȷ���Ƿ��Ѿ����ǰ�滷�����ֽڳ���
        memcpy���ö��뼼�ɣ�ÿ�ζ�ָ������Ϊ16���´ΰ�����ʵ���ȿ�ʼ����
    */
    Timer timer;
    CRange* range = (CRange*)arg;
    int maxcsz = (total_r / 8 + 1) * 7 * 11;
    int i = range->i, begin = range->begin, end = range->end, csize = 0;
    char* str = (char*)malloc(maxcsz * sizeof(char));
    for (int j = begin; j < end; j++)
    {
        int *p = pos[j], l = plen[j];
        for (int k = 0; k < l; k++)
        {
            int c = p[k];
            char* ch = node2str + c * 16;
            int lc = node2len[c];
            memcpy(str+csize, ch, 16);
            csize += lc;
        }
        str[csize-1] = '\n';
    }
    timer.stop("c2s*");
    int wp;
    if (i == 0) {
        wp = fsize; fsize += csize; done[0] = 1;
        pwrite(fd, str, csize, wp);
    } else {
        while (1) if (done[i-1]) {wp = fsize; fsize += csize; done[i] = 1; break;}
        pwrite(fd, str, csize, wp);
    }
}

void save()
{
    /*
        �����תΪ�ַ�����д��
    */
    // ����
    char* odata = (char*)malloc(10 * sizeof(char));
    sprintf(odata, "%d\n", total_r);
    fsize = strlen(odata);
    fd = open(output_path, O_RDWR | O_CREAT, 0666);
    write(fd, odata, fsize);
    // ����д��
    float ratio[8] = {0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875, 1.0};
    crange[0].begin = 0;
    for (int i = 0; i < 8; i++)
    {
        crange[i].i = i;
        crange[i].end = int(ratio[i] * total_r);
        done[i] = 0;
    }
    for (int i = 1; i < 8; i++)  crange[i].begin = crange[i-1].end;
    crange[7].end = total_r;
    pthread_t thread[8];
    for (int i = 0; i < 8; i++) pthread_create(&thread[i], NULL, circle2str, crange + i);
    for (int i = 0; i < 8; i++)  pthread_join(thread[i], NULL);
    printf("fsize: %d\n", fsize);
}

int main(int argc, char *argv[])
{
    Timer timer1;
    load();
    timer1.stop("load");

    Timer timer2;
    find();
    timer2.stop("find");

    Timer timer3;
    save();
    timer3.stop("save");
    return 0;
}
