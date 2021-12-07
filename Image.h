#pragma once

/* This file is Image.h

It provides the constructors and destructors, the assignment operators, iterators, size/resize functions and operators to use in within the image fgm_library.

*/

#include "std_lib_facilities.h"
#include <numeric>



static constexpr int N_DIM{ 5 };							//only possible for 5D atm

namespace fgm_library
{

	class Image
	{
	public:
		/* Internally used data types */
		typedef short T;
		typedef std::array<int, N_DIM> dimension;

		/* STL containerand iterator typedefs */
		typedef T         value_type;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef T& reference;
		typedef const T& const_reference;

		/* Constructors and destructor */
		Image(dimension dim) : _dimensions{ dim }, _data{ new T[dim[0] * dim[1] * dim[2] * dim[3] * dim[4]] } {}   //for creating blank images (allocating data of product of image dimensions)
		Image(const Image& i) : _dimensions{ i._dimensions }, _data{ new T[i._dimensions[0] * i._dimensions[1] * i._dimensions[2] * i._dimensions[3] * i._dimensions[4]] } {  //for having pixel intensities + dimensions. Making a copy of the image
			copy(i.begin(), i.end(), _data);
		}
		Image(Image&& i) noexcept : _dimensions{ i._dimensions }, _data{ i._data }  {		//move operator constructor. //add noexcept to suppress C26495 warning
			i._dimensions = { 1,1,1,1,1 };													//point dimensions to other values 
			i._data = nullptr;																//make i._data null
		}

		virtual ~Image() {		//virtual destructor
			delete[]_data;		//delete _data 
		}

		/* Assignment operators */
		Image& operator=(const Image& i) {
			auto num_voxels = accumulate(i._dimensions.begin(), i._dimensions.end(), 1, multiplies<>());
			T* p = new T[i._dimensions[0] * i._dimensions[1] * i._dimensions[2] * i._dimensions[3] * i._dimensions[4]];	//alocate new space
			for (int x = 0; x < num_voxels; ++x) {
				p[x] = i._data[x];						//assign data to space location
			}
			delete[] _data;								//deallocate old space
			_dimensions = i._dimensions;				//save new dimensions
			_data = p;									//reset _data
			return *this;								//return a self-reference
		}
		Image& operator=(Image&& i) noexcept {			//add noexcept to suppress C26495 warning
			delete[] _data;								//deallocate old space
			_data = i._data;							//copy i's data
			_dimensions = i._dimensions;				//copy i's dimensions' size
			i._data = nullptr;							//make i the empty vector
			i._dimensions = { 1,1,1,1,1 };				//point dimensions to other values 
			return *this;								//return self-reference
		}

		/* Basic iterators */
		iterator begin() {
			return _data;
		}
		iterator end() {
			return _data + accumulate(_dimensions.begin(), _dimensions.end(), 1, multiplies<>());  // 1 (_data) is initial value, using accumulate to find the last value
		}
		const_iterator begin() const {
			return _data;
		}
		const_iterator end() const {
			return _data + accumulate(_dimensions.begin(), _dimensions.end(), 1, multiplies<>());
		}

		/* Size and resize */
		dimension size() const {				 //the dimension object  (returning _dimensions object)
			return _dimensions;
		}
		unsigned int dim(unsigned int d) const { //get the size of a dimension (returning _dimension[0], [1]. [2])
			return _dimensions[d];
		}

		unsigned int nr_dims() const {		     //if number of dimensions > 1 return count
			unsigned int countDim = 0;
			for (auto x : _dimensions) {
				if (x > 1) countDim++;
			}
			return countDim;
		}
		void resize(const dimension& d) {						// resize this image // if the image should be bigger or smaller what should be done 
			_dimensions = d;									// save dimensions
			if (_data) delete[] _data;							// delete actual pixel values
			_data = new T[d[0] * d[1] * d[2] * d[3] * d[4]]();	// create empty (0 pixel values) of the size of the image dimensions 
		}

		/* Pixel value lookup, should support out - of - image coordinates by clamping to 0..dim */
		value_type operator()(int x = 0, int y = 0, int z = 0, int c = 0, int t = 0) const {
			/* range checking */
			x = max(0, min(x, _dimensions[0] - 1));   //clamping : if x = -2 _dimension[0] = 119 then MIN of them would be -2. Applying a MAX to this with 0, will take 0 as lower clamping limit. The same works in case x > 119, in that case we would have MIN(120,119) =  119 and MAX (0,119) which will always be 119 (higher clamping limit).
			y = max(0, min(y, _dimensions[1] - 1));
			z = max(0, min(z, _dimensions[2] - 1));
			c = max(0, min(c, _dimensions[3] - 1));
			t = max(0, min(t, _dimensions[4] - 1));

			return _data[(t * _dimensions[3] * _dimensions[0] * _dimensions[1] * _dimensions[2]) + (c * _dimensions[0] * _dimensions[1] * _dimensions[2]) + (z * _dimensions[0] * _dimensions[1]) + (y * _dimensions[0]) + x];
		}
		reference operator()(int x = 0, int y = 0, int z = 0, int c = 0, int t = 0) {
			///* range checking */ {
			x = max(0, min(x, _dimensions[0] - 1));   //clamping : if x = -2 _dimension[0] = 119 then MIN of them would be -2. Applying a MAX to this with 0, will take 0 as lower clamping limit. The same works in case x > 119, in that case we would have MIN(120,119) =  119 and MAX (0,119) which will always be 119 (higher clamping limit).
			y = max(0, min(y, _dimensions[1] - 1));
			z = max(0, min(z, _dimensions[2] - 1));
			c = max(0, min(c, _dimensions[3] - 1));
			t = max(0, min(t, _dimensions[4] - 1));

			return _data[(t * _dimensions[3] * _dimensions[0] * _dimensions[1] * _dimensions[2]) + (c * _dimensions[0] * _dimensions[1] * _dimensions[2]) + (z * _dimensions[0] * _dimensions[1]) + (y * _dimensions[0]) + x];
		}

	private:
		dimension _dimensions;
		T* _data;
	};

}
