pymilkcat
=========

pymilkcat是中文自然语言处理工具MilkCat的Python接口，支持Python2和Python3

安装
----

安装pymilkcat前需要安装MilkCat，安装方法参阅 [MilkCat](https://github.com/milkcat/MilkCat)

获取pymilkcat

```sh
git clone git@github.com:milkcat/milkcat-python.git
cd milkcat-python
```

Python2安装

```sh
python setup.py build
sudo python setup.py install
```

Python3安装

```sh
python3 setup.py build
sudo python3 setup.py install
```

运行
----

```python
>>> import pymilkcat
>>> pymilkcat.seg('这个是一个简单的例子')
['这个', '是', '一个', '简单', '的', '例子']
```
