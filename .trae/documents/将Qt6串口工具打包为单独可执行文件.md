# 将Qt6串口工具打包为单独可执行文件

## 1. 所需工具

### 1.1 Qt官方工具
- **windeployqt**：随Qt一起安装，用于收集应用程序依赖
  - 通常位于Qt安装目录下的`bin`文件夹中，例如：`C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe`

### 1.2 第三方打包工具（二选一）
- **Enigma Virtual Box**：免费工具，可将应用程序和所有依赖打包为单个可执行文件
  - 下载地址：https://enigmaprotector.com/en/downloads.html
- **Inno Setup**：免费开源的安装包制作工具，可创建包含所有依赖的安装程序
  - 下载地址：https://jrsoftware.org/isdl.php

## 2. 打包步骤

### 2.1 步骤一：使用windeployqt收集依赖
1. 找到windeployqt.exe的位置
2. 打开命令行工具，进入构建输出目录（release文件夹）
3. 运行命令：`windeployqt serialtool.exe`
4. 等待工具收集所有依赖文件

### 2.2 步骤二：使用Enigma Virtual Box打包为单个可执行文件
1. 下载并安装Enigma Virtual Box
2. 打开Enigma Virtual Box
3. 在"Input File Name"中选择你的`serialtool.exe`
4. 在"Output File Name"中设置输出文件名，例如：`serialtool_single.exe`
5. 点击"Add"按钮，选择release文件夹中的所有文件
6. 点击"Process"按钮开始打包
7. 等待打包完成，即可获得单个可执行文件

### 2.3 步骤二（可选）：使用Inno Setup创建安装程序
1. 下载并安装Inno Setup
2. 使用Inno Setup创建新的安装脚本
3. 在脚本中包含所有依赖文件
4. 编译脚本生成安装程序

## 3. 注意事项

1. 确保使用与构建应用程序相同的Qt版本的windeployqt
2. 如果系统中找不到windeployqt，可以在Qt Creator的"工具"菜单中找到"外部"->"Qt 6"->"windeployqt"选项
3. 打包完成后，测试生成的单个可执行文件是否能在没有Qt环境的电脑上正常运行
4. 对于Qt6应用，可能需要包含额外的依赖文件，如QtCore5Compat.dll等

## 4. 预期结果

通过上述步骤，你将获得一个可以在没有Qt环境的Windows电脑上独立运行的Qt6串口工具可执行文件。