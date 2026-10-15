@echo off
setlocal
cd /d "%~dp0"
if exist "build\Release\PatinhasEmMissao.exe" (
    "build\Release\PatinhasEmMissao.exe" --assets "%CD%"
    if errorlevel 1 goto erro
    exit /b 0
)
if exist "build\PatinhasEmMissao.exe" (
    "build\PatinhasEmMissao.exe" --assets "%CD%"
    if errorlevel 1 goto erro
    exit /b 0
)
if exist "bin\Windows\PatinhasEmMissao.exe" (
    "bin\Windows\PatinhasEmMissao.exe" --assets "%CD%"
    if errorlevel 1 goto erro
    exit /b 0
)
echo Executavel nao encontrado. Execute COMPILAR_WINDOWS.bat primeiro.
:erro
echo.
echo Nao foi possivel iniciar. Leia a mensagem acima e o README.md.
pause
exit /b 1
