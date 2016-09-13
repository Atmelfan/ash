#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    read -p "Board name? " name
else
    name=$1
fi
echo "Board name: $name"

function error {
	echo -e "\e[00;31m$1\e[00m"
}

function move {
	cp $1 $2 2> /dev/null
}

echo "Deleting export..."
rm -rf "export"
mkdir "export"

echo "Copying files..."
move "Top.gbr" "export/$name.gtl"
if [ $? -eq 1 ]; then
	error " No top layer!"
fi
move "Bottom.gbr" "export/$name.gbl"
if [ $? -eq 1 ]; then
	error " No bottom layer!"
fi
move "TopMask.gbr" "export/$name.gts"
if [ $? -eq 1 ]; then
	error " No top mask!"
fi
move "BottomMask.gbr" "export/$name.gbs"
if [ $? -eq 1 ]; then
	error " No bottom mask!"
fi
move "TopSilk.gbr" "export/$name.gto"
if [ $? -eq 1 ]; then
	error " No top silkscreen!"
fi
move "BottomSilk.gbr" "export/$name.gbo"
if [ $? -eq 1 ]; then
	error " No bottom silkscreen!"
fi
move "BoardOutline.gbr" "export/$name.gml"
if [ $? -eq 1 ]; then
	error " No board outline!"
fi
move "Through.drl" "export/$name.txt"
if [ $? -eq 1 ]; then
	error " No drill file!"
fi
echo "Deleting unused files..."
rm *.gbr *.drl  2> /dev/null
echo "Creating ZIP file..."
cd "export"
zip -r "$name.zip" *
echo "Done!"
