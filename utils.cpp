#include "utils.h"
#include <cstdlib>
#include<io.h>
#include<direct.h>
#include<iostream>
bool makedirs(const string& dirname)
{
	if (_access(dirname.c_str(), 0) != -1)
		return false;
	string parent_dir = dirname.substr(0, dirname.rfind('/'));
	makedirs(parent_dir);
	_mkdir(dirname.c_str());
	return true;
}
void deleteDirs(const string& dirname)
{
	system((string("rm -r") + dirname).c_str());
}

void rmDirs(const string& dir, bool includeSelf)

{
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst((dir+"/*").c_str(), &findData);
	if (handle == -1)        // 检查是否成功
		return;
	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			rmDirs(dir + "/" + findData.name,true);
		}
		else
			remove((dir + "/" + findData.name).c_str());
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);    // 关闭搜索句柄
	if(includeSelf)
		_rmdir(dir.c_str());
}
void listDirFiles(const string& dir, std::vector<string>& filenames,string postfix)
{
	intptr_t handle;
	_finddata_t findData;

	handle = _findfirst((dir + "/*" + postfix).c_str(), &findData);
	if (handle == -1)        // 检查是否成功
		return;
	do
	{
		if (findData.attrib & _A_SUBDIR)
		{
			continue;
		}
		else
			filenames.push_back(findData.name);
	} while (_findnext(handle, &findData) == 0);
	_findclose(handle);    // 关闭搜索句柄
}
