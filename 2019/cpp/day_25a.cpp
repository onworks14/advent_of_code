#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <queue>
#include <stack>
#include <chrono>
#include <thread>

constexpr long long pow(const int base, const int raised_to) {
  long long ans = 1;
  for (int i = 1; i <= raised_to; i++) {
    ans *= base;
  }
  return ans;
}

struct hash_pair {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, T2>& p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

class Program {
 public:
  Program(const std::vector<long long>& program) : init_program_state(program) {
    for (long long i = 0; i < program.size(); i++) {
      memory[i] = program[i];
    }
  }

  void InitIfNotInited(const long long key) {
    if (memory.find(key) == memory.end()) {
      memory[key] = 0;
    }
  }

  void reset() {
    for (long long i = 0; i < init_program_state.size(); i++) {
      memory[i] = init_program_state[i];
    }
    complete = false;
    inst_ptr = 0;
    relative_base = 0;
  }

  std::vector<long long> getNOps(const size_t n, const size_t inst_ptr,
                                 long long modes) {
    std::vector<long long> ops;
    for (size_t i = 1; i <= n; ++i) {
      size_t rem = modes % 10;
      modes = modes / 10;
      if (rem == 0) {
        InitIfNotInited(inst_ptr + i);
        InitIfNotInited(memory[inst_ptr + i]);
        ops.push_back(memory[memory[inst_ptr + i]]);
      } else if (rem == 1) {
        InitIfNotInited(inst_ptr + i);
        InitIfNotInited(memory[inst_ptr + i]);
        ops.push_back(memory[inst_ptr + i]);
      } else if (rem == 2) {
        InitIfNotInited(inst_ptr + i);
        InitIfNotInited(relative_base + memory[inst_ptr + i]);
        InitIfNotInited(memory[relative_base + memory[inst_ptr + i]]);
        ops.push_back(memory[relative_base + memory[inst_ptr + i]]);
      } else {
        // // std::cout << "Error" << '\n';
        exit(0);
      }
    }
    return ops;
  }

  long long getAddress(const size_t increment, long long modes) {
    long long address;
    const size_t rem = modes;
    if (rem == 0) {
      InitIfNotInited(inst_ptr + increment);
      address = memory[inst_ptr + increment];
    } else if (rem == 2) {
      InitIfNotInited(inst_ptr + increment);
      InitIfNotInited(relative_base + memory[inst_ptr + increment]);
      address = relative_base + memory[inst_ptr + increment];
    } else {
      // // std::cout << "Error" << '\n';
      exit(0);
    }
    return address;
  }

  void compute(const std::vector<long long>& input) {
    is_input_requested = false;
    long long output = -1;
    size_t input_ptr = 0;
    while (memory[inst_ptr] != 99) {
      // std::cout << inst_ptr << '\n';
      const long long instr = memory[inst_ptr];
      const long long opcode = instr % 100;
      const long long modes = instr / 100;
      if (opcode == 1) {
        constexpr int n_ops = 2;
        const auto ops = getNOps(n_ops, inst_ptr, modes);
        memory[getAddress(3, modes / pow(10, n_ops))] = ops[0] + ops[1];
        inst_ptr += 4;
      } else if (opcode == 2) {
        constexpr int n_ops = 2;
        const auto ops = getNOps(n_ops, inst_ptr, modes);
        memory[getAddress(3, modes / pow(10, n_ops))] = ops[0] * ops[1];
        inst_ptr += 4;
      } else if (opcode == 3) {
        if (input.size() == input_ptr) {
          input_ptr = 0;
          is_input_requested = true;
          return;
        }
        memory[getAddress(1, modes)] = input[input_ptr];
        inst_ptr += 2;
        input_ptr += 1;
      } else if (opcode == 4) {
        const auto ops = getNOps(1, inst_ptr, modes);
        output = ops[0];
        inst_ptr += 2;
        output_queue.push(output);
      } else if (opcode == 5) {
        const auto ops = getNOps(2, inst_ptr, modes);
        if (ops[0] != 0) {
          inst_ptr = ops[1];
        } else {
          inst_ptr += 3;
        }
      } else if (opcode == 6) {
        const auto ops = getNOps(2, inst_ptr, modes);
        if (ops[0] == 0) {
          inst_ptr = ops[1];
        } else {
          inst_ptr += 3;
        }
      } else if (opcode == 7) {
        constexpr int n_ops = 2;
        const auto ops = getNOps(n_ops, inst_ptr, modes);
        if (ops[0] < ops[1]) {
          memory[getAddress(3, modes / pow(10, n_ops))] = 1;
        } else {
          memory[getAddress(3, modes / pow(10, n_ops))] = 0;
        }
        inst_ptr += 4;
      } else if (opcode == 8) {
        constexpr int n_ops = 2;
        const auto ops = getNOps(n_ops, inst_ptr, modes);
        if (ops[0] == ops[1]) {
          memory[getAddress(3, modes / pow(10, n_ops))] = 1;
        } else {
          memory[getAddress(3, modes / pow(10, n_ops))] = 0;
        }
        inst_ptr += 4;
      } else if (opcode == 9) {
        const auto ops = getNOps(1, inst_ptr, modes);
        relative_base += ops[0];
        inst_ptr += 2;
      }
    }
    complete = true;
    output_queue.push(output);
  }

