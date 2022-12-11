#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

std::tuple<long long, long long> monkey_behaviour_model(const long long& old_level, const long long& rationalizer, const long long& to_multiply, const long long& to_add, const long long& to_divide, const long long& m1, const long long& m2) {
  const auto new_level = (old_level * to_multiply + to_add) % rationalizer;
  return {new_level, new_level % to_divide == 0 ? m1 : m2 };
}

struct Monkey {
  Monkey(const long long& id, const std::queue<long long>& item_worry_levels, const std::function<std::tuple<long long, long long>(long long, long long)>& action)
    : id(id), item_worry_levels(item_worry_levels), action(action) {}

  long long id;
  long long count = 0;
  std::queue<long long> item_worry_levels;
  std::function<std::tuple<long long, long long>(long long, long long)> action;

  void print() {
    std::cout << "Monkey " << id << ": ";
    const auto s = item_worry_levels.size();
    for (long long  i = 0; i < s; i++) {
      const auto ele = item_worry_levels.front();
      item_worry_levels.pop();
      std::cout << ele << ' ';
      item_worry_levels.push(ele);
    }
    std::cout << '\n';
  }
};

std::function<std::tuple<long long, long long>(long long, long long)> get_monkey_behaviour(const std::vector<std::string>& args) {
  // Args consists of the following values:
  // to_multiply, to_add, to_divide, monkey id if test() is true, monkey id if test() is false
  if (args[0] == "" && args[1] == "old") {
    return std::bind(monkey_behaviour_model, std::placeholders::_1, std::placeholders::_2, 1, std::placeholders::_1, std::stoll(args[2]), std::stoll(args[3]), std::stoll(args[4]));
  }
  else if (args[0] == "old" && args[1] == "") {
    return std::bind(monkey_behaviour_model, std::placeholders::_1, std::placeholders::_2, std::placeholders::_1, 0, std::stoll(args[2]), std::stoll(args[3]), std::stoll(args[4]));
  }
  else if (args[0] == "") {
    return std::bind(monkey_behaviour_model, std::placeholders::_1, std::placeholders::_2, 1, std::stoll(args[1]), std::stoll(args[2]), std::stoll(args[3]), std::stoll(args[4]));
  }
  else if (args[1] == "") {
    return std::bind(monkey_behaviour_model, std::placeholders::_1, std::placeholders::_2, std::stoll(args[0]), 0, std::stoll(args[2]), std::stoll(args[3]), std::stoll(args[4]));
  }
  else {
    std::cout << "This should not happen" << '\n';
    exit(0);
  }
}

// Assumes queue has at least 1 item; can be esily modified to generalize
std::queue<long long> parse_starting_items(const std::string& line) {
  std::queue<long long> q;
  size_t start = 17;
  size_t end = line.find(", ", start);
  while (end != std::string::npos) {
    q.push(std::stoll(line.substr(start, end - start)));
    start = end + 2;
    end = line.find(", ", start);
  }
  q.push(std::stoll(line.substr(start, line.size() - start)));
  return q;
}

int main(int argc, char * argv[]) {
  std::string input = "../input/day_11_input";
  if (argc > 1) {
    input = argv[1];
  }

  std::string line;
  std::fstream file(input);

  std::vector<Monkey> monkeys;

  long long rationalizer = 1;
  while(std::getline(file, line)) {
    const long long id = std::stoll(line.substr(7, line.size() - 7));
    std::getline(file, line);
    std::queue<long long> item_worry_levels = parse_starting_items(line);

    std::vector<std::string> args;
    // Args consists of the following values:
    // to_multiply, to_add, to_divide, monkey id if test() is true, monkey id if test() is false

    std::getline(file, line);  // Get sign & value used in operation
    if (line[23] == '+') {
      args.emplace_back();
      args.push_back(line.substr(25, line.size() - 25));
    }
    else if (line[23] == '*') {
      args.push_back(line.substr(25, line.size() - 25));
      args.emplace_back();
    } else {
      std::cout << "This should not happen" << '\n';
      exit(0);
    }
    std::getline(file, line);  // Get value used in test
    const auto val_used_in_test_s = line.substr(21, line.size() - 21);
    args.push_back(val_used_in_test_s);
    const auto val_used_in_test_ll = std::stoll(val_used_in_test_s);
    if (rationalizer % val_used_in_test_ll != 0) {
      rationalizer *= val_used_in_test_ll;
    }
    std::getline(file, line);  // Get id of monkey when test is true
    args.push_back(line.substr(29, line.size() - 29));
    std::getline(file, line);  // Get id of monkey when test is false
    args.push_back(line.substr(30, line.size() - 30));

    monkeys.emplace_back(id, item_worry_levels, get_monkey_behaviour(args));
    std::getline(file, line);
  }

  // Assumes monkey id and monkey index is the same; can be esily modified to generalize
  constexpr long long n_rounds = 10000;
  for (long long round = 0; round < n_rounds; round++) {
    for (long long monkey_index = 0; monkey_index < monkeys.size(); monkey_index++) {
      while (!monkeys[monkey_index].item_worry_levels.empty()) {
        monkeys[monkey_index].count++;
        const auto worry = monkeys[monkey_index].item_worry_levels.front();
        monkeys[monkey_index].item_worry_levels.pop();
        const auto [new_worry, new_monkey_index] = monkeys[monkey_index].action(worry, rationalizer);
        // std::cout << "Moving " << worry << " from " << monkey << " as " << new_worry << " to " << new_monkey << '\n';
        monkeys[new_monkey_index].item_worry_levels.push(new_worry);
      }
    }
  }

  // for (auto& monkey : monkeys) {
  //   monkey.print();
  // }

  std::nth_element(std::begin(monkeys), std::next(std::begin(monkeys), 2), std::end(monkeys), [](const auto& m1, const auto& m2) {return m1.count > m2.count;});
  std::cout << monkeys[0].count * monkeys[1].count  << '\n';
  // std::cout << rationalizer << '\n';
  return 0;
}
