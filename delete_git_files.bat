@echo off
setlocal enabledelayedexpansion

:: 删除.git\config中的[remote "origin"]开始的部分
set "file_path=.\.git\config"
set "search_string=[remote "origin"]"
set "temp_file=%temp%\temp_git_config.txt"

if exist "%file_path%" (
    echo Reading %file_path%...
    set /p content=<"%file_path%"
    set "start_deleting=0"
    for /f "delims=" %%a in ('type "%file_path%"') do (
        if "%%a"=="%search_string%" (
            set "start_deleting=1"
        ) else (
            if !start_deleting!==1 (
                set "content=!content:%%a=!"
            )
        )
    )
    >"%temp_file%" echo !content!
    move /y "%temp_file%" "%file_path%" >nul
) else (
    echo File not found: %file_path%
)

:: 删除.git\logs\refs\remotes文件夹及其下的所有文件
set "folder_path=.\.git\logs\refs\remotes"
if exist "%folder_path%" (
    echo Deleting %folder_path%...
    rd /s /q "%folder_path%"
) else (
    echo Folder not found: %folder_path%
)

:: 删除.git\refs\remotes文件夹及其下的所有文件
set "folder_path=.\.git\refs\remotes"
if exist "%folder_path%" (
    echo Deleting %folder_path%...
    rd /s /q "%folder_path%"
) else (
    echo Folder not found: %folder_path%
)

:: 删除脚本自身
set "script_path=%~dpnx0"
if exist "%script_path%" (
    echo Deleting %script_path%...
    del /f /q "%script_path%"
) else (
    echo Script not found: %script_path%
)

endlocal
