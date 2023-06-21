#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;

// 全局变量
int processNums;                            // 进程数
int resourceNums;                           // 资源数
vector<vector<int>> Max;                    // 最大需求矩阵
vector<vector<int>> Allocation;             // 已分配矩阵
vector<vector<int>> Need;                   // 需求矩阵
vector<int> Available(resourceNums, 0);     // 可用资源向量
vector<int> TotalResources = { 10, 5, 7 };  // 总资源向量
vector<int> Requests(resourceNums, 0);      // 请求资源向量
map<int, string> processDict;               // 进程字典
vector<bool> finish(processNums, false);    // 进程完成状态
vector<string> safeSeq;                     // 安全序列
vector<vector<string>> safeSequences;       // 所有安全序列

// 从文件中读取矩阵数据
vector<vector<int>> readMatrixFromFile(const string& filename);

// 打印一维向量
template <typename T>
void printVector(const vector<T>& vec);

// 打印二维向量
template <typename T>
void printVector(const vector<vector<T>>& vec, int dimension);

// 打印当前系统状态
void printCurrentStatus();

// 打印对齐的矩阵
void printAlignedMatrix();

// 计算两个向量的差值
vector<vector<int>> subtractVectors(const vector<vector<int>>& vec1, const vector<vector<int>>& vec2);

// 比较两个向量的大小（逐元素比较）
bool cmpResLess(const vector<int>& vec1, const vector<int>& vec2);

// 初始化进程字典
void initProcessDict();

// 初始化可用资源向量
vector<int> initAvailable();

// 初始化系统状态
void initSystem();

// 检查系统是否安全
bool isSafe();

// 递归回溯找出所有安全序列
void findAllSafeSequences(vector<bool>& finish, vector<string>& safeSeq);

// 银行家算法
void bankAlgorithm();

// 输出对齐的矩阵
void printAlignedMatrix();

// 判断进程是否可以释放资源
bool releaseProcess(int processID);

// 分配资源给进程
void allocateSource(vector<int> allocation);

// 释放已完成进程的资源
void restoreAllocation(vector<int> allocation);

// 请求资源
void requestResource();

int main()
{
    system("cls");
    initSystem();

    while (1)
    {
        cout << "\t**********************************************\t" << endl;
        cout << "\t********* Banker Algorithm Simulation ********\t" << endl;
        cout << "\t0. Quit" << endl;
        cout << "\t1. Manually allocate resources for a single process" << endl;
        cout << "\t2. Find a safe sequence for the current process state" << endl;
        cout << "\t3. Output all safe sequences" << endl;
        cout << "\t4. Output the current resource status" << endl;
        cout << "\t**********************************************\t" << endl;
        cout << "\t**********************************************\t" << endl;

        printAlignedMatrix();

        char option;
        cout << "please enter your option: ";
        cin >> option;

        switch (option)
        {
        case '0':return 0;
        case '1':
            finish = vector<bool>(processNums, false);
            safeSeq.clear();
            requestResource();
            break;
        case '2':
            finish = vector<bool>(processNums, false);
            safeSeq.clear();
            bankAlgorithm();
            break;
        case '3':
            finish = vector<bool>(processNums, false);
            safeSeq.clear();
            safeSequences.clear();
            findAllSafeSequences(finish, safeSeq);
            cout << "\nThe number of all security sequences is " << safeSequences.size() << ", as follows:" << endl;
            printVector(safeSequences, 2);
            break;
        case '4':
            printAlignedMatrix();
            break;

        default:
            cout << "error, please input a digit from 1 to 4." << endl;
            break;
        }
        cout << "Press any key to continue..." << endl;
        cin.get();
        cin.get();
        system("cls");
    }
    return 0;
}

vector<int> initAvailable()
{
    vector<int> available = TotalResources;
    for (const auto& allocation : Allocation)
    {
        for (int i = 0; i < allocation.size(); i++)
        {
            available[i] -= allocation[i];
        }
    }
    return available;
}

vector<vector<int>> readMatrixFromFile(const string& filename)
{
    vector<vector<int>> matrix;
    ifstream file(filename);
    string line;

    while (getline(file, line))
    {
        vector<int> row;
        istringstream iss(line);

        int value;
        while (iss >> value) row.push_back(value);

        matrix.push_back(row);
    }

    return matrix;
}

