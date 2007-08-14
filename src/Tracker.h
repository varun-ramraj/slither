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
        Tracker();

        // Accessors...

            // Get the nth worm, or null worm if no more...
            Worm const &GetWorm(unsigned int const unIndex) const;

            // Could this contour be a worm, independent of what we know?
            bool IsPossibleWorm(CvContour const &MysteryContour) const;

        // Mutators...
            
            // Acknowledge a contour... (vermiform or not)
            void Acknowledge(CvContour &Worm);

        // Deconstructor...
       ~Tracker();

    // Protected types...
    protected:

    // Protected methods...
    protected:

        // Accessors...
        Worm &FindBestMatch(CvContour &Key) const;

        // Mutators...

    // Protected attributes...
    protected:

        // Null worm...
        Worm const static NullWorm;
};

#endif

