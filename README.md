> ”做一个小项目可以更好地学习音视频解码“，所以这个小项目开始了  
> 由于注释不友好，项目暂不适合初学者

## 目前功能
- 常规拆包，能seek
- 音频解码，视频解码
- 音频重采样
- 视频格式转换与显示（QImage+paintEvent实现）
- 多线程（主要通过将处理音频和视频的程序分开，而达到加速的目的）
- 基于qt的音频、视频播放

## 待扩展功能
- 处理网络流、摄像头流
- 音视频同步、同时控制播放速度
- 视频格式转换改为shader的方式，从而加快速度
- 视频处理，如：滤镜、马赛克、缩放、二进制阀值化、饱和度
- 界面功能：如：进度条、更改播放速度、等
- 代码注释不太友好

## 运行
- 环境：ubuntu
- 编辑器：qt creator
- 提前安装：ffmpeg(我装的是当前的最新版！)、qt5, 并找到头文件位置
- clone
- CMakeLists.txt里 更改ffmpeg 头文件位置 ```include_directories()```
- 更改视频文件位置，将main.cpp中的```/home/king/workspace/opencv/resource/1.map4```替换为你电脑中视频文件的位置
- qt creator 打开（选择CMakeLists.txt） 运行

## 我的学习资源
- 入门：某宝上几块钱弄来的课程（强烈支持正版！），下图是该课程中的代码结构
- 扩展：github上的```ffmpeg qt```项目

![代码大致结构](https://github.com/russionbear/streamControl/doc/codeStruct1.png)


## 感悟
- 代码风格优化，如：采用内联函数给程序加速、利用宏调bug或配置多种程序设计方案
- 内存管理常是个问题（课程老师的观点）
- cmake + vscode 环境搭建 花了学多时间，建议虚拟机ubuntu+vscode的ssh，不用vs20XX, 少用qt creator
