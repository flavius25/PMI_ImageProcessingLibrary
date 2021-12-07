#pragma once

/* This header file contains MedianFilter Algorithm from Sonka book implementation

For public use
    setRadius (short value)
    setKernelDimensions (bool, bool, bool, bool, bool)

*/

#include "ImageFilter.h"
#include <map>

namespace fgm_library

{

    class MedianFilterSonka : public ImageFilter
    {
        typedef short T; 
    public:
        /* Constructor */
        MedianFilterSonka() : _r(0) {};                     //set radius to initial value 0
        void setRadius(T radius) { _r = radius; }           //set radius for window
        void setKernelDimensions(bool x_d = true, bool y_d = true, bool z_d = true, bool c_d = false, bool t_d = false) {   //assume that generally we use 3d. Maximum 5d
            if (x_d) x_r = 1;
            if (y_d) y_r = 1;
            if (z_d) z_r = 1;
            if (c_d) c_r = 1;
            if (t_d) t_r = 1;
        } 
    protected:
        /* Override the execute function */
        virtual void execute(const Image& i) override {

            /* we assign _r only for specific Kernel dimension (save execution time)*/
            if (x_r) x_r = _r;
            if (y_r) y_r = _r;
            if (z_r) z_r = _r;
            if (c_r) c_r = _r;
            if (t_r) t_r = _r;

            /* first resize output with the size of the image */
            _output.resize(i.size());
            for (t = 0; t < i.dim(4); t++) {

                for (c = 0; c < i.dim(3); c++) {

                    for (z = 0; z < i.dim(2); z++) {

                        for (y = 0; y < i.dim(1); y++) {

                            for (x = 0; x < i.dim(0); x++) {
                                window.clear();     //clear window vector's elements
                                histogram.clear();  //clear histogram vector's elements

                                //iterate with a radius size 
                                for (int var_t = t - t_r; var_t < t + t_r + 1; var_t++) {
                                    for (int var_c = c - c_r; var_c < c + c_r + 1; var_c++) {
                                        for (int var_z = z - z_r; var_z < z + z_r + 1; var_z++) {
                                            for (int var_y = y - y_r; var_y < y + y_r + 1; var_y++) {
                                                for (int var_x = x - x_r; var_x < x + x_r + 1; var_x++) {
                                                    window.push_back(i(var_x, var_y, var_z, var_c, var_t));     //push_back pixel values for sort
                                                    histogram[i(var_x, var_y, var_z, var_c, var_t)]++;          //build histogram using map
                                                }
                                            }
                                        }
                                    }
                                }
                                sort(window.begin(), window.end());
                                Image::value_type median = *(window.begin() + window.size() / 2);

                                /* following algorithm from IP Sonka book */
                                Nm = 0;                         //reset Nm for the new window processing
                                int middle = (int)(window.size()) / 2; // the position in the center of the window vector
                                for (auto& s: window) {
                                    if (s <= median) {
                                        Nm++;
                                    }
                                }
                               
                                for (int var_z = z - z_r; var_z < z + z_r + 1; var_z++) {           //make it work for 3d
                                    int var_x = x - x_r;
                                    int var_y = y - y_r;
                                    for (int var_y = y - y_r; var_y < y + y_r + 1; var_y++) {       // 2d
                                        int var_x = x - x_r;
                                        histogram[i(var_x, var_y, var_z)] = histogram[i(var_x, var_y, var_z)] - 1;

                                        if (i(var_x, var_y, var_z) <= median) {
                                            Nm = Nm - 1;
                                        }
                                    }
                                }

                                for (int var_z = z - z_r; var_z < z + z_r + 1; var_z++) {
                                    int var_x = x + x_r;
                                    int var_y = y + y_r;
                                    for (int var_y = y - y_r; var_y < y + y_r + 1; var_y++) {
                                        int var_x = x + x_r;

                                        histogram[i(var_x, var_y, var_z)] = histogram[i(var_x, var_y, var_z)] + 1;

                                        if (i(var_x, var_y) <= median) {
                                            Nm = Nm + 1;
                                        }
                                    }
                                }

                                while(Nm < middle) {
                                       median = median + 1;
                                       Nm = Nm + histogram[median];
                                       if (Nm >= middle) break;
                                }

                                while(Nm > middle) {
                                    Nm = Nm - histogram[median];
                                    median = median - 1;
                                    if (Nm <= middle) break;
                                }

                                if(Nm == middle) {
                                    _output(x, y, z, c, t) = median;
                                }

                                _output(x, y, z, c, t) = median;     //the median is parsed as pixel value intensity
                            }
                        }
                    }
                }
            }
        }
    private:
        /* The radius + variables for exploring */
        T _r;
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
        vector <Image::value_type> window = { 0 };      //define window vector
        map<Image::value_type, int> histogram;          //define map for creating histogram
        int Nm = 0;                                     //count Nm, number of pixels with intensity less than or equal to the median
    };

}