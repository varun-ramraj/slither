/*
  Name:         Worm.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Worm class...
*/

// Includes...
#include "Worm.h"
#include <new>
#include <cmath>
#include <cassert>

// Dummy default argument parameters...
unsigned int Worm::unDummy = 0;

// Output a point...
std::ostream & operator<<(std::ostream &Output, CvPoint Point)
{
    // Output
    Output << "(" << Point.x << ", " << Point.y << ")";

    // Return the stream...
    return Output;
}

// Output some info of what we know about this worm...
std::ostream & operator<<(std::ostream &Output, Worm &RequestedWorm)
{
    // Output attributes of note...
    Output << "Area:\t"     << RequestedWorm.dArea    << std::endl
           << "Centre:\t"   << RequestedWorm.Centre() << std::endl
           << "Length:\t"   << RequestedWorm.dLength  << std::endl
           << "Width:\t"    << RequestedWorm.dWidth   << std::endl
           << "Head:\t"     << RequestedWorm.Head()   << std::endl
           << "Tail:\t"     << RequestedWorm.Tail()   << std::endl;

    // Return the stream...
    return Output;
}

// Default constructor...
Worm::Worm()
    : pStorage(cvCreateMemStorage(0)),
      pContour(NULL),
      unUpdates(0),
      dArea(0.0f),
      GravitationalCentre(cvPoint(0, 0)),
      dLength(0.0f), 
      dWidth(0.0f),
      TerminalA(cvPoint(0, 0), 0),
      TerminalB(cvPoint(0, 0), 0)
{
    // Allocatation of base storage for contour and anything else we need failed...
    if(!pStorage)
        throw std::bad_alloc();
}

// Worm construction requires to just know it's contour and a bit of information about the image it rests on...
Worm::Worm(CvContour const &Contour, IplImage const &GrayImage)
    : pStorage(cvCreateMemStorage(0)),
      pContour(NULL),
      unUpdates(0),
      dArea(0.0f),
      GravitationalCentre(cvPoint(0, 0)),
      dLength(0.0f), 
      dWidth(0.0f),
      TerminalA(cvPoint(0, 0), 0),
      TerminalB(cvPoint(0, 0), 0)
{    
    // Allocatation of base storage for contour and anything else we need failed...
    if(!pStorage)
        throw std::bad_alloc();

    // Update the worm's metrics based on the contour...
    Discover(Contour, GrayImage);
}

// Adjust the distance of the second vertex by the given distance along the radial... 
inline void Worm::AdjustDirectedLineSegmentLength(LineSegment &A, 
                                                  double dLength) const
{
    // Create vector from directed line segment...
    CvPoint2D32f Vector = cvPoint2D32f(A.second.x - A.first.x, 
                                       A.second.y - A.first.y);
    
    // Compute angle of vector in degrees, then convert to radians...
    double const dThetaRadians = cvFastArctan(Vector.y, Vector.x) * 
                                    (Pi / 180.0f);
    
    // Adjust vector length to given...
    Vector.x = (dLength * cos(dThetaRadians));
    Vector.y = (dLength * sin(dThetaRadians));
    
    // Translate back again...
    A.second.x = A.first.x + Vector.x;
    A.second.y = A.first.y + Vector.y;
}

// Best guess of the area, considering everything we've seen thus far...
inline double const &Worm::Area() const
{
    // Return it...
    return dArea;
}

// Best guess of the worm's centre...
CvPoint const &Worm::Centre() const
{
    // Return it...
    return GravitationalCentre;
}

// Clip line against the image rectangle...
void Worm::ClipLineSegment(CvSize Size, LineSegment &A) const
{
    // Make a duplicate of the segment since Intel's clipping routine handles
    //  integral values only...
    CvPoint Start   = cvPointFrom32f(A.first);
    CvPoint End     = cvPointFrom32f(A.second);
    
    // Clip...
    cvClipLine(Size, &Start, &End);
    
    // To preserve double precision, use the original unclipped points where
    //  they are still in a valid portion of the plane...

        // Start was clipped, use the clipped value...
        if((A.first.x < 0.0f || A.first.x > Size.width) ||
           (A.first.y < 0.0f || A.first.y > Size.height))
            A.first = cvPointTo32f(Start);

        // End was clipped, use the clipped value...
        if((A.second.x < 0.0f || A.second.x > Size.width) ||
           (A.second.y < 0.0f || A.second.y > Size.height))
            A.second = cvPointTo32f(End);
}

// Is directed line segment Start->Second clockwise (> 0), counterclockwise 
//  (< 0), or collinear with respect to the directed line segment 
//  Start->First? θ(1)
inline int Worm::Direction(CvPoint2D32f const Start, 
                           CvPoint2D32f const First, 
                           CvPoint2D32f const Second) const
{
    // Calculate the cross product, but do it with both vectors translated back 
    //  to the origin to make it work...
    return (int) (((First.x - Start.x) * (Second.y - Start.y)) - 
                  ((Second.x - Start.x) * (First.y - Start.y)));
}

