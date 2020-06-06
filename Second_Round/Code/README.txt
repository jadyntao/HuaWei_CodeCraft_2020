test_main3.cpp  6+1 33s
test_main4.cpp  在test_main3.cpp的基础上优化了dfs和dfs_cut_backward的部分if语句 28s
test_main5.cpp  在test_main4.cpp的基础上将dfs_cut_backward从递归改成迭代 27s 取600时线上内存不够
test_main6.cpp  在test_main5.cpp的基础上dfs_cut_backward改成5+2
test_main7.cpp  在test_main5.cpp的基础上将Graph从array改成vector 线上15.5128 服务器31.5-33.5s
test_main8.cpp  在test_main7.cpp的基础上进行平行改动，将基于出结点排序编号变成基于入结点
test_main9.cpp  在test_main7.cpp的基础上对dfs_cut的三领域也进行了对menoy的判断 线上12.9 服务器25.8-27.8s
test_main10.cpp  在test_main9.cpp的基础上将基于出结点排序编号变成基于入结点 线上13.1
test_main11.cpp  在test_main10.cpp的基础上去掉dfs_cut的visit，对dfs2的if改成两个。线上12.8
test_mian14.cpp  在test_main11.cpp的基础上把6+1换成5+2  9.78
test_mian15.cpp  在test_main14.cpp的基础上加入前向第一个点优化 first_one 服务器25s
test_mian16.cpp  在test_main15.cpp的基础上将dfs递归改成迭代 线上9.459  服务器22.7s
test_mian17.cpp  在test_main16.cpp的基础上将优化了forward，把two_border_head从vector改成array 服务器21.5s  线上0% runtime error
test_mian18.cpp  在test_main17.cpp的基础上换成dfs递归，加入4+3与5+2的结合 服务器25s（未完成）
test_mian19.cpp  在test_main17.cpp的基础上，加入4+3与5+2的结合 服务器25s 线上11.38
test_mian20.cpp  在test_main17.cpp的基础上将graph2从vector改成前向星 服务器25s
test_main22.cpp(w18)在test_main14.cpp（即递归版，无first_one的5+2）上graph和graph2改成前向星 22.6s
test_main23.cpp  在test_main22.cpp的基础上改成将递归改成多函数 21.7s
test_main15_2.cpp 在test_main23.cpp的基础上将前向星改回vector 23.6s
test_main24.cpp  在test_main23.cpp的基础上将dfs里的<=&&换成<||,三个局部变量拆分 21.4s
test_main25.cpp  在test_main24.cpp的基础上将Graph,Graph2,two_border改成用指针遍历 21.1s
test_main26.cpp  在test_main25.cpp的基础上将three_border只存depth=3的点。20.9s

test_main27.cpp  多函数版 在test_main26.cpp的基础上改成复赛B榜，环最大长度为8，输入为浮点型  0%result
w22.cpp 迭代版 unsigned long long 48000000000 <=&& 非指针 改了input three_border存3 改了input 11.8606  
test_main29.cpp 在w22基础上three_border存1  11.84