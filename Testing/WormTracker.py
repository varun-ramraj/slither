## Python implementation of WormTracker{.h/.cpp}

import numpy as np
import math
import logging
import cv2 as cv

class WormTracker():

    def __init__(self, fovDiameter = 5.0, grayImg = None, thinkingImg = None,
        unWormsJustAdded = 0, unCurrentFrame = 0, unTotalFrames = 0,
        unThreshold = 150.0, unMaxThresholdValue = 255.0,
        unMinimumCandidateSize = 100.0, unMaximumCandidateSize = 350.0,
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

    def ConvertMillimetersToPixels(self, dMillimeters):
        width = np.size(self.grayImg, 1)
        logging.debug("imgwidth: %f" % width)
        return ((width / self.fovDiameter) * dMillimeters);

    def ConvertSquarePixelsToSquareMillimeters(self, dPixelsSquared):
        curval = dPixelsSquared * (1.0 /
            math.pow(self.ConvertMillimetersToPixels(1.0), 2))

        return curval

    def IsPossibleWorm(self, MysteryContour):

        logging.debug("Field of view: %f" % self.fovDiameter)

        if len(MysteryContour) < 6:
            return False

        dPixelArea = abs(cv.contourArea(MysteryContour))


        dMillimeterArea = self.ConvertSquarePixelsToSquareMillimeters(dPixelArea)
        if ((dMillimeterArea < self.unMinimumCandidateSize / 1000.0) or
            (self.unMaximumCandidateSize / 1000.0 < dMillimeterArea)):
            logging.debug("Absolute pixel area: %f" % dPixelArea)
            logging.debug("Absolute millimetre area: %f" % dMillimeterArea)

            return False

        # TODO: Image exterior check
        logging.debug('Possible worm contour area: %f' % dMillimeterArea)

        return True

    def Advance(self, grayImage):
        self.grayImg = grayImage

        thinkingImage = cv.cvtColor(grayImage, cv.COLOR_GRAY2BGR)
        morphologicalImage = grayImage.copy()

        if self.bInletDetection:
            pass # implement later

        thresh1, thresholdImage = cv.threshold(morphologicalImage, self.unThreshold,
            self.unMaxThresholdValue, cv.THRESH_BINARY)

        contours, hierarchy = cv.findContours(thresholdImage, cv.RETR_LIST,
            cv.CHAIN_APPROX_NONE)

        for pCurrentContour in contours:
            if self.IsPossibleWorm(pCurrentContour):
                cv.drawContours(grayImage, pCurrentContour, -1, (0, 255, 0), 3)


        cv.imshow('Tracker', grayImage)