// Discover the worm's metrics based on its new contour... (area, length, 
//  width, et cetera)
inline void Worm::Discover(CvContour const &NewContour, 
                           IplImage const &GrayImage)
{
    // Image must be a 8-bit, unsigned, grayscale...
    assert(GrayImage.depth == IPL_DEPTH_8U);

    // Image must not have a region of interest set...
    assert(GrayImage.roi == NULL);

    // Remember that how many times we have updated, which we need for 
    //  calculating arithmetic means...
  ++unUpdates;

    // Forget the old contour if we have one yet. This works by restoring the 
    //  old contour sequence blocks to the base storage pool. This is θ(1) 
    //  running time, usually...
    if(pContour)
        cvClearSeq((CvSeq *) pContour);

    // Clone the new contour sequence into our storage pool...
    pContour = (CvContour *) cvCloneSeq((CvSeq *) &NewContour, pStorage);

        // For some reason the bounding rectangle in the header is skipped.
        //  Intel bug?
        pContour->rect = NewContour.rect;

    // Update the gravitational centre from this image...
    UpdateGravitationalCentre();

    // Update the approximate area from the area calculated in this image...
    UpdateArea(fabs(cvContourArea(pContour)));

    // Update the approximate length from the length calculated in *this* image.
    //  The length is about half the perimeter all the way around the worm...
    double const dLengthAtThisMoment = 
        cvArcLength(pContour, CV_WHOLE_SEQ, true) / 2.0;
    UpdateLength(dLengthAtThisMoment);

    // Find both ends... (head and tail)

        // Find an end, either will do... θ(n)
        unsigned int const unMysteryEndVertexIndex = 
            PinchShiftForAnEnd(GrayImage, Forwards);

        // Find the other end of the worm which must be approximately the 
        //  length of the worm away... O(n)
        /*unsigned int const unOtherMysteryEndVertexIndex = 
            FindVertexIndexByLength(unMysteryEndVertexIndex, 
                                    dLengthAtThisMoment);*/
        unsigned int const unOtherMysteryEndVertexIndex = 
            PinchShiftForAnEnd(GrayImage, Backwards);

        // Make a reasonably intelligent guess as to which end is which, based 
        //  only on *this* image alone...

            // The first end we found was probably the head...
            if(IsFirstHeadCloisterCheck(unMysteryEndVertexIndex, 
                                        unOtherMysteryEndVertexIndex, 
                                        GrayImage))
            {
                // Remember for next time where approximately it was found...
                UpdateHeadAndTail(unMysteryEndVertexIndex, 
                                  unOtherMysteryEndVertexIndex);
            }

            // Nope, it was the other way around...
            else
            {
                // Remember for next time where approximately it was found...
                UpdateHeadAndTail(unOtherMysteryEndVertexIndex, 
                                  unMysteryEndVertexIndex);
            }
}

// Calculate the distance between the midpoints of two segments... θ(1)
inline double Worm::DistanceBetweenLineSegments(LineSegment const &A, 
                                                LineSegment const &B) const
{
    // Just measure the length of the imaginary line segment joining both 
    //  segment's middles...
    return LengthOfLineSegment(
        LineSegment(cvPoint2D32f((A.second.x - A.first.x) / 2.0f, 
                                 (A.second.y - A.first.y) / 2.0f), 
                    cvPoint2D32f((B.second.x - B.first.x) / 2.0f,
                                 (B.second.y - B.first.y) / 2.0f)));
}

// Calculate the absolute distance between two points...
inline double Worm::DistanceBetweenTwoPoints(CvPoint const &First, 
                                             CvPoint const &Second) const
{
    // Return it...
    return cvSqrt(pow(Second.x - First.x, 2) + pow(Second.y - First.y, 2));
} 

// Calculate the absolute distance between two points...
inline double Worm::DistanceBetweenTwoPoints(CvPoint2D32f const &First, 
                                             CvPoint2D32f const &Second) const
{
    // Return it...
    return cvSqrt(pow(Second.x - First.x, 2) + pow(Second.y - First.y, 2));
} 

// Find the vertex on the contour the given length away, starting in increasing 
//  order... O(n)
inline unsigned int const Worm::FindVertexIndexByLength(
    unsigned int const &unStartVertexIndex, 
    double const &dPerimeterLength,
    unsigned int &unVerticesTraversed) const
{
    /* TODO: We can do much better than this right down to logarithmic time by 
             using a skip list some how. Deal with it later after we're sure 
             this one even works properly at all. */

    // Variables...
    register    double          dDistanceWalkedAccumulator  = 0.0f;
                unsigned int    unCurrentVertexIndex        = 0;
                unsigned int    unNextVertexIndex           = 0;

    // Reset vertices traversed...
    unVerticesTraversed = 0;

    // Keep walking along the contour in the requested direction until we've 
    //  gone far enough...
    for(unCurrentVertexIndex = unStartVertexIndex;
        dDistanceWalkedAccumulator < fabs(dPerimeterLength);
      ++unVerticesTraversed)
    {
        // If the requested length is positive, use the next vertex...
        if(dPerimeterLength > 0)
        {
            // Grab the next index...
            unNextVertexIndex = GetNextVertexIndex(unCurrentVertexIndex);
            
            // Remember how far we've walked...
            dDistanceWalkedAccumulator += 
                DistanceBetweenTwoPoints(GetVertex(unCurrentVertexIndex), 
                                         GetVertex(unNextVertexIndex));

            // Next index now becomes the current...
            unCurrentVertexIndex = unNextVertexIndex;
        }
        
        // Otherwise, we are iterating along the contour backwards...
        else
        {
            // Grab the previous index...
            unNextVertexIndex = GetPreviousVertexIndex(unCurrentVertexIndex);
            
            // Remember how far we've walked...
            dDistanceWalkedAccumulator += 
                DistanceBetweenTwoPoints(GetVertex(unCurrentVertexIndex), 
                                         GetVertex(unNextVertexIndex));
            
            // Next index now becomes the current...
            unCurrentVertexIndex = unNextVertexIndex;
        }
    }
    
    // We've gone far enough...
    return unCurrentVertexIndex;
}

