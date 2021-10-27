#include "Matrix.h"

Matrix::Matrix() :m_size(new Size {0,0})
{}

Matrix::Matrix(std::vector<std::vector<float>>*a_matrix) : m_size(new Size {(int)a_matrix->size(),(int)a_matrix[0][0].size()})
{
	operator=(a_matrix);
}

Matrix::~Matrix()
{}

Size Matrix::size()
{
	return *m_size;
}

Matrix& Matrix::matrixMulti(const Matrix m1, const Matrix m2)
{
	Matrix* m3 = new Matrix;
	if(m1.m_size->colum == m2.m_size->row)
	{
		//m3 = new Matrix/*(m2.m_size->row, m1.m_size->colum)*/;
		for(int a = 0; a < m1.m_size->row; a++)
		{
			m3->m_matrix->push_back(std::vector<float>());
			for(int b = 0; b < m2.m_size->colum; b++)
			{
				float* m1v = new float[m1.m_size->colum];
				float *m2v = new float[m2.m_size->row];
				float value = 0;
				for(int d = 0; d < m1.m_size->colum; d++)
				{
				   //m1 values  					
					m1v[d] = m1.m_matrix[0][a][d];

				   //m2 values
					m2v[d] = m2.m_matrix[0][d][b];

				   //multiplication
					m1v[d] = m1v[d] * m2v[d];

				   //addition
					value += m1v[d];
				}
				(*m3->m_matrix)[a].push_back(value);
			}
		}
		*m3 = m3->m_matrix;
	}
	return *m3;

}

//Matrix& Matrix::rotaton(float x, float y, float z)
//{
//	Matrix position;
//	return position;
//}

void Matrix::scale(float x, float y, float z)
{
	if(m_size->row == 1 && m_size->colum == 4)
	{
		m_matrix[0][0][1] *= x;
		m_matrix[0][0][2] *= y;
		m_matrix[0][0][3] *= z;
	}
}

void Matrix::scale(float xyz)
{
	scale(xyz, xyz, xyz);
}

void Matrix::shear(float x, float y, float z)
{
	if(m_size->row == 1 && m_size->colum == 4)
	{
		std::vector<std::vector<float>> rz
		{
			{1,y,0},
			{x,1,0},
			{0,0,1}
		};
		std::vector<std::vector<float>> rx
		{
			{1,0,0},
			{0,1,y},
			{0,z,1}
		};
		std::vector<std::vector<float>> ry
		{
			{1,0,z},
			{0,1,0},
			{x,0,1}
		};
		std::vector<std::vector<float>> mx {{this[0][0][1], this[0][0][2], this[0][0][3]}};
		matrixMulti(&mx, &rx);
		matrixMulti(&mx, &ry);
		matrixMulti(&mx, &rz);
		this[0][0][1] = mx[0][0];
		this[0][0][2] = mx[0][1];
		this[0][0][3] = mx[0][2];
	}
}

void Matrix::shear(float xyz)
{
	shear(xyz, xyz, xyz);
}

void Matrix::print()
{
//	OutputDebugString("Stuff\n");
	for(int a = 0; a < m_size->row; a++)
	{
		printf("[");
		for(int b = 0; b < m_size->colum; b++)
		{
			printf(std::string("%f" + std::string(b != m_size->colum - 1 ? ", " : "")).c_str(), (*m_matrix)[a][b]);
		}
		printf("]\n");
	}
	printf("\n");
}

std::vector<std::vector<float>>* Matrix::getMatrix()
{
	return m_matrix;
}

void Matrix::operator=(const std::vector<std::vector<float>>*a_matrix)
{
	m_size->row = (int)(a_matrix->size()); //rows 
	m_size->colum = (int)((*a_matrix)[0].size()); //cols
	//delete m_matrix;
	//m_matrix = new std::vector<std::vector<float>>;
	*m_matrix = *a_matrix;
}

std::vector<float> Matrix::operator[](int m_index)
{
	return m_matrix[0][m_index];
}
