
https://mp.weixin.qq.com/s?__biz=MjM5MTIwMjY1Mg==&mid=2650004584&idx=7&sn=9b66df30ed7009a42d1afb510d7200d0&chksm=bebe505b89c9d94d29ffad20cabb019409102f6f0f6d5514032b41fc3953a95189a0bc1a1f93&scene=27

CS: [片选信号] 拉低选中
WR：[写信号] 向LCD写入数据控制线，上升沿有效，写数据时　RD拉高
RD：[读信号] 从LCD读数据控制线，上升沿有效，读数据时，WR拉高
RESET：[硬复位信号] 拉低，等待100ms，拉高
DC/RS：[命令/数据标志]（0：命令 1：数据）
