# NetWorkBookStore
# 网上书店系统

## 简介
>     该系统服务器端采用多并发处理客户端的请求，服务器与客户端采用TCP协议，如果并发高的话建议使用UDP协议
>     采用TCP的主要原因是由于TCP实现更容易，而且并发不是太高，并且采用UDP协议的话还要写算法，贼鸡儿麻烦
>     开启一个线程进行errno检测 类似于linux中信号的概念
>     当出错时有相应的错误处理函数执行，错误执行函数可以由开发者重新定义，类似于signal函数
>     你可以选择忽略此错误，或者采用默认错误处理，或者自定义默认错误处理

## 2018.5.11更
>     该系统我已经写了四天了，较前几个系统吧来说这个系统更接近于linux的编程风格
>     比如加了errno对程序进行实时的错误检测，采用多并发服务器处理客户端的请求
>     命名规范并且传输协议包更轻巧，函数分模块更科学，操作界面采用命令行的模式进行操作
>     前两个项目会等我哪天有心情再更吧
>     服务器端8个文件：
>     nwbs_errno.h、nwbs_errno.c、nwbs_handler.h、nwbs_handler.c、nwbs_menu.h、nwbs_menu.c、
>     nwbs_types.h、main.c
>     客户端6个文件：
>     nwbs_errno.h、nwbs_errno.c、nwbs_handler.h、nwbs_handler.c、nwbs_types.h、main.c

## 2018.5.12更
>     优化部分代码，修复部分bug，新增了图书列表、图书下载功能
>     用户可以从服务器获取图书列表和、下载图书
>     新增8个文件，其中Makefile和一个shell脚本文件，一键编译运行
>     服务器端4个文件：
>     nwbs_lib.h、nwbs_lib.c、Makefile、run.sh
>     客户端4个文件：
>     nwbs_lib.h、nwbs_lib.c、Makefile、run.sh 

## 2018.5.13更
>     优化部分代码，修复部分bug，新增了图书信息获取功能
>     用户可以从服务器获取指定图书的信息
>     无新增文件

## 2018.5.15更
>     优化部分代码，修复bug，新增管理员图书上传功能
>     管理员可以从本地上传至服务器
>     无新增文件
