#pragma once
class Cell
{
public:
	int row, col;
	Cell()
	{
		row = col = -1;
	}

	Cell(int row, int col) {
		this->row = row;
		this->col = col;
	}

	bool operator== (const Cell& c) const{
		return (row == c.row) && (col == c.col);
	}
	bool operator<(const Cell& c) const {
		if (row < c.row) return true;
		if (row > c.row) return false;

		return col < c.col;
	}
};

