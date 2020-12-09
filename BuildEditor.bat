
@echo off

echo Building Editor..

set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\msbuild.exe"

call %MSBUILD% "Game\build\vc\editor\EditorLauncher.sln" /m /p:Configuration=Release /v:normal

