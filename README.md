pymilkcat
=========

pymilkcat是中文自然语言处理工具MilkCat的Python接口，支持Python2和Python3

安装
----

安装pymilkcat前需要安装MilkCat，安装方法参阅 [MilkCat](https://github.com/milkcat/MilkCat)

获取pymilkcat

```sh
git clone git@github.com:milkcat/pymilkcat.git
cd pymilkcat
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

Python3例子

分词\&词性标注

```python
>>> import pymilkcat
>>> parser = pymilkcat.Parser()
>>> parser.Break('我的猫喜欢喝牛奶。喵～')
['我', '的', '猫', '喜欢', '喝', '牛奶', '。', '喵', '～']
>>> for item in parser.Predict('我的猫喜欢喝牛奶。喵～'):
...     print('{}/{}'.format(item.word, item.part_of_speech_tag))
... 
我/PN
的/DEG
猫/NN
喜欢/VV
喝/VV
牛奶/NN
。/PU
喵/VV
～/PU
>>> 
```

依存分析

```python
>>> import pymilkcat 
>>> options = pymilkcat.ParserOptions()
>>> options.UseBeamYamadaParser()
>>> parser = pymilkcat.Parser(options)
>>> for item in parser.Predict('我的猫喜欢喝牛奶。'):
...     print('{}\t{}\t{}\t{}'.format(
...         item.word,
...         item.part_of_speech_tag,
...         item.head,
...         item.dependency_label))
... 
我 PN  3 NMOD
的 DEG 1 DEG
猫 NN  4 SBJ
喜欢  VV  0 ROOT
喝 VV  4 OBJ
牛奶  NN  5 OBJ
。 PU  4 VMOD
>>> 
```

API
---

pymilkcat使用与MilkCat/C++一致的API, 详情请参照pymilkcat.py以及[MilkCat/C++ API](https://github.com/milkcat/MilkCat)
