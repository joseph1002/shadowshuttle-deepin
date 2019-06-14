# shadowshuttle-deepin
本工程基于[shadowsocks-deepin](https://github.com/lolimay/shadowsocks-deepin) 修改。

shadowsocks-deepin作者似乎停止更新了。出于对QT热爱和研究shadowsocks的目的，我在该项目v1.2.1基础上继续开发。因为底层做了大量的重构，就独立出来，工程叫shadowshuttle-deepin（以下称ss-deepin）。



# 开发目标

- 参考shadowsocks-windows（以下称ss-win）的设计与实现。将ss-win中常用的的功能移植过来。

- deepin的开发文档匮乏，尽量不使用dtk的控件。



# 构建

````
sudo apt update
sudo apt install qt5-default qttools5-dev-tools qt5-qmake g++ qtcreator qttools5-dev -y
sudo apt install libdframeworkdbus-dev libdtkwidget-dev libdtkcore-dev -y
sudo apt install libqtshadowsocks-dev libqrencode-dev libzbar-dev libquazip5-dev -y
cd shadowshuttle-deepin
mkdir build && cd build
cmake ..
make -j4
cpack
````

# 说明

我已经努力按照ss-win的交互去做，但是由于操作系统不同，以及使用到的库限制，仍有一些差异。

- ss-win本地代理端口不仅是socks5的监听端口，同时也实现了http的监听，因此它的本地pac url其实是http链接。ss-deepin用的libqtshadowsocks库只实现了socks5，我也不愿意把它源码拷进来修改。因此ss-deepin使用的是本地pac url。
- “保护本地PAC”没有。ss-win该功能做法是对本地pac的http地址加了时间戳，而ss-deepin本地pac没有http url，自然就没有这项功能了。
- “复制本地 PAC 网址”你会发现获取的是file://前缀的url。原因如上。
- “编辑在线 PAC 网址”其实也支持本地文件地址，只要file://+本地文件绝对路径即可。
- 便携模式个人认为没什么用，不考虑了。

