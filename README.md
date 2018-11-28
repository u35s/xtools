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

```
# xtools configue example
[a]
local 127.0.0.1 root pass 22 tag1|tag2
[b]
local 127.0.0.1 root pass 22 tag1|tag2
local 127.0.0.1 root pass 22 tag1|tag2
local 127.0.0.1 root pass 22 tag1|tag3
```

# 使用

```
" 远程登录 TODO

./xsh l(login)      列出所有机器
./xsh l(login) a    列出a组下的机器

./xsh l(login) 0    登录0号机器
./xsh l(login) a-0  登录a组下的0号机器

" 远程执行命令
./xsh r(run) 0 ls       在0号机器执行命令
./xsh r(run) a-0 ls     在a组的0号机器执行命令
./xsh r(run) a-tag1 ls  在a组持有tag1的所有机器执行命令
./xsh r(run) a ls       在a组的所有机器执行命令

" 脚本接口 TODO 
./xsh i 0       /tmp/bash.sh  执行脚本并把0号机器的IP,PORT,USER,PASSWORD当做参数传给脚本
./xsh i a-0     /tmp/bash.sh  执行脚本并把a组的0号机器的IP,PORT,USER,PASSWORD当做参数传给脚本
./xsh i a-tag1  /tmp/bash.sh  多次执行脚本并把a组持有tag1的所有机器的IP,PORT,USER,PASSWORD依次当做参数传给脚本
./xsh i a       /tmp/bash.sh  多次执行脚本并把a组的所有机器的IP,PORT,USER,PASSWORD依次当做参数传给脚本

" 远程拷贝
./xcp ./tmp/file    0:/tmp/     本地到远程0号机器
./xcp ./tmp/file    a-0:/tmp/   本地到远程a组下的0号机器
./xcp ./tmp/file    a:/tmp/     本地到远程a组下的所有机器
./xcp 0:/tmp/file   ./tmp/file  远程0号机器到本地
./xcp a-0:/tmp/file ./tmp/file  远程a组下0号机器到本地
```
