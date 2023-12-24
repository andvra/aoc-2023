#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <chrono>

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
    auto lines = read_file("aoc19_real.txt");
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
        int* the_min = nullptr;
        int* the_max = nullptr;
        Rule* rule = rule_data->rule;
        Minmax_values cur_vals = rule_data->vals;
        cur_vals.valid = false;

        switch (rule->param_id) {
        case Param_id::x: the_min = &cur_vals.xmin; the_max = &cur_vals.xmax; break;
        case Param_id::m: the_min = &cur_vals.mmin; the_max = &cur_vals.mmax; break;
        case Param_id::a: the_min = &cur_vals.amin; the_max = &cur_vals.amax; break;
        case Param_id::s: the_min = &cur_vals.smin; the_max = &cur_vals.smax; break;
        }

        int add_above = rule->operator_id == Operator_id::gt ? 1 : 0;
        int add_below = rule->operator_id == Operator_id::gt ? 0 : -1;

        int initial_min = *the_min;

        *the_min = rule->val + add_above;
        Minmax_values vals_above = cur_vals;
        if (*the_max >= *the_min) {
            vals_above.valid = true;
        }

        *the_min = initial_min;
        *the_max = rule->val + add_below;
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

    std::cout << "AOC19-2: " << tot_val << std::endl;
}

