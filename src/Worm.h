/*
  Name:         Worm.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Worm class...
*/

// Multiple include protection...
#ifndef _WORM_H_
#define _WORM_H_

// Includes...

    // OpenCV...
    #include <opencv/cv.h>
    
    // Standard libraries and STL...
    #include <ostream>
    #include <utility>

// Worm class...
class Worm
{   
    // Public methods...
    public:

        // Default constructor...
        Worm();
        
        // Worm constructor just needs to know it's contour and the image it rests on...
        Worm(CvContour const &Contour, IplImage const &GrayImage);

        // Accessors...

            // Best guess of the area, considering everything we've seen thus far...
            double const       &Area() const;

            // Best guess of the worm's centre...
            CvPoint const      &Centre() const;

            // Best guess as to the head's position at this moment in time, since it changes...
            CvPoint const      &Head() const;

            // Best guess of the length from head to tail, considering everything we've seen thus far...
            double const       &Length() const;

            // Best guess as to the tail's position at this moment in time, since it changes...
            CvPoint const      &Tail() const;

            // Best guess of the area, considering everything we've seen thus far...
            double const       &Width() const;

        // Mutators...

            // Discover worm's metrics based on new contour and image data...
            void                Discover(CvContour const &NewContour, 
                                         IplImage const &GrayImage);

        // Operators...

            // Show some info of what we know about this worm...
            friend std::ostream &operator<<(std::ostream &Output, 
                                            Worm &RequestedWorm);

            // Candidates are not similar enough...
            bool                operator<(Worm &RightWorm) const;

            // Candidates meet the minimum required similarity...
            bool                operator==(Worm &RightWorm) const;

        // Deconstructor...
       ~Worm();

    // Protected constants...
    protected:
    
            // Pinch-Shift iteration directions...
            enum IterationDirection
            {
                Forwards,
                Backwards
            };

            // The value of π...
            double const static Pi                          
                = 3.1415926535897932384626433832795f;
            
            // Infinity... (kind of)
            double const static Infinity                    = FLT_MAX;
            
            // The minimum required similarity needed for worm equivalency... 
            //  (0.0 < percentage <= 100.0) 
            double const static dMinimumRequiredSimilarity  = 95.0f;

    // Protected types...
    protected:
    
        // Line segment...
        typedef std::pair<CvPoint2D32f, CvPoint2D32f>
            LineSegment;
        
        // Terminal end scores are used to award terminal ends (head or tail)
        //  of the nematode, based on their likelihood of being the head or the
        //  tail...
        typedef struct TerminalEndNotes
        {
            // Inline constructor initializer...
            TerminalEndNotes(CvPoint _LastSeenLocus, 
                             unsigned int _unHeadScore)
                : LastSeenLocus(_LastSeenLocus),
                  unHeadScore(_unHeadScore)
            {
            }
            
            // Last place we saw the terminal end...
            CvPoint         LastSeenLocus;

            // Number of times this end has been guessed as the head...
            unsigned int    unHeadScore;

        }TerminalEndNotes;

    // Protected methods...
    protected:

        // Accessors...

            // Find the vertex on the contour a given length away, starting 
            //  from a given vertex... O(n)
            unsigned int const  FindVertexIndexByLength(
                                    unsigned int const &unStartVertexIndex, 
                                    double const &dPerimeterLength,
                                    unsigned int &unVerticesTraversed = unDummy) 
                                const;

            // Get the total brightness of a line...
            double const        GetLineBrightness(LineSegment const &A,
                                                  IplImage const &GrayImage)
                                const;

            // Get the total surrounding brightness of a central point ...
            double const        GetSurroundingBrightness(
                                    CvPoint Centre,
                                    IplImage const &GrayImage)
                                const;
            
            /* Get the average brightness of the area within a contour...
            double const        GetAverageBrightness(
                                    CvContour const &Contour,
                                    IplImage const &GrayImage) const;*/

            // Get the index of the next vertex in the contour after the given 
            //  index, O(1) average...
            unsigned int        GetNextVertexIndex(
                                    unsigned int const &unVertexIndex)
                                const;
            
            // Get the actual vertex of the given vertex index in the contour, 
            //  O(1) average...
            CvPoint            &GetVertex(
                                    unsigned int const &unVertexIndex)
                                const;
            
            // Get the index of the previous vertex in the contour after the 
            //  given index, O(1) average...
            unsigned int        GetPreviousVertexIndex(
                                    unsigned int const &unVertexIndex)
                                const;

        // Mutators...

            // Update the approximate area, based on the value at this moment in
            //  time. This will help us make a more informed answer when asked 
            //  via Area() for the size. θ(1) space and time...
            void                UpdateArea(double const &dAreaAtThisMoment);

            // Update the gravitational centre from this image...
            void                UpdateGravitationalCentre();