// Generate orthogonal of unit length from middle of given line segment 
//  outwards... θ(1)
inline void Worm::GenerateOrthogonalToLineSegment(
    LineSegment const &A, LineSegment &Orthogonal) const
{
    // Start with the given line...
    Orthogonal = A;
    
    /* Shift the line segment's start to half way...
    Orthogonal.first.x = ((A.second.x - A.first.x) / 2.0f) + A.first.x;
    Orthogonal.first.y = ((A.second.y - A.first.y) / 2.0f) + A.first.y;*/
    
    // Pivot the second point 90 degrees about the first...
    RotatePointAboutAnother(Orthogonal.second, Orthogonal.first, 
                            Pi / 2.0f, Orthogonal.second);
}

/* Get the average brightness of the area within a contour...
inline double const Worm::GetAverageBrightness(CvContour const &Contour,
                                               IplImage const &GrayImage) const
{
    // Variables...
    IplImage   *pMaskImage          = NULL;
    IplImage   *pMutableGrayImage   = NULL;

    // Create a mask image from the contour...
    
        // Allocate...
        pMaskImage = cvCreateImage(cvGetSize(&GrayImage), IPL_DEPTH_8U, 1);

        // To save time, just clear the portion that could contain the mask...
        cvSetImageROI(pMaskImage, Contour.rect);
        cvZero(pMaskImage);
        cvResetImageROI(pMaskImage);

cvRectangle(const_cast<IplImage *>(&GrayImage), 
               cvPoint(Contour.rect.x, Contour.rect.y),
               cvPoint(Contour.rect.x + Contour.rect.width,
                       Contour.rect.y + Contour.rect.height),
               CV_RGB(255, 255, 255), 1, 8);

cvDrawContours(const_cast<IplImage *>(&GrayImage), 
               (CvSeq *) &Contour, CV_RGB(255, 255, 255), 
               CV_RGB(255, 255, 255), 0, CV_FILLED, 8);

        // Fill the contour's mask with all white...
        cvDrawContours(pMaskImage, (CvSeq *) &Contour, CV_RGB(255, 255, 255), 
                       CV_RGB(255, 255, 255), -1, CV_FILLED, 8);

    // Calculate the average brightness by using the contour mask like a cookie
    //  cutter over the original gray image...

        // We can speed up luma value summation by just dealing with the region
        //  we care about...
        
            // Set the region of interest for our mask...
            cvSetImageROI(pMaskImage, Contour.rect);
            
            // The original gray image is read only, so we must make our own...
            
                // Clone...
                pMutableGrayImage = cvCloneImage(&GrayImage);
                
                // Set region of interest...
                cvSetImageROI(pMutableGrayImage, Contour.rect);

        // Calculate the brightness...
        CvScalar const AverageBrightness = cvAvg(pMutableGrayImage, pMaskImage);
    
    // Cleanup...
    cvReleaseImage(&pMutableGrayImage);
    cvReleaseImage(&pMaskImage);

if(AverageBrightness.val[0] == 0.0f)
    std::cout << "Warning: Dead brightness code..." << std::endl;

    // Done...
    return AverageBrightness.val[0];
}*/

// Get the maximum brightness along a line...
inline double const Worm::GetLineMaximumBrightness(
    LineSegment const &A,
    IplImage const &GrayImage) const
{
    // Variables...
    CvLineIterator      LineIterator;
    int                 nPixelIndex         = 0;
    //register double     dTotalBrightness    = 0.0f;
    unsigned char       MaxBrightness       = 0x00;
    CvPoint             CurrentPoint        = cvPoint(0, 0);

    // Initialize pixel iterator...
    int const nPixels = cvInitLineIterator(&GrayImage, cvPointFrom32f(A.first), 
                                           cvPointFrom32f(A.second), 
                                           &LineIterator, 8, 0);

/*cvLine(const_cast<IplImage *>(&GrayImage), cvPointFrom32f(A.first), 
       cvPointFrom32f(A.second), CV_RGB(0xFF, 0xFF, 0xFF));*/

    // Scan each pixel, totaling as we go...
    for(nPixelIndex = 0; nPixelIndex < nPixels; ++nPixelIndex)
    {
        // Discard those points that are not on the vermiform contour...
        
            // Calculate where the current point falls on the image plane...

                // Calculate pixel offset into image data...
                unsigned int const unOffset = 
                    LineIterator.ptr - (unsigned char *)(GrayImage.imageData);

                // Y-coordinate is the ratio of image offset to the size of 
                //  aligned image row in bytes...
                CurrentPoint.y  = unOffset / GrayImage.widthStep;
                
                // X-coordinate is a bit more complicated, with the divisor
                //  being the size of a pixel. One byte for unsigned 8-bit 
                //  grayscale...
                CurrentPoint.x = 
                    (unOffset - CurrentPoint.y * GrayImage.widthStep) / 
                    (1 * sizeof(unsigned char));
        
            // Point does not lie on the vermiform...
            if(cvPointPolygonTest(pContour, cvPointTo32f(CurrentPoint), 0) < 0)
            {
                // Discard and seek to next point...
                CV_NEXT_LINE_POINT(LineIterator);
                continue;
            }

        // We want the brightest pixel we can find...
        MaxBrightness = std::max(MaxBrightness, LineIterator.ptr[0]);

/*cvLine(const_cast<IplImage *>(&GrayImage), CurrentPoint, CurrentPoint,
       CV_RGB(0xFF, 0xFF, 0xFF));*/

        // Seek to next point...
        CV_NEXT_LINE_POINT(LineIterator);
    }

    // Return the average brightness...
    return (double) MaxBrightness;
}

// Get the index of the next vertex in the contour after the given index, 
//  O(1) average...
inline unsigned int Worm::GetNextVertexIndex(unsigned int const &unVertexIndex) 
    const
{
    // This should never happen, so we make sure...
    assert((int) unVertexIndex < pContour->total);
    
    // The next index is just one more than the given - unless at the end where 
    //  it jumps back to the beginning...
    return ((int) unVertexIndex + 1 < pContour->total) ? (unVertexIndex + 1) 
                                                       : 0;
}

