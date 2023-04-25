#include<omp.h>
#include<iostream>
#include"mpi.h"

using namespace std;

void merge(int arr[], int start, int mid, int end) {

	// Create L ← A[start..mid] and M ← A[mid+1..end]
	int n1 = mid - start + 1;
	int n2 = end - mid;

	int L[10], M[10];
	for (int i = 0; i < n1; i++)
		L[i] = arr[start + i];
	for (int j = 0; j < n2; j++)
		M[j] = arr[mid + 1 + j];

	// Maintain current index of sub-arrays and main array
	int i, j, k;
	i = 0;
	j = 0;
	k = start;
	while (i < n1 && j < n2) {
		if (L[i] <= M[j]) {
			arr[k] = L[i];
			i++;
		}
		else {
			arr[k] = M[j];
			j++;
		}
		k++;
	}
	while (i < n1) {
		arr[k] = L[i];
		i++;
		k++;
	}
	while (j < n2) {
		arr[k] = M[j];
		j++;
		k++;
	}
};
void mergeSort(int arr[],  int const begin, int const end) {
	if (begin >= end)
		return; // Returns recursively

	int mid = begin + (end - begin) / 2;
#pragma omp parallel sections num_threads(2)
	{
#pragma omp section
		{
			mergeSort(arr, begin, mid);
		}
#pragma omp section
		{
			mergeSort(arr, mid + 1, end);
		}
	}
	merge(arr, begin, mid, end);
};

int main(int argc, char** argv) {



	int rows, cols;
	cout << "Enter rows: ";
	cin >> rows;
	cout << "Enter cols: ";
	cin >> cols;

	int** matrix = new int* [rows];
	for (int i = 0; i < rows; i++) {
		matrix[i] = new int[cols];
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = rand() % 100;
		}
	}
	cout << "Matrix: \n";
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			cout << matrix[i][j] << "	";
		}
		cout << endl;
	}
	int* newarr = new int[cols * rows];
	int total_count = rows * cols;
	for (int i = 0; i < total_count; ++i)
	{
		newarr[i] = matrix[i / cols][i % cols];
	}
	int world_rank;
	int world_size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int size = (rows * cols) / world_size;
	int sum = 0;
#pragma omp parallel
	{
		int* sub_array = new int[total_count];
		MPI_Scatter(newarr, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);
		mergeSort(sub_array, 0, (total_count - 1));
		cout << "\n";
		int* arr_sorted = NULL;
		if (world_rank == 0)
		{
			arr_sorted = new int[total_count];
		}
		MPI_Gather(sub_array, size, MPI_INT, arr_sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
		if (world_rank == 0)
		{
			mergeSort(arr_sorted, 0, (total_count - 1));
			cout << "Sorted Array: \n" << endl;
			for (int c = (total_count - 1); c >= 0; --c)
			{
				cout << arr_sorted[c] << " ";
				sum += arr_sorted[c];
			}
			for (int v = 0; v < 2; v++) {
				cout << endl;
			}
			cout << "Total Sum = " << sum << endl; cout << endl;
			delete[]arr_sorted;

		}
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Finalize();
	}
	//**********************************************************************
}
