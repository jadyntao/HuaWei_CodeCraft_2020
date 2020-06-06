#!/bin/bash

# 使用说明：
#   接受一个参数，即测试代码的路径
#   如果有第二个参数，第二个参数需要是测试文件所在文件夹名，这时会单独测试这个文件
#   “常量定义”中的路径可以自行修改
#   代码输入输出地址请设置成build_path下的test_data.txt文件
#   test中的字符表示测试数据放置的文件夹名称，测试数据请统一命名为test_data.txt，答案命名为result.txt

# 测试数据集(可修改)
test=(  "selfTest1"\
        "office"\
        "test0" \
        "test1" \
        "test2" \
        "test3" \
        "test4" \
        "test5" \
        "test6" \
        "test7" \
        "test8" \
        "test9" \
        "test10"\
        "test11"\
        "test12"\
        "office1")

# 常量定义(可修改)
test_data_prefix="./data/final/"
output_path="./output/result.txt"
build_path="./build/"

# 其他变量
test_file="$build_path""test"
cnt=0
total_cnt=0

# 创建工作目录
if [ ! -d $build_path ]; then
    mkdir $build_path
fi

# 编译代码
if [ -f $test_file ]; then
    rm $test_file
fi
g++ -O3 $1 -o $test_file -lpthread -fpic -D TEST
if [ ! -f $test_file ]; then
    echo -e "\033[31mg++生成可执行文件失败\033[0m"
    exit 0
fi
echo -e "\033[33m编译完成\033[0m"

# 测试函数，接受两个参数，第一个是测试名（只用于显示）,第二个是测试路径（只到文件夹）
function checkDiff(){
    echo -e "\033[33m开始测试$1\033[0m"
    cp "$test_data_prefix$1""/test_data.txt" $build_path"test_data.txt"
    if [ -f $output_path ]; then
        rm $output_path
    fi
    time $test_file
    echo -e "\033[33m---------------------\033[0m"
    if [ -f $output_path ]; then
        diff "$test_data_prefix$1""/result.txt" $output_path -y > $build_path"diff$1.txt"
        diff_res=$(diff -qBw "$test_data_prefix$1""/result.txt" $output_path)
        if [[ $diff_res == *"differ"* ]]; then
            echo -e "\033[31m$1答案有误，参见文件""$build_path""diff""$1"".txt""\033[0m"
        else
            return 1
        fi
    else
        echo -e "\033[31m测试""$1""未生成文件\033[0m"
    fi
    return 0
}

# 开始比对答案
if [ $# -eq 1 ]; then
    for file in "${test[@]}"; do
        checkDiff "$file"
        ((cnt="$cnt"+"$?"))
        ((total_cnt="$total_cnt"+1))
    done
    # 输出测试情况
    echo -e "\033[33m最终结果：\033[32m$cnt/$total_cnt\033[0m"
elif [ $# -eq 2 ]; then
    checkDiff "$2"
else
    echo -e "\033[31m输入参数数量有误\033[0m"
fi