void aoc20() {
    enum class Module_type { broadcast, flipflop, con, dummy};
    struct Module {
        int id;
        std::string name;
        Module_type type;
        std::vector<Module*> src;
        std::vector<Module*> dst;
        bool is_on = false;
    };
    std::vector<Module> modules = {};

    auto lines = read_file("aoc20_real.txt");
    std::vector<std::vector<std::string>> line_parts = {};

    for (auto& line : lines) {
        auto ss = split_string(line, " -> ");
        ss[1] = ss[1].substr(3);
        auto dest_strings = split_string(ss[1], ",");
        std::vector<std::string> parts = {};
        parts.push_back(ss[0]);
        parts.push_back(dest_strings[0]);
        for (int i = 1; i < dest_strings.size(); i++) {
            parts.push_back(dest_strings[i].substr(1));
        }
        line_parts.push_back(parts);
    }

    for (int idx_module = 0; idx_module < line_parts.size(); idx_module++) {
        Module_type module_type = {};
        std::string name = {};
        switch (line_parts[idx_module][0][0]) {
        case 'b': module_type = Module_type::broadcast; name = line_parts[idx_module][0].substr(0); break;
        case '%': module_type = Module_type::flipflop; name = line_parts[idx_module][0].substr(1); break;
        case '&': module_type = Module_type::con; name = line_parts[idx_module][0].substr(1); break;
        }
        modules.push_back({ idx_module, name, module_type, {}, {} });
    }

    for (int idx_module = 0; idx_module < line_parts.size(); idx_module++) {
        for (int idx_dest = 1; idx_dest < line_parts[idx_module].size(); idx_dest++) {
            std::string name = line_parts[idx_module][idx_dest];
            int idx_swap = -1;
            for (int i = 0; i < modules.size(); i++) {
                if (modules[i].name == name) {
                    idx_swap = i;
                    break;
                }
            }
            if (idx_swap == -1) {
                Module new_module = { modules.size(), name, Module_type::dummy , {}, {} };
                modules.push_back(new_module);
                idx_swap = modules.size() - 1;
            }
            modules[idx_module].dst.push_back(&modules[idx_swap]);
            modules[idx_swap].src.push_back(&modules[idx_module]);
        }
    }

    Module* broadcaster = nullptr;
    for (auto& m : modules) {
        if (m.name == "broadcaster") {
            broadcaster = &m;
            break;
        }
    }

    Module* lb = nullptr;
    for (auto& m : modules) {
        // LB is the parent of rx
        if (m.name == "lb") {
            lb = &m;
            break;
        }
    }
    std::vector<Module*> lb_src = {};
    for (auto m : lb->src) {
        lb_src.push_back(m);
    }

    size_t num_presses = 100000;
    size_t low_sent = 0;
    size_t high_sent = 0;
    size_t num_presses_for_rx = 0;
    int max_num_to_process = 100;
    bool done = false;
    int cur_process_count = 0;
    int next_process_count = 0;
    int idx_buffer_read = 0;
    int idx_buffer_write = 1;
    int ctn_added = 0;
    int idx_p = 0;
    bool all_high = false;
    std::vector<Module*> to_process(2 * max_num_to_process);
    size_t res_pt1 = 0;
    size_t progress_print_interval = num_presses / 100;
    size_t progress_next_print = progress_print_interval;
    auto t_start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<unsigned long long>> lb_low_push(lb_src.size());
    for (size_t idx_press = 1; idx_press <= num_presses; idx_press++) {
        if (idx_press == progress_next_print) {
            auto t_current = std::chrono::high_resolution_clock::now();
            auto duration = duration_cast<std::chrono::seconds>(t_current - t_start);
            std::cout << 100.0f * idx_press / (float)num_presses << "% done (" << duration.count() << "s)" << std::endl;
            progress_next_print += progress_print_interval;
        }
        low_sent++; // Press the button
        done = false;
        cur_process_count = 0;
        next_process_count = 0;
        idx_buffer_read = 0;
        idx_buffer_write = 1;
        to_process[idx_buffer_read * max_num_to_process + cur_process_count++] = broadcaster;

        while (!done) {
            ctn_added = 0;
            for (idx_p = idx_buffer_read * max_num_to_process; idx_p < idx_buffer_read * max_num_to_process + cur_process_count; idx_p++) {
                Module* m = to_process[idx_p];
                low_sent += m->is_on ? 0 : m->dst.size();
                high_sent += m->is_on ? m->dst.size() : 0;
                for (auto dst : m->dst) {
                    bool do_add_module = false;
                    switch (dst->type) {
                    case Module_type::dummy:
                        do_add_module = true;
                        break;
                    case Module_type::flipflop:
                        if (!m->is_on) {
                            dst->is_on = !dst->is_on;
                            do_add_module = true;
                        }
                        break;
                    case Module_type::con:
                        all_high = true;
                        for (auto& src : dst->src) {
                            if (!src->is_on) {
                                all_high = false;
                            }
                        }
                        dst->is_on = !all_high;
                        do_add_module = true;
                        if (dst->is_on) {
                            for (int idx_lb = 0; idx_lb < lb_src.size(); idx_lb++) {
                                if (lb_src[idx_lb] == dst) {
                                    lb_low_push[idx_lb].push_back(idx_press);
                                }
                            }
                        }
                        break;
                    }
                    if (do_add_module) {
                        to_process[idx_buffer_write * max_num_to_process + ctn_added++] = dst;
                    }
                }
            }
            done = ctn_added == 0;
            idx_buffer_read = (idx_buffer_read + 1) % 2;
            idx_buffer_write = (idx_buffer_write + 1) % 2;
            cur_process_count = ctn_added;
        }
        if (idx_press == 1000) {
            res_pt1 = low_sent * high_sent;
        }
    }

    // Offset / step
    struct Trigger_interval {
        unsigned long long offset;
        unsigned long long interval;
    };

    Trigger_interval steps[4] = {};

    for (int idx_name = 0; idx_name < lb_low_push.size(); idx_name++) {
        steps[idx_name] = { lb_low_push[idx_name][0], lb_low_push[idx_name][1] - lb_low_push[idx_name][0] };
        std::cout << lb_src[idx_name]->name << ": " << steps[idx_name].offset << ": " << steps[idx_name].interval << std::endl;
    }

    bool is_ok;
    int idx_check;
    unsigned long long check_val;
    int mod;
    // Manually adjusted start-i to the correct answer for faster compute time
    // NB: it's currently not the correct answer, apparently..
    for (unsigned long long i = 32'244'839'204; i < 100'000'000'000; i++) {
        check_val = steps[0].offset + steps[0].interval * i;
        if (i % 100'000'000 == 0) {
            std::cout << "Checking val " << check_val << std::endl;
        }
        is_ok = true;
        for (idx_check = 1; idx_check < 4; idx_check++) {
            mod = (check_val - steps[idx_check].offset) % steps[idx_check].interval;
            if (mod != 0) {
                is_ok = false;
                break;
            }
        }
        if (is_ok) {
            num_presses_for_rx = check_val;
            break;
        }
    }

    std::cout << std::endl;

    std::cout << "AOC20-1: " << res_pt1 << std::endl;
    std::cout << "AOC20-2: " << num_presses_for_rx << std::endl;
    //130688333297869
    //253302889093151 -- hittad på nätet
}

