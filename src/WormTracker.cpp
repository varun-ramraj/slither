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
    : pStorage(cvCreateMemStorage(0)),
      pGrayImage(NULL),
      pThinkingImage(NULL)
{
    // Allocatation of base storage failed...
    if(!pStorage)
        throw bad_alloc();
        
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

// Acknowledge a worm contour...
void WormTracker::Acknowledge(CvContour const &WormContour)
{
    // Variables...
    unsigned int unFoundIndex   = (unsigned) -1;

    // We cannot do anything without at least the gray image...
    assert(pGrayImage);

    // Frame advancer was suppose to check for this already...
    assert(IsPossibleWorm(WormContour));

    // The worm is known, refresh it with the new information...
    if(IsKnown(WormContour, unFoundIndex))
        GetWorm(unFoundIndex).Refresh(WormContour, *pGrayImage);

    // The worm is not known, inform the tracker...
    else
        Add(WormContour);
}

// Add worm to tracker...
void WormTracker::Add(CvContour const &WormContour)
{
    // We cannot do anything without at least the gray image...
    assert(pGrayImage);

    // Breath life into a new worm from the given contour...
    Worm &NewWorm = *(new Worm(WormContour, *pGrayImage));

    // Add new worm...
    TrackingTable.push_back(&NewWorm);
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
void WormTracker::AdvanceNextFrame(IplImage const &NewGrayImage)
{
    // Variables...
    CvContour      *pFirstContour   = NULL;
    CvContour      *pCurrentContour = NULL;
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
    
    // Create threshold...
    IplImage *pThresholdImage = cvCloneImage(pGrayImage);
    cvThreshold(pGrayImage, pThresholdImage, 150, 255, CV_THRESH_BINARY);

    // Find the contours in the threshold image...
    cvFindContours(pThresholdImage, pStorage, (CvSeq **) &pFirstContour, 
                   sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, 
                   cvPoint(0, 0));

    // Cleanup...
    cvReleaseImage(&pThresholdImage);

    // Go through each contour found...
    for(pCurrentContour = pFirstContour; pCurrentContour;
        pCurrentContour = (CvContour *) pCurrentContour->h_next)
    {
        // Not a possible worm, ignore it...
        if(!IsPossibleWorm(*pCurrentContour))
            continue;

        // Otherwise, acknowledge it...
        else
            Acknowledge(*pCurrentContour);
    }
    
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
        ssCentre << "(worm " << unWormIndex << ", updated " 
                 << CurrentWorm.Refreshes() << ")";
        AddThinkingLabel(ssCentre.str(), CurrentWorm.Centre());
        AddThinkingLabel("tail", CurrentWorm.Tail());
    }
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

// Get the nth worm, or null worm if no more...
Worm &WormTracker::GetWorm(unsigned int const unIndex) const
{
    // Check bounds...
    assert(unIndex + 1 <= TrackingTable.size());

    // Return worm...
    return *TrackingTable.at(unIndex);
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

// Find the worm that probably created this contour, if any...
bool WormTracker::IsKnown(CvContour const &WormContour, 
                          unsigned int &unFoundIndex) const
{
   // Worm's rectangle...
    CvMoments       WormMoment;
    CvPoint         WormCentre              = {0, 0};
    double          dDistanceToClosestWorm  = FLT_MAX;
    unsigned int    unClosestWormIndex      = (unsigned) -1;

    // There is nothing being tracked at present, so the worm cannot be known...
    if(Tracking() == 0)
        return false;

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
    
    // Isolate the best match now...
    Worm &BestMatch = *TrackingTable.at(unClosestWormIndex);
    unFoundIndex = unClosestWormIndex;
    
    // If given worm contour isn't tangent to image exterior, assume known...
/*    if(!IsAnyPointOnImageExterior(WormContour))*/

    // If the rectangles intersect, then we have good chance of match...
    if(IsRectanglesIntersect(WormContour.rect, BestMatch.Rectangle()))
        return true;

    // Are they close enough to be a match?
    else if(dDistanceToClosestWorm <= 35.0f)
        return true;

    // No match...
    else
        return false;
}

// Could this contour be a worm, independent of what we know?
bool WormTracker::IsPossibleWorm(CvContour const &MysteryContour) const
{
    // Too few vertices...
    if(MysteryContour.total < 6)
        return false;

    // Calculate the area of the worm...
    double const dArea = fabs(cvContourArea(&MysteryContour));

    // Too small / too big to be a worm...
    if((dArea < 200.0) || (800.0 < dArea))
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
    
    // Cleanup base storage...
    cvReleaseMemStorage(&pStorage);    
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

