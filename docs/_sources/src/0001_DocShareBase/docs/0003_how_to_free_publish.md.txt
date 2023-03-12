# how to free publish

如何在公网也可以访问自己写的文档

# 搭建在公网可以访问的网页

* 在github上新建一个仓库，以github.io结尾，例如我的git仓库
  ```
  https://github.com/linlsOS/linls.summary.github.io.git
  ```
* 在创建好git 仓库，上传好文档后，进行github pages中进行配置
  * 在选择好Branch后，过个一分钟左右刷新下页面，page中会显示“Your site is live at”，就可以直接通过网页访问文档
  ![0003_github_cfg.png](images/0003_github_cfg.png)
* 如果觉得github上 路径比较长，为了方便可以买个阿里的域名便宜的10年180左右，然后将域名映射到github，域名的配置如下可以作为参考
  ![0003_cloud_server_cfg.png](images/0003_cloud_server_cfg.png)
* 配置好域名后，在git hub的page页面配置好DNS，输入购买的域名地址，就完成了域名的映射
