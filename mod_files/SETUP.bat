@setlocal
@prompt $
@echo off

echo Setup starting...

:: Copy mp3 sound files to media folder.
xcopy /f /i /y /s sound\mp3\*.mp3 media\mp3\

:: Disable the use of delta.lst
if exist delta.lst (
  echo Renaming delta.lst to delta.old.lst ...
  ren delta.lst delta.old.lst
)

if exist .\gfx\shell\kb_act.lst (
  echo Renaming gfx/shell/kb_act.lst to gfx/shell/kb_act.old.lst ...
  ren .\gfx\shell\kb_act.lst kb_act.old.lst
)

echo Setup successfully completed

endlocal
