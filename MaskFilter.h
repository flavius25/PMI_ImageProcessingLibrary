#pragma once

/* This header file contains MaskFilter implementation

For public use
    setInputMask (Image)
    Image = getInputMask()
    update ()

*/

#include "ImageFilter.h"

namespace fgm_library
{

    class MaskFilter : public ImageFilter
    {
        typedef short T;
    public:
        MaskFilter(): _inputMask(Image::dimension{ 1,1,1,1,1 }) {}  //simple constructor, initialising an empty image
        /* Set the input image Mask for this filter */
        void setInputMask(const Image& i) { 
            _inputMask = i;
        }

        /* get Input mask function */
        const Image& getInputMask() const {
            return const_cast<const Image&>(_inputMask);
        }

        /* override update function */
        virtual void update() override
        {
            Image mask = getInputMask();
            Image input = getInput();

            _output.resize(input.size());
            
            // Call transform function with 4 parameters, where mask is the third. When pixel value of the mask is 0, return 0 pixel value at output. If not zero, return the value of the input image.
            transform(begin(input), end(input), begin(mask), begin(_output), [](T value, T value_mask) { return value_mask == 0 ? T(0) : value; });
        }
    protected:
    private:
        /* defining private members */
        Image _inputMask;
        virtual void execute(const Image& i) override {}; //hide execute
    };

}