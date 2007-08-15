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

// Default constructor...
Tracker::Tracker(IplImage const &GrayImage)
    : pStorage(cvCreateMemStorage(0)),
      pGrayImage(cvCloneImage(&GrayImage))
{
    // Allocatation of base storage failed...
    if(!pStorage)
        throw std::bad_alloc();
    
    // Allocation of image failed...
    if(!pGrayImage)
        throw std::bad_alloc();
        
    // Image must be a 8-bit, unsigned, grayscale...
    assert(pGrayImage->depth == IPL_DEPTH_8U);

    // Image must not have a region of interest set...
    assert(pGrayImage->roi == NULL);
}

// Acknowledge a worm contour...
void Tracker::Acknowledge(CvContour &WormContour)
{
    // This worm's contour doesn't overlap with any other's...
    if(GetRectanglesBeneath(WormContour.rect) == 0)
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

                /* TODO: Update rectangle as well? */

                // Update it with the new contour...
                BestMatch.Update(WormContour, *pGrayImage);
            }
        }
    }
    
    // This worm's contour overlaps with at least one other...
    else
    {
        // Find the best match...
        Worm &BestMatch = FindBestMatch(WormContour);

        /* TODO: Update rectangle as well? */

        // Update it with the new contour...
        BestMatch.Update(WormContour, *pGrayImage);
    }
    
}

// Add worm to tracker...
void Tracker::Add(CvContour const &WormContour)
{
    // Add new worm...
    TrackingTable.push_back(new Worm(WormContour, *pGrayImage));
}

// Advance frame...
void Tracker::AdvanceNextFrame(IplImage const &NewGrayImage)
{
    // Release the old image...
    cvReleaseImage(&pGrayImage);
    
    // Allocate and clone the new one...
    pGrayImage = cvCloneImage(&NewGrayImage);
    
        // Failed...
        if(!pGrayImage)
            throw std::bad_alloc();
}

// Find the best match of this contour, or NullWorm if none...
Worm &Tracker::FindBestMatch(CvContour &Key) const
{

}

// Get the nth worm, or null worm if no more...
Worm const &Tracker::GetWorm(unsigned int const unIndex) const
{

}

// How many underlying rectangles does given one rest upon?
unsigned int const Tracker::GetRectanglesBeneath(CvRect const &Rectangle) const
{

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
    // Cleanup the gray image...
    cvReleaseImage(&pGrayImage);
    
    // Cleanup base storage...
    cvReleaseMemStorage(&pStorage);
}

// Output some info on current tracker state......
std::ostream & operator<<(std::ostream &Output, Worm &RequestedTracker)
{
    // Stubbed. Return the stream...
    return Output;
}

