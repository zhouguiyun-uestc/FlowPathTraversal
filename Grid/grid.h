#pragma once
#include <vector>
#include <array>
#include <exception>
#include <stdexcept>
#include "cell.h"
#include "flowDir.h"
#include <atomic>
#include <memory>
#include <string>
#include <limits>

//#define _MASSIVE_DATASET_
#ifdef _MASSIVE_DATASET_
	using size_type = long long;
#else
	using size_type = unsigned int;
#endif


template<typename T>
class Grid
{
private:
	std::vector<T> data_;
	//std::unique_ptr<std::vector<T>> data_;
	int height_, width_;
	T noDataValue_;
	std::array<double,6> geoTransform_;
public:
	Grid() {
		height_ = 0;
		width_ = 0;
		geoTransform_[0] = 0; geoTransform_[1] = 1; geoTransform_[2] = 0;
		geoTransform_[3] = 0; geoTransform_[4] = 0; geoTransform_[5] = 1;
	}
	Grid(int height, int width) {
		height_ = height;
		width_ = width;
		geoTransform_[0] = 0; geoTransform_[1] = 1; geoTransform_[2] = 0;
		geoTransform_[3] = 0; geoTransform_[4] = 0; geoTransform_[5] = 1;
	}
	
	template<typename T2>
	Grid(Grid<T2>& grid) {
		height_ = grid.height();
		width_ = grid.width();
		geoTransform_ = grid.GeoTransform();
	}

	std::array<double, 6>& GeoTransform()
	{
		return geoTransform_;
	}

	T& noDataValue()
	{
		return noDataValue_;
	}

	int& height()
	{
		return height_;
	}
	int& width()
	{
		return width_;
	}

	
	void setNoDataValue(double no_data) requires (std::is_same<T, FlowDir>::value)
	{		
		int no_data_value = (int)no_data;	
		if (no_data_value==0)
			noDataValue_ = FlowDir::NoData_Zero;
		else if (no_data_value == 255)
			noDataValue_ = FlowDir::NoData_255;
		else
			throw std::runtime_error("No_Data value of a flow direction grid is expected to be zero or 255. The input value is "+ std::to_string(no_data_value));
	}

	void setNoDataValue(double no_data)
	{
		noDataValue_ = (T)no_data;
	}

	void allocate()
	{
		if (height_ <= 0 || width_ <= 0) {
			throw std::runtime_error("the height or width of the grid is less than or equal to zero.");
		}

		//noDataValue_ = noDataValue;
#ifndef _MASSIVE_DATASET_
		if ((long long)height_ * (long long)width_ >= (long long)std::numeric_limits<unsigned int>::max()) {
			throw std::runtime_error("The grid to be allocated is too big for the current version. Please recompile the code with the _MASSIVE_DATASET_ predefined macro.");
		}
#endif
		data_.resize((size_type)height_ * (size_type)width_, noDataValue_);
	}


	T* getDataPtr()
	{
		//return (*data_).data();
		return data_.data();
	}
	void add(const Cell& cell, const T& value) {
#ifdef DEBUG
		if (cell.row < 0 || cell.row >= height_ || cell.col < 0 || cell.col >= width_) {
			throw std::runtime_error("the index of the cell is invalid");
		}
		size_type index = (size_type)cell.row * (size_type)width_ + (size_type)cell.col;

		if (index >= (size_type)height_ * (size_type)width_)
			throw std::runtime_error("the index of the cell is invalid");

		data_.at(index)+=value;
#else
		size_type index = (size_type)cell.row * (size_type)width_ + (size_type)cell.col;
		data_[index]+=value;
#endif 
	}

	//overload for bool type
	std::vector<bool>::reference operator() (const Cell& cell) requires std::is_same<T, bool>::value
	{
#ifdef DEBUG
		if (cell.row < 0 || cell.row >= height_ || cell.col < 0 || cell.col >= width_) {
			throw std::runtime_error("the index of the cell is invalid");
		}
		size_type index = (size_type)cell.row * (size_type)width_ + (size_type)cell.col;

		if (index >= (size_type)height_ * (size_type)width_)
			throw std::runtime_error("the index of the cell is invalid");

		return data_.at(index);
#else
		//
		size_type index = (size_type)cell.row * (size_type)width_ + (size_type)cell.col;
		//unsigned int index = (unsigned int)cell.row * (unsigned int)width_ + (unsigned int)cell.col;
		return data_[index];
#endif 

	}

	T& operator() (const Cell& cell) requires (!std::is_same<T, bool>::value)
	{
		return (*this)(cell.row, cell.col);
	}

	T& operator()(int row, int col) requires (!std::is_same<T,bool>::value)
	{
#ifdef DEBUG
		if (row < 0 || row >= height_ || col < 0 || col >= width_) {
			throw std::exception("the index of the cell is invalid");
		}
		size_type index = (size_type)row * (size_type)width_ + (size_type)col;

		if (index >= (size_type)height_ * (size_type)width_)
			throw std::runtime_error("the index of the cell is invalid");

		return data_.at(index);
#else
		size_type index = (size_type)row * (size_type)width_ + (size_type)col;
		//unsigned int index = (unsigned int)row * (unsigned int)width_ + (unsigned int)col;
		return data_[index];
#endif 
	}
	
	void changeNoDataValue(T newNoData, double orgNoData) {
		size_type total = (size_type)height_ * (size_type)width_;
		T* pSelf = getDataPtr();
		for (size_type index = 0; index < total; index++)
		{
			if (std::abs(pSelf[index] - orgNoData) < 0.000001)
				pSelf[index] = newNoData;
		}
		noDataValue_ = newNoData;
	}

	template<typename T2>
	Grid<T2> to()  
	{
		Grid<T2> grid(*this);
		grid.allocate();
		size_type total = (size_type)height_ * (size_type)width_;
		T2* pTarget = grid.getDataPtr();
		T* pSelf = getDataPtr();
		for (size_type index = 0; index < total; index++) {
			pTarget[index] = pSelf[index];
		}
		return grid;
	}

	bool isNoData(const Cell& cell) requires std::is_integral_v<T> 
	{
		if ((*this)(cell.row, cell.col) == noDataValue_) return true;
		return false;
	}

	bool isNoData(const Cell& cell) requires std::is_same<T,FlowDir>::value
	{
		if ((*this)(cell.row, cell.col) == noDataValue_) return true;
		return false;
	}

	//for float, requires C++ 20
	bool isNoData(const Cell& cell)  requires (!std::is_integral_v<T> && !std::is_same<T, FlowDir>::value)
	{
		if (std::abs(((*this)(cell.row, cell.col) - noDataValue_) < 0.000001)) return true;
		return false;
	}

	bool isInGrid(const Cell& cell)  {
		if ((cell.row >= 0 && cell.row < height_) && (cell.col >= 0 && cell.col < width_))
			return true;
		return false;
	}
};

