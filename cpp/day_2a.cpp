#include <algorithm>
// #include <iostream>
#include <fstream>
#include <unordered_set>

int main() {
  std::ifstream file;
  file.open("day_2_data.txt");
  std::unordered_set<int> entries;
  char dash, letter, colon;
  std::string pwd;
  int low, high;
  int count = 0;
  while(file >> low >> dash >> high >> letter >> colon >> pwd) {
    const int reps = std::count_if(std::begin(pwd), std::end(pwd), [=](const auto ele) { return ele == letter; } );
    if(low <= reps && reps <= high) {
      ++count;
    }
  }
  // std::cout << count << '\n';
  return count;
}
