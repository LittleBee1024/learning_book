# 决策树

> [《统计学习方法》 - 李航 - 第二版](https://1drv.ms/b/s!AkcJSyT7tq80f24rxQaaH3HMUWE?e=5vJQNK) 第五章，以及[《Python数据手册》 - Jake VanderPlas](https://1drv.ms/b/s!AkcJSyT7tq80gQBIJPqCGBXnxliQ?e=oCjr4e) 第五章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ml/decision_tree/code)中找到

## 理论

决策树(decision tree)可以认为是给定特征条件下类的条件概率分布。所有给定的特征条件将特征空间划分为互不相交的单元。决策树的一条路径对应于划分种的一个单元。各叶节点(单元)上的条件概率往往偏向某一个类，决策树分类时将该单元的实例强行分到条件概率大的那一类去。

决策树的主要优点是模型具有可读性，分类速度快。通常包括3个步骤：

* 特征选择
* 决策树的生成
* 决策树的修剪

常见的决策树算法有：

* ID3算法
* C4.5算法
* CART算法

三种算法的特点如下：

|算法|支持模型|树结构|特征选择|连续值处理|缺失值处理|剪枝|
|---|---|---|---|---|---|---|
|ID3 |分类|多叉树|信息增益|不支持|不支持|不支持|
|C4.5|分类|多叉树|信息增益比|支持|支持|支持|
|CART|分类，回归|二叉树|基尼系数,均方差|支持|支持|支持|

*表格来自[文章](https://www.cnblogs.com/pinard/p/6053344.html)*

### 特征选择

#### 信息增益

#### 信息增益比

#### 基尼指数

## 实验