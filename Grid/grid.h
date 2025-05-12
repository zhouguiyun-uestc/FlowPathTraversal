#pragma once
#include <vector>
#include <array>
#include <exception>
#include <stdexcept>
#include "cell.h"
#include "flowDir.h"
#include <atomic>
#include <memory>
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
		noDataValue_ = FlowDir::NoData;
		//throw std::exception("Nodata set to FlowDir::NoData");
	}

	void setNoDataValue(double no_data)
	{
		noDataValue_ = (T)no_data;
	}

	void allocate(const T& noDataValue =T())
	{
		if (height_ <= 0 || width_ <= 0) {
			throw std::runtime_error("the height or width of the grid is less than or equal to zero.");
		}

		noDataValue_ = noDataValue;
		data_.resize((unsigned int)height_ * (unsigned int)width_,noDataValue_);
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
		unsigned int index = (unsigned int)cell.row * (unsigned int)width_ + (unsigned int)cell.col;

		if (index >= (unsigned int)height_ * (unsigned int)width_)
			throw std::runtime_error("the index of the cell is invalid");

		data_.at(index)+=value;
#else
		unsigned int index = (unsigned int)cell.row * (unsigned int)width_ + (unsigned int)cell.col;
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
		unsigned int index = (unsigned int)cell.row * (unsigned int)width_ + (unsigned int)cell.col;

		if (index >= (unsigned int)height_ * (unsigned int)width_)
			throw std::runtime_error("the index of the cell is invalid");

		return data_.at(index);
#else
		unsigned int index = (unsigned int)cell.row * (unsigned int)width_ + (unsigned int)cell.col;
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
		unsigned int index = (unsigned int)row * (unsigned int)width_ + (unsigned int)col;

		if (index >= (unsigned int)height_ * (unsigned int)width_)
			throw std::exception("the index of the cell is invalid");

		return data_.at(index);
#else
		unsigned int index = (unsigned int)row * (unsigned int)width_ + (unsigned int)col;
		return data_[index];
#endif 
	}
	
	void changeNoDataValue(T newNoData, double orgNoData) {
		unsigned int total = (unsigned int)height_ * (unsigned int)width_;
		T* pSelf = getDataPtr();
		for (unsigned int index = 0; index < total; index++) 
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
		unsigned int total = (unsigned int )height_ * (unsigned int )width_;
		T2* pTarget = grid.getDataPtr();
		T* pSelf = getDataPtr();
		for (unsigned int index = 0; index < total; index++) {
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