void aoc21() {
    auto lines = read_file("aoc21_real.txt");
    int num_copies_per_row = 11;
    int num_copies_per_col = 11;
    std::vector<int> squares(num_copies_per_row * num_copies_per_col * lines.size() * lines[0].size());
    size_t idx_start = 0;
    size_t num_cols = num_copies_per_col * lines[0].size();
    size_t num_rows = num_copies_per_row * lines.size();

    for (int idx_line = 0; idx_line < lines.size(); idx_line++) {
        for (int idx_char = 0; idx_char < lines[idx_line].size(); idx_char++) {
            int c = lines[idx_line][idx_char];
            bool is_start = false;
            switch (c) {
            case 'S':
                c = -2;
                is_start = true;
                break;
            case '#':
                c = -1;
                break;
            case '.':
                c = -2;
                break;
            default:
                break;
            }

            for (int idx_row = 0; idx_row < num_copies_per_row; idx_row++) {
                for (int idx_col = 0; idx_col < num_copies_per_col; idx_col++) {
                    int idx_cur_square = idx_row * lines.size() * num_cols + idx_line * num_cols + idx_col * lines[0].size() + idx_char;
                    if (is_start && idx_row == num_copies_per_row / 2 && idx_col == num_copies_per_col / 2) {
                        squares[idx_cur_square] = 0;
                        idx_start = idx_cur_square;
                    }
                    else {
                        squares[idx_cur_square] = c;
                    }
                }
            }
        }
    }

    std::vector<int> idx_to_evaluate(num_cols* num_rows);
    idx_to_evaluate[0] = idx_start;
    int idx_eval_start = 0;
    int idx_eval_end_exclusive = 1;
    size_t num_evaluate_elem = 1;
    for (size_t idx_step = 0; idx_step < 65 + 131 * 5; idx_step++) {
        for (size_t i = idx_eval_start; i < idx_eval_end_exclusive; i++) {
            auto idx_eval = idx_to_evaluate[i];
            int idx_left = idx_eval - 1;
            int idx_right = idx_eval + 1;
            int idx_up = idx_eval - num_cols;
            int idx_down = idx_eval + num_cols;
            std::vector<int> idx_to_use = {};

            if (idx_left >= 0 && idx_left % num_cols == (idx_eval % num_cols) - 1) {
                idx_to_use.push_back(idx_left);
            }
            if (idx_right % num_cols == (idx_eval % num_cols) + 1) {
                idx_to_use.push_back(idx_right);
            }
            if (idx_up >= 0) {
                idx_to_use.push_back(idx_up);
            }
            if (idx_down < num_cols * num_rows) {
                idx_to_use.push_back(idx_down);
            }
            for (auto idx_cur : idx_to_use) {
                if (squares[idx_cur] == -2 || squares[idx_eval] + 1 < squares[idx_cur]) {
                    squares[idx_cur] = squares[idx_eval] + 1;
                    idx_to_evaluate[num_evaluate_elem++] = idx_cur;
                }
            }
        }
        idx_eval_start = idx_eval_end_exclusive;
        idx_eval_end_exclusive = num_evaluate_elem;
    }

    size_t num_possible = 0;
    for (auto c : squares) {
        if (c >= 0 && c <= 64 && (c % 2 == 0)) {
            num_possible++;
        }
    }

    for (int idx_block_row = 0; idx_block_row < num_copies_per_row; idx_block_row++) {
        for (int idx_block_col = 0; idx_block_col < num_copies_per_col; idx_block_col++) {
            int block_possible = 0;
            for (int idx_row = 0; idx_row < lines.size(); idx_row++) {
                for (int idx_col = 0; idx_col < lines[0].size(); idx_col++) {
                    int idx_cur = (idx_block_row * lines.size() + idx_row) * num_cols + idx_block_col * lines[0].size() + idx_col;
                    if (squares[idx_cur] >= 0 && (squares[idx_cur] % 2 == 0)) {
                        block_possible++;
                    }
                }
            }
            std::cout << block_possible << "\t";
        }
        std::cout << std::endl;
    }

    // Numbers below are extracted from the print above, where we see number of possibilities at the edges
    // In between follows a simple pattern
    unsigned long long num_gardens_across = (26501365 - 65) * 2 / 131 + 1;
    unsigned long long val_first_row = 971 + 5859 + 992;
    unsigned long long val_last_row = 974 + 5853 + 985;
    unsigned long long val_middle_row = 5846 + 5866 + (7780 * (num_gardens_across - 2 + 1)) / 2 + (7769 * (num_gardens_across - 2 - 1)) / 2;
    unsigned long long val_others = 0;
    for (unsigned long long i = 1; i < num_gardens_across / 2; i++) {
        unsigned long long cur_gardens_across = i * 2 + 3;
        unsigned long long val_from_above = 971 + 992 + 6810 + 6818 + (7780 * (cur_gardens_across - 4 + 1)) / 2 + (7769 * (cur_gardens_across - 4 - 1)) / 2;
        unsigned long long val_from_below = 974 + 985 + 6805 + 6817 + (7780 * (cur_gardens_across - 4 + 1)) / 2 + (7769 * (cur_gardens_across - 4 - 1)) / 2;
        val_others += val_from_above + val_from_below;
    }

    std::cout << "AOC21-1: " << num_possible << std::endl;
    std::cout << "AOC21-2: " << val_first_row + val_last_row + val_middle_row + val_others << std::endl;
}

