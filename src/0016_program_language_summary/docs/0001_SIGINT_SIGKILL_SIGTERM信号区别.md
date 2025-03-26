# SIGINT SIGKILL SIGTERM信号区别

SIGINT,SIGKILL,SIGTERM信号区别,各类信号总结

# 参考文档

* [SIGINT,SIGKILL,SIGTERM信号区别,各类信号总结](https://blog.csdn.net/WJSZMD/article/details/89331751)


* SIGINT
  产生方式：键盘Ctrl+C
  产生结果: 只对当前前台进程,和他的所在的进程组的每个进程都发送SIGINT信号,之后这些进程会执行信号处理程序再终止

* SIGTERM
  产生方式: 和任何控制字符无关,用kill函数发送
  本质: 相当于shell> kill不加-9时 pid
  产生结果: 当前进程会收到信号,而其子进程不会收到.如果当前进程被kill(即收到SIGTERM),则其子进程的父进程将为init,即pid为1的进程.
  与SIGKILL的不同: SIGTERM可以被阻塞,忽略,捕获,也就是说可以进行信号处理程序,那么这样就可以让进程很好的终止,允许清理和关闭文件.

* SIGKILL
  产生方式: 和任何控制字符无关,用kill函数发送
  本质: 相当于shell> kill -9 pid.
  产生结果: 当前进程收到该信号,注意该信号时无法被捕获的,也就是说进程无法执行信号处理程序,会直接发送默认行为,也就是直接退出.  
  这也就是为何kill -9 pid一定能杀死程序的原因. 故这也造成了进程被结束前无法清理或者关闭资源等行为,这样时不好的.


