echo off

REM This file runs the pcy2stl conversion executable
REM Created by Hardik @ 2018 (May)

REM takes two arguments:
REM 1. STP file
REM 2. PCD file

set PATH=%PATH%;.;.\lib

if "%1"=="" (
echo Syntax is - coa.bat "filename.stp" "filename2.asc" "1(optional)"
goto :end
)
if "%2"== "" (
echo Syntax is - coa.bat "filename.stp" "filename2.asc" "1(optional)"
goto :end
)

REM Empty temp folder
del /q "temp\*"
FOR /D %%p IN ("temp\*.*") DO rmdir "%%p" /s /q

REM empty output folder
del /q "output\*"
FOR /D %%p IN ("output\*.*") DO rmdir "%%p" /s /q

REM Step 1: Convert asc file to pcd
bin\\asc2pcd.exe %2 temp\\%2.pcd

REM Step 2: convert stp to stl with OCC executable
bin\\stp2stl.exe %1 temp\\%1.stl
REM stp2stl.exe %1% %1%.stl > %1%.stl.info 

REM Step 3: convert stl to pcd with pcl executable
bin\\stl2pcd.exe temp\\%1.stl temp\\%1.pcd


REM Step 4: compare PCD to PCD with PCL executable
bin\\pcd_read.exe temp\\%1.pcd> temp\\text.txt
set /P var1=<temp\\text.txt
del temp\\text.txt

bin\\pcd_read.exe temp\\%2.pcd > temp\\text.txt
set /P var2=< temp\\text.txt
del temp\\text.txt

if %var1% LSS %var2% (bin\\octree_change_detection.exe temp\\%1.pcd temp\\%2.pcd)
if %var1% GEQ %var2% (bin\\octree_change_detection.exe temp\\%2.pcd temp\\%1.pcd)

ren output\\Difference.pcd %1-%2.pcd

REM Step 5: Convert newCloud.pcd to newMesh.vtk
bin\\pcd2vtk.exe output\\%1-%2.pcd temp\\newMesh.vtk

REM Step 6: Convert VTK to OBJ
bin\\vtk2obj.exe temp\\newMesh.vtk temp\\newObj.obj

REM Step 7: Convert OBJ to STL
bin\\stl2pcd.exe temp\\newObj.obj temp\\newStl.stl

REM Step 8: Convert STL to STEP
bin\\stl2step.exe temp\\newStl.stl output\\%1-%2.stp



REM Delete the temporary files
if [%3] == [] (del temp\\%1.stl
del temp\\%1.pcd
del temp\\newMesh.vtk
del temp\\newObj.obj
del temp\\newStl.stl
del temp\\%2.pcd
)

:end
REM end of file

