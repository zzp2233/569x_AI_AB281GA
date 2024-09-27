@echo off
mkdir app
cd app
mkdir projects
cd projects
mkdir AB5681G
cd AB5681G
mkdir Output
cd Output
mkdir bin
cd ..\..\..\..\
xcopy /s/y ..\..\..\app\projects\AB5681G\Output\bin\res\ app\projects\AB5681G\Output\bin\res\
xcopy /s/y ..\..\..\app\projects\AB5681G\Output\bin\Settings\ app\projects\AB5681G\Output\bin\Settings\
xcopy /s/y ..\..\..\app\projects\AB5681G\Output\bin\ui\ app\projects\AB5681G\Output\bin\ui\

timeout /nobreak /t 3
::pause