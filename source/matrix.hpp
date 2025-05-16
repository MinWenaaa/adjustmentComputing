#pragma once
#include <math.h>
#include <string>
#include <sstream>

namespace WenMin {
	template <typename T>
	class Matrix {
	public:
		Matrix(int rows, int cols) : rows(rows), cols(cols) {
			data = new T[rows * cols];
			for (int i = 0; i < rows * cols; ++i)
				data[i] = 0;
		}
		Matrix(int rows, int cols, std::string data);
		Matrix(const Matrix<T>& other) : rows(other.rows), cols(other.cols) {
			data = new T[rows * cols];
			for (int i = 0; i < rows * cols; ++i)
				data[i] = other.data[i];
		}
		~Matrix() {
			//delete[] data;
		}

		int getRows() const { return rows };
		int getCols() const { return cols };

		Matrix<T> operator*(const Matrix<T>& other) const;
		Matrix<T> operator+(const Matrix<T>& other) const;
		Matrix<T> operator-(const Matrix<T>& other) const;
		Matrix<T> inverse();
		Matrix<T> transpose() const;
		std::string toString();

		T& operator()(int i, int j) { return data[i * cols + j]; }

	private:
		int rows, cols;
		T* data;
	};

	template <typename T>
	Matrix<T>::Matrix(int rows, int cols, std::string input) : rows(rows), cols(cols) {
		std::istringstream s(input);
		data = new T[rows * cols];
		for (int i = 0; i < rows * cols; ++i)
			s >> data[i];
	}

	// æÿ’Û≥À∑®
	template <typename T>
	Matrix<T> Matrix<T>::operator*(const Matrix<T>&other) const{
		if (cols != other.rows) {
			throw std::invalid_argument("Matrix dimensions do not match for multiplication");
		}
		Matrix<T> result(rows, other.cols);
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < other.cols; ++j) {
				result.data[i * other.cols + j] = 0;
				for (int k = 0; k < cols; ++k) {
					result.data[i * other.cols + j] += data[i * cols + k] * other.data[k * other.cols + j];
				}
			}
		}
		return result;
	}

	// æÿ’Û◊™÷√	
	template <typename T>
	Matrix<T> Matrix<T>::transpose() const{
		Matrix<T> result(cols, rows);
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				result.data[j * rows + i] = data[i * cols + j];
			}
		}
		return result;
	}

	// æÿ’Û«ÛƒÊ
	template <typename T> 
	Matrix<T> Matrix<T>::inverse() {
		if (rows != cols) {
			throw std::invalid_argument("Matrix must be square to compute inverse");
		}
		Matrix<T> t(rows, cols);
		Matrix<T> R(rows, cols);
		
		for (int i = 0; i < rows*cols; ++i) {
			t.data[i] = 0; R.data[i] = 0; 
		}	
		
		// «ÛTæÿ’Û
		t(0, 0) = sqrt(abs((*this)(0, 0)));
		for (int j = 1; j < cols; ++j) {
			t(0, j) = (*this)(0, j) / t(0, 0);
		}	
		for (int i = 1; i < rows; ++i) {
			T sum = 0;
			for (int k = 0; k < i; ++k) 
				sum += t(k, i) * t(k, i);
			t(i, i) = sqrt((*this)(i, i) - sum);
			for (int j = i + 1; j < cols; ++j) {
				sum = 0;
				for (int k = 0; k < i; ++k)
					sum += t(k, i) * t(k, j);
				t(i, j) = ((*this)(i, j) - sum) / t(i, i);
			}	
		}	

		// «ÛRæÿ’Û	
		for (int i = 0; i < rows; ++i) {
			R(i, i) = 1 / t(i, i);
			for (int j = i + 1; j < cols; ++j) {
				T sum = 0;
				for (int k = i; k < j; ++k)
					sum += t(k, j) * R(i, k);
				R(i, j) = -sum / t(j, j);
			}
		}	

		Matrix<T> RT = R.transpose();
		return R*RT;
	}

	template <typename T>
	std::string Matrix<T>::toString() {
		std::string result = "";
		for (int i = 0; i < rows; ++i) {
			for (int j = 0; j < cols; ++j) {
				result += (std::to_string((*this)(i, j)) + " ");
			}
			result += "\n";
		}
		return result;
	}

	template <typename T>	
	Matrix<T> Matrix<T>::operator+(const Matrix<T>& other) const {
		if (rows != other.rows || cols != other.cols) {
			throw std::invalid_argument("Matrix dimensions do not match for addition");
		}
		Matrix<T> result(rows, cols);
		for (int i = 0; i < rows * cols; ++i) {
			result.data[i] = data[i] + other.data[i];
		}
		return result;
	}

	template <typename T>
	Matrix<T> Matrix<T>::operator-(const Matrix<T>& other) const {
		if (rows != other.rows || cols != other.cols) {
			throw std::invalid_argument("Matrix dimensions do not match for subtraction");
		}
		Matrix<T> result(rows, cols);
		for (int i = 0; i < rows * cols; ++i) {
			result.data[i] = data[i] - other.data[i];
		}
		return result;
	}
}