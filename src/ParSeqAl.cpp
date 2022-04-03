#include <iostream>
#include <omp.h>
#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>
#include <fstream>

struct data{
    std::string A;
    std::string B;
    std::vector<std::vector<int>> T_1;
    std::vector<std::vector<int>> T_2;
    std::vector<std::vector<int>> T_3;
    std::vector<int> x;
    int g;
    int h;
};

int score(data* D,int i ,int j){if(D->A[i]==D->B[j]){return 1;}else{return 0;}}

void Initialise(data* D){
    int m = D->A.length();int n = D->B.length();
    int _infty = -10E+8;// std::numeric_limits<int>::min();
    D->T_1.resize(m+1,std::vector<int>(n+1, 0));
    D->T_2.resize(m+1,std::vector<int>(n+1, 0));
    D->T_3.resize(m+1,std::vector<int>(n+1, 0));
    D->x.resize(n+1,0);
    // first row -/infty
    for (size_t j = 0; j < n; j++){D->T_1[0][j] = _infty;D->T_2[0][j] = _infty;D->T_3[0][j] = _infty;}
    // first column -/infty
    for (size_t i = 0; i < m; i++){D->T_1[i][0] = _infty;D->T_2[i][0] = _infty;D->T_3[i][0] = _infty;}
    // T_1[0,0] = 0
    D->T_1[0][0] = 0;
    // T_2[0,j] = h + gj
    for (size_t j = 0; j < n; j++){D->T_2[0][j] = D->h+D->g*j;}
    // T_3[i,0] = h + gi
    for (size_t i = 0; i < m; i++){D->T_3[i][0] = D->h+D->g*i;}
}

int T_1(data* D, int i, int j){if (i== 0 || j == 0){return score(D,i-1,j-1) +  D->T_1[i][j];}else{return score(D,i,j) + std::max( std::max(D->T_1[i-1][j-1] , D->T_2[i-1][j-1]) , D->T_3[i-1][j-1] );}}

int T_2(data* D, int i, int j){if (j == 0){return D->T_2[i][j];}else{return std::max( std::max(D->T_1[i][j-1] - (D->g+D->h) , D->T_2[i][j-1] - (D->g))  , D->T_3[i][j-1] - (D->g+D->h) );}}

int T_3(data* D, int i, int j){if (i == 0){return D->T_3[i][j];}else{return std::max( std::max( D->T_1[i-1][j] - (D->g+D->h) , D->T_2[i-1][j] - (D->g + D->h))  , D->T_3[i-1][j] - (D->g) );}}

int w(data* D, int i, int j){return std::max(D->T_1[i][j-1],D->T_3[i][j-1]);}

int x(data* D,int i,int j){if (j==0){return std::max((w(D , i , j) + (j * D->g)) , D->x[0]);}else{return std::max((w(D , i , j) + (j * D->g)) , D->x[j-1]);}}

void print_score(data* D){int m = D->A.length(); int n = D->B.length();for(size_t i = 0; i < m; i++){for(size_t j = 0; j < n; j++){std::cout << D->T_1[i][j] << "\t";}std::cout << " " << std::endl;}}

void flush_result(data* D){std::ofstream file;file.open("data/aligned_out.csv");int m = D->A.length(); int n = D->B.length();for (size_t i = 0; i < m; i++){for (size_t j = 0; j < n; j++){file << D->T_1[i][j] << "\t";}file << " " << std::endl;}file.close();}

// Compute Sequential
void Compute_Sequential(data* D){
    int m = D->A.length(); int n = D->B.length();
    for (size_t i = 1; i <= m; i++)
    {
        for (size_t j = 1; j <= n; j++)
        {
            D->T_1[i][j] = T_1(D,i,j);
            D->T_2[i][j] = T_2(D,i,j);
            D->T_3[i][j] = T_3(D,i,j);
        }
    }
}

void Compute_Parallel(data* D){
    int m = D->A.length(); int n = D->B.length();
    std::vector<int> sumx;
    for (size_t i = 1; i <= m; i++)
    {
        D->x.resize(n+1,0);
        #pragma omp parallel
        {
            int thread_num = omp_get_thread_num();
            int num_threads = omp_get_num_threads();
            int s = 0;
            #pragma omp single
            sumx.resize(num_threads+1,0);

            #pragma omp for schedule(static) nowait
            for (size_t j = 1 ; j <= n; j++)
            {
                D->T_1[i][j] = T_1(D,i,j);
                D->T_3[i][j] = T_3(D,i,j);
                D->x[j] = x(D,i,j);
                s += D->x[j];
            }

            sumx[thread_num+1] = s; // thread 2 will have sum upto 1st and so on
 
            #pragma omp barrier
            int offset = 0;
            for (size_t j = 0; j < num_threads+1; j++){ offset += sumx[j];}
            #pragma omp for schedule(static)
            for (size_t j = 1; j <= n; j++)
            {
                D->T_2[i][j] = D->x[j] - j*D->g;
            }
            sumx.clear();
            D->x.clear();
        }
    }
}

std::string read_file(std::string filename){
    std::string A;
    std::ifstream input;
    input.open(filename);
    std::string temp_line;
    if(input.fail()){ 
        std::cout << " Failed to open File : " << filename << std::endl;
    }else{
        while (std::getline(input, temp_line)){
            if (temp_line[0] == '>'){
                std::cout << " Reference : " << filename << std::endl; 
            }else
            {
                A+=temp_line;
            }
        }
    }
    input.close();
    return A;
}

int main(int argc, char* argv[]){
    // Initialise struct
    data* D = new data();

    //Define Parameters
    D->g = 1;
    D->h = 1;
    // D->A = "ATCGATCGATCGATCG";
    // D->B = "ATCGATCGATCGATCG";
    
    // Reading Arguments from file
    std::string file1 = argv[1];
    std::string file2 = argv[2];

    // Reading A
    D->A = read_file(file1);
    // Reading B
    D->B = read_file(file2);
    
    std::cout << " Reference Length :" << D->A.length() << std::endl;
    std::cout << " Query Length :" << D->B.length() << std::endl;

    // Initialise
    Initialise(D);
    // Compute Sequential
    auto start = std::chrono::system_clock::now();
    Compute_Sequential(D);
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << " Sequetial Execution Time : "<< elapsed.count() << " milliseconds" << std::endl;

    // Initialise
    Initialise(D);
    start = std::chrono::system_clock::now();
    Compute_Parallel(D);
    end = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << " Parallel Execution Time : "<< elapsed.count() << " milliseconds" << std::endl;
    // Print Scores
    // print_score(D);
    // Writing Results
    flush_result(D);
    std::cout << " Writed Results to data/aligned_out.csv " << std::endl; 

    return EXIT_SUCCESS;
}