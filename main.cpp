#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <set>

std::vector<std::string> read_file(std::string fn) {
    std::string root_dir = R"(D:\dev\test\aoc-2023\input\)";
    fn = root_dir + fn;
    std::ifstream infile(fn);
    std::string line;
    std::vector<std::string> ret = {};

    while (std::getline(infile, line))
    {
        ret.push_back(line);
    }

    return ret;
}

std::vector<std::string> split_string(std::string s, std::string delimiter) {
    std::vector<std::string> ret = {};

    int offset = 0;

    while (true) {
        int idx = s.find(delimiter, offset);
        if (idx == -1) {
            ret.push_back(s.substr(offset));
            break;
        }
        ret.push_back(s.substr(offset, idx - offset));
        offset = idx + 1;
    }

    return ret;
}

void aoc19() {
    bool is_rules = true;
    auto lines = read_file("aoc19_test.txt");
    std::vector<std::string> line_rules = {};
    std::vector<std::string> line_parts = {};

    for (auto& line : lines) {
        if (line.empty()) {
            is_rules = false;
            continue;
        }
        if (is_rules) {
            line_rules.push_back(line);
        }
        if (!is_rules) {
            line_parts.push_back(line);
        }
    }

    enum class Operator_id { gt, lt, none };
    enum class Param_id { x, m, a, s };
    enum class Action_id { accept, reject, move, none };

    struct Action_data {
        Action_id action_id;
        int move_to_id;
    };

    struct Rule {
        Param_id param_id;
        Operator_id operator_id;
        Action_data action_data;
        Rule* next_rule;
        int val;
    };

    struct Process {
        int id;
        std::string name;
        std::vector<Rule> rules;
    };

    struct Part_values {
        unsigned long long x;
        unsigned long long m;
        unsigned long long a;
        unsigned long long s;
    };

    struct Part {
        Part_values vals;
        bool accepted;
    };


    std::vector<Part> parts = {};

    auto extract_number_from_part_line = [](std::string& l, char c_look, char c_end) {
        auto pos_start = l.find(c_look);
        pos_start += 2;
        auto pos_end = l.find(c_end, pos_start);
        auto c_val = l.substr(pos_start, (pos_end - pos_start));
        return std::atoi(c_val.c_str());
    };

    for (auto& line_part : line_parts) {
        Part part;
        part.vals.x = extract_number_from_part_line(line_part, 'x', ',');
        part.vals.m = extract_number_from_part_line(line_part, 'm', ',');
        part.vals.a = extract_number_from_part_line(line_part, 'a', ',');
        part.vals.s = extract_number_from_part_line(line_part, 's', '}');
        part.accepted = false;
        parts.push_back(part);
    }

    std::vector<Process> processes = {};

    for (int idx_line = 0; idx_line < line_rules.size(); idx_line++) {
        auto& line_rule = line_rules[idx_line];
        std::string name = line_rule.substr(0, line_rule.find('{'));
        processes.push_back({ idx_line, name });
    }

    auto action_data_from_value = [&](std::string action_string) {
        Action_data ret;
        if (action_string == "A") {
            ret.action_id = Action_id::accept;
        }
        else if (action_string == "R") {
            ret.action_id = Action_id::reject;
        }
        else {
            ret.action_id = Action_id::move;
            int move_to_id = -1;
            for (auto& p : processes) {
                if (p.name == action_string) {
                    move_to_id = p.id;
                }
            }
            if (move_to_id == -1) {
                std::cout << "ERROR: move_to_id == -1" << std::endl;
            }
            ret.move_to_id = move_to_id;
        }
        return ret;
    };

    for (int idx_line = 0; idx_line < line_rules.size(); idx_line++) {
        auto& line_rule = line_rules[idx_line];
        auto rule_splits = split_string(line_rule.substr(processes[idx_line].name.length() + 1, line_rule.length() - processes[idx_line].name.length() - 2), ",");
        for (auto& rule_split : rule_splits) {
            Rule rule;
            rule.operator_id = Operator_id::none;
            auto split_gt = split_string(rule_split, ">");
            auto split_lt = split_string(rule_split, "<");
            std::vector<std::string> value_parts = {};
            if (split_gt.size() == 2 || split_lt.size() == 2) {
                auto the_split = split_gt.size() == 2 ? split_gt : split_lt;
                auto value_parts = split_string(the_split[1], ":");
                rule.operator_id = split_gt.size() == 2 ? Operator_id::gt : Operator_id::lt;
                rule.val = std::atoi(value_parts[0].c_str());
                switch (split_gt[0][0]) {
                case 'x':rule.param_id = Param_id::x; break;
                case 'm':rule.param_id = Param_id::m; break;
                case 'a':rule.param_id = Param_id::a; break;
                case 's':rule.param_id = Param_id::s; break;
                }
                auto action_string = value_parts[1];
                rule.action_data = action_data_from_value(action_string);
            }
            if (split_gt.size() == 1 && split_lt.size() == 1) {
                auto action_string = rule_split;
                rule.action_data = action_data_from_value(rule_split);
            }
            processes[idx_line].rules.push_back(rule);
        }
    }

    for (auto& p : processes) {
        for (int i = 0; i < p.rules.size() - 1; i++) {
            p.rules[i].next_rule = &p.rules[i + 1];
        }
    }

    std::set<int> move_ids = {};
    int cnt_used = 0;
    for (auto& p : processes) {
        for (auto& r : p.rules) {
            if (r.action_data.action_id == Action_id::move) {
                cnt_used++;
                move_ids.insert(r.action_data.move_to_id);
            }
        }
    }

    if (cnt_used == move_ids.size()) {
        std::cout << "Each process is only referred to once" << std::endl;
    }

    Process* start_process = nullptr;

    for (auto& p : processes) {
        if (p.name == "in") {
            start_process = &p;
            break;
        }
    }

    if (start_process == nullptr) {
        return;
    }

    for (auto& part : parts) {
        Process* current_process = start_process;
        Action_data action_data = { Action_id::none };
        bool switched_process = true;
        while (switched_process) {
            switched_process = false;
            for (auto& rule : current_process->rules) {
                bool use_action_data = true;
                int val_to_match = 0;
                switch (rule.param_id) {
                case Param_id::x: val_to_match = part.vals.x; break;
                case Param_id::m: val_to_match = part.vals.m; break;
                case Param_id::a: val_to_match = part.vals.a; break;
                case Param_id::s: val_to_match = part.vals.s; break;
                }
                switch (rule.operator_id) {
                case Operator_id::gt:
                    if (val_to_match <= rule.val) {
                        use_action_data = false;
                    }
                    break;
                case Operator_id::lt:
                    if (val_to_match >= rule.val) {
                        use_action_data = false;
                    }
                    break;
                }

                if (use_action_data && rule.action_data.action_id == Action_id::accept) {
                    part.accepted = true;
                    break;
                }
                if (use_action_data && rule.action_data.action_id == Action_id::reject) {
                    break;
                }
                if (use_action_data && rule.action_data.action_id == Action_id::move) {
                    current_process = &processes[rule.action_data.move_to_id];
                    switched_process = true;
                    break;
                }
            }
        }
    }

    int the_sum = 0;
    for (auto& p : parts) {
        if (p.accepted) {
            the_sum += p.vals.x + p.vals.m + p.vals.a + p.vals.s;
        }
    }

    std::cout << "AOC19-1: " << the_sum << std::endl;

    struct Minmax_values {
        int xmin, xmax;
        int mmin, mmax;
        int amin, amax;
        int smin, smax;
        bool valid;
    };
    struct Rule_data {
        Rule* rule;
        bool accepted;
        Minmax_values vals;
        std::vector<Rule*> rule_chain;
    };

    std::vector<Rule_data> rule_data_vector;
    rule_data_vector.push_back({ &start_process->rules[0],false,1,4000,1,4000,1,4000,1,4000,false,{} });

    auto shrink_range = [](Rule_data* rule_data) {
        int* the_min;
        int* the_max;
        Rule* rule = rule_data->rule;
        Minmax_values cur_vals = rule_data->vals;
        cur_vals.valid = false;

        switch (rule->param_id) {
        case Param_id::x: the_min = &cur_vals.xmin; the_max = &cur_vals.xmax; break;
        case Param_id::m: the_min = &cur_vals.mmin; the_max = &cur_vals.mmax; break;
        case Param_id::a: the_min = &cur_vals.amin; the_max = &cur_vals.amax; break;
        case Param_id::s: the_min = &cur_vals.smin; the_max = &cur_vals.smax; break;
        }

        int initial_min = *the_min;
        int initial_max = *the_max;

        *the_min = rule->val + 1;
        Minmax_values vals_above = cur_vals;
        if (*the_max >= *the_min) {
            vals_above.valid = true;
        }
        *the_min = initial_min;
        *the_max = rule->val - 1;
        Minmax_values vals_below = cur_vals;
        if (*the_max >= *the_min) {
            vals_below.valid = true;
        }

        std::vector<Minmax_values> minmax_values = {};
        // Shrink to above and below the criteria
        // First set is if we meet the criteria. Second is if not
        if (rule->operator_id == Operator_id::gt) {
            minmax_values.push_back(vals_above);
            minmax_values.push_back(vals_below);
        }
        if (rule->operator_id == Operator_id::lt) {
            minmax_values.push_back(vals_below);
            minmax_values.push_back(vals_above);
        }

        return minmax_values;
    };

    int idx_progress = 0;
    while (idx_progress < rule_data_vector.size()) {
        rule_data_vector[idx_progress].rule_chain.push_back(rule_data_vector[idx_progress].rule);
        Rule* rule = rule_data_vector[idx_progress].rule;
        bool use_action_data = false;

        if (rule->operator_id == Operator_id::none) {
            use_action_data = true;
        }
        if (rule->operator_id == Operator_id::gt || rule->operator_id == Operator_id::lt) {
            auto shrinked = shrink_range(&rule_data_vector[idx_progress]);
            if (shrinked[0].valid) {
                rule_data_vector[idx_progress].vals = shrinked[0];
                use_action_data = true;
            }
            if (shrinked[1].valid) {
                rule_data_vector.push_back({ rule->next_rule, false, shrinked[1], rule_data_vector[idx_progress].rule_chain });
            }
        }
        if (use_action_data) {
            switch (rule->action_data.action_id) {
            case Action_id::accept: rule_data_vector[idx_progress].accepted = true; break;
            case Action_id::reject: break;
            case Action_id::move:
                auto r = &processes[rule->action_data.move_to_id].rules[0];
                rule_data_vector.push_back({ r,false,rule_data_vector[idx_progress].vals,rule_data_vector[idx_progress].rule_chain });
                break;
            }
        }
        idx_progress++;
    }

    unsigned long long max_val = 0;
    Rule_data* max_rule_data = nullptr;
    int cnt_accepted = 0;
    unsigned long long tot_val = 0;
    for (auto& el : rule_data_vector) {
        if (el.accepted) {
            cnt_accepted++;
            unsigned long long xrange = (el.vals.xmax - el.vals.xmin + 1);
            unsigned long long mrange = (el.vals.mmax - el.vals.mmin + 1);
            unsigned long long arange = (el.vals.amax - el.vals.amin + 1);
            unsigned long long srange = (el.vals.smax - el.vals.smin + 1);
            unsigned long long cur_val = xrange * mrange * arange * srange;
            tot_val += cur_val;
            if (cur_val > max_val) {
                max_val = cur_val;
                max_rule_data = &el;
            }
        }
    }
    // TODO: min/max för "xmas" i varje flöde. Undvik sedan att räkna överlapp med de resulterande 4d-boxarna dubbelt
    std::cout << "Tot: " << tot_val << " Max: " << max_val << std::endl;
    std::cout << "AOC19-2: " << tot_val << std::endl;
    // TODO: Målvärde, test: 167409079868000

}

int main() {
	aoc19();

	return 0;
}