


# 视频编码中熵编码之基于上下文的变长编码（Huffman霍夫曼编码和指数哥伦布）


@[TOC](视频编码中熵编码之基于上下文的变长编码Huffman霍夫曼编码和指数哥伦布）)

</font>

<hr style=" border:solid; width:100px; height:1px;" color=#000000 size=1">



![视频编码流程图](https://i-blog.csdnimg.cn/direct/a6d61f10a02d4076b4a372734585de3a.jpeg#pic_center)


 1、霍夫曼（Huffman）算法简介

  2、霍夫曼（Huffman）编码步骤

 3、霍夫曼（Huffman）编码特点
 
 4、霍夫曼（Huffman）编码举例

   [①、视频编码中熵编码之熵及熵编码基本原理](https://chensongpoixs.github.io/cvideo_codec/)

  [②、视频编码中熵编码之"Z"字扫描原理(变换DCT系数和小波变换系统分布特性和统计特性)](https://chensongpoixs.github.io/cvideo_codec/)

  [③、视频编码中熵编码之基于上下文的变长编码（Buffman霍夫曼编码和指数哥伦布）](https://chensongpoixs.github.io/cvideo_codec/)

  [④、视频编码中熵编码之基于上下文的算术编码](https://chensongpoixs.github.io/cvideo_codec/)

  [⑤、视频编码中熵编码之位平面编码](https://chensongpoixs.github.io/cvideo_codec/)

  [⑥、视频编码中熵编码之零数编码](https://chensongpoixs.github.io/cvideo_codec/)

  [⑦、视频编码中熵编码之游程编码](https://chensongpoixs.github.io/cvideo_codec/)

 5、 环路滤波


# 前言
 
 视频编码中熵编码的目标是去除信源符号在信息表达上的表示冗余，也称为信息熵冗余或者编码冗余。 熵编码技术是视频编码系统中的基础性关键技术之一。
 

## 一、霍夫曼（Huffman）算法简介

- Huffman编码是1952年为压缩文本文件所设计的编码方法，也是目前消除视频信息冗余最常使用的方法之一
- 对出现<font color='red'>概率最大</font>的符号赋以<font color='red'>最短的码</font>字， 概率越小表示的码字越长， 从而表示每个符号的<font color='red'>平均比特数最小 </font>
- 

## 二、霍夫曼（Huffman）编码步骤（分为两类 赋值与反推）

1. 把信源符号按照概率大小顺序排列（概率排序）
2. 在分配码字长度时，首先将两个出现概率最小的两个符号的概率相加，合成一个概率；（合并）
3. 把这个合成概率看作是一个新组合符号的概率， 重复上述做法，直到最后只城下两个符号的概率为之（置换）
4. 完成上述步骤后，在返回向前进行编码，每一层有两个分支，分别赋予0和1（大的赋0或者小的赋1 但是必须一致）


## 三、霍夫曼（Huffman）编码特点


### 1、霍夫曼编码是瞬时唯一的可解块编码

- ①  瞬时：符号串中每个码字无需参考后继符号就可解码
- ② 唯一可解码：任何符号串只能以一种方式解码
- ③ 块编码：每个信源符号都映射到一个编码符号的固定序列中

### 2、霍夫曼编码是唯一可译码。短的码不会成为更长码的起始部分

### 3、霍夫曼编码的平均码长接近于熵

### <font color='red'>4、与计算机的数据结构不匹配</font>

### 5、需要多次排序， 耗费时间


<font color='red'>注意： 1、霍夫曼编码的算法是确定的， 但是编出的码并非是唯一的
                                    2、由于霍夫曼编码的依据是信源符号的概率分布， 故其编码效率取决于信源的统计特性

</font>

原因是： 在编码步骤过程中赋值 是有区别的（例如： 大赋0， 小赋1）



## 四、Huffman编码特点

 
 1. 霍夫曼编码的局限性在于，该编码方法只适用于离散信源，即信源符号个数为有限数
 2. 编码时需要知道输入符号集的概率分布
 3. 在进行Huffman编码压缩时，计算量大而复杂，尤其是译码复杂度较高
 4. 由于码长不等，还存在一个输入与输出的速率匹配问题
 


## 五、Huffman编码举例

### 1、简单编码流程

 ![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/be941e64d9da4fe0ac7d4775442f455f.jpeg)


![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/13d8ea0498414ebdb343d8a2a61518ae.jpeg)

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/ed7f16f8db56452aba8a95ec21e2f44b.jpeg)

### 2、Huffman编码（树结构分析法）

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/b95039bf6a554f6c99e7624ef6b324a3.jpeg)




![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a15ba1957d8049ecaa5d4b41b12a39f0.jpeg)




# 总结


[数字视频编码技术原理地址：https://chensongpoixs.github.io/cvideo_codec/](https://chensongpoixs.github.io/cvideo_codec/)
