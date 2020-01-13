#pragma once

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstdint>


void statusMess(const std::string& mess);

#define FUNC_BEGIN try {
#define FUNC_END } catch(std::exception& e) { \
statusMess(std::string(__FILE__) + ":" + std::to_string(__LINE__) + " Exception: " + e.what()); return false; } return true;

#define CALL(func, mess) if (!func){ statusMess(mess); return false; } 

#define PROFILE_START clock_t ctm = clock();
#define PROFILE_END(func) statusMess(std::string("Profile ") + func + " " + std::to_string(clock() - ctm)); ctm = clock();

    