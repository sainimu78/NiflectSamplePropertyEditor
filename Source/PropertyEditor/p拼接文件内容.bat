@echo off
rem ��ջ򴴽�a.txt
type nul > a.txt

rem �ݹ����������Ŀ¼�е�.cpp��.h�ļ������ϲ���a.txt
for /r %%f in (*.cpp *.h) do (
    type "%%f" >> a.txt
)

echo �ļ��ϲ���ɣ������ a.txt