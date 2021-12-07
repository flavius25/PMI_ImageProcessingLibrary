#pragma once

/* This file is ImageIOFactory.h

String filename is processed and depending on the type of the file, objects to PipImageIO or MhdImageIO are returned.

*/

#include "ImageIOBase.h"
#include "PipImageIO.h"
#include "MhdImageIO.h"

namespace fgm_library
{

	class ImageIOFactory
	{
	public:
		static unique_ptr<ImageIOBase> getIO(const string& filename);		// public function to get filename from main.cpp
	protected:
	private:
	};

	/* function to process filename information  */
	unique_ptr<ImageIOBase> ImageIOFactory::getIO(const string& filename) {
		/* defining variables */
		const string delimiter = ".";
		const string pip = "pip";
		const string mhd = "mhd";

		auto pos = filename.find(delimiter);
		auto file_type = filename.substr(pos);			// split from delimiter inclusive
		file_type.erase(0, delimiter.length());

		if (file_type == pip) {
			return unique_ptr<ImageIOBase> {new PipImageIO(filename)};			//if file is pip, return PipImageIO object with filename unique_ptr (the managing unique_ptr object is destroyed automatically after use)
		}
		else if (file_type == mhd)
		{
			return unique_ptr<ImageIOBase> {new MhdImageIO(filename)};			//if file is mhd, return PipImageIO object with filename
		}
		else {
			cout << "not a right file name" << endl;
		}
		return nullptr;
	}

}