# git commit

常见git 操作

# git 常规操作及遇到的问题

* git init
* git add .
* git commit -m ""
* git 同远程仓库关联
```
git remote add origin https://github.com/xxx/xxxx.github.io.git
```
*  git push origin master
* git push提示“Connection was reset, errno 10054”
```
$ git push origin master
fatal: unable to access 'https://github.com/xxxx/xxxx.githu.io.git/': OpenSSL SSL_read: Connection was reset, errno 10054
fatal: unable to access 'https://github.com/xxxx/xxxx.githu.io.git/': OpenSSL SSL_read: Connection was reset, errno 10054
Administrator@MS-UBGOLQHHLPCZ MINGW64 /g/github/test (master)
$ git config --global http.sslVerify false
```