template <typename T>
void printVector(const vector<T>& vec)
{
    for (const auto& element : vec)
        cout << element << '\t';
    cout << endl;
}

template <typename T>
void printVector(const vector<vector<T>>& vec, int dimension)
{
    for (const auto& element : vec)
        printVector(element);
    cout << "-----------------------" << endl;
}

vector<vector<int>> subtractVectors(
    const vector<vector<int>>& vec1,
    const vector<vector<int>>& vec2)
{
    vector<vector<int>> result;

    if (vec1.size() != vec2.size() || vec1.empty())
    {
        cout << "Error: Vectors must have the same non-zero dimensions." << endl;
        return result;
    }

    auto it1 = vec1.begin();
    auto it2 = vec2.begin();
    while (it1 != vec1.end() && it2 != vec2.end())
    {
        if (it1->size() != it2->size())
        {
            cout << "Error: Each row of the vectors must have the same number of elements." << endl;
            return result;
        }

        vector<int> tempRow;
        auto element1 = it1->begin();
        auto element2 = it2->begin();
        while (element1 != it1->end() && element2 != it2->end())
        {
            int diff = *element1 - *element2;
            tempRow.push_back(diff);
            ++element1, ++element2;
        }

        result.push_back(tempRow);
        ++it1, ++it2;
    }

    return result;
}

void initProcessDict()
{
    for (int i = 0; i < processNums; i++)
        processDict[i] = "P" + to_string(i);
    cout << "process has initialized!" << endl;
}

void allocateSource(vector<int> allocation)
{
    for (int i = 0; i < resourceNums; i++)
        Available[i] += allocation[i];
}

void restoreAllocation(vector<int> allocation)
{
    for (int i = 0; i < resourceNums; i++)
        Available[i] -= allocation[i];
}

void initSystem()
{
    Max = readMatrixFromFile("max.txt");
    // Max = readMatrixFromFile("max_test.txt");
    Allocation = readMatrixFromFile("allocation.txt");
    // Allocation = readMatrixFromFile("allocation_test.txt");
    Need = subtractVectors(Max, Allocation);
    Available = initAvailable();
    processNums = Max.size();
    resourceNums = (*Max.begin()).size();
    initProcessDict();

    finish = vector<bool>(processNums, false);
    Requests = vector<int>(resourceNums, 0);
}


void printCurrentStatus()
{
    cout << "processNums: " << processNums << endl;
    cout << "resourceNums: " << resourceNums << endl;

    cout << "Max matrix" << endl;
    printVector(Max, 2);

    cout << "Allocation matrix" << endl;
    printVector(Allocation, 2);

    cout << "Need matrix" << endl;
    printVector(Need, 2);

    cout << "Available resources: ";
    printVector(Available);
}

bool isSafe()
{
    vector<vector<int>> temp_Allocation = Allocation;
    vector<vector<int>> temp_Need = Need;
    vector<int>         temp_Available = Available;
    vector<bool>        temp_finish = finish;
    safeSeq.clear();

    int count = 0;
    while (count < processNums)
    {
        bool found = false;
        for (int i = 0; i < processNums; i++)
        {
            if (!finish[i])
            {
                bool canAllocate = true;
                for (int j = 0; j < resourceNums; j++)
                    if (Need[i][j] > Available[j])
                    {
                        canAllocate = false;
                        break;
                    }

                if (canAllocate)
                {
                    allocateSource(Allocation[i]);
                    finish[i] = true;
                    safeSeq.push_back(processDict[i]);
                    found = true;
                    count++;
                }
            }
        }
        if (not found) break;
    }

    // printAlignedMatrix();

    bool safe = (count == processNums);

    Allocation = temp_Allocation;
    Need = temp_Need;
    Available = temp_Available;
    finish = temp_finish;

    // printAlignedMatrix();

    return safe;
}

// 递归回溯找出所有安全序列
void findAllSafeSequences(vector<bool>& finish, vector<string>& safeSeq) {

    bool isSafe = true;

    for (int i = 0; i < processNums; i++) {
        if (!finish[i]) {
            bool canAllocate = true;

            for (int j = 0; j < resourceNums; j++) {
                if (Need[i][j] > Available[j]) {
                    canAllocate = false;
                    break;
                }
            }

            if (canAllocate) {
                allocateSource(Allocation[i]);

                finish[i] = true;
                safeSeq.push_back(processDict[i]);

                findAllSafeSequences(finish, safeSeq);

                finish[i] = false;
                safeSeq.pop_back();

                restoreAllocation(Allocation[i]);

                isSafe = false;
            }
        }
    }

    if (isSafe)
        safeSequences.push_back(safeSeq);
}

