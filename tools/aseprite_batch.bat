@echo off
set aseprite="C:\Program Files (x86)\Steam\steamapps\common\Aseprite\Aseprite.exe"


@REM set file1=C:\gamedev\PlatformerMD\res\Levels\LDtk_project\world_levels\png\Level_1-AutoMainBG.png
@REM set file2=C:\gamedev\PlatformerMD\res\Levels\LDtk_project\world_levels\png\Level_1-AutoMainFG.png
@REM set output1=C:\gamedev\PlatformerMD\res\Levels\LDtk_project\world_levels\png\outputtest.ase
@REM set finaloutput=C:\gamedev\PlatformerMD\res\Levels\LDtk_project\world_levels\Level_1-AutoMain.png
set file1=%1
set file2=%2
set filecol=%3
set output1=%4
set finaloutput=%5
set outputcol=%6

set palscript=C:\Users\owner\AppData\Roaming\Aseprite\scripts\DuplicateShiftPalette.lua
set shiftscript=C:\Users\owner\AppData\Roaming\Aseprite\scripts\ShiftAllColorsLayer.lua
set pileupscript=C:\Users\owner\AppData\Roaming\Aseprite\scripts\MakeSheetIntoLayer.lua


%aseprite% -b  %filecol% --color-mode indexed --save-as %outputcol%

%aseprite% -b -script-param f1=%file1% -script-param f2=%file2% -script-param out=%output1% -script %pileupscript%

%aseprite% -b  %output1% --color-mode indexed --save-as %output1%

%aseprite% -b  %output1% -script %palscript% --save-as %output1%

%aseprite% -b  -script-param cli="yes" %output1% -script %shiftscript% --save-as %finaloutput%
