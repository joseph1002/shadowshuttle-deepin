# shadowshuttle-deepin
本工程基于[shadowsocks-deepin](https://github.com/lolimay/shadowsocks-deepin) 修改。

shadowsocks-deepin作者似乎停止更新了。出于对QT热爱和研究shadowsocks的目的，我就在v1.2.1代码源码基础上继续开发。因为做了大量的重构，就独立出来，工程叫shadowshuttle-deepin。



# 开发思路

参考shadowsocks-windows的设计与实现。

deepin的开发文档匮乏，尽量去掉不必要的dtk依赖。



# 构建

````
sudo apt update
sudo apt install qt5-default qttools5-dev-tools qt5-qmake g++ qtcreator qttools5-dev -y
sudo apt install libdframeworkdbus-dev libdtkwidget-dev libdtkcore-dev -y
sudo apt install libqtshadowsocks-dev libqrencode-dev libzbar-dev libquazip-qt5-dev -y
cd shadowshuttle-deepin
mkdir build && cd build
cmake ..
make -j4
cpack
````
