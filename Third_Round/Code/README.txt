test_main1��jsw7��:dijskra 

test_main2: ��test_main1�Ļ����ϼ��϶�ʱ��ӡ��������#define maxn 2500000��
    ����juesai���ݼ���init: 0.115785(s)  get: 0.001547(s)  solve: 0.019346(s)  save: 0.002406(s)
    ����9153_2���ݼ���init: 0.114673(s)  get: 0.006526(s)  solve: 0.212423(s)  save: 0.004681(s)

test_main3: ��test_main2�Ļ����Ͻ�cal_vcnt�ĳ�brandes�ļ��㷽����#define maxn 2500000��
    ����juesai���ݼ���init: 0.124728(s)  get: 0.001597(s)  solve: 0.016351(s)  save: 0.004208(s)
    ����9153_2���ݼ���init: 0.124725(s)  get: 0.006451(s)  solve: 0.169883(s)  save: 0.008796(s)

test_main4:��test_main3�Ļ�����get_input�ĳ������Ŷӵ�load_input��Gʹ�÷Ѿջ�ͼ�Ĵ洢������ʹ�ù̶�����MAX_OUT_DU��

test_mian5:��test_main4�Ļ����Ͻ�G�洢�ĳɷǾջ�ͼ�Ĵ洢��ʽ���Լ�dji�Ĳ��ֱ����ĳ�memset�����
    ����ʱ�䣺1349.3581
    �������ݼ�juesai1��real    14m17.339s
        init: 0.351145(s)
        get: 0.572759(s)
        solve: 854.985229(s)
        save: 0.846762(s)
        result is :1
        compare: 0.000158(s)
        max_id : 792421
        dji_num :3237110540
    �������ݼ�697518��real    1m10.068s
        init: 0.337848(s)
        get: 0.021612(s)
        solve: 69.372345(s)
        save: 0.019434(s)
        result is :1
        compare: 0.000129(s)
        max_id : 25000
        dji_num :605575777
    �������ݼ�juesai:  real    0m0.108s
        init: 0.038963(s)
        get: 0.001237(s)
        solve: 0.039504(s)
        save: 0.003970(s)
        result is :1
        compare: 0.000122(s)
        max_id : 5001
        dji_num :46574


test_mian7:��test_main5�Ļ����Ͻ������Ϊ1�ĺϲ���һ��ȥDji
    ����juesai1: real    15m19.965s
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

test_mian8:��test_main7�Ļ������Ż������Ĳ���
    ����juesai    real    0m0.488s
        init: 0.297526(s)
        get: 0.001772(s)
        solve: 0.003455(s)
        save: 0.004050(s)
        result is :1
        compare: 0.000109(s)
        max_id : 5001
        dji_num :26906
    ����juesai1:   real    8m32.205s
        init: 0.293317(s)
        get: 0.601110(s)
        solve: 510.171326(s)
        save: 0.825101(s)
        result is :1
        compare: 0.000132(s)
        max_id : 792421
        dji_num :3023291485
    ����juesai2:   real    11m40.521s
        init: 0.296986(s)
        get: 0.109227(s)
        solve: 699.851929(s)
        save: 0.049987(s)
        result is :1
        compare: 0.000140(s)
        max_id : 59900
        dji_num :3588010000

test_main9:��test_mian5�����Ϻϲ�0���1���ȵĽڵ㣬��stack��������
    ����juesai2���ݼ���real    10m37.348s
        init: 0.308928(s)
        get: 0.118141(s)
        solve: 636.535461(s)
        save: 0.050431(s)
        result is :1
        compare: 0.000122(s)
        max_id : 59900
        dji_num :3588010000
    
    ����juesai1���ݼ���real    1m54.880s  2m11.198s
        init: 0.308659(s)
        get: 0.604168(s)
        solve: 112.652687(s)
        save: 0.836281(s)
        result is :1
        compare: 0.000137(s)
        max_id : 792421
        dji_num :891462716

test_main10:��test_main9�Ļ������Ż���stk��ı����������˾ջ�ͼ�ͷǾջ�ͼ
����juesai2���ݼ���real    9m17.153s - 10m41.665s
    init: 0.344892(s)
    get: 0.129644(s)
    solve: 556.234497(s)
    save: 0.050727(s)
    result is :1
    compare: 0.000136(s)
    max_id : 59900
    dji_num :3588010000

test_main12:��test_main10�Ļ����ϣ���prv��dis�ȶ�ά��������һά�ֲ�����
    ����juesai1���ݼ���real 1m27.818s  1m43.385s
        init: 0.000006(s)
        get: 0.760992(s)
        max_id:792421
        solve: 86.089485(s)
        save: 0.849860(s)
        result is :1
        compare: 0.000125(s)
        max_id : 792421
    ����juesai2���ݼ���real 7m42.173s
        init: 0.000009(s)
        get: 0.126942(s)
        max_id:59900
        solve: 461.984406(s)
        save: 0.050243(s)
        result is :1
        compare: 0.000114(s)
        max_id : 59900
