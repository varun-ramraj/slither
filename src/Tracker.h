/*
  Name:         Tracker.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  Tracker class...
*/

// Multiple include protection...
#ifndef _TRACKER_H_
#define _TRACKER_H_

// Includes...

    // Worm class...
    #include "Worm.h"

    // OpenCV...
    #include <opencv/cv.h>
    
    // Standard libraries and STL...
    #include <ostream>
    #include <utility>

// Tracker class...
class Tracker
{   
    // Public methods...
    public:

        // Default constructor...
        Tracker(IplImage const &NewGrayImage);

        // Accessors...

            // Get the nth worm, or null worm if no more...
            Worm const &GetWorm(unsigned int const unIndex) const;

            // Could this contour be a worm, independent of what we know?
            bool IsPossibleWorm(CvContour const &MysteryContour) const;
            
            // The number of worms we are currently tracking...
            unsigned int Tracking() const;

        // Mutators...

            // Acknowledge a worm contour...
            void Acknowledge(CvContour &WormContour);

            // Advance frame...
            void AdvanceNextFrame(IplImage const &NewGrayImage);

        // Operators...

            // Output some info on current tracker state......
            friend std::ostream &operator<<(std::ostream &Output, 
                                            Tracker &RequestedTracker);

        // Deconstructor...
       ~Tracker();

    // Protected types and constants...
    protected:

        // Null worm...
        Worm const static NullWorm;

    // Protected methods...
    protected:

        // Accessors...
            
            // Find the best match of this contour, or NullWorm if none...
            Worm &FindBestMatch(CvContour &Key) const;

            // How many underlying rectangles does given one rest upon?
            unsigned int const GetRectanglesBeneath(CvRect const &Rectangle) 
                const;

            // Does this worm's contour lie along the outer edge of the frame?
            bool IsOuterEdge(CvContour const &WormContour) const;

        // Mutators...

            // Add worm to tracker...
            void Add(CvContour const &WormContour);

    // Protected attributes...
    protected:

        // Base storage to store contour sequence and any other dynamic 
        //  OpenCV data structures...
        CvMemStorage   *pStorage;

        // Current frame's gray image...
        IplImage       *pGrayImage;
};

#endif