// Get the total surrounding brightness of a central point ...
inline double const Worm::GetSurroundingBrightness(
    CvPoint Centre, 
    IplImage const &GrayImage) const
{
    // Constants...
    unsigned int const  unRadius            = 10;

    // Create a cross over the given centre point...
    
        // Form the vertical component...
        LineSegment const
            Vertical(cvPoint2D32f(Centre.x, Centre.y - unRadius),
                     cvPoint2D32f(Centre.x, Centre.y + unRadius));
        
        // Form the horizontal component...
        LineSegment const 
            Horizontal(cvPoint2D32f(Centre.x - unRadius, Centre.y),
                       cvPoint2D32f(Centre.x + unRadius, Centre.y));

    // Return the maximum brightness on the cross...
    return GetLineMaximumBrightness(Vertical, GrayImage) +
           GetLineMaximumBrightness(Horizontal, GrayImage);
}

// Get the actual vertex of the given vertex index in the contour, O(1) 
//  average...
inline CvPoint &Worm::GetVertex(unsigned int const &unVertexIndex) const
{
    // This should never happen, so we make sure...
    assert((int) unVertexIndex < pContour->total);
    
    // Find the actual vertex and return it...
    return *((CvPoint *) cvGetSeqElem((CvSeq *) pContour, unVertexIndex));
}

// Get the index of the previous vertex in the contour after the given index, 
//  O(1) average...
inline unsigned int Worm::GetPreviousVertexIndex(
    unsigned int const &unVertexIndex) const
{
    // This should never happen...
    assert((int) unVertexIndex < pContour->total);
    
    // The previous index is just one less than the given - unless at the start 
    //  where it jumps back to the end... 
    return (unVertexIndex == 0) ? (pContour->total - 1) : (unVertexIndex - 1);
}

// Best guess as to the head's position at this moment in time, since it 
//  changes...
CvPoint const &Worm::Head() const
{
    // We'll make a reasonably informed guess by returning the more likely of
    //  the two terminal ends that received the higher head hits. In the
    //  boundary condition that they've both received the same hits, use the
    //  first...
    
        // Terminal end A...
        if(TerminalA.unHeadScore >= TerminalB.unHeadScore)
            return TerminalA.LastSeenLocus;
        
        // Terminal end B...
        else
            return TerminalB.LastSeenLocus;
}

// Can the collinear point be found on the line segment? θ(1)
inline bool Worm::IsCollinearPointOnLineSegment(
    LineSegment const &A, 
    CvPoint2D32f const &CollinearPoint) const
{
    // Found...
    if(((std::min(A.first.x, A.second.x) <= CollinearPoint.x) && 
        (CollinearPoint.x <= std::max(A.first.x, A.second.x))) &&
       ((std::min(A.first.y, A.second.y) <= CollinearPoint.y) && 
        (CollinearPoint.y <= std::max(A.first.y, A.second.y))))
        return true;

    // Not found....
    else
        return false;
}                                                   

// Given only the two vertex indices, *this* image, and assuming they are 
//  opposite ends of the worm, would the first of the two most likely be the 
//  head if we had but this image alone to consider?
inline bool Worm::IsFirstHeadCloisterCheck(
    unsigned int const &unCandidateHeadVertexIndex,
    unsigned int const &unCandidateTailVertexIndex,
    IplImage const     &GrayImage) const
{
    // Variables...
    CvPoint const  &CandidateHeadStart  = GetVertex(unCandidateHeadVertexIndex);
    CvPoint const  &CandidateTailStart  = GetVertex(unCandidateTailVertexIndex);
/*    unsigned int    unTempIndexOne      = 0;
    unsigned int    unTempIndexTwo      = 0;
    CvPoint         EndPoint            = cvPoint(0, 0);*/

    // Compare the candidate head and tail brightness, as requested...
    return (GetSurroundingBrightness(CandidateHeadStart, GrayImage) >
            GetSurroundingBrightness(CandidateTailStart, GrayImage));

    /* Find the average brightness of the candidate head...

        // Walk a 1/6th the length of the vermiform one way from the head...
        unTempIndexOne = FindVertexIndexByLength(unCandidateHeadVertexIndex, 
                                                 Length() / -2.0f);

        // Walk a 1/6th the length of the vermiform the other way from head...
        unTempIndexTwo = FindVertexIndexByLength(unCandidateHeadVertexIndex, 
                                                 Length() / 2.0f);

        // The midpoint between the two sides we will use to form a line from
        //  the head out hopefully into the body...
        EndPoint.x = int((GetVertex(unTempIndexTwo).x - 
                          GetVertex(unTempIndexOne).x) / 2.0f)
                        + GetVertex(unTempIndexOne).x;
        EndPoint.y = int((GetVertex(unTempIndexTwo).y - 
                          GetVertex(unTempIndexOne).y) / 2.0f)
                        + GetVertex(unTempIndexOne).y;

        // Adjust line length...
        LineSegment CandidateHeadLineSegment(cvPointTo32f(CandidateHeadStart), 
                                             cvPointTo32f(EndPoint));
        AdjustDirectedLineSegmentLength(CandidateHeadLineSegment, 10.0f);

        // Calculate average brightness...
        double const dCandidateHeadBrightness = 
            GetLineBrightness(CandidateHeadLineSegment, GrayImage);

    // Find the average brightness of the candidate tail...

        // Walk a 1/6th the length of the vermiform one way from the tail...
        unTempIndexOne = FindVertexIndexByLength(unCandidateTailVertexIndex, 
                                                 Length() / -2.0f);

        // Walk a 1/6th the length of the vermiform the other way from tail...
        unTempIndexTwo = 
            FindVertexIndexByLength(unCandidateTailVertexIndex, 
                                    Length() / 2.0f);

        // The midpoint between the two sides we will use to form a line from
        //  the tail out hopefully into the body...
        EndPoint.x = int((GetVertex(unTempIndexTwo).x - 
                          GetVertex(unTempIndexOne).x) / 2.0f)
                        + GetVertex(unTempIndexOne).x;
        EndPoint.y = int((GetVertex(unTempIndexTwo).y - 
                          GetVertex(unTempIndexOne).y) / 2.0f)
                        + GetVertex(unTempIndexOne).y;

        // Adjust line length...
        LineSegment CandidateTailLineSegment(cvPointTo32f(CandidateTailStart), 
                                             cvPointTo32f(EndPoint));
        AdjustDirectedLineSegmentLength(CandidateTailLineSegment, 10.0f);

        // Calculate average brightness...
        double const dCandidateTailBrightness = 
            GetLineBrightness(CandidateTailLineSegment, GrayImage);

    // Compare the candidate head and tail brightness, as requested...
    return (dCandidateHeadBrightness > dCandidateTailBrightness);*/
}

