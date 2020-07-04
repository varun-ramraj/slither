#!/usr/bin/env python3

import argparse
import sys
import os
import logging
import numpy as np
import cv2 as cv

# internal Slither libraries
from WormTracker import *

def setupArguments():

    parser = argparse.ArgumentParser('Tracker test for Slither')

    parser.add_argument('imagefiles',type=str, nargs='+',
        help='One or more image file names')

    parser.add_argument('-fov', '--field-of-view', type=float, default=5.0)

    parser.add_argument('-g', '--logfile', help='Optional log file name. If \
        omitted, log will be written to standard output/error.')
    return parser.parse_args()


def main():

    args = setupArguments()

    logging.getLogger('SlitherLogger')
    logging.basicConfig(level=logging.DEBUG, filename = args.logfile,
        format='%(asctime)s %(levelname)s - %(message)s',
        datefmt='%d-%b-%y %H:%M:%S')

    TestTracker = WormTracker(fovDiameter=args.field_of_view)

    cv.namedWindow('Tracker', cv.WINDOW_AUTOSIZE)

    for nCurrentFrame in args.imagefiles:
        logging.debug("Currently processing: %s" % nCurrentFrame)

        grayImage = cv.imread(nCurrentFrame, cv.IMREAD_GRAYSCALE)
        cv.imshow('Tracker', grayImage)

        TestTracker.Advance(grayImage)

        while(True):
            nKey = cv.waitKey(500)

            if nKey == 27: #Escape exits
                cv.destroyAllWindows()
                sys.exit(0)

            elif nKey == 32: #Spacebar moves to next frame
                break

            else:
                continue


    cv.destroyAllWindows()

    return

if __name__ == '__main__':
    main()
