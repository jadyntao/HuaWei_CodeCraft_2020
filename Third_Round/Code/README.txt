test_main1（jsw7）:dijskra 

test_main2: 在test_main1的基础上加上定时打印函数。（#define maxn 2500000）
    对于juesai数据集：init: 0.115785(s)  get: 0.001547(s)  solve: 0.019346(s)  save: 0.002406(s)
    对于9153_2数据集：init: 0.114673(s)  get: 0.006526(s)  solve: 0.212423(s)  save: 0.004681(s)

test_main3: 在test_main2的基础上将cal_vcnt改成brandes的计算方法（#define maxn 2500000）
    对于juesai数据集：init: 0.124728(s)  get: 0.001597(s)  solve: 0.016351(s)  save: 0.004208(s)
    对于9153_2数据集：init: 0.124725(s)  get: 0.006451(s)  solve: 0.169883(s)  save: 0.008796(s)

test_main4:在test_main3的基础上get_input改成以上团队的load_input（G使用费菊花图的存储方法，使用固定区间MAX_OUT_DU）

test_mian5:在test_main4的基础上将G存储改成非菊花图的存储方式，以及dji的部分变量改成memset来清空
    线上时间：1349.3581
    对于数据集juesai1：real    14m17.339s
        init: 0.351145(s)
        get: 0.572759(s)
        solve: 854.985229(s)
        save: 0.846762(s)
        result is :1
        compare: 0.000158(s)
        max_id : 792421
        dji_num :3237110540
    对于数据集697518：real    1m10.068s
        init: 0.337848(s)
        get: 0.021612(s)
        solve: 69.372345(s)
        save: 0.019434(s)
        result is :1
        compare: 0.000129(s)
        max_id : 25000
        dji_num :605575777
    对于数据集juesai:  real    0m0.108s
        init: 0.038963(s)
        get: 0.001237(s)
        solve: 0.039504(s)
        save: 0.003970(s)
        result is :1
        compare: 0.000122(s)
        max_id : 5001
        dji_num :46574


test_mian7:在test_main5的基础上将出入度为1的合并到一起去Dji
    对于juesai1: real    15m19.965s
        init: 0.297409(s)
        one_in_out: 655830
        max_id: 792421
        get: 0.609961(s)
        solve: 917.771790(s)
        save: 0.825891(s)
        result is :1
        compare: 0.000122(s)
        max_id : 792421
        dji_num :3193146418

test_mian8:在test_main7的基础上优化了锁的部分
    对于juesai    real    0m0.488s
        init: 0.297526(s)
        get: 0.001772(s)
        solve: 0.003455(s)
        save: 0.004050(s)
        result is :1
        compare: 0.000109(s)
        max_id : 5001
        dji_num :26906
    对于juesai1:   real    8m32.205s
        init: 0.293317(s)
        get: 0.601110(s)
        solve: 510.171326(s)
        save: 0.825101(s)
        result is :1
        compare: 0.000132(s)
        max_id : 792421
        dji_num :3023291485
    对于juesai2:   real    11m40.521s
        init: 0.296986(s)
        get: 0.109227(s)
        solve: 699.851929(s)
        save: 0.049987(s)
        result is :1
        compare: 0.000140(s)
        max_id : 59900
        dji_num :3588010000

test_main9:在test_mian5基础上合并0入度1出度的节点，将stack换成数组
    对于juesai2数据集：real    10m37.348s
        init: 0.308928(s)
        get: 0.118141(s)
        solve: 636.535461(s)
        save: 0.050431(s)
        result is :1
        compare: 0.000122(s)
        max_id : 59900
        dji_num :3588010000
    
    对于juesai1数据集：real    1m54.880s  2m11.198s
        init: 0.308659(s)
        get: 0.604168(s)
        solve: 112.652687(s)
        save: 0.836281(s)
        result is :1
        compare: 0.000137(s)
        max_id : 792421
        dji_num :891462716

test_main10:在test_main9的基础上优化了stk后的遍历，区分了菊花图和非菊花图
对于juesai2数据集：real    9m17.153s - 10m41.665s
    init: 0.344892(s)
    get: 0.129644(s)
    solve: 556.234497(s)
    save: 0.050727(s)
    result is :1
    compare: 0.000136(s)
    max_id : 59900
    dji_num :3588010000

test_main12:在test_main10的基础上，将prv，dis等二维变量换成一维局部变量
    对于juesai1数据集：real 1m27.818s  1m43.385s
        init: 0.000006(s)
        get: 0.760992(s)
        max_id:792421
        solve: 86.089485(s)
        save: 0.849860(s)
        result is :1
        compare: 0.000125(s)
        max_id : 792421
    对于juesai2数据集：real 7m42.173s
        init: 0.000009(s)
        get: 0.126942(s)
        max_id:59900
        solve: 461.984406(s)
        save: 0.050243(s)
        result is :1
        compare: 0.000114(s)
        max_id : 59900
