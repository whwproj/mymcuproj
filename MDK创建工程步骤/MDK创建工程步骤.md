##### 1. 创建文件目录如下结构：

> * Bsp 【common.h 开发的程序】
>
> * Core 
>
>   > * Inc 【main.h py32f0xx_it.h】
>   >
>   > * Src 【main.c py32f0xx_it.c】
>
> * MDK-ARM
>
> * Doc

<img src="img\image-20240109171212022.png" alt="image-20240109171212022" style="zoom:50%;" />



##### 2. 打开MDK选择创建新工程

![image-20240109165825854](img\image-20240109165825854.png)



##### 3.  选择MDK-ARM目录保存工程

<img src="img\image-20240109171533909.png" alt="image-20240109171533909" style="zoom:80%;" />

##### 4. 选择对应芯片包

<img src="img\image-20240109171621583.png" alt="image-20240109171621583" style="zoom:67%;" />



##### 4. 运行环境配置

<img src="img\image-20240109171847315.png" alt="image-20240109171847315" style="zoom: 67%;" />



##### 5. 编辑分类工程目录，并添加相关c文件到工程中

<img src="img\image-20240109172223802.png" alt="image-20240109172223802" style="zoom:67%;" />



##### 6. 配置魔术棒选项卡

<img src="img\image-20240109172838989.png" alt="image-20240109172838989" style="zoom:67%;" />

Target里勾选Use MicroLIB

<img src="img\image-20240109172913844.png" alt="image-20240109172913844" style="zoom:67%;" />



C/C++里添加.h文件路径

![image-20240109173420870](img\image-20240109173420870.png)

添加之后如下：

<img src="img\image-20240109185943611.png" alt="image-20240109185943611" style="zoom: 80%;" />

配置全局宏定义并勾选C99 Mode

（宏定义从 main.h -> #include "py32f0xx.h" 文件条件编译里找到)

```
USE_HAL_DRIVER,PY32F002Ax5
```

<img src="img\image-20240109174402280.png" alt="image-20240109174402280" style="zoom: 67%;" />

配置完成后电机OK

##### 7. 点击编译，编辑成功，配置完成

<img src="img\image-20240109174539404.png" alt="image-20240109174539404" style="zoom: 67%;" />



<img src="img\image-20240109174613025.png" alt="image-20240109174613025" style="zoom:67%;" />