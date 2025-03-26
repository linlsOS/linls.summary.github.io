# androidkey生成

android 签名key生成

# Author Info

 Field  | Info
--------|----------
 Author | linliangsong 
 Date   | 2025-3-25
 Email  | linliangsong@boe.com.cn 

# 参考文档

* [Android使用keytool-importkeypair生成系统签名](https://blog.csdn.net/qiaoyl113/article/details/118900246)
* [android系统签名及修改](https://blog.csdn.net/zyfzhangyafei/article/details/123076140)
* [0008_keytool-importkeypair.tar](refers/0008_keytool-importkeypair.tar)


* 生成androidstudio 可以使用的系统签名文件
```
1. 依赖系统签名文件，文件目录  build/make/target/product/security
2. ./keytool-importkeypair -k ./platform.keystore -p android -pk8 3588/platform.pk8 -cert 3588/platform.x509.pem -alias platform
```
