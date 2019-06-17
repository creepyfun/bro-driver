#!/bin/bash

for imgfile in *.png ; do
	composite $imgfile ../special/darkgrey.bmp result/$imgfile.bmp
done
rename 's/\.png\.bmp$/\.bmp/' result/* 
