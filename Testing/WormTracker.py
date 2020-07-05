## Python implementation of WormTracker{.h/.cpp}

import numpy as np
import math
import logging
import cv2 as cv
from Worm import *

class WormTracker():

    def __init__(self, fovDiameter = 5.0, grayImg = None, thinkingImg = None,
        unWormsJustAdded = 0, unCurrentFrame = 0, unTotalFrames = 0,
        unThreshold = 150.0, unMaxThresholdValue = 255.0,
        unMinimumCandidateSize = 150.0, unMaximumCandidateSize = 455.0,
        bInletDetection = True, unMorphologySize = 5.0):

        logging.getLogger('SlitherLogger')
        self.fovDiameter = fovDiameter
        self.grayImg = grayImg
        self.thinkingImg = thinkingImg
        self.unWormsJustAdded = unWormsJustAdded
        self.unCurrentFrame = unCurrentFrame
        self.unTotalFrames = unTotalFrames
        self.unThreshold = unThreshold
        self.unMaxThresholdValue = unMaxThresholdValue
        self.unMinimumCandidateSize = unMinimumCandidateSize
        self.unMaximumCandidateSize = unMaximumCandidateSize
        self.bInletDetection = bInletDetection
        self.unMorphologySize = unMorphologySize

        # initialise the thinking label font attributes
        self.fontHScale = 0.7
        self.fontVScale = 0.7
        self.fontThickness = 0
        self.fontLineWidth = 1
        self.fontFace = cv.FONT_HERSHEY_PLAIN

        # worm tracking table
        self.TrackingTable = []

    def ConvertMillimetersToPixels(self, dMillimeters):
        width = np.size(self.grayImg, 1)
        logging.debug("imgwidth: %f" % width)
        return ((width / self.fovDiameter) * dMillimeters);

    def ConvertSquarePixelsToSquareMillimeters(self, dPixelsSquared):
        curval = dPixelsSquared * (1.0 /
            math.pow(self.ConvertMillimetersToPixels(1.0), 2))

        return curval

    def IsAnyPointOnImageExterior(self, MysteryContour):

        height, width = self.grayImg.shape #same as np.size(img, 0 or 1)
        for i in MysteryContour:
            currentPoint = i[0]

            if(currentPoint[0] == 0 or currentPoint[0] == width):
                return True

            if(currentPoint[1] == 0 or currentPoint[1] == height):
                return True

        return False

    def IsPossibleWorm(self, MysteryContour):

        logging.debug("Field of view: %f" % self.fovDiameter)

        if len(MysteryContour) < 6:
            return False

        dPixelArea = abs(cv.contourArea(MysteryContour))

        dMillimeterArea = self.ConvertSquarePixelsToSquareMillimeters(dPixelArea)

        # Too small/too big to be a worm
        if ((dMillimeterArea < self.unMinimumCandidateSize / 1000.0) or
            (self.unMaximumCandidateSize / 1000.0 < dMillimeterArea)):
            logging.debug("Absolute pixel area: %f" % dPixelArea)
            logging.debug("Absolute millimetre area: %f" % dMillimeterArea)

            return False

        if (self.IsAnyPointOnImageExterior(MysteryContour)):
            return False


        logging.debug('Possible worm contour area: %f' % dMillimeterArea)

        return True

    def Tracking(self):
        return len(self.TrackingTable)

    def Add(self, WormContour):
        self.TrackingTable.append(WormContour)
        self.unWormsJustAdded += 1

    def Advance(self, grayImage):
        self.grayImg = grayImage

        thinkingImage = cv.cvtColor(grayImage, cv.COLOR_GRAY2BGR)
        morphologicalImage = grayImage.copy()

        if self.bInletDetection:
            pass # TODO: implement later

        thresh1, thresholdImage = cv.threshold(morphologicalImage, self.unThreshold,
            self.unMaxThresholdValue, cv.THRESH_BINARY)

        contours, hierarchy = cv.findContours(thresholdImage, cv.RETR_LIST,
            cv.CHAIN_APPROX_NONE)

        bInitialDiscovery = True
        if self.Tracking() > 0:
            bInitialDiscovery = False

        for pCurrentContour in contours:
            if not self.IsPossibleWorm(pCurrentContour):
                continue

            elif bInitialDiscovery is True:
                self.Add(pCurrentContour) #TODO: create Worm class and add

            else:
                cv.drawContours(thinkingImage, pCurrentContour, -1, (0, 255, 0), 1)


        cv.imshow('Tracker', thinkingImage)
