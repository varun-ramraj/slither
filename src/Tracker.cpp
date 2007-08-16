/*
  Name:         Tracker.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Tracker class...
*/

// Includes...
#include "Tracker.h"
#include <new>
#include <cmath>
#include <cassert>
#include <algorithm>

// Default constructor...
Tracker::Tracker()
    : pStorage(cvCreateMemStorage(0)),
      pGrayImage(NULL),
      pThinkingImage(NULL)
{
    // Allocatation of base storage failed...
    if(!pStorage)
        throw std::bad_alloc();
}

// Acknowledge a worm contour...
void Tracker::Acknowledge(CvContour &WormContour)
{
    // We cannot do anything without at least the gray image...
    assert(pGrayImage);

    // Not a possible worm, ignore it...
    if(!IsPossibleWorm(WormContour))
        return;

    // This worm's contour doesn't overlap with any other's...
    if(CountRectanglesIntersected(WormContour.rect) == 0)
    {
        // This worm is within the outermost edge of the frame, add it...
        if(IsWithinOuterFrameEdge(WormContour))
            Add(WormContour);

        // This worm is within the inner portion of the frame...
        else
        {
            // We aren't tracking anything yet, so it must be new...
            if(Tracking() == 0)
                Add(WormContour);

            // We are tracking, so assume it is somewhere in our tables...
            else
            {
                // Find the best match...
                Worm &BestMatch = FindBestMatch(WormContour);

                // Update it with the new contour and image data...
                BestMatch.Update(WormContour, *pGrayImage);
            }
        }
    }
    
    // This worm's contour overlaps with at least one other...
    else
    {
        // Find the best match...
        Worm &BestMatch = FindBestMatch(WormContour);

        // Update it with the new contour...
        BestMatch.Update(WormContour, *pGrayImage);
    }
}

// Add worm to tracker...
void Tracker::Add(CvContour const &WormContour)
{
    // We cannot do anything without at least the gray image...
    assert(pGrayImage);

    // Add new worm...
    TrackingTable.push_back(new Worm(WormContour, *pGrayImage));
}

// Advance frame...
void Tracker::AdvanceNextFrame(IplImage const *pNewGrayImage)
{
    // Variables...
    CvContour  *pFirstContour   = NULL;
    CvContour  *pCurrentContour = NULL;

    // Release the old image, if any...
    if(pGrayImage)
        cvReleaseImage(&pGrayImage);
        
    // Release the old thinking image, if any...
    if(pThinkingImage)
        cvReleaseImage(&pThinkingImage);
    
    // Allocate and clone the new one...
    pGrayImage = cvCloneImage(pNewGrayImage);
    
        // Failed...
        if(!pGrayImage)
            throw std::bad_alloc();

    // Prepare the thinking image...
    
        // Allocate...
        pThinkingImage = cvCreateImage(cvGetSize(pGrayImage), IPL_DEPTH_8U, 3);
        
            // Failed...
            if(!pThinkingImage)
                throw std::bad_alloc();

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

        /* Acknowledge...
        Acknowledge(*pCurrentContour);*/
        
        cvDrawContours(pThinkingImage, (CvSeq*) pCurrentContour,
                       CV_RGB(rand() & 255, rand() & 255, rand() & 255),
                       CV_RGB(rand() & 255, rand() & 255, rand() & 255), 0,
                       2);
    }
}

// How many underlying rectangles does given one rest upon?
unsigned int const Tracker::CountRectanglesIntersected(CvRect const &Rectangle) 
    const
{
    // Variables...
    unsigned int unIntersections = 0;
    
    // Count the number of intersections...
    for(std::vector<Worm *>::const_iterator Iterator = TrackingTable.begin();
        Iterator != TrackingTable.end();
      ++Iterator)
    {
        // Worm to check...
        Worm const *pCurrentWorm = *Iterator;
        
        // Intersection detected...
        if(IsRectanglesIntersect(Rectangle, pCurrentWorm->Rectangle()))
          ++unIntersections;
    }

    // Return the count...
    return unIntersections;
}

