cd ../../

@if {%1}=={cs} goto CS

@REM ADS Compiler
:ADS 
@REM Get current directory
for %%x in (%0) do set BatchPath=%%~dpsx
for %%x in (%BatchPath%) do set BatchPath=%%~dpsx
IF "%BatchPath:~-1%"=="\" SET BatchPath=%BatchPath:~0,-1%

@REM Change ROOT to your source dir
@set PhyRoot=%BatchPath%
@REM set path=%SystemRoot%\system32;X:\tools\make_tools;x:\make\MakeResources\perl\bin;c:\perl2exe;
set path=%SystemRoot%\system32;X:\tools\make_tools;c:\perl\bin;c:\perl2exe;

subst x: /D
@subst y: /D
@set ROOT=x:
@REM set MRVDRV=y:
@subst %ROOT% %PhyRoot%
@REM subst %MRVDRV% %ROOT%\driver\marvell
@REM set init=$(ROOT)\inc
SET PATH=C:\Program Files\CSL Arm Toolchain\bin;X:;%PATH%
SET PATH=C:\Program Files\ARM\ADSv1_2\Bin;X:;%PATH%

@REM For PVCS !!
@SET PATH=C:\PVCS\cb\nt;%PATH%

@REM for ADS !!
SET PATH=C:\Program Files\ARM\ADSv1_2\Bin;X:;%PATH%
@cd ./tools/make_tools
@cls

%ROOT%
@copy  x:\tools\make_tools\Makefile_axd.utl x:\tools\make_tools\Makefile.utl
@echo ===ADS Compiler environment Initial OK===
@goto END

@REM CodeSourcery Compiler
:CS

@REM Get current directory
for %%x in (%0) do set BatchPath=%%~dpsx
for %%x in (%BatchPath%) do set BatchPath=%%~dpsx
IF "%BatchPath:~-1%"=="\" SET BatchPath=%BatchPath:~0,-1%


@REM Change ROOT to your source dir
@set PhyRoot=%BatchPath%

set path=%SystemRoot%\system32;X:\tools\make_tools;c:\perl\bin;c:\perl2exe;

subst x: /D
@subst y: /D
@set ROOT=x:
@REM set MRVDRV=y:
@subst %ROOT% %PhyRoot%
@REM subst %MRVDRV% %ROOT%\driver\marvell
@REM set init=$(ROOT)\inc
SET PATH=C:\Program Files\CodeSourcery\Sourcery_CodeBench_for_ARM_EABI\bin;%PATH%;X:\tool\maketools\arm-none-eabi\bin;X:\;%PATH%
@copy  x:\tools\make_tools\Makefile_cs.utl x:\tools\make_tools\Makefile.utl
@cd ./tools/make_tools
@cls

%ROOT%
@echo ===CS Compiler environment Initial OK===
@goto END

:END




