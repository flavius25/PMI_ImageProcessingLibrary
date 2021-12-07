#pragma once

/* This header file contains IOHandling for PIP , implementation of read and write functions

For public use
	read()
	write(image, dimensions)

*/

#include "ImageIOBase.h"

namespace fgm_library
{

	class PipImageIO : public ImageIOBase		// PipImageIO is a concrete class that inherits ImageIOBase
	{
	public:
		PipImageIO(const string& filename) : ImageIOBase(filename) {}		// PipImageIO constructor with parsing filename from ImageIoBase as argument 
		~PipImageIO() {};													// simple destructor
		Image read() const override;										// override read function
		void write(const Image&, const array<int, N_DIM>&) const override;  // override write function
	protected:
	private:
	};

	Image PipImageIO::read() const {
		/* defining variables */
		vector<short> v;
		unsigned char data_type = '0';
		unsigned int image_size = 0;
		int xDim = 0;
		int yDim = 0;
		int zDim = 0;
		int cDim = 0;
		int tDim = 0;

		/* open ifstream for reading */
		ifstream ist{ filename, ios_base::binary };
		if (!ist) error("can't open input file ", filename);

		/* reading header information (5D possible) */
		ist.read(as_bytes(data_type), sizeof(data_type));		//reading data_type
		ist.read(as_bytes(xDim), sizeof(xDim));					//reading x_dimension size
		ist.read(as_bytes(yDim), sizeof(yDim));					//reading y_dimension size
		ist.read(as_bytes(zDim), sizeof(zDim));					//reading z_dimension size
		ist.read(as_bytes(cDim), sizeof(cDim));					//reading c_dimension size
		ist.read(as_bytes(tDim), sizeof(tDim));					//reading z_dimension size
		
		image_size = xDim * yDim * zDim * cDim * tDim;			//calculating image_size

		/* Error handling corrupt_1.pip */
		if (data_type < 1 || data_type>4) {
			error("UNKNOWN Pixel Data Type");
		}

		/* Error handling corrupt_2.pip */
		if (xDim < 1 || yDim < 1 || zDim < 1 || cDim < 1 || tDim < 1) {      
			error("Bad image dimensions");
		}

		Image image({ xDim,yDim,zDim,cDim,tDim });				//assign dimensions to image object

		/* read bytes */
		for (short i; ist.read(as_bytes(i), sizeof(short));) {
			
			v.push_back(i);										//read bytes in a vector for proper error handling
		}

		/* Error handling corrupt_3.pip */
		if (image_size > v.size()) {
			error("Image Incomplete");
		}

		copy(v.begin(), v.end(), image.begin());				//use copy to assign pixel values to image object

		/* close ofstream */
		ist.close();

		/* return image object */
		return image;
	}

	void PipImageIO::write(const Image& image, const array<int, N_DIM>& dimensions) const {
		/* open ofstream for writing */
		ofstream ofs{ filename,ios_base::binary };
		if (!ofs) error("can't open output file ", filename);

		/* reading header from argument input, defining variables */
		unsigned char data_type = 2;								//data_type 2 (SHORT)
		const int xDim = dimensions[0];
		const int yDim = dimensions[1];
		const int zDim = dimensions[2];
		const int cDim = dimensions[3];
		const int tDim = dimensions[4];

		/* writing header information (5D possible) */
		ofs.write((char*)&data_type, sizeof(data_type));			//writing data_type
		ofs.write((char*)&xDim, sizeof(xDim));						//writing x_dimension size
		ofs.write((char*)&yDim, sizeof(yDim));						//writing y_dimension size
		ofs.write((char*)&zDim, sizeof(zDim));						//writing z_dimension size
		ofs.write((char*)&cDim, sizeof(cDim));						//writing c_dimension size
		ofs.write((char*)&tDim, sizeof(tDim));						//writing z_dimension size

		/* write bytes */
		for (Image::const_reference x : image) {
			ofs.write(as_bytes(x), sizeof(Image::value_type)); 
		}

		/* close ofstream */
		ofs.close();
	}

}