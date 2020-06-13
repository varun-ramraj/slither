/*
  Name:         WormTracker.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  WormTracker class...
*/

// Includes...
#include "WormTracker.h"
#include <new>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <sstream>

// Default constructor...
WormTracker::WormTracker()
    : fFieldOfViewDiameter(0.0f),
      pGrayImage(NULL),
      pThinkingImage(NULL),
      unWormsJustAdded(0),
      unCurrentFrame(0),
      unTotalFrames(0),
      unThreshold(150),
      unMaxThresholdValue(255),
      unMinimumCandidateSize(150),
      unMaximumCandidateSize(255),
      bInletDetection(true),
      unMorphologySize(5)
{
    // Initialize the thinking label font...
    
        // Font constants...
        double const        fHorizontalScale    = 0.7;
        double const        fVerticalScale      = 0.7;
        unsigned int const  unThickness         = 0;
        unsigned int const  unLineWidth         = 1;

        // Initialize the font structure...
        cvInitFont(&ThinkingLabelFont, CV_FONT_HERSHEY_PLAIN, 
                   fHorizontalScale, fVerticalScale, unThickness, unLineWidth);
}

// Add new worm to tracker...
void WormTracker::Add(CvContour const &WormContour)
{
    // We cannot do anything without at least the gray image...
    assert(pGrayImage);

    // Breathe life into a new worm from the given contour...
    Worm &NewWorm = *(new Worm(WormContour, *pGrayImage));

    // Add new worm...
    TrackingTable.push_back(&NewWorm);
    
    // Increment just found count...
  ++unWormsJustAdded;
}

// Add a text label to the thinking image at a point...
void WormTracker::AddThinkingLabel(string const sLabel, CvPoint Point)
{
    // Draw label line...
    cvLine(pThinkingImage, cvPoint(Point.x + 20, Point.y + 20), Point,
           CV_RGB(0xfe, 0x00, 0x00));

    // Draw text...
    cvPutText(pThinkingImage, sLabel.c_str(), 
              cvPoint(Point.x + 25, Point.y + 25), &ThinkingLabelFont,
              CV_RGB(0xfe, 0x00, 0x00));
}

