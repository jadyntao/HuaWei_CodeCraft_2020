test_main1��֪�����еĵ�һ��baseline�汾 9.5
test_main2����test_main1�Ļ���������5+2����
test_main3����test_main1�Ļ����ϴ��㽫vector�ĳ�array��û���
test_main4����test_main1�Ļ����ϴ��㽫dfs�ݹ�ĳɵ�����û���
test_main5����test_main1�Ļ����Ͻ�ofstream�ĳ�fwrite��������3��Ⱥ�3���ȵ�dfs��֦ 3.1
test_main6����test_main5�Ļ�����ɾ����3���ȵ�dfs��֦ 2.8
test_main7����test_main6�Ļ���������˷ֳ��ȴ�����vector 2.59
test_main8����test_main7�Ļ����Ͻ�ʹ��vectorɾ�ػ���ʹ��set 
test_main9����test_main8�Ļ��������6+1 2.8
test_main10����test_main7�Ļ����Ͻ�vector�ĳ����飬����input 2.40
main(5)����test_main7�Ļ����Ͻ�vector�ĳ����飬������input 2.37 ������
test_main11����test_main7�Ļ�����ʹ��reserve�޸�vector���������������Ƶ�����ݡ�
test_main12����main(5)�Ļ����ϼ���6+1
test_main13����main(5)�Ļ������޸�Ϊ��Graph��Graph2ÿ������ 1.55
main(7)����main(5)�Ļ����Ͽ�ͷ��Ͱ�ţ�G1����G2�����򣬼�6+1��ÿ��dfs��lower_bound��������head����ʼ�㿪ʼ�� 1.38
test_mian14: ��main(7)�Ļ����ϼ��Ͽ��ӻ���ʱ
test_main15����test_main14�Ļ�����ȥ����ids
test_main16����test_main14�Ļ�����ȥ��res��path��vector���array
test61mmap(2)����test_main14�Ļ����Ͻ�fread��fwrite�޸ĳ�mmap
test_main17����test_main16�Ļ����Ͻ�fread��fwrite�޸ĳ�mmap 0.67
test_main18����test_main17�Ļ�����ȥ����ids 0.6284
finalmmapdxcstring2 :��test_main17�Ļ����ϼ��϶��߳�dfs��outbuf(12��)
3�߳�[0,0.2,0.7,1]��0.4262�֣�[0,0.12,0.7,1]��0.4045��
main(finalmmapdxcstring2)����finalmmapdxcstring2�Ļ����ϸ�Ϊ4�߳�[0,0.12,0.4,0.7,1] 0.4034��
main20:��main(finalmmapdxcstring2)�Ļ�����ȥ��res��outbuf(3��)��dfs����У�4�߳� 0.3797������total_res_size�д�����������з֣�����֣�
test_main21:��main20�Ļ�����������total_res_size�Ĵ���
test_main22:��main20�Ļ�����ȥ��total_res_size��ֱ�����700��0.3711
test_main23:��main22�Ļ����Ͻ�4�̵߳�{0,0.12,0.4,0.7,1}�ĳ�{0,0.06,0.12,0.23,1} 0.3492
test_main24:��test_main21�Ļ������޸�Ϊ���߳���ռ��0.3674
test_main25:��test_main23�Ļ������Ż�for����ж���䣬dfs���memcpy������bug��Ϊ�̶�7 0.3453
main(026):��test_main24�Ļ������Ż�for����ж���䣬dfs���memcpy��Ϊ�̶�7����test_main24������ȥ��mmap_write��һ�䣬thread�ĳ�pthread 0.26
main_final:��main(026)�Ļ�����dfs_cut����һ�㣬����Ϊ5W����㣬dfs���memcpy��Ϊ�̶�6��main�������Ż���*cs+i���� 0.2101