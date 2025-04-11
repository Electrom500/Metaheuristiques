#!/bin/bash

ffmpeg -framerate 1 -start_number 1 -i algorithme/cycle_%d.png -c:v libopenh264 -pix_fmt yuv420p algorithme.mp4