// Advance frame...
void WormTracker::Advance(IplImage const &NewGrayImage)
{
    // Variables...
    CvMemStorage   *pStorage        = NULL;
    CvContour      *pFirstContour   = NULL;
    CvContour      *pCurrentContour = NULL;
    unsigned int    unFoundIndex    = (unsigned) - 1;
    CvSize const    ImageSize       = cvGetSize(&NewGrayImage);

    // Lock resources...
    wxMutexLocker   Lock(ResourcesMutex);
    
    // Lock should have been gained successfully...
    assert(Lock.IsOk());

    // Release the old image, if any...
    if(pGrayImage)
        cvReleaseImage(&pGrayImage);
        
    // Release the old thinking image, if any...
    if(pThinkingImage)
        cvReleaseImage(&pThinkingImage);
    
    // Allocate and clone the new one...
    pGrayImage = cvCloneImage(&NewGrayImage);
    
        // Failed...
        if(!pGrayImage)
            throw bad_alloc();

    // Prepare the thinking image...
    
        // Allocate...
        pThinkingImage = cvCreateImage(ImageSize, IPL_DEPTH_8U, 3);
        
            // Failed...
            if(!pThinkingImage)
                throw bad_alloc();

        // Copy in the original grayscale image as colour now...
        cvConvertImage(pGrayImage, pThinkingImage, CV_GRAY2BGR);

    // Image must be a 8-bit, unsigned, grayscale...
    assert(pGrayImage->depth == IPL_DEPTH_8U);

    // Image must not have a region of interest set...
    assert(pGrayImage->roi == NULL);

    // Apply morphological operations to get rid of inlets in worm contours...

        // Duplicate the gray image before editing...
        IplImage *pMorphologicalImage = cvCloneImage(pGrayImage);

        // User requested the operation, so edit the image...
        if(bInletDetection)
        {
            // Allocate conversion kernel...
            IplConvKernel *pConversionKernel = cvCreateStructuringElementEx(
                unMorphologySize, unMorphologySize, unMorphologySize / 2, 
                unMorphologySize / 2, CV_SHAPE_RECT);

            // Eroding and then dilating the image is same as the higher order
            //  operation of opening...
            
                // Erode...
                cvErode(pGrayImage, pMorphologicalImage, pConversionKernel, 1);
            
                // Dilate...
                cvDilate(
                    pMorphologicalImage, pMorphologicalImage, pConversionKernel, 
                    1);

            // Done with conversion kernel...
            cvReleaseStructuringElement(&pConversionKernel);
        }

    // Find the contours in the threshold image...

        // Create threshold...
        IplImage *pThresholdImage = cvCloneImage(pMorphologicalImage);
        cvThreshold(
            pMorphologicalImage, pThresholdImage, unThreshold, 
            unMaxThresholdValue, CV_THRESH_BINARY);

        // Allocate contour storage space...
        pStorage = cvCreateMemStorage(0);
        
        // Find contours...
        cvFindContours(
            pThresholdImage, pStorage, (CvSeq **) &pFirstContour, 
            sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, 
            cvPoint(0, 0));

        // Cleanup...
        cvReleaseImage(&pThresholdImage);
        cvReleaseImage(&pMorphologicalImage);

    // Check to see if the tracker is being shown all the worms at once for
    //  the first time...
    bool const bInitialDiscovery = Tracking() > 0 ? false : true;

    // Go through each contour found...
    for(pCurrentContour = pFirstContour; pCurrentContour;
        pCurrentContour = (CvContour *) pCurrentContour->h_next)
    {
        // Not a possible worm, ignore it...
        if(!IsPossibleWorm(*pCurrentContour))
            continue;

        // Possible worm and initiating for first time, assume every worm
        //  unique...
        else if(bInitialDiscovery)
            Add(*pCurrentContour);

        // Possible worm and some things are already known about the world...
        else
        {
            // Find the nearest worm to this one...
            unFoundIndex = FindNearestWorm(*pCurrentContour);
            
            // Let's hope they are really one and the same. Refresh it with the
            //  new information...
            TrackingTable.at(unFoundIndex)->Refresh(*pCurrentContour, 
                                                    *pGrayImage);
        }
    }
    
    // Cleanup...
    cvReleaseMemStorage(&pStorage); 
    
    // Show some information on each worm contour...
    for(unsigned int unWormIndex = 0; unWormIndex < TrackingTable.size();
      ++unWormIndex)
    {
        // Get the current worm...
        Worm const &CurrentWorm = GetWorm(unWormIndex);

        // Draw the contours onto the thinking image...
        cvDrawContours(pThinkingImage, (CvSeq *) &CurrentWorm.Contour(),
                       CV_RGB(0x00, 0x00, 0xfe), CV_RGB(0x00, 0x00, 0xfe), 0, 
                       1);

        // Show some information about the worm on the thinking image...
        AddThinkingLabel("head", CurrentWorm.Head());
        std::ostringstream ssCentre;
        ssCentre << "(worm " << unWormIndex + 1 << ", updated " 
                 << CurrentWorm.Refreshes() << ")";
        AddThinkingLabel(ssCentre.str(), CurrentWorm.Centre());
        AddThinkingLabel("tail", CurrentWorm.Tail());
    }
    
    // Show one millimeter legend...
    
        // Convert one millimeter to pixels...
        unsigned int const unLegendLength = 
            (unsigned int) ConvertMillimetersToPixels(1.0f);
    
        // Draw the legend line...
        cvLine(pThinkingImage, 
               cvPoint(50, ImageSize.height - 5),
               cvPoint(50 + unLegendLength, ImageSize.height - 5),
               CV_RGB(0x00, 0x00, 0xff), 2);

        // Draw label...
        cvPutText(pThinkingImage, "1 mm", 
                    cvPoint(50 + unLegendLength + 5, ImageSize.height - 3), 
                    &ThinkingLabelFont, CV_RGB(0x00, 0x00, 0xff));

    // Advance frame counter...
  ++unCurrentFrame;
}

// Convert from pixels to millimeters...
double WormTracker::ConvertMillimetersToPixels(double const dMillimeters) const
{
    // Get the image size...
    CvSize const ImageSize = cvGetSize(pGrayImage);

    // Convert units...
    return ((ImageSize.width / fFieldOfViewDiameter) * dMillimeters);
}

// Convert millimeters to pixels...
double WormTracker::ConvertPixelsToMillimeters(double const dPixels) const
{
    // Get the image size...
    CvSize const ImageSize = cvGetSize(pGrayImage);

    // Convert units...
    return ((fFieldOfViewDiameter / ImageSize.width) * dPixels);
}

// Convert from pixels² to millimeters²...
double WormTracker::ConvertSquarePixelsToSquareMillimeters(
    double const dPixelsSquared) const
{
    // Convert units...
    return dPixelsSquared * (1.0f / pow(ConvertMillimetersToPixels(1.0f), 2));
}

// How many underlying rectangles does given one rest upon?
unsigned int const WormTracker::CountRectanglesIntersected(
    CvRect const &Rectangle) const
{
    // Variables...
    unsigned int unIntersections = 0;
    
    // Count the number of intersections...
    for(vector<Worm *>::const_iterator Iterator = TrackingTable.begin();
        Iterator != TrackingTable.end();
      ++Iterator)
    {
        // Worm to check...
        Worm const &CurrentWorm = **Iterator;
        
        // Intersection detected...
        if(IsRectanglesIntersect(Rectangle, CurrentWorm.Rectangle()))
          ++unIntersections;
    }

    // Return the count...
    return unIntersections;
}

