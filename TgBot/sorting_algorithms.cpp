#include "sorting_algorithms.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Преобразование строки в вектор
vector<char> toVector(const string& s)
{
    vector<char> v;
    for (size_t i = 0; i < s.length(); i++)
    {
        v.push_back(s[i]);
    }
    return v;
}

// Преобразование вектора в строку
string fromVector(const vector<char>& v)
{
    string s = "";
    for (size_t i = 0; i < v.size(); i++)
    {
        s = s + v[i];
    }
    return s;
}

// Пузырьковая сортировка
string bubbleSort(const string& s)
{
    vector<char> v = toVector(s);
    int n = (int)v.size();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (v[j] > v[j + 1])
            {
                char temp = v[j];
                v[j] = v[j + 1];
                v[j + 1] = temp;
            }
        }
    }

    return fromVector(v);
}

void quickSortRec(vector<char>& v, int low, int high)
{
    if (low >= high)
        return;

    char pivot = v[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (v[j] <= pivot)
        {
            i++;
            char temp = v[i];
            v[i] = v[j];
            v[j] = temp;
        }
    }

    char temp = v[i + 1];
    v[i + 1] = v[high];
    v[high] = temp;

    int pi = i + 1;
    quickSortRec(v, low, pi - 1);
    quickSortRec(v, pi + 1, high);
}

// Сортировка Хоара
string quickSort(const string& s)
{
    vector<char> v = toVector(s);
    int n = (int)v.size();
    quickSortRec(v, 0, n - 1);
    return fromVector(v);
}

// Сортировка вставками
string insertionSort(const string& s)
{
    vector<char> v = toVector(s);
    int n = (int)v.size();

    for (int i = 1; i < n; i++)
    {
        char key = v[i];
        int j = i - 1;

        while (j >= 0 && v[j] > key)
        {
            v[j + 1] = v[j];
            j = j - 1;
        }

        v[j + 1] = key;
    }

    return fromVector(v);
}

// Сортировка выбором
string selectionSort(const string& s)
{
    vector<char> v = toVector(s);
    int n = (int)v.size();

    for (int i = 0; i < n; i++)
    {
        int minIdx = i;
        for (int j = i + 1; j < n; j++)
        {
            if (v[j] < v[minIdx])
            {
                minIdx = j;
            }
        }

        char temp = v[i];
        v[i] = v[minIdx];
        v[minIdx] = temp;
    }

    return fromVector(v);
}

void merge(vector<char>& v, int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<char> L;
    vector<char> R;

    for (int i = 0; i < n1; i++)
        L.push_back(v[left + i]);
    for (int j = 0; j < n2; j++)
        R.push_back(v[mid + 1 + j]);

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            v[k] = L[i];
            i++;
        }
        else
        {
            v[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        v[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        v[k] = R[j];
        j++;
        k++;
    }
}

void mergeSortRec(vector<char>& v, int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;
    mergeSortRec(v, left, mid);
    mergeSortRec(v, mid + 1, right);
    merge(v, left, mid, right);
}

// Сортировка слиянием
string mergeSort(const string& s)
{
    vector<char> v = toVector(s);
    int n = (int)v.size();
    mergeSortRec(v, 0, n - 1);
    return fromVector(v);
}