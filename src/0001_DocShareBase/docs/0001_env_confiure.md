# 文档总结框架环境配置

介绍如何进行环境配置

# steps

* 工具
   * visual studio code

以下以windows 为例，linux下比较简单，根据requrirements进行工具安装。  
如果在VS code右键无显示MDPlant，请确认是否安装MDPlant插件，如果已经安装请更新VS code  
之前遇到过由于VS code太老，插件兼容问题，所以无法使用MDPlant。确认VScode版本在1.79.2及以上


# 安装依赖
   * 打开 VS code，在Extensions中分别搜索vim、MDPlant、Markdown PDF,进行安装  
     ![0001_env_configure.png](images/0001_env_configure.png)
   * 配置shell，选择powershell
     ![0001_env_configure_sh.png](images/0001_env_configure_sh.png)
   * 安装python
     * window在开始搜索microsoft Store,打开microsoft store 搜索Python，点击获取会自动安装
       ![0001_env_cfg_install_python.png](images/0001_env_cfg_install_python.png)
   * 安装依赖工具
     * 在VS code中打开新建文件夹，拥有项目存储；
     * 右键，选择“MDPlant Creat Template”，新建工程，输入工程名；
     * 在工程根目录下会有requrements.txt，其中包含需要依赖的工具
   * 配置sphinx-build路径为环境变量
     * 找到sphinx-build文件的路径，在环境变量中增加其路径
       ![0001_env_cfg_sphinx-build.png](images/0001_env_cfg_sphinx-build.png)
   * 安装sphinx-rtd-theme
     * 点击Terminal->new Terminal
     * pip3 install sphinx-rtd-theme==1.0.0
   
   * 到这依赖已经完成，可以新建文档，然后在terminal窗口中输入指令
     * make html    //将文档转为html,每次有更新都需要执行生成新的html
     * make server //执行后，可以反问http://127.0.0.1:8080/ 查看效果
     * make docs   //将页面文件拷贝到指定路径，github中页面访问对文件路径有要求