// Get the current frame index...
unsigned int const WormTracker::GetCurrentFrameIndex() const
{
    // Lock resources...
    wxMutexLocker   Lock(ResourcesMutex);

    // Return count...
    return unCurrentFrame;
}

// Get a copy of the current thinking image. Caller frees...
IplImage *WormTracker::GetThinkingImage() const
{
    // Lock resources...
    wxMutexLocker   Lock(ResourcesMutex);
    
    // If a deadlock was encountered, just abort...
    if(!Lock.IsOk())
        return NULL;

    // Clone the thinking image, if any...
    if(pThinkingImage)
        return cvCloneImage(pThinkingImage);
    
    // Otherwise, no thinking image available yet...
    else
        return NULL;
}

// Get the total number of frames...
unsigned int const WormTracker::GetTotalFrames() const
{
    // Lock resources...
    wxMutexLocker   Lock(ResourcesMutex);

    // Return count...
    return unTotalFrames;
}

// Get the nth worm, or null worm if no more...
Worm const &WormTracker::GetWorm(unsigned int const unIndex) const
{
    // Check bounds...
    assert(unIndex + 1 <= TrackingTable.size());

    // Return worm...
    return *TrackingTable.at(unIndex);
}

// Get the number of worms just added since last check...
unsigned int const WormTracker::GetWormsAddedSinceLastCheck()
{
    // Backup original for caller before resetting count...
    unsigned int unTemp = unWormsJustAdded;
    unWormsJustAdded    = 0;
    
    // Done...
    return unTemp;
}

// Find the nearest worm to given...
unsigned int WormTracker::FindNearestWorm(CvContour const &WormContour) const
{
    // Variables...
    CvMoments       WormMoment;
    CvPoint         WormCentre              = {0, 0};
    double          dDistanceToClosestWorm  = FLT_MAX;
    unsigned int    unClosestWormIndex      = (unsigned) -1;

    // It doesn't make sense to ask us if we have no data...
    assert(Tracking() != 0);

    // Calculate the gravitational centre of the given contour...

        // Compute all moments of the contour...
        cvMoments(&WormContour, &WormMoment);

        // Extract centre of gravity...
        WormCentre.x = int(WormMoment.m10 / WormMoment.m00);
        WormCentre.y = int(WormMoment.m01 / WormMoment.m00);

    // Check each worm's proximity to this one...
    for(unsigned int unWormIndex = 0; unWormIndex < TrackingTable.size();
      ++unWormIndex)
    {
        // Worm to check...
        Worm const &CurrentWorm = *TrackingTable.at(unWormIndex);

        // How far away is the given worm to this iteration's...
        double const dDistanceToWorm = 
            cvSqrt(pow(double(WormCentre.x) - CurrentWorm.Centre().x, 2) + 
                   pow(double(WormCentre.y) - CurrentWorm.Centre().y, 2));

        // Remember only if its centre of mass has best proximity...
        if(dDistanceToWorm < dDistanceToClosestWorm)
        {
            // Make a note of how close it was and which worm...
            dDistanceToClosestWorm  = dDistanceToWorm;
            unClosestWormIndex      = unWormIndex;
        }
    }
    
    // Return index...
    return unClosestWormIndex;
}

// Do any points on the mystery contour lie on the image exterior?
bool WormTracker::IsAnyPointOnImageExterior(CvContour const &MysteryContour)
    const
{
    // Image size...
    CvSize const Size = cvGetSize(pGrayImage);

    // Check each point to see if any lie on image exterior...
    for(unsigned int unVertexIndex = 0; 
        unVertexIndex < (unsigned) MysteryContour.total; 
        unVertexIndex++)
    {
        // Get the point...
        CvPoint const &Point = *((CvPoint *) 
            cvGetSeqElem((CvSeq *) &MysteryContour, unVertexIndex));

        // On either the left or right extremity...
        if(Point.x == 0 || Point.x == Size.width)
            return true;

        // On either the top or bottom extremity...
        if(Point.y == 0 || Point.y == Size.height)
            return true;
    }

    // Every vertex came out non-tangent to the exterior...
    return false;
}

