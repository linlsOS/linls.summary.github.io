# ubuntu uninstall sw

ubuntu 卸载软件

# 说明

  * dpkg --list |grep sw
  * sudo apt-get --purge remove 
    ```sh
    muxi@ubuntu:/var/lib/tomcat9/webapps$ dpkg --list |grep tomcat9
    ii  libtomcat9-java                            9.0.31-1ubuntu0.4                   all          Apache Tomcat 9 - Servlet and JSP engine -- core libraries
    ii  tomcat9                                    9.0.31-1ubuntu0.4                   all          Apache Tomcat 9 - Servlet and JSP engine
    ii  tomcat9-common                             9.0.31-1ubuntu0.4                   all          Apache Tomcat 9 - Servlet and JSP engine -- common files
    muxi@ubuntu:/var/lib/tomcat9/webapps$ sudo apt-get --purge remove tomcat9
    
    ```

