#pragma once
#include <vector>
#include <string>
#define degtorad(deg) (deg*M_PI/180)

struct Size
{
	int row, colum;
	int& operator[](int m_index)
	{
		switch(m_index)
		{
		case 0:
			return row;
		case 1:
			return colum;
		}
		int *a = nullptr;
		return *a;
	}
};

class Matrix
{
public:

	Matrix();
	Matrix(std::vector<std::vector<float>>*);
	~Matrix();

	Size size();

	static Matrix& matrixMulti(const Matrix m1, const Matrix m2);
	Matrix& rotaton(float xRot, float yRot, float zRot);

	void scale(float x , float y , float z = 1);
	void scale(float xyz);
	void shear(float x , float y , float z = 1);
	void shear(float xy);

	void print();

	std::vector<std::vector<float>>* getMatrix();

	void operator=(const std::vector<std::vector<float>>*);
	std::vector<float> operator[](int);

protected:
	std::vector<std::vector<float>>* m_matrix = new std::vector<std::vector<float>>();
	Size *m_size;
};