test_main13������graph.txt������� ���� �����
test_main13������graph.txt������� ����� ; ���Ƚ����
test_main14:��test_main12�Ļ����ϣ��Ż���sigma��dis�����
    ����juesai1���ݼ���real 1m27.818s  1m17.372s
        init: 0.000006(s)
        get: 0.746279(s)
        max_id:792421
        solve: 75.667000(s)
        save: 0.838945(s)
        result is :0
        compare: 0.000072(s)
        max_id : 792421
    ����juesai2���ݼ���real 7m22.957s
        init: 0.000013(s)
        get: 0.187627(s)
        max_id:59900
        solve: 442.703705(s)
        save: 0.051290(s)
        result is :1
        compare: 0.000706(s)
        max_id : 59900

test_main15:��test_main14�Ļ����Ͻ����ȶ��л�������+��������
test_main16:��test_main14�Ļ����ϣ���dis��sigma��delta�ϳ�һ��struct��
test_main17:��test_main16�Ļ����ϣ���prv�������飬����ṹ����
    ����juesai2��real    8m20.699s
        init: 0.000005(s)
        get: 0.130435(s)
        max_id:59900
        solve: 500.503571(s)
        save: 0.050227(s)
        result is :1
        compare: 0.000110(s)
        max_id : 59900
    ����juesai1��real    1m8.241s
        init: 0.000012(s)
        get: 0.758413(s)
        max_id:792421
        solve: 66.483276(s)
        save: 0.847847(s)
        result is :1
        compare: 0.000124(s)
        max_id : 792421

test_main18:��test_main14�Ļ����Ͻ�prv��vector��������
    ����juesai2���ݼ���    real    7m28.583s
        init: 0.000010(s)
        get: 0.132707(s)
        max_id:59900
        solve: 448.386047(s)
        save: 0.050197(s)
        result is :1
        compare: 0.000106(s)
        max_id : 59900
    ����juesai1���ݼ���        real    1m35.614s
        init: 0.000010(s)
        get: 0.755294(s)
        max_id:792421
        solve: 93.886681(s)
        save: 0.852424(s)
        result is :1
        compare: 0.000123(s)
        max_id : 792421
test_main19:��test_main14�Ļ����Ͻ����ȶ��иĳ����ԶΣ�δ��ɣ�
test_main20:��test_main14�Ļ����Ͻ�һ���Ⱥϲ���dji��δ��ȷ��
t1417�������14��17�����ʹ�þջ�ͼ�ͷǾջ�ͼ��*T����λ����

    
jsw12ep:��jsw12mm��������push��emplace��ע��munmap ���ϣ�1414.9111 
jsw14:��jsw12ep������assign��insert��lendouble��lenlֻ��һ�δ���� �������ݼ����ϣ�2142.5711
�������ݼ�697518��real    1m12.038s
    init: 0.395254(s)
    get: 0.080802(s)
    solve: 71.142212(s)
    save: 0.003833(s)
    result is :1
    compare: 0.000150(s)
    node_num : 25001
����juesai1���ݼ���7m48.5s
    init: 0.379658(s)
    get: 1.025564(s)
    solve: 466.405365(s)
    save: 0.156728(s)
    dji_num : 3419433147

jsw13mm:jsw14�����ĳ�dij+bfs��ʵ�ַ�����struct���𰸴���
wz13mm:jsw14�����ĳ�dij+bfs��ʵ�ַ�����struct�����ϳ�ʱ

128.cpp:jsw14+test_mian8��
����juesai1���ݼ���8m11.058s
init: 0.383583(s)
get: 1.047885(s)
solve: 488.942230(s)
save: 0.156944(s)
dji_num : 3193244285

z17:jsw14�����Ϻϲ�0���1���ȵĽڵ㣬��stack��������
����juesai1���ݼ���2m18s
init: 0.385023(s)
get and cut: 1.100193(s)
solve: 136.118591(s)
save: 0.170248(s)
����juesai2���ݼ���10m11.950s
init: 0.379319(s)
get and cut: 0.512547(s)
solve: 610.741028(s)
save: 0.009240(s)

wz77new:z17�Ľ��������ݽṹ��struct��װ������cachemiss
����juesai1���ݼ���
real    1m35.447s
����juesai���ݼ���
real    12m24.85s

wz88:wz77new�Ľ�,Graph��Ϊ��̬�����С����棬������dji����
����juesai1���ݼ���
real    1m26s
����juesai2���ݼ���
real    10m9.080s


wz99:w88�Ľ���resize��������Ϊ��ͼ��juesai1resize=30��juesai2resize=5��dis��dij�ڵ�������һά����
����juesai1���ݼ���
real    1m26s


����juesai2���ݼ���
init: 1.102627(s)
get and cut: 0.412901(s)
solve: 611.364380(s)
save: 0.009445(s)
result is :1
compare: 0.000134(s)

real    10m13.504s
