# harmonyos简介

harmonyos 简介

# 参考文档

* [OpenHarmony开源项目](https://gitee.com/openharmony)
* [OpenHarmony-v4.0-release.md](https://gitee.com/openharmony/docs/blob/OpenHarmony-4.0-Release/zh-cn/release-notes/OpenHarmony-v4.0-release.md#%E6%BA%90%E7%A0%81%E8%8E%B7%E5%8F%96)


# 代码下载

```
git config --global user.name "yourname"
git config --global user.email "your-email-address"
git config --global credential.helper store

curl -s https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 > /usr/local/bin/repo 
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple requests

sudo apt-get install git-lfs

repo init -u git@gitee.com:openharmony/manifest.git -b OpenHarmony-4.0-Release --no-repo-verify
repo sync -c
repo forall -c 'git lfs pull'
```