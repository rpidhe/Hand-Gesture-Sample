#include"GestureOptions.h"
std::vector<std::wstring> GestureOptions::ges_options = { L"Fist",L"WristBackward",L"WristForward",L"WristSide" };
std::vector<std::string> GestureOptions::ges_options_value = { "Fist","WristBackward","WristForward","WristSide" };
std::vector<std::wstring> GestureOptions::completion_options = { L"0%",L"50%",L"100%" };
std::vector<int>  GestureOptions::completion_options_value = { 0,50,100 };