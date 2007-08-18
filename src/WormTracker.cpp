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
    : nClippingRegionThickness(80),
      ClippingRegion(cvRect(0, 0, 0, 0)),
      pStorage(cvCreateMemStorage(0)),
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
void WormTracker::Acknowledge(CvContour &WormContour)
{
    // We cannot do anything without at least the gray image...
    assert(pGrayImage);

    // Frame advancer was suppose to check for this already...
    assert(IsPossibleWorm(WormContour));

    // This worm's contour doesn't overlap with any other's...
    if(CountRectanglesIntersected(WormContour.rect) == 0)
    {
/*
    TODO: Forget this outer edge stuff and just check worm contour in 
          IsPossibleWorm() for vertices on image exterior.
*/

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
    
    // Prepare clipping region...
    ClippingRegion.x        = nClippingRegionThickness;
    ClippingRegion.y        = nClippingRegionThickness;
    ClippingRegion.width    = ImageSize.width - (2 * nClippingRegionThickness);
    ClippingRegion.height   = ImageSize.height - (2 * nClippingRegionThickness);
    
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

        // Acknowledge...
        Acknowledge(*pCurrentContour);
    }
    
    // Show the clipping region...
    cvRectangle(pThinkingImage, 
                cvPoint(ClippingRegion.x, ClippingRegion.y),
                cvPoint(ClippingRegion.x + ClippingRegion.width,
                        ClippingRegion.y + ClippingRegion.height),
                CV_RGB(0x00, 0xff, 0x00));
                
    
    // Show some information on each worm contour...
    for(unsigned int unWormIndex = 0; unWormIndex < TrackingTable.size();
      ++unWormIndex)
    {
        // Get the current worm...
        Worm const &CurrentWorm = GetWorm(unWormIndex);
        
        // This should still be within bounds...
        assert(&CurrentWorm != &NullWorm);

        // Draw the contours onto the thinking image...
        cvDrawContours(pThinkingImage, (CvSeq *) &CurrentWorm.Contour(),
                       CV_RGB(0x00, 0x00, 0xfe), CV_RGB(0x00, 0x00, 0xfe), 0, 
                       1);

        // Show some information about the worm on the thinking image...
        AddThinkingLabel("head", CurrentWorm.Head());
        std::ostringstream ssCentre;
        ssCentre << "(worm " << unWormIndex << ", updated " 
                 << CurrentWorm.Updates() << ")";
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

// Find the best match of this contour...
Worm &WormTracker::FindBestMatch(CvContour &WormContour) const
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
            cvSqrt(pow(double(WormCentre.x) - pCurrentWorm->Centre().x, 2) + 
                   pow(double(WormCentre.y) - pCurrentWorm->Centre().y, 2));

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
IplImage const *WormTracker::GetThinkingImage() const
{
    // Return it...
    return pThinkingImage;
}

// Get the nth worm, or null worm if no more...
Worm const &WormTracker::GetWorm(unsigned int const unIndex) const
{
    // Check bounds...
    if(unIndex + 1 > TrackingTable.size())
        return NullWorm;

    // Return worm...
    return *TrackingTable.at(unIndex);
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

// Does this worm's contour lie within the outer edge of the frame?
bool WormTracker::IsWithinOuterFrameEdge(CvContour const &WormContour) const
{
    // Check our assumptions...
    assert((pGrayImage->height - nClippingRegionThickness) > 0);
    assert((pGrayImage->width  - nClippingRegionThickness) > 0);

    // Bounding rectangle of worm...
    CvRect const &WormRectangle = WormContour.rect;

    // Within left border...
    if((WormRectangle.x + WormRectangle.width) < nClippingRegionThickness)
        return true;

    // Within top border...
    else if((WormRectangle.y + WormRectangle.height) < nClippingRegionThickness)
        return true;

    // Within right border...
    else if(WormRectangle.x > (pGrayImage->width - nClippingRegionThickness))
        return true;

    // Within bottom border...
    else if(WormRectangle.y > (pGrayImage->height - nClippingRegionThickness))
        return true;
        
    // Not within any of the borders...
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
        
    // Meets worm minima...
    return true;
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

