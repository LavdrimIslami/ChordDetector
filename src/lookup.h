#pragma once
#include <unordered_map>
#include <string>
#include <vector>


extern std::unordered_map<uint16_t, std::string> chordTable;

uint16_t makeChordMask(const std::vector<int>& notes);

void InitTable();