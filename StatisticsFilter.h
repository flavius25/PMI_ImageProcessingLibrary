#pragma once

/* This header file contains StatisticsFilter implementation

For public use
        GetMin() 
        GetMax() 
        GetMean() 
        GetSum() 
        GetSigma() 
        GetVariance() 

*/

#include "ImageFilter.h"
#include <numeric>

namespace fgm_library
{

    class StatisticsFilter : public ImageFilter
    {
        typedef short T;
    public:
        /* Get and set functions for the threshold value */
        T GetMin() const { return this->minimum; };
        T GetMax() const { return this->maximum; };
        double GetMean() const { return this->mean; };
        double GetSum() const { return this->sum; };
        double GetSigma() const { return this->sigma; };
        double GetVariance() const { return this->variance; };

    protected:
        /* Override the execute function */
        virtual void execute(const Image& i) override
        {
            double localmean;
            double voxels_number = i.dim(0) * i.dim(1) * i.dim(2) * i.dim(3) * i.dim(4);    //calculate voxel numbers based on dimensions
            vector<double> dev((__int64)voxels_number,0);                        //to fill with zeroes with size of voxels_number. cast to eliminate warning

            sum = accumulate(i.begin(), i.end(), 0.0);			        //calculate the sum of all pixel values using accumulate (+) function
            mean = sum / voxels_number;                                 //calculate mean 

            minimum = *min_element(i.begin(), i.end());                 //calculate minimum

            maximum = *max_element(begin(i), end(i));                   //calculate maximum

            localmean = mean;                                           //needed this local variable to use mean in the transform lamda function (compiler error otherwise)

            transform(i.begin(), i.end(), dev.begin(), [localmean](double x) { return x - localmean; });    //difference of each pixel with respect to the mean and save in dev                                                      // calculate variance
            squared_sum = inner_product(dev.begin(), dev.end(), dev.begin(), 0.0);                          //calculate the sum of squares of each pixel value in the vector
            variance = squared_sum / dev.size();                                                            //calculate variance
            sigma = sqrt(squared_sum / voxels_number);                                                      //calculate sigma
        }
    private:
        T minimum=0;
        T maximum=0;
        double sum=0;
        double mean=0;
        double sigma=0;
        double squared_sum=0;
        double variance=0;
        using ImageFilter::getOutput;                                   //hide getOutput 
    };
}