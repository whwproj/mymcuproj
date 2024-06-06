tar -zcf mcugit.tar.gz .git
del /f /q C:\Users\whw\Desktop\BaiduSyncdisk\whw项目文件网盘备份\其他资料\FILE\file\*
move "%~dp0\mcugit.tar.gz" "C:\Users\whw\Desktop\BaiduSyncdisk\whw项目文件网盘备份\其他资料\FILE\file\mcugit.tar.gz"
cd C:\Users\whw\Desktop\BaiduSyncdisk\whw项目文件网盘备份\其他资料\FILE\
start create.exe