/* Given only the two vertex indices, *this* image, and assuming they are 
//  opposite ends of the worm, would the first of the two most likely be the 
//  head if we had but this image alone to consider?
inline bool Worm::IsFirstProbablyHeadViaCloisterCheck(
    unsigned int const &unCandidateHeadVertexIndex,
    unsigned int const &unCandidateTailVertexIndex,
    IplImage const     &GrayImage) const
{
    // Variables...
    unsigned int    unStartVertexIndex  = 0;
    unsigned int    unVerticesTraversed = 0;

    // Create a contour around the candidate head...

//We are building the contour wrong here. That's why luma calculation is wrong.

        // Allocate...
        CvContour &CandidateHeadContour = 
            *((CvContour *) cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvContour), 
                                        sizeof(CvPoint), pStorage));

        // Enclose 1/8th of the worm's perimeter...
        
            // Backtrack an 1/8th from the candidate head...
            unStartVertexIndex  = 
                FindVertexIndexByLength(
                    unCandidateHeadVertexIndex, 
                    -1.0f * Length() / 8.0f);

            // Now to encompass a full 1/8th, we go 1/8th forward twice...
            FindVertexIndexByLength(
                unStartVertexIndex, 
                Length() / 4.0f, 
                unVerticesTraversed);

        // Assemble the vertices that enclose the candidate head region...
        cvSeqPushMulti((CvSeq *) &CandidateHeadContour, 
                       &GetVertex(unStartVertexIndex), 
                       unVerticesTraversed,
                       CV_BACK);

        // Store the bounding rectangle...
        cvBoundingRect(&CandidateHeadContour, 1);

    // Create a contour around the candidate tail...

        // Allocate...
        CvContour &CandidateTailContour = 
            *((CvContour *) cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvContour), 
                                        sizeof(CvPoint), pStorage));

        // Enclose 1/8th of the worm's perimeter...
        
            // Backtrack an 1/8th from the candidate head...
            unStartVertexIndex  = FindVertexIndexByLength(
                unCandidateTailVertexIndex, 
                -1.0f * Length() / 8.0f);
            
            // Now to encompass a full 1/8th, we go 1/8th forward twice...
            FindVertexIndexByLength(
                unStartVertexIndex, 
                Length() / 4.0f, 
                unVerticesTraversed);

        // Assemble the vertices that enclose the candidate tail region...
        cvSeqPushMulti((CvSeq *) &CandidateTailContour, 
                       &GetVertex(unStartVertexIndex), 
                       unVerticesTraversed,
                       CV_BACK);

        // Store the bounding rectangle...
        cvBoundingRect(&CandidateTailContour, 1);

    // Calculate the average brightness of the candidate head blob...
    double const dCandidateHeadBrightness = 
        GetAverageBrightness(CandidateHeadContour, GrayImage);

double dTemp = GetAverageBrightness(*pContour, GrayImage);
dTemp = dTemp;

    // Calculate the average brightness of the candidate tail blob...
    double const dCandidateTailBrightness =
        GetAverageBrightness(CandidateTailContour, GrayImage);

    // Cleanup...
    cvClearSeq((CvSeq *) &CandidateHeadContour);
    cvClearSeq((CvSeq *) &CandidateTailContour);

    // If the candidate head truly is the head, brighter than the tail we shall
    //  find it to be...
    return (dCandidateHeadBrightness > dCandidateTailBrightness);
}*/

// Check if two line segments intersect... θ(1)
inline bool Worm::IsLineSegmentsIntersect(LineSegment const &A, 
                                          LineSegment const &B) const
{
    // Calculate the relative orientation of each endpoint with respect to the 
    //  other segment...
    int nDirection1 = Direction(B.first, B.second, A.first);
    int nDirection2 = Direction(B.first, B.second, A.second);
    int nDirection3 = Direction(A.first, A.second, B.first);
    int nDirection4 = Direction(A.first, A.second, B.second);
    
    /* 
       See pp.934-938 of Cormen et al, 2003, for the verbose explanation of how 
       this works. In short, it works by checking for the straddling of line 
       segments...
    */
        
    // Intersects...
    if(((nDirection1 > 0 && nDirection2 < 0) || 
        (nDirection1 < 0 && nDirection2 > 0)) &&
       ((nDirection3 > 0 && nDirection4 < 0) || 
        (nDirection3 < 0 && nDirection4 > 0)))
        return true;
    
    // Intersects...
    else if((nDirection1 == 0) && IsCollinearPointOnLineSegment(
                                    LineSegment(B.first, B.second), A.first))
        return true;
    
    // Intersects...
    else if((nDirection2 == 0) && IsCollinearPointOnLineSegment(
                                    LineSegment(B.first, B.second), A.second))
        return true;

    // Intersects...
    else if((nDirection3 == 0) && IsCollinearPointOnLineSegment(
                                    LineSegment(A.first, A.second), B.first))
        return true;
        
    // Intersects...
    else if((nDirection4 == 0) && IsCollinearPointOnLineSegment(
                                    LineSegment(A.first, A.second), B.second))
        return true;
    
    // The necessary and sufficient condition for two line segments to 
    //  intersect has not been satisfied...
    else
        return false;
}