            // Update the approximate head and tail position, based on the value 
            //  at this moment in time. This will help us make a more informed 
            //  answer when asked via Head() or Tail() for the actual 
            //  coordinates...
            void                UpdateHeadAndTail(
                                    unsigned int const &unHeadVertexIndex,
                                    unsigned int const &unTailVertexIndex);

            // Update the approximate length, based on the value at this moment
            //  in time. This will help us make a more informed answer when
            //  asked via Length() for the length. θ(1) space and time...
            void                UpdateLength(double const &dLengthAtThisMoment);
            
            // Update the approximate width, based on the value at this moment 
            //  in time. This will help us make a more informed answer when
            //  asked via Width() for the width. θ(1) space and time...
            void                UpdateWidth(double const &dWidthAtThisMoment);

        // Mutationless math and computational geometry helpers...

            // Adjust the distance of the second vertex by the given distance 
            //  along the radial... 
            void                AdjustDirectedLineSegmentLength(
                                    LineSegment &A, 
                                    double dLength) const;
            
            // Clip line against the image rectangle...
            void                ClipLineSegment(CvSize Size, 
                                                LineSegment &A) const;
            
            // Calculate the distance between the midpoints of two segments... 
            //  θ(1)
            double              DistanceBetweenLineSegments(
                                    LineSegment const &A, 
                                    LineSegment const &B) const;

            // Calculate the absolute distance between two points...
            double              DistanceBetweenTwoPoints(
                                    CvPoint const &First, 
                                    CvPoint const &Second) const;
            double              DistanceBetweenTwoPoints(
                                    CvPoint2D32f const &First, 
                                    CvPoint2D32f const &Second) const;

            // Is directed line segment Start->Second clockwise (> 0), 
            //  counterclockwise (< 0), or collinear with respect to the
            //  directed line segment Start->First? θ(1)
            int                 Direction(CvPoint2D32f const Start, 
                                          CvPoint2D32f const First, 
                                          CvPoint2D32f const Second) const;
        
            // Generate orthogonal of unit length from middle of given line 
            //  segment outwards... θ(1)
            void                GenerateOrthogonalToLineSegment(
                                    LineSegment const &A, 
                                    LineSegment &Orthogonal) const;

            // Can the collinear point be found on the line segment? θ(1)
            bool                IsCollinearPointOnLineSegment(
                                    LineSegment const &A, 
                                    CvPoint2D32f const &CollinearPoint) const;

            // Given only the two vertex indices, *this* image, and assuming 
            //  they are opposite ends of the worm, would the first of the two
            //  most likely be the head if we had but this image alone to
            //  consider?
            bool                IsFirstHeadCloisterCheck(
                                    unsigned int const &unCandidateHeadVertexIndex,
                                    unsigned int const &unCandidateTailVertexIndex,
                                    IplImage const     &GrayImage) const;

            // Check if two line segments intersect... θ(1)
            bool                IsLineSegmentsIntersect(
                                    LineSegment const &A, 
                                    LineSegment const &B) const;

            // Calculate the length of a line segment... θ(1)
            double              LengthOfLineSegment(
                                    LineSegment const &A) const;
            
            // Find the vertex index in the contour sequence that contains 
            //  either end of the worm, and update width while we're at it... 
            //  θ(n)
            unsigned int        PinchShiftForAnEnd(
                                    IplImage const &GrayImage,
                                    IterationDirection Direction = Forwards);

            // Rotate a line segment about a point counterclockwise by an 
            //  angle...
            void                RotateLineSegmentAboutPoint(
                                    LineSegment &LineToRotate, 
                                    CvPoint2D32f const &Origin,
                                    double const &dRadians) const;

            // Rotate a point around another to be used as the origin...
            CvPoint2D32f       &RotatePointAboutAnother(
                                    CvPoint2D32f const &OldPointToRotate,
                                    CvPoint2D32f const &Origin,
                                    double const &dRadians,
                                    CvPoint2D32f &NewPoint) const;

    // Protected attributes...
    protected:

            // Base storage to store contour sequence and any other dynamic 
            //  OpenCV data structures...
            CvMemStorage       *pStorage;

                // Contour around the worm...
                CvContour      *pContour;
            
            // Some book keeping information that we use for computing 
            //  arithmetic averages for the metrics...
            unsigned int        unUpdates;
            
            // The worm's metrics...
            
                // Area...
                double          dArea;
                
                // Centre...
                CvPoint         GravitationalCentre;
                
                // Length of the worm...
                double          dLength;
                
                // Width of the worm...
                double          dWidth;

            // Terminal end scores...
            TerminalEndNotes    TerminalA;
            TerminalEndNotes    TerminalB;
            
            // Dummy default argument parameters...
            static unsigned int unDummy;

};

#endif