// Could this contour be a worm, independent of what we know?
bool WormTracker::IsPossibleWorm(CvContour const &MysteryContour) const
{
    // Too few vertices...
    if(MysteryContour.total < 6)
        return false;

    // We must have had the field of view diameter set...
    assert(fFieldOfViewDiameter > 0.0f);

    // Calculate the pixel area of the worm...
    double const dPixelArea = fabs(cvContourArea(&MysteryContour));
    
    // Convert the pixel area to mm²...
    double const dMillimeterArea = 
        ConvertSquarePixelsToSquareMillimeters(dPixelArea);

    // Too small / too big to be a worm...
//    if((dPixelArea < 200.0) || (800.0 < dPixelArea))
    if((dMillimeterArea < ((float) unMinimumCandidateSize / 1000.0f)) || 
       (((float) unMaximumCandidateSize / 1000.0f) < dMillimeterArea))
        return false;

    // Contours with points on image exterior not permitted...
    if(IsAnyPointOnImageExterior(MysteryContour))
        return false;

    // Meets worm minima...
    return true;
}

// Do the two rectangles have a non-zero intersection area?
bool WormTracker::IsRectanglesIntersect(CvRect const &RectangleOne,
                                        CvRect const &RectangleTwo) const
{
    // Check...
    return (RectangleOne.x < RectangleTwo.x + RectangleTwo.width) && 
           (RectangleOne.y < RectangleTwo.y + RectangleTwo.height) &&
           (RectangleOne.x + RectangleOne.width > RectangleTwo.x) && 
           (RectangleOne.y + RectangleOne.height > RectangleTwo.y);
}

// The number of worms we are currently tracking...
unsigned int WormTracker::Tracking() const
{
    // Return the size of the table...
    return TrackingTable.size();
}

// Reset the tracker...
void WormTracker::Reset(unsigned int const _unTotalFrames)
{
    // Lock resources...
    wxMutexLocker   Lock(ResourcesMutex);
        
    // Cleanup the worms...
        
        // Deallocate each worm...
        for(vector<Worm *>::const_iterator Iterator = TrackingTable.begin();
            Iterator != TrackingTable.end();
          ++Iterator)
        {
            // Deallocate
            delete *Iterator;
        }
        
        // Clear the dead pointer table space...
        TrackingTable.clear();

    // Cleanup the gray image, if any...
    if(pGrayImage)
        cvReleaseImage(&pGrayImage);
    pGrayImage = NULL;

    // Cleanup the thinking image, if any...
    if(pThinkingImage)
        cvReleaseImage(&pThinkingImage);
    pThinkingImage = NULL;
        
    // Worms just added in this frame...
    unWormsJustAdded = 0;
    
    // Reset current frame and total count...
    unCurrentFrame  = 0;
    unTotalFrames   = _unTotalFrames;
}

// Set the field of view diameter...
void WormTracker::SetFieldOfViewDiameter(float const fDiameter)
{
    // Store...
    fFieldOfViewDiameter = fDiameter > 0.01 ? fDiameter : 0.01;
}

// Set artificial intelligence magic numbers / flags...
void WormTracker::SetArtificialIntelligenceMagic(
    unsigned int const  _unThreshold, 
    unsigned int const  _unMaxThresholdValue,
    unsigned int const  _unMinimumCandidateSize, 
    unsigned int const  _unMaximumCandidateSize, 
    bool const          _bInletDetection,
    unsigned int        _unMorphologySize)
{
    // Store new numbers / flags...
    unThreshold             = _unThreshold;
    unMaxThresholdValue     = _unMaxThresholdValue;
    unMinimumCandidateSize  = _unMinimumCandidateSize;
    unMaximumCandidateSize  = _unMaximumCandidateSize;
    bInletDetection         = _bInletDetection;
    unMorphologySize        = _unMorphologySize;
}

// Deconstructor...
WormTracker::~WormTracker()
{
    // Cleanup the worms...
    for(vector<Worm *>::const_iterator Iterator = TrackingTable.begin();
        Iterator != TrackingTable.end();
      ++Iterator)
    {
        // Deallocate
        delete *Iterator;
    }

    // Cleanup the gray image, if any...
    if(pGrayImage)
        cvReleaseImage(&pGrayImage);

    // Cleanup the thinking image, if any...
    if(pThinkingImage)
        cvReleaseImage(&pThinkingImage);   
}

// Output some info on current tracker state......
ostream & operator<<(ostream &Output, WormTracker &RequestedWormTracker)
{
    // Lock resources...
    wxMutexLocker   Lock(RequestedWormTracker.ResourcesMutex);
    
    // Lock should have been gained successfully...
    assert(Lock.IsOk());

    // Show some general information about tracker...
    cout << "Tracking " << RequestedWormTracker.TrackingTable.size() 
         << " worms..." << endl;

    // Iterate through each worm in the tracker...
    for(unsigned int unWormIndex = 0;
        unWormIndex < RequestedWormTracker.TrackingTable.size();
      ++unWormIndex)
    {
        // Show some information about each worm...
        cout << "Worm " << unWormIndex << endl;
        cout << *RequestedWormTracker.TrackingTable.at(unWormIndex) << endl;
    }

    // Return the stream...
    return Output;
}