// Find the best match of this contour...
Worm &Tracker::FindBestMatch(CvContour &WormContour) const
{
    // Worm's rectangle...
    CvMoments       WormMoment;
    CvPoint         WormCentre              = {0, 0};
    double          dDistanceToClosestWorm  = FLT_MAX;
    unsigned int    unClosestWormIndex      = (unsigned) -1;

    // There had to be at least one worm tracking...
    assert(Tracking() > 0);

    // Calculate the gravitational centre of the given contour...

        // Compute all moments of the contour...
        cvMoments(&WormContour, &WormMoment);

        // Extract centre of gravity...
        WormCentre.x = int(WormMoment.m10 / WormMoment.m00);
        WormCentre.y = int(WormMoment.m01 / WormMoment.m00);

    // Check each worm's proximity to this one...
    for(unsigned int unWormIndex = 0; 
        unWormIndex < TrackingTable.size();
      ++unWormIndex)
    {
        // Worm to check...
        Worm const * const pCurrentWorm = TrackingTable.at(unWormIndex);

        // How far away is the given worm to this iteration's...
        double const dDistanceToWorm = 
            cvSqrt(pow(WormCentre.x - pCurrentWorm->Centre().x, 2) + 
                    pow(WormCentre.y - pCurrentWorm->Centre().y, 2));

        // Remember only if its centre of mass has best proximity...
        if(dDistanceToWorm < dDistanceToClosestWorm)
        {
            // Make a note of how close it was and which worm...
            dDistanceToClosestWorm  = dDistanceToWorm;
            unClosestWormIndex      = unWormIndex;
        }
    }

    // Return reference to the best worm found...
    return *TrackingTable.at(unClosestWormIndex);
}

// Get the current thinking image...
IplImage const *Tracker::GetThinkingImage() const
{
    // Return it...
    return pThinkingImage;
}

// Get the nth worm, or null worm if no more...
Worm const &Tracker::GetWorm(unsigned int const unIndex) const
{
    // Check bounds...
    if(unIndex + 1 >= TrackingTable.size())
        return NullWorm;

    // Return worm...
    return *TrackingTable.at(unIndex);
}

// Do the two rectangles have a non-zero intersection area?
bool Tracker::IsRectanglesIntersect(CvRect const &RectangleOne,
                                    CvRect const &RectangleTwo) const
{
    // Check...
    return (RectangleOne.x < RectangleTwo.x + RectangleTwo.width) && 
           (RectangleOne.y < RectangleTwo.y + RectangleTwo.height) &&
           (RectangleOne.x + RectangleOne.width > RectangleTwo.x) && 
           (RectangleOne.y + RectangleOne.height > RectangleTwo.y);
}

// Does this worm's contour lie within the outer edge of the frame?
bool Tracker::IsWithinOuterFrameEdge(CvContour const &WormContour) const
{
    // Border thickness...
    int const   nBorderThickness    = 80;

    // Check our assumptions...
    assert((pGrayImage->height - nBorderThickness) > 0);
    assert((pGrayImage->width  - nBorderThickness) > 0);

    // Bounding rectangle of worm...
    CvRect const &WormRectangle = WormContour.rect;

    // Within left border...
    if((WormRectangle.x + WormRectangle.width) < nBorderThickness)
        return true;

    // Within top border...
    else if((WormRectangle.y + WormRectangle.height) < nBorderThickness)
        return true;

    // Within right border...
    else if(WormRectangle.x > (pGrayImage->width - nBorderThickness))
        return true;

    // Within bottom border...
    else if(WormRectangle.y > (pGrayImage->height - nBorderThickness))
        return true;
        
    // Not within any of the borders...
    else
        return false;
}

// Could this contour be a worm, independent of what we know?
bool Tracker::IsPossibleWorm(CvContour const &MysteryContour) const
{
    // Too few vertices...
    if(MysteryContour.total < 6)
        return false;

    // Calculate the area of the worm...
    double const dArea = fabs(cvContourArea(&MysteryContour));

    // Too small / too big to be a worm...
    if((dArea < 200.0) || (800.0 < dArea))
        return false;
        
    // Meets worm minima...
    return true;
}

// The number of worms we are currently tracking...
unsigned int Tracker::Tracking() const
{
    // Return the size of the table...
    return TrackingTable.size();
}

// Deconstructor...
Tracker::~Tracker()
{
    // Cleanup the worms...
    for(std::vector<Worm *>::const_iterator Iterator = TrackingTable.begin();
        Iterator != TrackingTable.end();
      ++Iterator)
    {
        // Deallocate
        delete *Iterator;
    }

    // Cleanup the gray image...
    cvReleaseImage(&pGrayImage);
    
    // Cleanup base storage...
    cvReleaseMemStorage(&pStorage);    
}

// Output some info on current tracker state......
std::ostream & operator<<(std::ostream &Output, Tracker &RequestedTracker)
{
    // Show some general information about tracker...
    std::cout << "Tracking " << RequestedTracker.TrackingTable.size() 
              << " worms..." << std::endl;

    // Iterate through each worm in the tracker...
    for(unsigned int unWormIndex = 0;
        unWormIndex < RequestedTracker.TrackingTable.size();
      ++unWormIndex)
    {
        // Show some information about each worm...
        std::cout << *RequestedTracker.TrackingTable.at(unWormIndex) 
                  << std::endl;
    }

    // Return the stream...
    return Output;
}

