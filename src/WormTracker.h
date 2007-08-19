/*
  Name:         WormTracker.h (definition)
  Author:       Kip Warner (Kip@TheVertigo.com)
  Description:  WormTracker class...
*/

// Multiple include protection...
#ifndef _WORMTRACKER_H_
#define _WORMTRACKER_H_

// Includes...

    // Worm class...
    #include "Worm.h"

    // OpenCV...
    #include <opencv/cv.h>
    #include <opencv/highgui.h>
    
    // Standard libraries, STL, and namespace...
    using namespace std;
    #include <iostream>
    #include <string>
    #include <utility>
    #include <vector>

// WormTracker class...
class WormTracker
{   
    // Public methods...
    public:

        // Default constructor...
        WormTracker();

        // Accessors...
            
            // The number of worms we are currently tracking...
            unsigned int Tracking() const;
            
            // Get the current thinking image...
            IplImage const &GetThinkingImage() const;

        // Mutators...

            // Acknowledge a worm contour...
            void Acknowledge(CvContour const &WormContour);

            // Advance frame...
            void AdvanceNextFrame(IplImage const &NewGrayImage);

        // Operators...

            // Output some info on current tracker state......
            friend ostream &operator<<(ostream &Output, 
                                       WormTracker &RequestedWormTracker);

        // Deconstructor...
       ~WormTracker();

    // Protected types and constants...
    protected:

    // Protected methods...
    protected:

        // Accessors...
            
            // How many underlying rectangles does given one rest upon?
            unsigned int const CountRectanglesIntersected(
                CvRect const &Rectangle) const;

            // Get the nth worm, or null worm if no more...
            Worm &GetWorm(unsigned int const unIndex) const;

            // Find the worm that probably created this contour, if any...
            bool IsKnown(CvContour const &WormContour, 
                         unsigned int &unFoundIndex) const;

            // Could this contour be a worm, independent of what we know?
            bool IsPossibleWorm(CvContour const &MysteryContour) const;

            // Do the two rectangles have a non-zero intersection area?
            bool IsRectanglesIntersect(CvRect const &RectangleOne,
                                       CvRect const &RectangleTwo) const;

        // Mutators...

            // Add worm to tracker...
            void Add(CvContour const &WormContour);

            // Add a text label to the thinking image at a point...
            void AddThinkingLabel(string const sLabel, CvPoint Point);

    // Protected attributes...
    protected:

        // Base storage to store contour sequence and any other dynamic 
        //  OpenCV data structures...
        CvMemStorage   *pStorage;
        
        // Thinking image label font...
        CvFont          ThinkingLabelFont;

        // Current frame's gray image and thinking image...
        IplImage       *pGrayImage;
        IplImage       *pThinkingImage;
        
        // Table of worms being tracked...
        vector<Worm *>  TrackingTable;
};

#endif

