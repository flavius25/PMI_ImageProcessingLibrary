#pragma once

/* This file is ImageIOBase.h

This file is the base class for IO PIP and IO MHD

It contains:
-A protected constructor that takes a filename
-A protected string that holds the filename
-A virtual destructor
-Deleted copy constructor and deleted assignment operator
-A pure virtual read and write function

*/

#include "Image.h"

namespace fgm_library
{
    //Base class for all ImageIO classes, providing the read and write functions.
    class ImageIOBase
    {
    public:
        virtual ~ImageIOBase() {}                                                      //virtual destructor
        virtual Image read() const = 0;                                                //pure virtual read function
        virtual void write(const Image&, const array<int, N_DIM>&) const = 0;          //pure virtual write function
    protected:
        string filename;                                                               //protected string that holds the filename
        ImageIOBase(const string& original_filename) {                                 //protected constructor that takes a filename
            filename = original_filename;
        }
        ImageIOBase() {}                                                               //this class cannot be created, only the derivate class (mhd..pip). This class becomes abstract.
        ImageIOBase& operator=(const ImageIOBase&) = delete;                           //deleted copy constructor and assignment operator
    private:
    };
}