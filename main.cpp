#pragma warning(default:4996)
#pragma warning(disable:4018)
#include "../std_lib_facilities.h"
#include "ImageIOBase.h"
#include "ImageIOFactory.h"
#include "ImageFilter.h"
#include "ThresholdImageFilter.h"
#include "StatisticsFilter.h"
#include "Image.h"
#include "MaskFilter.h"
#include "ConvolutionFilter.h"
#include "MedianFilter.h"
#include "MedianFilterSonka.h"
#include "GaussianFilter.h"
#include "laplacianFilter.h"
#include <numeric>
#include <chrono>

/* This file is main.cpp it includes: 

Pipelines in main.cpp:
0. Get Image Statistics
1. Get Image Mean + Smoothing image with convolutionFilter (it can be with Gaussian also) +  Threshold the image (below the mean) and create a new image + Apply Mask Filter. (in the darker sides of the image will be 0 now)
2. Apply Gaussian smoothing (better than average convolution for edge detection) + Apply Laplacian  + write laplacian image +  for loop to invert pixel values + Threshold the image to display only the inverted ex negative values + Apply Mask Filter to extract edges
3. Read brain.mhd + Apply Median  + create Image (pipeline3_medianfilter) +  Apply Median Filter Sonka + Create Image (pipeline3_medianfiltersonka) (compare efficiency, execution time between the 2 filters). It seems that Median filter from Sonka book slighlty preservers edges better. In terms of execution time, i tried even with radius 5 and the algorithm of Sonka is slower than the normal Median Filter, which would be obvious when looking at the number of loops Sonka requires.  It also might by my old laptop (intel i3 processor)
4. There are some commented lines at the bottom, that test corrupt files. Uncomment them for testing Error_handling. Error_handling is implemented in PipImageIO.h and MhdImageIO.h which throws run_time errors for corrupt files.

*/

using namespace fgm_library;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

