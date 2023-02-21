# 线性回归

> [《Hands-on Machine Learning with Scikit-Learn, Keras & TensorFlow》 - 第二版](https://1drv.ms/b/s!AkcJSyT7tq80gQMC23h9BYE9cIKW?e=4IPacy) 第四章，[《Python数据手册》 - Jake VanderPlas](https://1drv.ms/b/s!AkcJSyT7tq80gQBIJPqCGBXnxliQ?e=oCjr4e) 第五章，[《机器学习》 - 周志华](https://1drv.ms/b/s!AkcJSyT7tq80gQHa_bUybXe9tjCV?e=shgZXm)第三章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ml/linear_reg/code)中找到

## 数学模型

线性模型试图学得一个通过属性的线性组合来进行预测的函数，即：

$$\hat{y} = \theta_0 + \theta_1x_1 + \theta_2x_2+...+\theta_nx_n$$

一般用向量形式写成：

$$\hat{y} = \theta^Tx$$

其中，

* $\hat{y}$ 表示预测结果
* $n$ 表示特征的个数
* $x_i$ 表示第 $i$ 个特征的值
* $\theta_j$ 表示第 $j$ 个参数(包括偏置项 $\theta_0$ 和特征权重值 $\theta_1, \theta_2, ...\theta_n$ )

## 正规方程求解

我们可以采用公式解，通过正规方程直接得到 $\theta$ 的值。正规方程如下：

$$\hat{\theta} = (X^T \cdot X)^{-1} \cdot X^T \cdot y$$

其中，

* $\hat{\theta}$ 指最小化损失 $\theta$ 的值
* $y$ 是一个向量，其包含了 $y^{(1)}$ 到 $y^{(m)}$ 的值

[例子“normal_equation.ipynb”](https://github.com/LittleBee1024/learning_book/blob/main/docs/booknotes/ml/linear_reg/code/normal_equation.ipynb)展示了如何利用方程直接求解线性回归方程，并和`sklearn`提供的`LinearRegression`模型进行了对比，两者结果相近。

## 梯度下降求解

当特征个数较大时，正规方程求解较慢。梯度下降是一种非常通用的优化算法。其整体思路是通过迭代来逐渐调整参数使得损失函数达到最小值。

### 批量梯度下降

线性回归模型的损失函数采用均方误差MSE(mean square error)：

$$MSE(\theta) = \frac{1}{m} \sum_{i=1}^m(\theta^T \cdot x^{(i)} - y^{(i)})^2$$

梯度下降需要求损失函数的偏导数：

$$\frac{\partial}{\partial \theta_j}MSE(\theta) = \frac{2}{m}  \sum_{i=1}^m (\theta^T \cdot x^{(i)} - y^{(i)})x_j^{(i)}$$

对所有 $\theta$ 都计算偏导数，可以用矩阵运算，得到梯度向量：

$$\bigtriangledown_{\theta} MSE(\theta) = \begin{pmatrix}
\frac{\partial}{\partial \theta_0}MSE(\theta)\\ 
\frac{\partial}{\partial \theta_1}MSE(\theta)\\ 
...\\ 
\frac{\partial}{\partial \theta_n}MSE(\theta)\\ 
\end{pmatrix} = \frac{2}{m}X^T \cdot (X \cdot \theta - y)$$

梯度向量是 $\theta$ 每次变化的步长，通过学习率 $\eta$ 控制 $\theta$ 的变化速度，得到 $\theta$ 的迭代公式：

$$ \theta^{(next step)} = \theta - \eta \bigtriangledown_{\theta} MSE(\theta)$$

梯度向量的计算包含了整个训练集 $X$，因此这个算法称为**批量梯度下降**。

[例子“batch_gradient_descent.ipynb”](https://github.com/LittleBee1024/learning_book/blob/main/docs/booknotes/ml/linear_reg/code/batch_gradient_descent.ipynb)分别选择了“0.02, 0.1, 0.5”三种学习率，观察每次迭代的变化情况。

![](./images/batch_gd.png)

* 左侧图的学习率太小了，花了大量时间也没能求出最后的结果
* 中间图的学习率最合适，迭代几次后就收敛了
* 右侧图的学习率太大了，算法是发散的

