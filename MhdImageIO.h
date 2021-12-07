#pragma once

/* This header file contains IOHandling for MHD , implementation of read and write functions

For public use
	read()
	write(image, dimensions)

*/

#include "ImageIOBase.h"

namespace fgm_library
{

	class MhdImageIO : public ImageIOBase	// MhdImageIO is a concrete class that inherits ImageIOBase
	{
	public:
		MhdImageIO(const string& filename) : ImageIOBase(filename) {}		// MhdImageIO constructor with parsing filename from ImageIoBase as argument
		~MhdImageIO() {};													// simple destructor
		Image read() const override;										// override read function
		void write(const Image&, const array<int, N_DIM>&) const override;	// override write function
	protected:
	private:
	};

	Image MhdImageIO::read() const {
		/* defining variables, strings of interest */
		string s_Ndims = "NDims";
		string s_Dimsize = "DimSize";
		string s_Elemtype = "ElementType";
		string s_Elemtype_Ref = "MET_SHORT";
		string s_Elemdatafile = "ElementDataFile";
		string delimiter = "=";
		string line;
		string raw_filename;
		string element_type;
		size_t pos = 0;
		vector<int> dimensions = { 1,1,1,1,1 };				//initialize vector dimensions with 1 (5D possible)
		vector<short> v;
		int NDims = 0;
		unsigned int image_size = 1;

		/* open ifstream for reading */
		ifstream ist{ filename };
		if (!ist) error("can't open input file ", filename);

		/* explore the input file line by line */
		while (getline(ist, line)) {

			if (line.find(s_Ndims) != string::npos) {		//search for the line with NDims
				pos = line.find_last_of(delimiter);			//find position (pos) of '=' in this line
				auto Ndim = line.substr(pos + 1);			//create substring from pos + 1 ('=') itself 
				stringstream(Ndim) >> NDims;				//parsing the string as int (ignoring whitespaces (space), use of stringstream)
			}

			if (line.find(s_Dimsize) != string::npos) {		//search for the line with DimSize
				pos = line.find_last_of(delimiter);			//find position (pos) of '=' in this line
				auto size = line.substr(pos + 1);			//create substring from pos + 1 ('=') itself  
				stringstream ss{ size };
				int i = 0;
				for (int i_size; ss >> i_size;) {			//parsing the string as int (ignoring whitespaces (space), use of stringstream) . Updating dimensions vector.
					dimensions.at(i) = i_size;
					i++;
				}
			}

			if (line.find(s_Elemtype) != string::npos) {		//search for the line with ElementType
				pos = line.find_last_of(delimiter);				//find position (pos) of '=' in this line
				auto el_type = line.substr(pos + 1);			//create substring from pos + 1 ('=') itself 
				stringstream(el_type) >> element_type;			//parsing the string as string (ignoring whitespaces (space), use of stringstream)
			}

			if (line.find(s_Elemdatafile) != string::npos) {		//search for the line with ElementDataFile
				pos = line.find_last_of(delimiter);					//find position (pos) of '=' in this line
				auto raw = line.substr(pos + 1);					//create substring from pos + 1 ('=') itself
				stringstream(raw) >> raw_filename;					//parsing the string as string raw_filename (ignoring whitespaces (space), use of stringstream)
			}
		}

		/* close ifstream */
		ist.close();

		/* assign dimensions to image object */
		Image image({ dimensions[0],dimensions[1],dimensions[2],dimensions[3],dimensions[4] });

		/* Error handling corrupt_4.mhd */
		if (NDims != image.nr_dims()) {
			error("Number of dimensions does not match the DimSize vector elements "); //
		}

		/* Error handling corrupt_6.mhd */
		if (element_type != s_Elemtype_Ref) {
			error("Unknown Data Type");
		}

		/* open ifstream for reading a raw file */
		ifstream ist_raw{ raw_filename, ios_base::binary };
		if (!ist_raw) error("can't open input file ", raw_filename);

		/* read bytes */
		for (short i; ist_raw.read(as_bytes(i), sizeof(short));) {
			v.push_back(i);
		}

		for (const auto& i : dimensions) {
			image_size *= i;															//multiply dimensions size elements to find out the required image_size 
		}

		/* Error handling corrupt_5.mhd */
		if (image_size < v.size()) {
			error("The raw_data vector size is bigger than the required image_size"); 
		}

		copy(v.begin(), v.end(), image.begin());										//use copy to assign pixel values to image object

		/* return image object */
		return image;
	}


	void MhdImageIO::write(const Image& image, const array<int, N_DIM>& dimensions) const {
		/* defining variables, strings of interest */
		vector<string> header_document = {
			"ObjectType = Image",
			"NDims = ",
			"BinaryData = True",
			"BinaryDataByteOrderMSB = False",
			"CompressedData = False",
			"TransformMatrix = 0.031386405548051721 0.99950717667896094 -0.00054526509638437104 0.029358096516141685 -0.0014671998258907449 -0.99956788138356056 -0.99907607103350782 0.031356834952584761 -0.029389678290272659", //make sure to write these values well ( they are negative values)
			"Offset = 73.4191 -132.86099999999999 119.277",
			"CenterOfRotation = 0 0 0",
			"AnatomicalOrientation = ASL",
			"ElementSpacing = 1.9531199932098389 1.9531300067901611 2",
			"DimSize = ",
			"ElementType = MET_SHORT"
		};
		string Elemdatafile = "ElementDataFile = ";
		string delimiter = ".";
		const string change_type = ".raw";
		string dimsize_forwrite;

		/* change from mhd to raw file type */
		auto pos = filename.find(delimiter);						// find . delimiter
		auto file_type = filename.substr(pos);						// split from delimiter inclusive (find .mhd)
		file_type = change_type;									// change .mhd to .raw	
		auto raw_filename = filename.substr(0, pos) + file_type;	// create the new raw_filename by concatenating .raw to the first part of the string

		/* print NDims and DimSize values in .mhd based on input image */
		header_document[1] = header_document[1] + to_string(image.nr_dims());		//convert nr_dims to string and print in .mhd file
		for (unsigned int x = 0; x < image.nr_dims(); x++) {
			char space = ' ';
			dimsize_forwrite += to_string(image.dim(x)) + space;
		}
		header_document[10] = header_document[10] + dimsize_forwrite;				//print image dimensions in .mhd file

		/* open ofstream for writing .mhd file */
		ofstream ofs{ filename };
		if (!ofs) error("can't open output file ", filename);

		/* write .mhd file */
		if (ofs.is_open())
		{
			for (int i = 0; i < header_document.size(); i++) {
				ofs << header_document[i] << "\n";
			}
			ofs << Elemdatafile << raw_filename << endl;			// print the last line of the .mhd file
			ofs.close();											// close ofstream for writing .mhd
		}

		/* open ofstream for writing .raw file */
		ofstream ofs_raw{ raw_filename, ios_base::binary };
		if (!ofs_raw) error("can't open output file ", raw_filename);

		/* write pixel values (bytes) in .raw file */
		for (Image::const_reference x : image) {
			ofs_raw.write(as_bytes(x), sizeof(Image::value_type));
		}

		/* close ofstream for writing .raw */
		ofs_raw.close();
	}
}