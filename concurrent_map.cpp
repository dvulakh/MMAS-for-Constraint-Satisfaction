
#include "concurrent_map.h"

bool concurrent_map_bucket_compare(const pair<int, int>& b1, const pair<int, int>& b2) { return b1.second == b2.second ? b1.first < b2.first : b1.second > b2.second; }