// Best guess of the length from head to tail, considering everything we've 
//  seen thus far...
inline double const &Worm::Length() const
{
    // Return it...
    return dLength;
}

// Calculate the length of a line segment... θ(1)
inline double Worm::LengthOfLineSegment(LineSegment const &A) const
{
    // Calculate...
    return cvSqrt(((A.second.x - A.first.x) * (A.second.x - A.first.x)) + 
                  ((A.second.y - A.first.y) * (A.second.y - A.first.y)));
}

// Candidates are not similar enough... (see below)
inline bool Worm::operator<(Worm &RightWorm) const
{
    /* TODO: Implement this. */
    return false;
}

// Candidate meet the minimum required similarity...
inline bool Worm::operator==(Worm &RightWorm) const
{
    // Hmm, we have no information about our own worm just yet...
    if(unUpdates == 0)
        return false;

    
}

// Find the vertex index in the contour sequence that contains either end of 
//  the worm, and update width while we're at it... θ(n)
inline unsigned int Worm::PinchShiftForAnEnd(
    IplImage const &GrayImage, 
    IterationDirection Direction)
{
    // Variables...
    unsigned int const  unStartVertexIndex                  = 0;
    unsigned int        unCurrentOppositeVertexIndex        = 0;
    unsigned int        unClosestOppositeVertexIndexFound   = 0;
    double              dClosestOppositeVertexDistanceFound = Infinity;
    LineSegment         StartingLineSegment(
                            cvPoint2D32f(0.0f, 0.0f), 
                            cvPoint2D32f(0.0f, 0.0f));
    LineSegment         OrthogonalLineSegment(
                            cvPoint2D32f(0.0f, 0.0f), 
                            cvPoint2D32f(0.0f, 0.0f));
    LineSegment         CorrectedOrthogonal(
                            cvPoint2D32f(0.0f, 0.0f), 
                            cvPoint2D32f(0.0f, 0.0f));;

    // We begin by forming a line segment from an arbitrary point on the 
    //  contour to its neighbour...
    StartingLineSegment.first   = 
        cvPointTo32f(GetVertex(unStartVertexIndex));
    StartingLineSegment.second  = 
        cvPointTo32f(GetVertex(
            FindVertexIndexByLength(unStartVertexIndex, 5.0f)));

    // Make the starting line segment now a tangent to the curve...
    AdjustDirectedLineSegmentLength(StartingLineSegment, 30.0f);

    // Generate an orthogonal for the starting line segment...
    GenerateOrthogonalToLineSegment(StartingLineSegment, OrthogonalLineSegment);

    // Ensure that orthogonal is directed into the worm, rather than outwards...
    CorrectedOrthogonal = OrthogonalLineSegment;
    for(unsigned int unOrthogonalCorrection = 1;
        cvPointPolygonTest(pContour, CorrectedOrthogonal.second, 0) <= 0.0f;
      ++unOrthogonalCorrection)
    {
        // Preserve precision by starting with the original orthogonal...
        CorrectedOrthogonal = OrthogonalLineSegment;

        // Compute new length to try...
        double dNewLength = (unOrthogonalCorrection / 20.0f);
        
        // Alternate its direction for every other correction...
        if(unOrthogonalCorrection % 2 == 0)
            dNewLength *= -1.0f;

        // Alternate between pointing in either direction...
        AdjustDirectedLineSegmentLength(CorrectedOrthogonal, dNewLength);
    }

    // Use corrected orthogonal now...
    OrthogonalLineSegment = CorrectedOrthogonal;

        // The orthogonal directed segment's other side should always be within 
        //  the worm...
        assert(cvPointPolygonTest(pContour, OrthogonalLineSegment.second, 0) 
                > 0.0f);

    // Extend the directed orthogonal line segment out very far and clip to the 
    //  very edge of the image...
    AdjustDirectedLineSegmentLength(OrthogonalLineSegment, 10000.0f);
    ClipLineSegment(cvGetSize(&GrayImage), OrthogonalLineSegment);

/*cvLine(const_cast<IplImage *>(&GrayImage), cvPointFrom32f(OrthogonalLineSegment.first), 
       cvPointFrom32f(OrthogonalLineSegment.second),
       CV_RGB(0xFF, 0xFF, 0xFF));*/

    // Now find the closest line segment that this orthogonal, which serves us
    //  as a guide, pierces on the other side. Although the worm may be in some 
    //  peculiar shape, it will not matter since the first segment pierced must
    //  be somewhere on the other side, though not necessarily directly 
    //  opposite...
    
        // Start searching beginning with the next neighbour of our starting 
        //  segment...
        unCurrentOppositeVertexIndex = GetNextVertexIndex(unStartVertexIndex);
        
        // Now go around the creature, looking for those segments that the 
        //  above pierces...
        while(unStartVertexIndex != 
              GetNextVertexIndex(unCurrentOppositeVertexIndex))
        {
            // The line segment we are going to test...
            LineSegment CandidateLineSegment(
                cvPointTo32f(GetVertex(unCurrentOppositeVertexIndex)), 
                cvPointTo32f(GetVertex(
                        GetNextVertexIndex(unCurrentOppositeVertexIndex))));
            
            // Ah ha! We have found a segment that intersects the orthogonal...
            if(IsLineSegmentsIntersect(OrthogonalLineSegment, 
                                       CandidateLineSegment))
            {
                // How far away were they?
                double const dDistanceBetweenMiddleOfLineSegments = 
                    DistanceBetweenLineSegments(StartingLineSegment, 
                                                CandidateLineSegment);
                
                // Was distance closer than anything encountered thus far?
                if(dDistanceBetweenMiddleOfLineSegments < 
                   dClosestOppositeVertexDistanceFound)
                {
                    // Make a note of where it was and how far away it was...
                    unClosestOppositeVertexIndexFound   = 
                        unCurrentOppositeVertexIndex;
                    dClosestOppositeVertexDistanceFound = 
                        dDistanceBetweenMiddleOfLineSegments;
                }
            }
            
            // Let's move to the next segment and try that one...
            unCurrentOppositeVertexIndex = 
                GetNextVertexIndex(unCurrentOppositeVertexIndex);
        }

    // We now have both the start and opposite side vertex of the worm. This is
    //  all we need now for the shifting...
    unsigned int unVertexIndexSideA = unStartVertexIndex;
    unsigned int unVertexIndexSideB = unClosestOppositeVertexIndexFound;

/*cvLine(const_cast<IplImage *>(&GrayImage), 
       GetVertex(unVertexIndexSideA),
       GetVertex(unVertexIndexSideB),
       CV_RGB(0xFF, 0xFF, 0xFF));*/
       
    // Pinch-Shift doesn't always pick the best two pinch vertices. However, 
    // the line segment formed between the two will probably have an upper 
    // bound of the worm's actual width. Having said that, while we're at it,
    // update the vermiform width from the width of the pinch line segment...

        // Calculate...
        double const &dWidthAtThisMoment = 
            DistanceBetweenTwoPoints(GetVertex(unVertexIndexSideA),
                                     GetVertex(unVertexIndexSideB));

        // Update...
        UpdateWidth(dWidthAtThisMoment);

    // Keep shifting the two points along the worm's body. When the two 
    //  vertices finally coalesce into one (they have the same coordinates), we 
    //  have found an end... (hopefully)
    while(unVertexIndexSideA != unVertexIndexSideB)
    {
        // Variables...
        unsigned int    unShiftedVertexIndexSideA   = 0;
        unsigned int    unShiftedVertexIndexSideB   = 0;

        // Depending on which direction we were requested to iterate, we need to
        //  decide what constitutes the "next" vertex on either side...
            
            // Iterating forwards...
            if(Direction == Forwards)
            {
                // Remember what the next vertex on side A is...
                unShiftedVertexIndexSideA = 
                    GetPreviousVertexIndex(unVertexIndexSideA);
                
                // Remember what the next vertex on side B is...
                unShiftedVertexIndexSideB = 
                    GetNextVertexIndex(unVertexIndexSideB);
            }
            
            // Iterating backwards...
            else
            {
                // Remember what the next vertex on side A is...
                unShiftedVertexIndexSideA = 
                    GetNextVertexIndex(unVertexIndexSideA);
                
                // Remember what the next vertex on side B is...
                unShiftedVertexIndexSideB = 
                    GetPreviousVertexIndex(unVertexIndexSideB);            
            }

        // Since the vertex density per area of space may differ on either side
        //  of the worm, we cannot simply shuffle both sides together at the
        //  same rate. To solve this problem, assuming one of the sides has to
        //  move, move the side that keeps the closure distance between the two
        //  minimized at each iteration...
        
            // If we shift vertex of side A, how far apart would the two be?
            double const dDistanceBetweenIfShiftA = 
                DistanceBetweenTwoPoints(
                    GetVertex(unShiftedVertexIndexSideA),
                    GetVertex(unVertexIndexSideB));
                                                                           
            // If we shift vertex of side B, how far apart would the two be?
            double const dDistanceBetweenIfShiftB = 
                DistanceBetweenTwoPoints(
                    GetVertex(unVertexIndexSideA),
                    GetVertex(unShiftedVertexIndexSideB));
        
            // Shifting side A is the best choice to make, so do it...
            if(dDistanceBetweenIfShiftA <= dDistanceBetweenIfShiftB)
                unVertexIndexSideA = unShiftedVertexIndexSideA;
        
            // Shifting side B is the best choice to make, so do it...
            else
                unVertexIndexSideB = unShiftedVertexIndexSideB;
    }

    // The index of the vertex of the head / tail is either vertex, since they 
    //  converged...
    return unVertexIndexSideA;
}

