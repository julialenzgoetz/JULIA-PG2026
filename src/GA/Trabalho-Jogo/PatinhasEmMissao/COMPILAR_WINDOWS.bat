@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul
if errorlevel 1 (
    echo CMake nao encontrado. Veja a secao de compilacao do README.md.
    pause
    exit /b 1
)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto erro
cmake --build build --config Release --parallel 4
if errorlevel 1 goto erro
echo.
echo Compilacao concluida! Iniciando a versao que voce acabou de compilar.
if exist "build\Release\PatinhasEmMissao.exe" (
    "build\Release\PatinhasEmMissao.exe" --assets "%CD%"
) else (
    "build\PatinhasEmMissao.exe" --assets "%CD%"
)
if errorlevel 1 goto erro
exit /b 0
:erro
echo.
echo A compilacao ou a execucao falhou. Veja a mensagem acima.
echo Visual Studio precisa do componente Desenvolvimento para desktop com C++.
echo Para MinGW, veja o comando alternativo no README.md.
pause
exit /b 1
