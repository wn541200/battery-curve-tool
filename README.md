# battery-curve-tool

说明：
  用于绘制手机充电和放电曲线，充电时主要观察电池电压，温度，充电电流。放电时主要用于观察电池电量误差，因此需要开启飞行模式，关闭自动背光，apk设置屏幕常亮使手机接近恒流放电，
  开始放电和结束这两个点会拉出一条直线用于对比电量误差。
  


1. 手机上安装power-record.apk，开启飞行模式，关闭自动背光，将电池充满电后进行放电记录放电曲线，或者将电池放空后进行充电记录充电曲线。
2. 数据记录在手机目录 /sdcard/powerrecord.txt
3. PowerFileParse为QT源码编译出来的工具用于解析/sdcard/powerrecord.txt绘制曲线
4. PowerRecord为apk的源文件

![Image text](https://github.com/wn541200/battery-curve-tool/blob/main/demo-img/%E5%85%85%E7%94%B5%E6%9B%B2%E7%BA%BF.PNG)
![Image text](https://github.com/wn541200/battery-curve-tool/blob/main/demo-img/%E6%94%BE%E7%94%B5%E7%99%BE%E5%88%86%E6%AF%94.PNG)
