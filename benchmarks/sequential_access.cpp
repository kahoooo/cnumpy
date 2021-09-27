#include <cmath>
#include <iostream>
#include <vector>
#include <chrono>
#include <cnumpy/ndarray.hpp>

using namespace std;
using namespace cnumpy;

void print(const vector<chrono::duration<double, std::micro>> &durations) {
    int n = durations.size();
    vector<double> t;
    t.reserve(n);
    for (auto dur : durations)
        t.push_back(dur.count());

    vector<double> sum = t;
    while (sum.size() > 1) {
        int m = sum.size();
        for (int i = 0, j = (m+1) / 2; j < m; i++, j++) {
            sum[i] += sum[j];
        }
        sum.resize((m+1)/2);
    }
    double mean = sum[0] / n;

    vector<double> sumsq = t;
    for (auto &s : sumsq)
        s = (s - mean) * (s - mean);
    while (sumsq.size() > 1) {
        int m = sumsq.size();
        for (int i = 0, j = (m+1) / 2; j < m; i++, j++) {
            sumsq[i] += sumsq[j];
        }
        sumsq.resize((m+1) / 2);
    }
    double rms = sqrt(sumsq[0] / n);
    cout << mean << " +/- " << rms << endl;
}

int main() {
    int nit = 10;
    int a = 1000, b = 1000, c = 1000;

    {
        vector<chrono::duration<double, std::micro>> cnpy_time;
        ndarray<double, 3> arr1(a, b, c);
        for (int it = 0; it < nit; it++) {
            auto p1 = chrono::system_clock::now();
            for (int i = 0; i < a; i++) {
                for (int j = 0; j < b; j++) {
                    for (int k = 0; k < c; k++) {
                        arr1(i, j, k) = 0;
                    }
                }
            }
            auto p2 = chrono::system_clock::now();
            cnpy_time.push_back(p2 - p1);
        }
        print(cnpy_time);
    }

    {
        vector<chrono::duration<double, std::micro>> vec_time;
        vector<vector<vector<double>>> arr2(a, vector<vector<double>>(b, vector<double>(c)));
        for (int it = 0; it < nit; it++) {
            auto p1 = chrono::system_clock::now();
            for (int i = 0; i < a; i++) {
                for (int j = 0; j < b; j++) {
                    for (int k = 0; k < c; k++) {
                        arr2[i][j][k] = 0;
                    }
                }
            }
            auto p2 = chrono::system_clock::now();
            vec_time.push_back(p2 - p1);
        }
        print(vec_time);
    }

    {
        vector<chrono::duration<double, std::micro>> cnpy_time;
        ndarray<double, 3> arr1(a, b, c);
        for (int it = 0; it < nit; it++) {
            auto p1 = chrono::system_clock::now();
            for (int i = 0; i < a; i++) {
                for (int j = 0; j < b; j++) {
                    for (int k = 0; k < c; k++) {
                        arr1(i, j, k) = 0;
                    }
                }
            }
            auto p2 = chrono::system_clock::now();
            cnpy_time.push_back(p2 - p1);
        }
        print(cnpy_time);
    }

    return 0;
}