int main() {
	/* Create instance objects */
	StatisticsFilter s;
	ConvolutionFilter c;
	ThresholdImageFilter f;
	MaskFilter m;
	GaussianFilter Gf;
	laplacianFilter Lf;
	MedianFilter mf;
	MedianFilterSonka mfs;
	
	unique_ptr<ImageIOBase> io = ImageIOFactory::getIO("brain.mhd");

	auto image = io->read();

	cout << "Image statistics: " << endl;
	
	s.setInput(image);
	s.update();
	auto min = s.GetMin();
	auto max = s.GetMax();
	auto mean = s.GetMean();
	auto sum = s.GetSum();
	auto sigma = s.GetSigma();
	auto variance = s.GetVariance();

	cout << "min: " << min << endl;
	cout << "max: " << max << endl;
	cout << "average: " << mean << endl;
	cout << "sum: " << sum << endl;
	cout << "deviation: " << sigma << endl;
	cout << "variance: " << variance << endl;

	/* Pipeline 1: Get Image Mean + Smoothing image with convolutionFilter +  Threshold the image (below the mean) and create a new image + Apply Mask Filter */
	cout << "\nStart of Pipeline 1... " << endl;

	//smoothing image
	c.setInput(image);
	c.setRadius(1);
	c.setKernelDimensions(1,1,1,0,0); //3d originally
	auto t_smooth1 = high_resolution_clock::now();
	c.update();
	auto t_smooth2 = high_resolution_clock::now();
	image = c.getOutput();

	auto conv_int = duration_cast<milliseconds>(t_smooth2 - t_smooth1);
	std::cout << "Conv Average Smoothing Duration: " << conv_int.count() << " ms\n";

	//threshold image
	f.setInput(image);
	f.setThreshold((Image::value_type)mean);
	auto t_threshold1 = high_resolution_clock::now();
	f.update();
	auto t_threshold2 = high_resolution_clock::now();
	auto image_masked = f.getOutput();

	auto thres_int = duration_cast<milliseconds>(t_threshold2 - t_threshold1);
	std::cout << "Threshold Duration: " << thres_int.count() << " ms\n";

	//Mask the initial image
	m.setInput(image);
	m.setInputMask(image_masked);
	auto t_mask1 = high_resolution_clock::now();
	m.update();
	auto t_mask2 = high_resolution_clock::now();
	image = m.getOutput();

	auto mask_int = duration_cast<milliseconds>(t_mask2 - t_mask1);
	std::cout << "Mask Duration: " << mask_int.count() << " ms\n";

	io = ImageIOFactory::getIO("pipeline_1.mhd");
	io->write(image, { 109, 91, 80, 1, 1 }); 
	cout << "End of Pipeline 1" << endl;

	/* Pipeline 2: Apply Gaussian smoothing + Apply Laplacian  + write laplacian image +  for loop to invert pixel values + Threshold the image to display only the inverted ex negative values + Apply Mask Filter to extract edges */
	cout << "\nStart of Pipeline 2... " << endl;

	io = ImageIOFactory::getIO("brain.pip");

	//apply gaussian for smoothing 
	Gf.setInput(image);
	Gf.setRadius(1);
	Gf.setKernelDimensions(1, 1, 1, 0, 0); //3d
	auto gaussian_1 = high_resolution_clock::now();
	Gf.update();
	auto gaussian_2 = high_resolution_clock::now();
	image = Gf.getOutput();

	auto gaussian_int = duration_cast<milliseconds>(gaussian_2 - gaussian_1);
	std::cout << "Gaussian Duration: " << gaussian_int.count() << " ms\n";

	//process Laplacian
	Lf.setInput(image);
	Lf.setRadius(1);
	Lf.setKernelDimensions(1, 1, 1, 0, 0); //3d initially
	auto t_laplace1 = high_resolution_clock::now();
	Lf.update();
	auto t_laplace2 = high_resolution_clock::now();
	image = Lf.getOutput();

	auto laplace_int = duration_cast<milliseconds>(t_mask2 - t_mask1);
	std::cout << "Laplace Duration: " << laplace_int.count() << " ms\n";

	//write laplacian image
	io = ImageIOFactory::getIO("laplacian_brain.pip");
	io->write(image, { 109, 91, 80, 1, 1 }); 

	for (auto& i : image) i = -i;			 // invert pixel values

	//keep only negative values that were inverted in the for loop
	f.setInput(image);
	f.setThreshold(0);
	t_threshold1 = high_resolution_clock::now();
	f.update();
	t_threshold2 = high_resolution_clock::now();
	image_masked = f.getOutput();

	thres_int = duration_cast<milliseconds>(t_threshold2 - t_threshold1);
	std::cout << "Threshold Duration: " << thres_int.count() << " ms\n";

	//read laplacian image created
	io = ImageIOFactory::getIO("laplacian_brain.pip");
	auto image_laplacian = io->read();

	//Apply the Mask to extract edges
	m.setInput(image_laplacian);
	m.setInputMask(image_masked);
	t_mask1 = high_resolution_clock::now();
	m.update();
	t_mask2 = high_resolution_clock::now();
	image = m.getOutput();

	mask_int = duration_cast<milliseconds>(t_mask2 - t_mask1);
	std::cout << "Mask Duration: " << mask_int.count() << " ms\n";

	io = ImageIOFactory::getIO("pipeline2.pip");
	io->write(image, { 109, 91, 80, 1, 1 });

	cout << "End of Pipeline 2 " << endl;
	
	/* Pipeline 3: Read brain.mhd + Apply Median  + create Image +  Apply Median Filter Sonka + Create Image (compare efficiency, execution time between the 2 filters) */
	cout << "\nStart of Pipeline 3... " << endl;

	io = ImageIOFactory::getIO("brain.mhd");
	image = io->read();				
	
	//apply median filter to reduce noise
	mf.setInput(image);
	mf.setRadius(1);
	mf.setKernelDimensions(1, 1, 1, 0, 0); //3d 
	auto median_1 = high_resolution_clock::now();
	mf.update();
	auto median_2 = high_resolution_clock::now();
	image = mf.getOutput();

	auto median_int = duration_cast<milliseconds>(median_2 - median_1);
	std::cout << "Median Filter Duration: " << median_int.count() << " ms\n";
	
	//write median filter.mhd file
	io = ImageIOFactory::getIO("pipeline3_medianfilter.mhd");
	io->write(image, { 109, 91, 80, 1, 1 });

	io = ImageIOFactory::getIO("brain.mhd");
	image = io->read();							//read from brain.mhd again

	//apply median filter sonka algorithm
	mfs.setInput(image);
	mfs.setRadius(1);
	mfs.setKernelDimensions(1, 1, 1, 0, 0); //3d originally
	auto mediansonka_1 = high_resolution_clock::now();
	mfs.update();
	auto mediansonka_2 = high_resolution_clock::now();
	image = mfs.getOutput();

	auto mediansonka_int = duration_cast<milliseconds>(mediansonka_2 - mediansonka_1);
	std::cout << "Median Filter Sonka Duration: " << mediansonka_int.count() << " ms\n";

	io = ImageIOFactory::getIO("pipeline3_mediansonkafilter.mhd");
	io->write(image, { 109, 91, 80, 1, 1 });

	cout << "End of Pipeline 3" << endl;

	/* for testing error handling */
	//try {
	//	//io = ImageIOFactory::getIO("corrupt_1.pip");	//test corrupt_1.pip
	//	//image = io->read();
	//	//io = ImageIOFactory::getIO("corrupt_2.pip");	//test corrupt_2.pip
	//	//image = io->read();							
	//	//io = ImageIOFactory::getIO("corrupt_3.pip");	//test corrupt_3.pip
	//	//image = io->read();
	//	//io = ImageIOFactory::getIO("corrupt_4.mhd");	//test corrupt_4.mhd
	//	//image = io->read();
	//	//io = ImageIOFactory::getIO("corrupt_5.mhd");	//test corrupt_5.mhd
	//	//image = io->read();
	//	//io = ImageIOFactory::getIO("corrupt_6.mhd");	//test corrupt_6.mhd
	//	//image = io->read();
	//}
	//catch (runtime_error e) {
	//	cout << "\nRuntime Error: " << e.what() << endl;
	//}
}
