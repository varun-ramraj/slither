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
        // This worm is on the outermost edge of the frame, add it...
        if(IsOnOuterEdge(WormContour))
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
        
        // Update it with the new contour...
        BestMatch.Update(WormContour, *pGrayImage);
    }
    
}

// Add worm to tracker...
void Tracker::Add(CvContour const &WormContour)
{

}

// Advance frame...
void Tracker::AdvanceNextFrame(IplImage const &NewGrayImage)
{

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

// Does this worm's contour lie along the outer edge of the frame?
bool Tracker::IsOuterEdge(CvContour const &WormContour) const
{

}

// Could this contour be a worm, independent of what we know?
bool Tracker::IsPossibleWorm(CvContour const &MysteryContour) const
{

}

// The number of worms we are currently tracking...
unsigned int Tracker::Tracking() const
{

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

