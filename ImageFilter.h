#pragma once
 
/* This file is ImageFilter.h

The base class of all filters
It provides a public interface where execute function is used as a tool builder while functions like setInput(image), update(), getOuput() are for users.

*/

#include "Image.h"

namespace fgm_library
{
    /* Base class for all image filters to be implemented. */
    class ImageFilter
    {
        typedef short T; // all we do is short 
    public:
        /* Constructor */
        ImageFilter() :_output(Image::dimension{ 1,1,1,1,1 }), _input(Image::dimension{ 1,1,1,1,1 }) {};       //initializes output with a blank image
        // Destructor. This class does not own any free store allocated objects; 
        // but derived classes might, so it's virtual and empty 
        virtual ~ImageFilter() {};
        // Because derived classes can add data/function/type members, we want to 
        // avoid copying (and thus slicing) 
        ImageFilter(const ImageFilter&) = delete;
        ImageFilter& operator=(const ImageFilter&) = delete;

        /* Set the input image for this filter */
        void setInput(const Image& i) {
            _input = i;
        }

        /* Get the input back as a const ref */
        const Image& getInput() const {
            // Didn't touch _input, cast it back to const; 
            return const_cast<const Image&>(_input);
        }

        /* Get the output image result of this filter;
         available after calling update() */
        Image getOutput() const { return _output; };

        /* Update the image filter and compute the output */
        virtual void update()
        {
            // Didn't touch _input, cast it back to const; then execute() 
            execute(getInput());
        }
    protected:
        /* Container for the output image */
        Image _output;
        // This method should will be overloaded in the derived class
        virtual void execute(const Image& i) = 0;
    private:
        /* private definition of _input */
        Image _input;
    };

}



