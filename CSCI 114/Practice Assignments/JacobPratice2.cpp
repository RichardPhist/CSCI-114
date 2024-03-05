#include <iostream>
#include <vector>
#include <thread>

using namespace std;

// function prototype
vector<vector<int>> matrixMultiplication(const vector<vector<int>> first,
										 const vector<vector<int>> second);
void printMatrix(const vector<vector<int>> &vect);
void vectorDotProduct(const vector<int> row,
					  const vector<int> col,
					  int &result);
vector<vector<int>> transposeMatrix(const vector<vector<int>> &vect);

int main()
{
	vector<vector<int>> test0 = {
		{1, 2},
		{0, 1},
		{2, 5},
		{10, -4}};
	vector<vector<int>> test1 = {
		{1, 4, 5},
		{-2, 3, 1}};

	printMatrix(matrixMultiplication(test0, test1));

	return 0;
}

vector<vector<int>> transposeMatrix(const vector<vector<int>> &vect)
{
	vector<vector<int>> t{vect[0].size(), vector<int>(vect.size(), 0)};

	for (size_t i = 0; i < vect.size(); i++)
	{
		for (size_t j = 0; j < vect[0].size(); j++)
		{
			t[j][i] = vect[i][j];
		}
	}

	return t;
}

void vectorDotProduct(const vector<int> row, const vector<int> col, int &result)
{
	// check that vectors can be multiplied
	if (row.size() != col.size())
	{
		cout << "Row and columns must be same size to perform dot product!" << endl;
		return;
	}

	// get track total of dot product
	int total = 0;
	for (size_t i = 0; i < row.size(); i++)
		total += row[i] * col[i];

	result = total;
}

vector<vector<int>> matrixMultiplication(const vector<vector<int>> first, const vector<vector<int>> second)
{
	// return empty vector with warning if matricies cannot be multiplied
	if (first[0].size() != second.size())
	{
		cout << "Matricies of these shapes cannot be multiplied!" << endl;
		return vector<vector<int>>();
	}

	// create vectors to store answer and transpose of second matrix (makes matrix multiplication simpler)
	vector<vector<int>> answer{first.size(), vector<int>(second[0].size(), 0)};
	vector<vector<int>> transpose = transposeMatrix(second);

	// vector to track threads
	vector<thread> myThreads;

	// compute new vect
	for (size_t i = 0; i < first.size(); i++)
	{
		for (size_t j = 0; j < transpose.size(); j++)
		{
			myThreads.push_back(thread(vectorDotProduct,
									   first[i],
									   transpose[j],
									   ref(answer[i][j])));
		}
	}

	// wait for all threads to finish
	for (auto &th : myThreads)
		th.join();

	return answer;
}

void printMatrix(const vector<vector<int>> &vect)
{
	for (vector<int> row : vect)
	{
		for (int col : row)
		{
			cout << col << "\t";
		}
		cout << "\n";
	}
}