#pragma once

/* This header file contains GaussianFilter implementation

For public use
    setRadius (short value)
    setKernelDimensions (bool, bool, bool, bool, bool)

*/

#include "ImageFilter.h"

namespace fgm_library

{
    /* implement gaussian Blur filter->wiki https ://en.wikipedia.org/wiki/Kernel_(image_processing) */
    class GaussianFilter : public ImageFilter
    {
        typedef short T; 
    public:
        /* Constructor */
        GaussianFilter() : _r(0) {};                //set radius to value 0 in constructor
        void setRadius(T radius) { _r = radius; }   //set radius for window
        void setKernelDimensions(bool x_d = true, bool y_d = true, bool z_d = true, bool c_d = false, bool t_d = false) {   //assuming that generally we use 3d. Maximum 5d
            if (x_d) x_r = 1;
            if (y_d) y_r = 1;
            if (z_d) z_r = 1;
            if (c_d) c_r = 1;
            if (t_d) t_r = 1;
        }
    protected:
        /* Override the execute function */
        virtual void execute(const Image& i) override {

            double window_size = pow((2 * _r + 1), i.nr_dims());  //calculate window_size for all dimensions (for 3d = 27, 3x3x3)

            /* we assign _r only for specific Kernel dimension (save execution time)*/
            if (x_r) x_r = _r;
            if (y_r) y_r = _r;
            if (z_r) z_r = _r;
            if (c_r) c_r = _r;
            if (t_r) t_r = _r;

            /* first resize output with the size of the image */
            _output.resize(i.size());

            /* explore in max 5 dimensions */
            for (t = 0; t < i.dim(4); t++) {

                for (c = 0; c < i.dim(3); c++) {

                    for (z = 0; z < i.dim(2); z++) {

                        for (y = 0; y < i.dim(1); y++) {

                            for (x = 0; x < i.dim(0); x++) {
                                sum = 0;                // reset sum   
                                counter = 0;            // reset counter
                                //iterate within a radius size
                                for (int var_t = t - t_r; var_t < t + t_r + 1; var_t++) {
                                    counter = 0;
                                    for (int var_c = c - c_r; var_c < c + c_r + 1; var_c++) {
                                        counter = 0;
                                        for (int var_z = z - z_r; var_z < z + z_r + 1; var_z++) {
                                            counter = 0;
                                            for (int var_y = y - y_r; var_y < y + y_r + 1; var_y++) {
                                                for (int var_x = x - x_r; var_x < x + x_r + 1; var_x++) {
                                                    sum += gaussian_mask[counter] * i(var_x, var_y, var_z, var_c, var_t);   //apply gaussian convolution operation (sum)
                                                    counter++;
                                                }
                                            }
                                        }
                                    }
                                }
                                _output(x, y, z, c, t) = (Image::value_type)(sum / window_size);
                            }
                        }
                    }
                }
            }
        }
    private:
        /* The radius + variables for exploring */
        T _r;
        int gaussian_mask[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };   //define gaussian 3x3 mask
        int t = 0;
        int c = 0;
        int z = 0;
        int y = 0;
        int x = 0;
        int x_r = 0;
        int y_r = 0;
        int z_r = 0;
        int c_r = 0;
        int t_r = 0;
        double sum = 0;
        int counter = 0;
    };

}