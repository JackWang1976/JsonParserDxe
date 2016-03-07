@echo off

REM This batch file is the same for every Dell app. There is no need to edit it
REM if you copy it to a new application's source directory, since it figures out
REM what directory it is in (that is the leaf directory name, not the entire
REM directory path).

setlocal
set HELP=0
if "%1" == "/h" set HELP=1
if "%1" == "/H" set HELP=1
if "%1" == "-h" set HELP=1
if "%1" == "-H" set HELP=1
if "%1" == "/?" set HELP=1
if "%1" == "-?" set HELP=1
if %HELP% == 0 goto DoIt

echo.
echo USAGE: BuildMe [options]
echo.
echo where options are:
echo.
echo   -DoNotRemapU   Don't remap the U: drive.
echo   -32            Build an IA32 app (instead of X64).
echo.
goto Exit

:DoIt
set GWK=..\..\..\..\BuildTools\gawk\gawk

REM ----------------------------------------------------------------------------
REM Create a batch file using GAWK in order to set an environment variable,
REM LEAF, to the name of the current directory's leaf directory. That is,
REM suppose the current directory is:
REM
REM  C:\BiosCode\13g\120519\DellPkgs\DellApplicationsPkg\Application\AwesomeApp
REM
REM then the command below would create a batch file containing the single line:
REM
REM                          @set LEAF=AwesomeApp
REM
REM It looks somewhat bad because the '\' and '"' chars in the script have to be
REM escaped for the shell to pass them in correctly.
REM
REM The script simply splits the current drive:path at each '\' and assigns the
REM parts to an array named d. The variable n is set to the number of elements
REM in the array, so d[n] is the name of the leaf directory.
REM ----------------------------------------------------------------------------
cd | %GWK% "{n=split($0,d,\"\\\\\"); print(\"@set LEAF=\" d[n])}" > Set$Dir$.bat

REM Now call the batch file to set DIR's value.
call Set$Dir$

echo call ..\BuildAnApp %LEAF% %*
call ..\BuildAnApp %LEAF% %*

REM Delete the batch file.
del Set$Dir$.bat >nul 2>nul

endlocal

:Exit
