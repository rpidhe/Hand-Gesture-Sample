#pragma once
#include<string>
#include<vector>
using namespace std;
bool makedirs(const string& dirname);
void deleteDirs(const string& dirname);
void rmDirs(const string& dir,bool includeSelf = true);
void listDirFiles(const string& dir, vector<string>& filenames, string postfix = "");