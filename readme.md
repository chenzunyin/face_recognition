# 本文档用于记录人脸特征提取SDK结构和使用说明

.
├── database
├── func
├── images
├── include
├── lib
└── output

	- database目录 用于存放人脸注册数据库, 默认名称czy_face.db
	- func目录 存放不同功能实现的代码
	- images目录 用于保存测试图片
	- include目录 存放头文件
	- lib目录 存放人脸特征动态算法库
	- output目录 存放编译后生成的可执行文件
	- Makefile
	- reade.md

	
ps: 注意将动态链接库添加到用户环境变量中```export LD_LIBRARY_PATH=/home/tiny/Work/ubuntu-feature-sdk/lib:$LD_LIBRARY_PATH```

2. Makefile文件介绍

执行make命令, 默认编译所有文件, 如需单独编译, 后面跟随编译选择项

	- single: 输入单张图片, 注册特征到人脸特征库
	- compare: 输入两张图片, 提取两张图片特征后比对特征相似度后输出
	- identify: 输入一张图片, 在当前人脸特征库中进行查找, 输出查找结果
	- read: 读取一个文件夹下所有文件然后输出文件名称, 递归查找其中子文件夹下文件
	- cycle: 遍历注册一个文件下所有jpg图片到人脸特征库中　
	
ps: SDK有opencv依赖, 当前测试使用的opencv版本为2.4.9.1, 使用如下命令查询opencv版本

	```pkg-config --modversion opencv```
