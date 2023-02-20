# 线性回归

> [《Hands-on Machine Learning with Scikit-Learn, Keras & TensorFlow》 - 第二版](https://1drv.ms/b/s!AkcJSyT7tq80gQMC23h9BYE9cIKW?e=4IPacy) 第四章，[《Python数据手册》 - Jake VanderPlas](https://1drv.ms/b/s!AkcJSyT7tq80gQBIJPqCGBXnxliQ?e=oCjr4e) 第五章，[《机器学习》 - 周志华](https://1drv.ms/b/s!AkcJSyT7tq80gQHa_bUybXe9tjCV?e=shgZXm)第三章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ml/linear_reg/code)中找到

## 概念

线性模型试图学得一个通过属性的线性组合来进行预测的函数，即：

$$\hat{y} = \theta_0 + \theta_1x_1 + \theta_2x_2+...+\theta_nx_n$$

一般用向量形式写成：

$$\hat{y} = \theta^Tx$$

其中，

* $\hat{y}$ 表示预测结果
* $n$ 表示特征的个数
* $x_i$ 表示第 $i$ 个特征的值
* $\theta_j$ 表示第 $j$ 个参数(包括偏置项 $\theta_0$ 和特征权重值 $\theta_1, \theta_2, ...\theta_n$ )

### 正规方程

我们可以采用公式解，通过正规方程直接得到 $\theta$ 的值。正规方程如下：

$$\hat{\theta} = (X^T \cdot X)^{-1} \cdot X^T \cdot y$$

其中，

* $\hat{\theta}$ 指最小化损失 $\theta$ 的值
* $y$ 是一个向量，其包含了 $y^{(1)}$ 到 $y^{(m)}$ 的值
