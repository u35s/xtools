# xtools
xtools包含xsh xcp工具
提供ssh登录,scp拷贝功能,但是多台机器登录拷贝更方便

# 安装
```shell
yum install gcc-c++ cmake
git clone https://github.com/u35s/xtools.git
mkdir build && cd build && cmake ../xtools  && make 
```
# 配置

# 使用

```
" 远程登录

./xsh l(login)      列出所有机器
./xsh l(login) a    列出a组下的机器

./xsh l(login) 0    登录0号机器
./xsh l(login) a-0  登录a组下的0号机器

" 远程执行命令
./xsh r(run) 0 ls
./xsh r(run) a-0 ls
./xsh r(run) a-tag1 ls
./xsh r(run) a ls

" 远程拷贝
./xcp ./tmp/file    0:/tmp/     本地到远程0号机器
./xcp ./tmp/file    a-0:/tmp/   本地到远程a组下的0号机器
./xcp ./tmp/file    a:/tmp/     本地到远程a组下的所有机器
./xcp 0:/tmp/file   ./tmp/file  远程0号机器到本地
./xcp a-0:/tmp/file ./tmp/file  远程a组下0号机器到本地
```
