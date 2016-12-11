
echo Building Game..
call "C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe" "Game\build\vc\game\GameLauncher.sln" /m /p:Configuration=Release /v:normal

echo Building FileSystemGenerator..
call "C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe" "Shoot\tools\FileSystemGenerator\vc\FileSystemGenerator.sln" /m /p:Configuration=Release /v:normal

echo Building PC Assets..
pushd Game
call FileSystemGenerator.bat
popd