void aoc22() {
    auto lines = read_file("aoc22_real.txt");

    struct Coord3i {
        int x;
        int y;
        int z;
    };

    struct Cube_extension {
        int id;
        Coord3i e1;
        Coord3i e2;
        std::vector<Coord3i> cubes;
        std::set<Cube_extension*> supported_by;
        std::set<Cube_extension*> support_to;
    };

    // Found this by scanning the input: 0 <= x,y <= 9
    std::vector<std::vector<std::vector<Cube_extension*>>> occupied_z(10);
    for (auto& x : occupied_z) {
        x.resize(10);
        for (auto& y : x) {
            // There are z-values in the range 1-305. With 306 elements, we can index
            //  with the actual z-value
            y.resize(306);
        }
    }

    std::vector<Cube_extension> cube_extensions = {};
    Cube_extension ground = { 0,0,1,9,9,1 };
    int id = 65;
    for (auto& line : lines) {
        auto parts = split_string(line, "~");
        auto e1s = split_string(parts[0], ",");
        auto e2s = split_string(parts[1], ",");
        Coord3i e1 = { std::atoi(e1s[0].c_str()),std::atoi(e1s[1].c_str()) ,std::atoi(e1s[2].c_str()) };
        Coord3i e2 = { std::atoi(e2s[0].c_str()),std::atoi(e2s[1].c_str()) ,std::atoi(e2s[2].c_str()) };
        cube_extensions.push_back({ id++, e1,e2 });
    }

    for (auto& el : cube_extensions) {
        int min_x = std::min(el.e1.x, el.e2.x);
        int min_y = std::min(el.e1.y, el.e2.y);
        int min_z = std::min(el.e1.z, el.e2.z);
        int max_x = std::max(el.e1.x, el.e2.x);
        int max_y = std::max(el.e1.y, el.e2.y);
        int max_z = std::max(el.e1.z, el.e2.z);
        for (int x = min_x; x <= max_x; x++) {
            for (int y = min_y; y <= max_y; y++) {
                for (int z = min_z; z <= max_z; z++) {
                    occupied_z[x][y][z] = &el;
                    el.cubes.push_back({ x,y,z });
                }
            }
        }
    }
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            occupied_z[x][y][0] = &ground;
        }
    }

    bool done = false;
    int steps_left = 100000;
    while (!done && steps_left > 0) {
        bool something_moved = false;
        for (auto& el : cube_extensions) {
            bool can_move = true;
            for (auto& cube : el.cubes) {
                if (occupied_z[cube.x][cube.y][cube.z - 1] != nullptr && occupied_z[cube.x][cube.y][cube.z - 1] != &el) {
                    can_move = false;
                }
            }
            if (can_move) {
                for (auto& cube : el.cubes) {
                    occupied_z[cube.x][cube.y][cube.z] = nullptr;
                }
                for (auto& cube : el.cubes) {
                    cube.z -= 1;
                    occupied_z[cube.x][cube.y][cube.z] = &el;
                }
                something_moved = true;
            }
        }
        steps_left--;
        done = !something_moved;
    }

    for (auto& el : cube_extensions) {
        bool can_move = true;
        for (auto& cube : el.cubes) {
            if (occupied_z[cube.x][cube.y][cube.z - 1] != nullptr && occupied_z[cube.x][cube.y][cube.z - 1] != &el) {
                el.supported_by.insert(occupied_z[cube.x][cube.y][cube.z - 1]);
                occupied_z[cube.x][cube.y][cube.z - 1]->support_to.insert(&el);
            }
        }
    }

    int cnt_possible_remove = 0;
    for (auto& el : cube_extensions) {
        bool can_remove = true;
        for (auto x : el.support_to) {
            if (x->supported_by.size() == 1) {
                can_remove = false;
            }
        }
        if (can_remove) {
            cnt_possible_remove++;
        }
    }

    std::cout << "AOC22-1: " << cnt_possible_remove << std::endl;

    // NB: z is downwards
    // Found this by scanning the input: 0 <= x,y <= 9
}

int main() {
    auto t_start = std::chrono::high_resolution_clock::now();
	//aoc19();
    //aoc20();
    //aoc21();
    aoc22();
    auto t_end = std::chrono::high_resolution_clock::now();

    auto duration = duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;

	return 0;

}