// Rotate a line segment about a point counterclockwise by an angle...
inline void Worm::RotateLineSegmentAboutPoint(LineSegment &LineToRotate, 
                                              CvPoint2D32f const &Origin, 
                                              double const &dRadians) const
{
    // Variables...
    CvPoint2D32f NewPoint = {0.0f, 0.0f};
    
    // Apply rotation about the specified origin for the first coordinate...
    LineToRotate.first  = RotatePointAboutAnother(LineToRotate.first, Origin, 
                                                  dRadians, NewPoint);
    
    // Apply rotation about the specified origin for the second coordinate...
    LineToRotate.second = RotatePointAboutAnother(LineToRotate.second, Origin, 
                                                  dRadians, NewPoint);
}

// Rotate a point around another to be used as the origin...
inline CvPoint2D32f &Worm::RotatePointAboutAnother(
                                CvPoint2D32f const &OldPointToRotate, 
                                CvPoint2D32f const &Origin, 
                                double const &dRadians, 
                                CvPoint2D32f &NewPoint) const
{
    /* This is the decomposed form of the combined linear transformation that 
       translates back to origin, rotates about the origin, then translates 
       back again to the starting point, built from this transformation...
      
            | 1  0  r_x |     | cos(θ)  -sin(θ)   0 |     | 1  0 -r_x |   | x |
            | 0  1  r_y |  *  | sin(θ)   cos(θ)   0 |  *  | 0  1 -r_y | * | y |
            | 0  0  1   |     |    0        0     1 |     | 0  0   1  |   | 1 |
            
                (3)                    (2)                    (1)
            
            (1) First translate coordinate system back to real origin.
            (2) Rotate about the real origin.
            (3) Restore coordinate system back.
            
            Note: Transforms are applied in reverse order, like a stack.
    */

        // Variables...
        CvPoint2D32f TempPoint;

        // Calculate new x-coordinate... 
        TempPoint.x = ((cos(dRadians) * OldPointToRotate.x) -
                       (sin(dRadians) * OldPointToRotate.y) +
                       (Origin.x * (1 - cos(dRadians))) +
                       (Origin.y * sin(dRadians)));
               
        // Calculate new y-coordinate...
        TempPoint.y = ((sin(dRadians) * OldPointToRotate.x) +
                       (cos(dRadians) * OldPointToRotate.y) +
                       (Origin.y * (1 - cos(dRadians))) -
                       (Origin.x * sin(dRadians)));

    // Done...
    NewPoint = TempPoint;
    return NewPoint;
}

