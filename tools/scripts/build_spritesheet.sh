#!/bin/bash
#converts all images in folder to specified size, stitches them together into 
#specified rows and columns
#TODO: add whereis check to make sure imagemagick is installed
rm -Rf exported && mkdir exported
mogrify -resize $1x$2 -quality 100 -path ./exported *.png && cd exported && convert -append *.png spritesheet.png

