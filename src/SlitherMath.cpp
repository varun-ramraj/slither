/*
  Name:         SlitherMath.cpp (implementation)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  General Slither related math routines...
*/

// Includes...

    // Our definitions...
    #include "SlitherMath.h"

    // Min / max templates...
    #include <algorithm>

    // Runtime assertion checks for debug builds...
    #include <cassert>

    // Standard math routines...
    #include <cmath>

// Within the SlitherMath namespace...
namespace SlitherMath
{
    // Adjust distance of second vertex by the given distance along radial... 
    void AdjustDirectedLineSegmentLength(LineSegment &A, double dLength)
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
    
    // Clip line against the image rectangle...
    void ClipLineSegment(CvSize const Size, LineSegment &A)
    {
        // Make a duplicate of the segment since Intel's clipping routine 
        //  handles integral values only...
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
    int Direction(
        CvPoint2D32f const Start, CvPoint2D32f const First, 
        CvPoint2D32f const Second)
    {
        // Calculate the cross product, but do it with both vectors translated back 
        //  to the origin to make it work...
        return (int) (((First.x - Start.x) * (Second.y - Start.y)) - 
                      ((Second.x - Start.x) * (First.y - Start.y)));
    }

    // Calculate the distance between the midpoints of two segments... θ(1)
    double DistanceBetweenLineSegments(
        LineSegment const &A, LineSegment const &B)
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
    double DistanceBetweenTwoPoints(
        CvPoint const &First, CvPoint const &Second)
    {
        // Return it...
        return cvSqrt(pow(Second.x - First.x, 2) + pow(Second.y - First.y, 2));
    } 

    // Calculate the absolute distance between two points...
    double DistanceBetweenTwoPoints(
        CvPoint2D32f const &First, CvPoint2D32f const &Second)
    {
        // Return it...
        return cvSqrt(pow(Second.x - First.x, 2) + pow(Second.y - First.y, 2));
    }    

    // Generate orthogonal of unit length from middle of given line segment 
    //  outwards... θ(1)
    void GenerateOrthogonalToLineSegment(
        LineSegment const &A, LineSegment &Orthogonal)
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

    // Can the collinear point be found on the line segment? θ(1)
    bool IsCollinearPointOnLineSegment(
        LineSegment const &A, CvPoint2D32f const &CollinearPoint)
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

    // Check if two line segments intersect... θ(1)
    bool IsLineSegmentsIntersect(LineSegment const &A, LineSegment const &B)
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

    // Calculate the length of a line segment... θ(1)
    double LengthOfLineSegment(LineSegment const &A)
    {
        // Calculate...
        return cvSqrt(((A.second.x - A.first.x) * (A.second.x - A.first.x)) + 
                      ((A.second.y - A.first.y) * (A.second.y - A.first.y)));
    }

    // Rotate a line segment about a point counterclockwise by an angle...
    void RotateLineSegmentAboutPoint(
        LineSegment &LineToRotate, CvPoint2D32f const &Origin, 
        double const &dRadians)
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
    CvPoint2D32f &RotatePointAboutAnother(
        CvPoint2D32f const &OldPointToRotate, CvPoint2D32f const &Origin, 
        double const &dRadians, CvPoint2D32f &NewPoint)
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
}