test_main13：生成graph.txt，即入度 出度 结点数
test_main13：生成graph.txt，即入度 结点数 ; 出度结点数
test_main14:在test_main12的基础上，优化了sigma和dis的清空
    对于juesai1数据集：real 1m27.818s  1m17.372s
        init: 0.000006(s)
        get: 0.746279(s)
        max_id:792421
        solve: 75.667000(s)
        save: 0.838945(s)
        result is :0
        compare: 0.000072(s)
        max_id : 792421
    对于juesai2数据集：real 7m22.957s
        init: 0.000013(s)
        get: 0.187627(s)
        max_id:59900
        solve: 442.703705(s)
        save: 0.051290(s)
        result is :1
        compare: 0.000706(s)
        max_id : 59900

test_main15:在test_main14的基础上将优先队列换成数组+插入排序
test_main16:在test_main14的基础上，将dis，sigma，delta合成一个struct。
test_main17:在test_main16的基础上，将prv换成数组，放入结构体中
    对于juesai2：real    8m20.699s
        init: 0.000005(s)
        get: 0.130435(s)
        max_id:59900
        solve: 500.503571(s)
        save: 0.050227(s)
        result is :1
        compare: 0.000110(s)
        max_id : 59900
    对于juesai1：real    1m8.241s
        init: 0.000012(s)
        get: 0.758413(s)
        max_id:792421
        solve: 66.483276(s)
        save: 0.847847(s)
        result is :1
        compare: 0.000124(s)
        max_id : 792421

test_main18:在test_main14的基础上将prv从vector换成数组
    对于juesai2数据集：    real    7m28.583s
        init: 0.000010(s)
        get: 0.132707(s)
        max_id:59900
        solve: 448.386047(s)
        save: 0.050197(s)
        result is :1
        compare: 0.000106(s)
        max_id : 59900
    对于juesai1数据集：        real    1m35.614s
        init: 0.000010(s)
        get: 0.755294(s)
        max_id:792421
        solve: 93.886681(s)
        save: 0.852424(s)
        result is :1
        compare: 0.000123(s)
        max_id : 792421
test_main19:在test_main14的基础上将优先队列改成线性段（未完成）
test_main20:在test_main14的基础上将一出度合并到dji（未正确）
t1417：结合了14与17分情况使用菊花图和非菊花图，*T换成位运算

    
jsw12ep:在jsw12mm基础加了push变emplace，注释munmap 线上：1414.9111 
jsw14:在jsw12ep基础用assign加insert，lendouble，lenl只算一次存好了 更新数据集线上：2142.5711
对于数据集697518：real    1m12.038s
    init: 0.395254(s)
    get: 0.080802(s)
    solve: 71.142212(s)
    save: 0.003833(s)
    result is :1
    compare: 0.000150(s)
    node_num : 25001
对于juesai1数据集：7m48.5s
    init: 0.379658(s)
    get: 1.025564(s)
    solve: 466.405365(s)
    save: 0.156728(s)
    dji_num : 3419433147

jsw13mm:jsw14基础改成dij+bfs，实现分数类struct，答案错误
wz13mm:jsw14基础改成dij+bfs，实现分数类struct，线上超时

128.cpp:jsw14+test_mian8：
对于juesai1数据集：8m11.058s
init: 0.383583(s)
get: 1.047885(s)
solve: 488.942230(s)
save: 0.156944(s)
dji_num : 3193244285

z17:jsw14基础上合并0入度1出度的节点，将stack换成数组
对于juesai1数据集：2m18s
init: 0.385023(s)
get and cut: 1.100193(s)
solve: 136.118591(s)
save: 0.170248(s)
对于juesai2数据集：10m11.950s
init: 0.379319(s)
get and cut: 0.512547(s)
solve: 610.741028(s)
save: 0.009240(s)

wz77new:z17改进，把数据结构用struct包装，减少cachemiss
对于juesai1数据集：
real    1m35.447s
对于juesai数据集：
real    12m24.85s

wz88:wz77new改进,Graph分为动态数组和小数组存，分两种dji方法
对于juesai1数据集：
real    1m26s
对于juesai2数据集：
real    10m9.080s


wz99:w88改进，resize比例调整为按图，juesai1resize=30，juesai2resize=5，dis等dij内的数组用一维数组
对于juesai1数据集：
real    1m26s


对于juesai2数据集：
init: 1.102627(s)
get and cut: 0.412901(s)
solve: 611.364380(s)
save: 0.009445(s)
result is :1
compare: 0.000134(s)

real    10m13.504s