  bool isComplete() { return complete; }

  bool isProgramWaitingForInput() {
    return is_input_requested;
  }

  void modifyMemoryAddress(const long long address, const long long value) {
    memory[address] = value;
  }

  std::queue<long long> getOutputQueue() {
    return output_queue;
  }

  void clearOutputQueue() {
    while(!output_queue.empty()) output_queue.pop();
  }


 private:
  const std::vector<long long> init_program_state;
  std::unordered_map<long long, long long> memory;
  bool complete = false;
  size_t inst_ptr = 0;
  size_t relative_base = 0;
  bool is_input_requested = false;
  std::queue<long long> output_queue;
};

class IntcodeComputer {
 public:
  void addProgram(const std::vector<long long>& program) {
    programs.emplace_back(program);
  }

  void runProgram(const size_t index,
                       const std::vector<long long>& input) {
    programs[index].compute(input);
  }

  void resetProgram(const size_t index) { programs[index].reset(); }

  void modifyProgramMemoryAddress(const size_t index, const long long address, const long long value) {
    programs[index].modifyMemoryAddress(address, value);
  }

  bool isLastComplete() { return programs.back().isComplete(); }

  bool isComplete(size_t index) { return programs[index].isComplete(); }

  void reset() { programs.clear(); }

  bool isProgramWaitingForInput(const size_t index) {
    return programs[index].isProgramWaitingForInput();
  }

  std::queue<long long> getAndClearProgramOutputQueue(const size_t index) {
    const auto q = programs[index].getOutputQueue();
    programs[index].clearOutputQueue();
    return q;
  }

 private:
  std::vector<Program> programs;
};

std::vector<long long> convertToAscii(const std::string& str) {
  std::vector<long long> ret;
  for(const char c: str) {
    ret.push_back(int(c));
  }
  return ret;
}

char convertFromAscii(const long long ascii) {
  return char(ascii + '0');
}

int main(int argc, char* argv[]) {
  // Get input
  std::string input = "../input/day_25_input";
  if (argc > 1) {
    input = argv[1];
  }
  std::ifstream file(input);
  std::string input_str;
  std::getline(file, input_str);
  const std::string delimiter = ",";

  std::size_t start = 0;
  std::size_t end = input_str.find(delimiter);
  std::vector<long long> program;
  while (end != std::string::npos) {
    program.emplace_back(std::stoll(input_str.substr(start, end - start)));
    start = end + delimiter.size();
    end = input_str.find(delimiter, start);
  }
  program.emplace_back(std::stoll(input_str.substr(start, end - start)));

  auto computer = IntcodeComputer();
  computer.addProgram(program);
  computer.runProgram(0, {});

  std::pair<int, int> pos = {0, 0};
  while(true) {
    auto output = computer.getAndClearProgramOutputQueue(0);
    while(!output.empty()){
      const auto out = output.front();
      output.pop();
      std::cout << char(out);
    }
    std::cout << '\n';

    std::string cmd;
    std::getline(std::cin, cmd);
    std::cout << "Command: " << cmd << '\n';
    if (cmd == "north") {
      pos.first-=1;
    }
    if (cmd == "south") {
      pos.first+=1;
    }
    if (cmd == "east") {
      pos.second+=1;
    }
    if (cmd == "west") {
      pos.second-=1;
    }
    std::cout << "Position: (" << pos.first << ", " << pos.second << ")\n";
    auto ascii_cmd = convertToAscii(cmd);
    // std::cout << ascii_cmd << '\n';
    ascii_cmd.push_back(10);
    computer.runProgram(0, {ascii_cmd});
  }
  return 0;
}