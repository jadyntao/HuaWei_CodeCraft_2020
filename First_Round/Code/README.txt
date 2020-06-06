test_main1：知乎大佬的第一个baseline版本 9.5
test_main2：在test_main1的基础上引入5+2策略
test_main3：在test_main1的基础上打算将vector改成array，没完成
test_main4：在test_main1的基础上打算将dfs递归改成迭代，没完成
test_main5：在test_main1的基础上将ofstream改成fwrite，加入了3入度和3出度的dfs剪枝 3.1
test_main6：在test_main5的基础上删除了3出度的dfs剪枝 2.8
test_main7：在test_main6的基础上添加了分长度存结果的vector 2.59
test_main8：在test_main7的基础上将使用vector删重换成使用set 
test_main9：在test_main8的基础上添加6+1 2.8
test_main10：在test_main7的基础上将vector改成数组，包括input 2.40
main(5)：在test_main7的基础上将vector改成数组，不包括input 2.37 永明的
test_main11：在test_main7的基础上使用reserve修改vector的最大容量，避免频繁扩容。
test_main12：在main(5)的基础上加入6+1
test_main13：在main(5)的基础上修改为先Graph和Graph2每行排序 1.55
main(7)：在main(5)的基础上开头用桶排，G1排序，G2不排序，加6+1，每次dfs用lower_bound搜索大于head的起始点开始搜 1.38
test_mian14: 在main(7)的基础上加上可视化计时
test_main15：在test_main14的基础上去除了ids
test_main16：在test_main14的基础上去将res和path从vector变成array
test61mmap(2)：在test_main14的基础上将fread和fwrite修改成mmap
test_main17：在test_main16的基础上将fread和fwrite修改成mmap 0.67
test_main18：在test_main17的基础上去除了ids 0.6284
finalmmapdxcstring2 :在test_main17的基础上加上多线程dfs，outbuf(12亿)
3线程[0,0.2,0.7,1]是0.4262分，[0,0.12,0.7,1]是0.4045分
main(finalmmapdxcstring2)：在finalmmapdxcstring2的基础上改为4线程[0,0.12,0.4,0.7,1] 0.4034分
main20:在main(finalmmapdxcstring2)的基础上去掉res，outbuf(3亿)在dfs里进行，4线程 0.3797（发现total_res_size有错，但结果还是有分，好奇怪）
test_main21:在main20的基础上修正了total_res_size的错误
test_main22:在main20的基础上去掉total_res_size，直接输出700万，0.3711
test_main23:在main22的基础上将4线程的{0,0.12,0.4,0.7,1}改成{0,0.06,0.12,0.23,1} 0.3492
test_main24:在test_main21的基础上修改为多线程抢占，0.3674
test_main25:在test_main23的基础上优化for里的判断语句，dfs里的memcpy发现了bug改为固定7 0.3453
main(026):在test_main24的基础上优化for里的判断语句，dfs里的memcpy改为固定7，在test_main24基础上去掉mmap_write的一句，thread改成pthread 0.26
main_final:在main(026)的基础上dfs_cut减少一层，限制为5W个结点，dfs里的memcpy改为固定6，main函数里优化了*cs+i步骤 0.2101