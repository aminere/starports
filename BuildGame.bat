
@echo off

set MSBUILD="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\msbuild.exe"

echo Building Game..
call %MSBUILD% "Game\build\vc\game\GameLauncher.sln" /m /p:Configuration=Release /v:normal

echo Building FileSystemGenerator..
call %MSBUILD% "Shoot\tools\FileSystemGenerator\vc\FileSystemGenerator.sln" /m /p:Configuration=Release /v:normal

echo Building PC Assets..
pushd Game
call FileSystemGenerator.bat
popd