void bankAlgorithm()
{
    if (isSafe())
    {
        cout << "Security sequence exists!" << endl;
        printVector(safeSeq);
        safeSeq.clear();
    }
    else cout << "No security sequence exists!" << endl;
}

// 输出对齐的矩阵
void printAlignedMatrix() {
    // 输出表头
    cout << " PID" << "  | ";
    cout << setw(resourceNums * 3) << "Max" << setw(resourceNums * 3) << " | ";
    cout << setw(resourceNums * 3) << "Alloc" << setw(resourceNums * 3) << " | ";
    cout << setw(resourceNums * 3) << "Need" << setw(resourceNums * 3) << " | ";
    cout << setw(resourceNums * 4) << "Available" << endl;

    // // 输出分割线
    for (int j = 0; j < 25 * resourceNums + 3; j++) {
        cout << "-";
    }
    cout << endl;

    // 输出每一行的数据
    for (int i = 0; i < processNums; i++) {
        cout << setw(3) << "P" << i << "  | ";
        for (int j = 0; j < resourceNums; j++) {
            cout << setw(4) << Max[i][j] << " ";
        }
        cout << " | ";
        for (int j = 0; j < resourceNums; j++) {
            cout << setw(4) << Allocation[i][j] << " ";
        }
        cout << " | ";
        for (int j = 0; j < resourceNums; j++) {
            cout << setw(4) << Need[i][j] << " ";
        }
        cout << " | ";
        if (i == 0) {
            for (int j = 0; j < resourceNums; j++) {
                cout << setw(4) << Available[j] << " ";
            }
        }
        cout << endl;
    }
}

bool cmpResLess(const vector<int>& vec1, const vector<int>& vec2)
{
    for (int i = 0; i < resourceNums; i++)
        if (vec1[i] > vec2[i]) return false;
    return true;
}

bool releaseProcess(int processID)
{
    for (int i = 0; i < resourceNums; i++)
        if (Need[processID][i] > 0) return false;
    return true;
}

void requestResource()
{
    int processID;
    do
    {
        cout << "Please enter the process ID requesting the resource: ";
        cin >> processID;
        if (processID >= processNums)
            std::cout << "Illegal input, the maximum process ID is " << processNums - 1 << endl;
    } while (processID >= processNums);

    bool allowAllocation;
    do
    {
        cout << "Please enter a request vector of length " << resourceNums << ", separated by spaces: ";
        for (int i = 0; i < resourceNums; i++) cin >> Requests[i];
        allowAllocation = cmpResLess(Requests, Need[processID]) && cmpResLess(Requests, Available);
        if (not cmpResLess(Requests, Need[processID]))
            cout << "Process " << processID << " request request exceeds maximum requirement, please re-enter." << endl;
        if (not cmpResLess(Requests, Available))
            cout << "Process " << processID << " request has not enough resources to be allocated, please re-enter." << endl;
    } while (not allowAllocation);

    for (int i = 0; i < resourceNums; i++)
    {
        Available[i] -= Requests[i];
        Allocation[processID][i] += Requests[i];
        Need[processID][i] -= Requests[i];
    }

    if (releaseProcess(processID))
    {
        cout << "Process " << processID << " is completed and resources have been released" << endl;
        for (int i = 0; i < resourceNums; i++)
            Available[i] += Allocation[processID][i], Allocation[processID][i] = 0;
        printAlignedMatrix();
    }
    else {

        // printAlignedMatrix();

        bool safe = isSafe();
        if (safe)
        {
            cout << "Safe, already allocated resource for process " << processID << endl;
            printAlignedMatrix();
        }
        else {
            printAlignedMatrix();
            cout << "Unsafe, cannot allocate resource for process " << processID << endl;
            for (int i = 0; i < resourceNums; i++)
            {
                Available[i] += Requests[i];
                Allocation[processID][i] -= Requests[i];
                Need[processID][i] += Requests[i];
            }
            printAlignedMatrix();
        }
    }
    // printAlignedMatrix();
}