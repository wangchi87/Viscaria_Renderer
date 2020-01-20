#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys, os
import numpy
import OpenEXR
import Imath
import imageio
from PIL import Image

path = './'

path = 'white_rabbit/'


def exr_to_png(exrfile, pngfile):
    File = OpenEXR.InputFile(exrfile)
    PixType = Imath.PixelType(Imath.PixelType.FLOAT)
    DW = File.header()['dataWindow']
    Size = (DW.max.x - DW.min.x + 1, DW.max.y - DW.min.y + 1)

    rgb = [numpy.fromstring(File.channel(c, PixType), dtype=numpy.float32) for c in 'RGB']
    for i in range(3):
        rgb[i] = numpy.where(rgb[i]<=0.0031308,
                (rgb[i]*12.92)*255.0,
                (1.055*(rgb[i]**(1.0/2.2))-0.055) * 255.0)
    
    rgb8 = [Image.frombytes("F", Size, c.tostring()).convert("L") for c in rgb]
    Image.merge("RGB", rgb8).save(pngfile, "png", quality=100)

def get_all_exrs():

	files = []
	for f in os.listdir(path):
		if f.endswith('exr'):
			files.append(f)
	return files

if __name__ == '__main__':

    all_exrs = get_all_exrs()

    if not os.path.exists(path+'png'):
        os.mkdir(path+'png')

    for f in all_exrs:

        exr = path + f
        png = path + 'png/' + f[0:-3] + 'png'

        if os.path.exists(png):
            continue

        print exr, png
        exr_to_png(exr, png)

    