// Best guess as to the tail's position at this moment in time, since it 
//  changes...
CvPoint const &Worm::Tail() const
{
    // We'll make a reasonably informed guess by returning the more likely of
    //  the two terminal ends that received the lesser score. In the
    //  boundary condition that they've both received the same hits, use the
    //  second...
    
        // Terminal end B...
        if(TerminalA.unHeadScore >= TerminalB.unHeadScore)
            return TerminalB.LastSeenLocus;
        
        // Terminal end A...
        else
            return TerminalA.LastSeenLocus;
}

// Update the approximate area, based on the value at this moment in time. This 
//  will help us make a more informed answer when asked via Area() for the size. 
//  θ(1) space and time...
inline void Worm::UpdateArea(double const &dAreaAtThisMoment)
{
    // Not all contours are created equal. However, each contour always 
    //  encompassing less than or equal to the entire vermiform. It is best 
    //  then to forget averages and just store the greatest we find then...
    dArea = std::max(dArea, dAreaAtThisMoment);
}

// Update the gravitational centre from this image...
inline void Worm::UpdateGravitationalCentre()
{
    // Variables...
    CvMoments   CurrentMoment;

    // Calculate all moments of the contour...
    cvMoments(pContour, &CurrentMoment);

    // Extract the centre of gravity...
    GravitationalCentre.x = int(CurrentMoment.m10 / CurrentMoment.m00);
    GravitationalCentre.y = int(CurrentMoment.m01 / CurrentMoment.m00);
}

// Update the approximate head and tail position, based on the value at this 
//  moment in time. This will help us make a more informed answer when asked 
//  via Head() or Tail() for the actual coordinates θ(1) space and time...
inline void Worm::UpdateHeadAndTail(unsigned int const &unHeadVertexIndex,
                                    unsigned int const &unTailVertexIndex)
{
    // Get the location of the supposed head and tail in this frame...
    CvPoint    &CurrentHeadVertex = GetVertex(unHeadVertexIndex);
    CvPoint    &CurrentTailVertex = GetVertex(unTailVertexIndex);

    // Either we have no previous data to compare by, and so we assume initial
    //  data to be correct for starting, or, we have data already. In the latter
    //  case, the head is closer to terminal end A than B in this frame...
    if((unUpdates <= 1) || 
       (DistanceBetweenTwoPoints(CurrentHeadVertex, TerminalA.LastSeenLocus) <
        DistanceBetweenTwoPoints(CurrentHeadVertex, TerminalB.LastSeenLocus)))
    {
        // Make a note of where it was right now for next time...
        TerminalA.LastSeenLocus = CurrentHeadVertex;
        
        // Also note that terminal A gets one more point for looking like the
        //  head in this frame...
      ++TerminalA.unHeadScore;
        
        // Assume the tail was the other terminal end...
        TerminalB.LastSeenLocus = CurrentTailVertex;
    }

    // Head is closer to terminal end B than A in this frame...
    else
    {
        // Make a note of where it was right now for next time...
        TerminalB.LastSeenLocus = CurrentHeadVertex;
        
        // Also note that terminal B gets one more point for looking like the
        //  head in this frame...
      ++TerminalB.unHeadScore;
      
        // Assume the tail was the other terminal end...
        TerminalA.LastSeenLocus = CurrentTailVertex;
    }
}

// Update the approximate length, based on the value at this moment in time. 
//  This will help us make a more informed answer when asked via Length() for
//  the length. θ(1) space and time...
inline void Worm::UpdateLength(double const &dLengthAtThisMoment)
{
    // Store the new arithmetic mean in constant space. Just multiply your old
    //  average by n, add x_{n+1}, and then divide the whole thing by n+1...
    dLength = ((dLength * unUpdates) + dLengthAtThisMoment) / (unUpdates + 1);
}

// Update the approximate width, based on the value at this moment in time. 
//  This will help us make a more informed answer when asked via Width() for
//  the width. θ(1) space and time...
inline void Worm::UpdateWidth(double const &dWidthAtThisMoment)
{
    // Pinch-Shift doesn't always pick the best two pinch vertices. However, 
    // the line segment formed between the two will probably have an upper 
    // bound of the worm's actual width...
    dWidth = std::max(dWidth, dWidthAtThisMoment);
}

// Best guess of the area, considering everything we've seen thus far...
inline double const &Worm::Width() const
{
    // Return it...
    return dWidth;
}
    
// Deconstructor...
Worm::~Worm()
{
    // Deallocate storage pool...
    cvReleaseMemStorage(&pStorage);
}

