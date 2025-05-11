@echo off
rem 清空或创建a.txt
type nul > a.txt

rem 递归遍历所有子目录中的.cpp和.h文件，并合并到a.txt
for /r %%f in (*.cpp *.h) do (
    type "%%f" >> a.txt
)

echo 文件合并完成！输出至 a.txt