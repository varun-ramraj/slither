/*
  Name:         SlitherMath.cpp (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  General Slither related math routines...
*/

// Multiple include protection...
#ifndef _SLITHERMATH_H_
#define _SLITHERMATH_H_

// Includes...

    // OpenCV types...
    // $VR$: Adding necessary headers for OpenCV 4
    #include <opencv2/opencv.hpp>
    #include <opencv2/highgui/highgui.hpp>
    #include <opencv2/highgui/highgui_c.h>
    #include <opencv2/imgcodecs.hpp>
    #include <opencv2/imgproc/imgproc.hpp>
    #include <opencv2/imgproc/imgproc_c.h>
    #include <opencv2/core/core.hpp>
    #include <opencv2/videoio/videoio_c.h>
    //#include <opencv2/imgcodecs/imgcodecs_c.h> $VR$: 2020/06/10 - deprecated
    #include <opencv2/imgcodecs/legacy/constants_c.h>
    
    // STL pair...
    #include <utility>

// Slither math routines...
namespace SlitherMath
{
    // Constants...

        // The value of π...
        double const Pi         = 3.1415926535897932384626433832795f;

        // Infinity... (kind of)
        double const Infinity   = FLT_MAX;

    // Types...

        // Line segment...
        typedef std::pair<CvPoint2D32f, CvPoint2D32f> LineSegment;

    // Functions. Mostly computational geometry related...

        // Adjust the distance of the second vertex by the given distance along
        //  the radial... 
        void AdjustDirectedLineSegmentLength(
            LineSegment &A, double const dLength);
        
        // Clip line against the image rectangle...
        void ClipLineSegment(CvSize const Size, LineSegment &A);
        
        // Calculate the distance between the midpoints of two segments... θ(1)
        double DistanceBetweenLineSegments(
            LineSegment const &A, LineSegment const &B);

        // Calculate the absolute distance between two points...
        double DistanceBetweenTwoPoints(
            CvPoint const &First, CvPoint const &Second);
        double DistanceBetweenTwoPoints(
            CvPoint2D32f const &First, CvPoint2D32f const &Second);

        // Is directed line segment Start->Second clockwise (> 0), 
        //  counterclockwise (< 0), or collinear with respect to the directed 
        //  line segment Start->First? θ(1)
        int Direction(
            CvPoint2D32f const Start, 
            CvPoint2D32f const First, 
            CvPoint2D32f const Second);

        // Generate orthogonal of unit length from middle of given line segment
        //  outwards... θ(1)
        void GenerateOrthogonalToLineSegment(
            LineSegment const &A, 
            LineSegment &Orthogonal);

        // Can the collinear point be found on the line segment? θ(1)
        bool IsCollinearPointOnLineSegment(
            LineSegment const &A, 
            CvPoint2D32f const &CollinearPoint);
                                
        // Check if two line segments intersect... θ(1)
        bool IsLineSegmentsIntersect(
            LineSegment const &A, 
            LineSegment const &B);

        // Calculate the length of a line segment... θ(1)
        double LengthOfLineSegment(LineSegment const &A);
                                
        // Rotate a line segment about a point counterclockwise by an angle...
        void RotateLineSegmentAboutPoint(
            LineSegment &LineToRotate, 
            CvPoint2D32f const &Origin,
            double const &dRadians);

        // Rotate a point around another to be used as the origin...
        CvPoint2D32f &RotatePointAboutAnother(
            CvPoint2D32f const &OldPointToRotate,
            CvPoint2D32f const &Origin,
            double const &dRadians,
            CvPoint2D32f &NewPoint);
};

#endif

