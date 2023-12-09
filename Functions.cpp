#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <queue>
#include <sstream>
#include <functional>
using namespace std;
int SizeAll = 0;
AvailList availList;
int checkAvailList(int size);
template<typename T>
vector<T> Temprory;


int searchByAuthorId(string authorId)
{
    fstream File;
    File.open("PrimaryAu.txt", ios::in|ios::out|ios::app);
    string line;
    int Current = -1;

    while (getline(File, line))
    {
        string currentID;
        string currentRRN;

        int i;
        for (i = 0; i < line.length(); i++)
        {
            if (line[i] == '|')
                break;
            currentID += line[i];
        }

        if (currentID == authorId)
        {
            for (int j = i + 1; j < line.length(); j++)
            {
                currentRRN += line[j];
            }
            Current = stoi(currentRRN);
            break;
        }
    }

    File.close();
    return Current;
}

///****************************************************************************************************
int searchByIsbn(string isbn)
{
    fstream File;
    File.open("PrimaryBo.txt", ios::in|ios::out|ios::app);

    if (!File.is_open())
    {
        cerr << "Error opening file." << endl;
        return -1;
    }

    string line;
    int Current = -1;

    while (getline(File, line))
    {
        string currentID;
        string currentRRN;

        int i;
        for (i = 0; i < line.length(); i++)
        {
            if (line[i] == '|')
                break;
            currentID += line[i];
        }

        if (currentID == isbn)
        {
            for (int j = i + 1; j < line.length(); j++)
            {
                currentRRN += line[j];
            }
            Current = stoi(currentRRN);
            break;
        }
    }

    File.close();
    return Current;

}
///****************************************************************************************************
template <typename T>
bool CheckIDs(string ID,vector<T>& OriginalVector)
{
    for(T elem : OriginalVector)
    {
        if(elem.PrimaryKey() == ID)
            return true;
    }
}
///****************************************************************************************************
void SortFile(string filePath)
{
    fstream File(filePath);
    vector<string> lines;
    if (File.is_open())
    {
        string line;
        while (getline(File, line))
        {
            lines.push_back(line);
        }
        File.close();
    }
    if(lines.size() == 1)
        return;
    sort(lines.begin(), lines.end());//Sort the vector
    File.open(filePath,std::fstream::out | std::fstream::trunc );//Empty the original one
    for (const auto& line : lines)
    {
        File << line << endl;
    }
    File.close();

}
///****************************************************************************************************
int calculateQueueSum(queue<int>& myQueue)
{
    queue<int> tempQueue = myQueue;
    int sum = 0;

    while (!tempQueue.empty())
    {
        sum += tempQueue.front();
        tempQueue.pop();
    }
    return sum;
}
///****************************************************************************************************
template <typename T>
void AddtoFiles(string filepath,vector<T>& OriginalVector,T newT)
{


    fstream File;
    File.open(filepath, ios::out|ios::app|ios::in);
    string newTString = newT.toString();
    SizeAll = newTString.length();
    File << newTString;//Add the Author to the file
    string key = newT.PrimaryKey();
    if(!CheckIDs(key,OriginalVector))
        OriginalVector.push_back(newT);//Add it to the vector
}
///****************************************************************************************************
template <typename T>
void AddForDataFile(string filepath,vector<T>& OriginalVector,string member1,string member2,string member3)
{
    T newT(member1, member2, member3);//Create one
    string String = newT.toString();
    int size = String.length();
    int availRRN;
    if(!CheckIDs(member1,Temprory<T>))
        availRRN = checkAvailList(size-2);

    if (availRRN != NULL && !CheckIDs(member1,Temprory<T>))
    {
        cout<<"Check"<<endl;
        stringstream stream;
        stream << availRRN;
        string RRNasString;
        stream >> RRNasString;
        char space = ' ';
        RRNasString += space;
        RRNasString += space;

        for(T element : OriginalVector)
        {
            string ID = element.PrimaryKey();
            if(ID == RRNasString)
            {
                element.setFirst(member1);
                element.setSecond(member2);
                element.setThird(member3);
                AddtoFiles(filepath,OriginalVector,element);
                break;
            }
        }

    }
    else  //if there is no place in the avail list
    {
        AddtoFiles(filepath,OriginalVector,newT);
    }


}
///****************************************************************************************************
template <typename T>
void AddForPIFile(string filepath,vector<T>& OriginalVector,string member,queue<int>& offsetTemp)
{
    int Offset = calculateQueueSum(offsetTemp);
    offsetTemp.push(SizeAll);

    T newT(member, Offset);//Create one
    AddtoFiles(filepath,OriginalVector,newT);
    SortFile(filepath);

}
///****************************************************************************************************
template <typename T>
void AddForSIFile(string filepath,vector<T>& OriginalVector,string member1,string member2)
{
    T newT(member1, member2);//Create one
    AddtoFiles(filepath,OriginalVector,newT);
    SortFile(filepath);
}
///****************************************************************************************************
template <typename T>
void UpdateForAll(vector<T>& OriginalVector,string Old,string New,queue<int>& offsetTemp)
{
    queue<int> EmptyoffsetT;
    swap(offsetTemp, EmptyoffsetT);
    offsetTemp.push(0);
    vector<T> TempOfdata;
    for (T elem : OriginalVector)
    {
        if (elem.getFmember() == Old)
        {
            elem.setSecond(New);
        }
        TempOfdata.push_back(elem);
    }

    OriginalVector.clear();
    OriginalVector = TempOfdata;
}
///****************************************************************************************************
template <typename T>
void DeleteForAll(string filepath,vector<T>& OriginalVector,string DeletedTarget, queue<int>& offsetTemp,int RRN)
{
    queue<int> EmptyoffsetT;
    swap(offsetTemp, EmptyoffsetT);
    offsetTemp.push(0);
    vector<T> TempOfdata;
    for (T elem : OriginalVector)
    {
        if (elem.getFmember() == DeletedTarget)
        {
            continue;
            stringstream stream;
            stream << RRN;
            string RRNasString;
            stream >> RRNasString;
            char space = ' ';
            RRNasString += space;
            RRNasString += space;
            elem.setFirst(RRNasString);
            string String = elem.toString();
            int size = String.length();
            availList.insert(RRN, size);

        }
        TempOfdata.push_back(elem);
        Temprory<T> = TempOfdata;
    }



    OriginalVector.clear();
    OriginalVector = TempOfdata;

}
///****************************************************************************************************
int checkAvailList(int size)
{
    int availRRN = availList.updateAvailList(size);

    if (availRRN != NULL)
    {
        return availRRN;
    }
    else
    {
        return NULL;
    }

}
///****************************************************************************************************


