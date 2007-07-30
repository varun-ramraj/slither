/*
  Name:         Worm.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Worm class...
*/

// Multiple include protection...
#ifndef _WORM_H_
#define _WORM_H_

// Includes...

    // wxWidgets...
    #include <wx/wx.h>
    
    // OpenCV...
    #include <opencv/cv.h>
    
    // Standard libraries and STL...
    #include <iostream>
    #include <utility>

// Worm class...
class Worm
{
    // Public types...
    public:
    
        // Line segment...
        typedef pair<CvPoint, CvPoint> LineSegment;
    
    // Public methods...
    public:

        // Worm constructor just needs to know it's contour and the image it rests on...
        Worm(CvSeq const &Contour, IplImage const &Image);

        // Accessors...

            // Best guess of the area, considering everything we've seen thus far...
            float const        &Area() const;

            // Best guess as to the head's position at this moment in time, since it changes...
            CvPoint             Head() const;

            // Best guess of the length from head to tail, considering everything we've seen thus far...
            float const        &Length() const;

            // Best guess as to the tail's position at this moment in time, since it changes...
            CvPoint             Tail() const;

            // Best guess of the area, considering everything we've seen thus far...
            float const        &Width() const;

        // Mutators...

            // Discover the worm's metrics based on its new contour and image data...
            void                Discover(CvSeq const &NewContour, IplImage const &Image);

        // Operators...

            // Show some info of what we know about this worm...
            friend std::ostream &operator<<(std::ostream &Output, 
                                            Worm &RequestedWorm) const;

            // Candidate is not similar enough... (see below)
            friend bool         operator<(Worm &CandidateWorm) const;

            // Candidate meets the minimum required similarity... (see below)
            friend bool         operator==(Worm &CandidateWorm) const;

        // Deconstructor...
       ~Worm();

    // Private methods...
    private:

        // Accessors...

            // Find the vertex on the contour the given length away, starting in increasing order... O(n)
            unsigned int const  FindNearestVertexIndexByPerimeterLength(unsigned int const &unStartVertex, 
                                                                        float const &fPerimeterLength) const;

            // Get the index of the next vertex in the contour after the given index, O(1) average...
            unsigned int        GetNextVertexIndex(unsigned int const &unVertexIndex) const;
            
            // Get the actual vertex of the given vertex index in the contour, O(1) average...
            CvPoint const      &GetVertex(unsigned int const &unVertexIndex) const;
            
            // Get the index of the previous vertex in the contour after the given index, O(1) average...
            CvPoint const      &GetPreviousVertexIndex(unsigned int const &unVertexIndex) const;

        // Mutators...

            // Update the approximate area, based on the value at this moment in time. This will help us make a
            //  more informed answer when asked via Area() for the size. θ(1) space and time...
            void                UpdateArea(float const &fAreaAtThisMoment);

            // Update the approximate head and tail position, based on the value at this moment in time. This will
            //  help us make a more informed answer when asked via Head() or Tail() for the actual coordinates...
            void                UpdateHeadAndTail(unsigned int const &unHeadVertexIndex,
                                                  unsigned int const &unTailVertexIndex);

            // Update the approximate length, based on the value at this moment in time. This will help us make a
            //  more informed answer when asked via Length() for the length. θ(1) space and time...
            void                UpdateLength(float const &fLengthAtThisMoment);
            
            // Update the approximate width, based on the value at this moment in time. This will help us make a
            //  more informed answer when asked via Width() for the width. θ(1) space and time...
            void                UpdateWidth(float const &fWidthAtThisMoment);

        // Mutationless math and computational geometry helpers...

            // Adjust the distance of the second vertex by the given distance along the radial... 
            void                AdjustDirectedLineSegmentLength(LineSegment &A, 
                                                                float fLength) const;
            
            // Calculate the distance between the midpoints of two segments... θ(1)
            float               DistanceBetweenLineSegments(LineSegment const &A, 
                                                            LineSegment const &B) const;

            // Calculate the absolute distance between two points...
            float               DistanceBetweenTwoPoints(CvPoint const &First, 
                                                         CvPoint const &Second) const; 

            // Is directed line segment Start->Second clockwise (> 0), counterclockwise (< 0), or collinear with 
            //  respect to the directed line segment Start->First? θ(1)
            int                 Direction(CvPoint const Start, 
                                          CvPoint const First, 
                                          CvPoint const Second) const;
        
            // Generate orthogonal of unit length from middle of given line segment outwards... θ(1)
            void                GenerateOrthogonalToLineSegment(LineSegment const &A, 
                                                                LineSegment &Orthogonal) const;

            // Can the collinear point be found on the line segment? θ(1)
            bool                IsCollinearPointOnLineSegment(LineSegment const &A, 
                                                              CvPoint const &CollinearPoint) const;

            // Given only the two vertex indices, *this* image, and assuming they are opposite ends of the worm, 
            //  would the first of the two most likely be the head if we had only this image to consider?
            bool                IsFirstProbablyHeadByCloisteredCheck(unsigned int const &unCandidateHeadVertexIndex,
                                                                     unsigned int const &unCandidateTailVertexIndex,
                                                                     IplImage const &Image) const;

            // Check if two line segments intersect... θ(1)
            bool                IsLineSegmentsIntersect(LineSegment const &A, 
                                                        LineSegment const &B) const;

            // Calculate the length of a line segment... θ(1)
            float               LengthOfLineSegment(LineSegment const &A) const;
            
            // Find the vertex index in the contour sequence that contains either end of the worm... θ(n)
            unsigned int        PinchShiftForAnEnd() const;

            // Rotate a line segment about a point counterclockwise by an angle...
            void                RotateLineSegmentAboutPoint(LineSegment &LineToRotate, 
                                                            CvPoint const &Origin,
                                                            float const &fRadians) const;

            // Rotate a point around another to be used as the origin...
            CvPoint            &RotatePointAboutAnother(CvPoint const &OldPointToRotate, 
                                                        CvPoint const &Origin, 
                                                        float const &fRadians, 
                                                        CvPoint &NewPoint) const;

    // Private attributes...
    private:

            // Base storage to store contour sequence and any other dynamic OpenCV data structures...
            CvMemStorage       *pStorage;

                // Contour around the worm...
                CvSeq          *pContour;
            
            // Some book keeping information that we use for computing arithmetic averages for the metrics...
            unsigned int        unUpdates;
            
            // The worm's metrics...
            
                // Area...
                float           fArea;
                
                // Length of the worm...
                float           fLength;
                
                // Width of the worm...
                float           fWidth;

    // Private constants...
    private:
    
            // The value of π...
            float const     Pi                          = 3.1415926535897932384626433832795;
            
            // Infinity... (kind of)
            float const     Infinity                    = FLT_MAX;
            
            // The minimum required similarity needed for worm equivalency... (0.0 < percentage <= 100.0) 
            float const     fMinimumRequiredSimilarity  = 95.0;

};

#endif

