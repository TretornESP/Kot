cd ../Kernel
start build.bat
cd ../build
diskpart /s %CD%\mount.txt
cd ../bin
copy %CD%\*.* e:
cd ../build
diskpart /s %CD%\dismount.txt
pause