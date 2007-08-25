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
    
    // wxWidgets for thread safe usage...
    #include <wx/thread.h>
    
    // Standard libraries and STL...
    #include <iostream>
    #include <string>
    #include <utility>
    #include <vector>
    
    // Using the standard namespace...
    using namespace std;

// WormTracker class...
class WormTracker
{   
    // Public methods...
    public:

        // Default constructor...
        WormTracker();

        // Accessors...

            // Convert millimeters to pixels...
            double              ConvertMillimetersToPixels(
                                    double const dMillimeters) const;

            // Convert from pixels to millimeters...
            double              ConvertPixelsToMillimeters(
                                    double const dPixels) const;

            // Convert from pixels² to millimeters²...
            double              ConvertSquarePixelsToSquareMillimeters(
                                    double const dPixelsSquared) const;

            // Get a copy of the current thinking image. Caller frees...
            IplImage           *GetThinkingImage() const;

            // Get the nth worm, or null worm if no more...
            Worm const         &GetWorm(unsigned int const unIndex) const;

            // The number of worms we are currently tracking...
            unsigned int        Tracking() const;

        // Mutators...

            // Advance frame...
            void                Advance(IplImage const &NewGrayImage);
            
            // Get the number of worms just added since last check...
            unsigned int const  GetWormsAddedSinceLastCheck();
            
            // Reset the tracker...
            void                Reset();
            
            // Set the field of view diameter...
            void                SetFieldOfViewDiameter(float const fDiameter);

        // Operators...

            // Output some info on current tracker state......
            friend ostream &operator<<(ostream &Output, 
                                       WormTracker &RequestedWormTracker);

        // Deconstructor...
       ~WormTracker();

    // Protected methods...
    protected:

        // Accessors...
            
            // How many underlying rectangles does given one rest upon?
            unsigned int const CountRectanglesIntersected(
                CvRect const &Rectangle) const;

            // Find the nearest worm to given...
            unsigned int FindNearestWorm(CvContour const &WormContour) const;

            // Do any points on the mystery contour lie on the image exterior?
            bool IsAnyPointOnImageExterior(CvContour const &MysteryContour)
                const;

            // Could this contour be a worm, independent of what we know?
            bool IsPossibleWorm(CvContour const &MysteryContour) const;

            // Do the two rectangles have a non-zero intersection area?
            bool IsRectanglesIntersect(CvRect const &RectangleOne,
                                       CvRect const &RectangleTwo) const;

        // Mutators...

            // Add new worm to tracker...
            void Add(CvContour const &WormContour);

            // Add a text label to the thinking image at a point...
            void AddThinkingLabel(string const sLabel, CvPoint Point);

    // Protected attributes...
    protected:
        
        // The microscope field of view diameter...
        float               fFieldOfViewDiameter;

        // Thinking image label font...
        CvFont              ThinkingLabelFont;

        // Current frame's gray image and thinking image...
        IplImage           *pGrayImage;
        IplImage           *pThinkingImage;
        
        // Table of worms being tracked...
        vector<Worm *>      TrackingTable;
        
        // Worms just added in this frame...
        unsigned int        unWormsJustAdded;
        
        // Resources mutex...
        mutable wxMutex     ResourcesMutex;
};

#endif

