#pragma once
#include<vector>
#include<cstdio>
#include<string>
using namespace std;
class GestureOptions
{
public:
	static std::vector<std::wstring> ges_options;
	static std::vector<std::string> ges_options_value;
	static std::vector<std::wstring> completion_options;
	static std::vector<int> completion_options_value;
	int ges_idx;
	int left_completion_idx, right_completion_idx;
	GestureOptions() {}
	GestureOptions::GestureOptions(int ges_idx, int left_completion_idx, int right_completion_idx)
		:ges_idx(ges_idx), left_completion_idx(left_completion_idx), right_completion_idx(right_completion_idx) {
	}	
	string toString() const{
		return ges_options_value[ges_idx] + "-R" + std::to_string(completion_options_value[right_completion_idx])
			+ "-L" + std::to_string(completion_options_value[left_completion_idx]);
	}

private:

};

