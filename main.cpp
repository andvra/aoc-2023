#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <chrono>
#include <algorithm>
#include <map>
#include <bit>
#include <bitset>

std::vector<std::string> read_file(std::string fn) {
    //std::string root_dir = R"(D:\dev\test\aoc-2023\input\)";
    std::string root_dir = R"(C:\Users\andre\source\test\aoc-2023\input\)";
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
        offset = idx + delimiter.size();
    }

    return ret;
}

// The last element on each row is the scalar to compare with
std::vector<double> linear_solver(std::vector<std::vector<double>> input) {
    bool done = false;
    int variables_complete = 0;
    int steps = 0;

    while (!done) {
        // 1. Swap rows if needed
        if (input[variables_complete][variables_complete] == 0) {
            bool did_switch = false;
            for (int i = variables_complete + 1; i < input.size(); i++) {
                if (input[i][variables_complete] != 0) {
                    auto tmp = input[i];
                    input[i] = input[variables_complete];
                    input[variables_complete] = tmp;
                    did_switch = true;
                    break;
                }
            }
            if (!did_switch) {
                std::cout << "Couldn't switch. Aborting" << std::endl;
                return {};
            }
        }
        // 2. Scale row
        double factor = input[variables_complete][variables_complete];
        if (factor != 1) {
            for (int i = variables_complete; i < input[variables_complete].size(); i++) {
                input[variables_complete][i] /= factor;
            }
        }
        // 3. Multiply onto other rows
        for (int idx_row = 0; idx_row < input.size(); idx_row++) {
            if (idx_row == variables_complete) {
                continue;
            }
            double factor = input[idx_row][variables_complete];
            if (factor != 0) {
                for (int i = variables_complete; i < input[variables_complete].size(); i++) {
                    input[idx_row][i] -= factor * input[variables_complete][i];
                }
            }
        }
        steps++;
        if (++variables_complete == input.size()) {
            done = true;
        }
    }

    std::vector<double> ret(input.size(), 0);

    for (int i = 0; i < input.size(); i++) {
        ret[i] = input[i][input[i].size() - 1];
    }

    return ret;
}

struct Graph_node {
    int id;
    std::string name;
    std::vector<Graph_node*> neighbors;
};

struct Node_head {
    Graph_node* node;
    Node_head* last_head;
};

std::vector<Graph_node*> shortest_path(Graph_node* src, Graph_node* dst, int num_graph_nodes) {
    std::vector<int> dist(num_graph_nodes, 0);
    int cur_distance = 1;
    std::vector<Graph_node*> to_search(num_graph_nodes * num_graph_nodes); // NB could be optimized when needed
    int num_search_nodes = 0;
    int idx_start = 0;
    to_search[num_search_nodes++] = src;
    int idx_end_exclusive = num_search_nodes;
    std::vector<Graph_node*> ret = {};
    bool done = false;

    while (!done) {
        for (int i = idx_start; i < idx_end_exclusive; i++) {
            if (to_search[i] == dst) {
                Graph_node* el = to_search[i];
                while (el != src) {
                    ret.push_back(el);
                    for (auto n : el->neighbors) {
                        auto dist1 = dist[n->id];
                        auto dist2 = dist[el->id];
                        if ((n == src || dist1 != 0) && dist1 < dist2) {
                            el = n;
                            break;
                        }
                    }
                }
                break;
            }
            for (auto& n : to_search[i]->neighbors) {
                if (n == src) {
                    continue;
                }
                if (dist[n->id] == 0 || dist[n->id] > cur_distance) {
                    dist[n->id] = cur_distance;
                    to_search[num_search_nodes++] = n;
                }
            }
        }
        idx_start = idx_end_exclusive;
        idx_end_exclusive = num_search_nodes;
        cur_distance++;
        if (idx_end_exclusive == idx_start) {
            done = true;
        }
    }

    return ret;
}

std::vector<Graph_node*> connected_nodes(std::vector<Graph_node>& all_nodes, Graph_node* src, int num_graph_nodes) {
    std::vector<bool> visited(num_graph_nodes, false);
    std::vector<Graph_node*> to_search(num_graph_nodes * num_graph_nodes); // NB could be optimized when needed
    int num_search_nodes = 0;
    int idx_start = 0;
    to_search[num_search_nodes++] = src;
    int idx_end_exclusive = num_search_nodes;
    bool done = false;

    while (!done) {
        for (int i = idx_start; i < idx_end_exclusive; i++) {
            visited[to_search[i]->id] = true;
            for (auto& n : to_search[i]->neighbors) {
                if (!visited[n->id]) {
                    to_search[num_search_nodes++] = n;
                }
            }
        }
        idx_start = idx_end_exclusive;
        idx_end_exclusive = num_search_nodes;
        if (idx_end_exclusive == idx_start) {
            done = true;
        }
    }

    std::vector<Graph_node*> ret = {};
    for (int i = 0; i < visited.size(); i++) {
        if (visited[i]) {
            ret.push_back(&all_nodes[i]);
        }
    }

    return ret;
}

std::string replace_all(std::string s, std::string to_replace, std::string replace_with) {
    auto pos = s.find(to_replace, 0);
    while (pos != std::string::npos) {
        s.replace(pos, to_replace.size(), replace_with);
        pos = s.find(to_replace, 0);
    }

    return s;
}

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long m = a % b;
        a = b;
        b = m;
    }

    return a;
}

long long lcm(long long a, long long b) {
    return b * (a / gcd(a, b));
}

long long lcm(std::vector<long long> vals) {
    long long ret = 0;

    if (vals.size() == 0) {
        return 0;
    }

    ret = vals[0];

    for (int i = 1; i < vals.size(); i++) {
        ret = lcm(ret, vals[i]);
    }

    return ret;
}

void aoc01() {
    auto lines = read_file("aoc01_real.txt");
    std::vector<std::string> to_replace = {
        "one",  "two",  "three", "four", "five", "six", "seven", "eight", "nine"
    };
    // twoone should be 21 (apparently), so we lazy-replace in this way
    std::vector<std::string> replace_with = {
    "o1e",  "t2o",  "th3ee", "f4ur", "f5ve", "s6x", "se7en", "ei8ht", "n9ne"
    };

    for (int idx_part = 0; idx_part < 2; idx_part++) {
        int val = 0;
        for (auto& line : lines) {
            auto s = line;
            if (idx_part == 1) {
                bool done = false;
                while (!done) {
                    bool found_any = false;
                    int min_idx = 10000;
                    int pos[9] = {};
                    for (int i = 0; i < to_replace.size(); i++) {
                        auto cur_pos = s.find(to_replace[i]);
                        if (cur_pos != std::string::npos) {
                            found_any = true;
                            pos[i] = cur_pos;
                            if (min_idx == 10000 || cur_pos < pos[min_idx]) {
                                min_idx = i;
                            }
                        }
                    }
                    if (found_any) {
                        s = s.replace(pos[min_idx], to_replace[min_idx].size(), replace_with[min_idx]);
                    }
                    else {
                        done = true;
                    }
                }
            }
            int first = 100;
            int last = 0;
            for (auto c : s) {
                if (c >= '0' && c <= '9') {
                    last = c - '0';
                    if (first == 100) {
                        first = c - '0';
                    }
                }
            }
            val += (10 * first + last);
        }

        std::cout << std::format("AOC01-{}: {}", idx_part + 1, val) << std::endl;
    }
}

void aoc02() {
    auto lines = read_file("aoc02_real.txt");

    std::vector<int> max_per_color = { 12,13,14 };

    std::map<std::string, int> color_type{
        {"red",     0},
        {"green",   1},
        {"blue",    2}
    };

    struct Cube {
        int type;
        int count;
    };
    struct Round {
        std::vector<Cube> cubes;
    };
    struct Game {
        int id;
        std::vector<Round> rounds;
    };

    int ret1 = 0;
    int ret2 = 0;
    std::vector<Game> games = {};

    // Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
    for (auto& line : lines) {
        auto game = split_string(line, ": ");
        auto game_split = split_string(game[0], " ");
        auto game_number_string = game_split[1];
        auto sets = split_string(game[1], "; ");
        Game cur_game = { std::atoi(game_number_string.c_str()), {} };
        for (auto set : sets) {
            auto cubes = split_string(set, ", ");
            Round round = {};
            for (auto cube : cubes) {
                auto el = split_string(cube, " ");
                auto num_string = el[0];
                auto color_string = el[1];
                if (color_type.count(color_string) > 0) {
                    round.cubes.push_back({ color_type[color_string], std::atoi(num_string.c_str()) });
                }
                else {
                    std::cout << "Unknown color " << el[1] << std::endl;
                }
            }
            cur_game.rounds.push_back(round);
        }
        games.push_back(cur_game);
    }

    for (auto& game : games) {
        int max_counts[3] = {};
        for (auto& round : game.rounds) {
            for (auto& cube : round.cubes) {
                if (cube.count > max_counts[cube.type]) {
                    max_counts[cube.type] = cube.count;
                }
            }
        }
        bool valid_game = true;
        for (int i = 0; i < 3; i++) {
            if (max_counts[i] > max_per_color[i]) {
                valid_game = false;
                break;
            }
        }
        if (valid_game) {
            ret1 += game.id;
        }
        ret2 += max_counts[0] * max_counts[1] * max_counts[2];
    }

    std::cout << "AOC02-1: " << ret1 << std::endl;
    std::cout << "AOC02-2: " << ret2 << std::endl;

}

void aoc04() {
    auto lines = read_file("aoc04_real.txt");

    int ret1 = 0;
    std::vector<int> num_copies(lines.size(), 1);

    for (int idx_line = 0; idx_line < lines.size(); idx_line++) {
        auto& line = lines[idx_line];
        auto id_split = split_string(line, ": ");
        auto card_split = split_string(id_split[1], " | ");
        for (int i = 0; i < 5; i++) {
            for (int idx_split = 0; idx_split < 2; idx_split++) {
                card_split[idx_split] = replace_all(card_split[idx_split], "  ", " ");
            }
        }
        auto win_number_string = split_string(card_split[0], " ");
        auto my_number_string = split_string(card_split[1], " ");

        std::set<int> numbers = {};
        for (auto& n : my_number_string) {
            numbers.insert(std::atoi(n.c_str()));
        }
        int cnt_before = numbers.size();
        for (auto& n : win_number_string) {
            numbers.insert(std::atoi(n.c_str()));
        }
        int cnt_after = numbers.size();

        int num_missed_winners = cnt_after - cnt_before;
        int num_winners = win_number_string.size() - num_missed_winners;

        int cur_ret = 0;
        if (num_missed_winners < win_number_string.size()) {
            cur_ret = 1;
            auto idx_end_exclusive = win_number_string.size() - num_missed_winners;
            for (int i = 1; i < idx_end_exclusive; i++) {
                cur_ret *= 2;
            }
        }
        ret1 += cur_ret;

        for (int i = idx_line + 1; i < idx_line + 1 + num_winners; i++) {
            num_copies[i] += num_copies[idx_line];
        }
    }

    int ret2 = 0;
    for (auto c : num_copies) {
        ret2 += c;
    }

    std::cout << "AOC04-1: " << ret1 << std::endl;
    std::cout << "AOC04-2: " << ret2 << std::endl;
}

void aoc05() {
    auto lines = read_file("aoc05_test.txt");

    struct Range {
        long long offset;
        long long cnt;
    };

    struct Resource_map {
        int type;
        Range range;
        long long map_offset;
    };

    std::vector<std::vector<Resource_map>> maps(7);

    int cur_type = 0;
    bool read_numbers = false;
    std::map<std::string, int> label_to_type = {
        {"seed", 0},
        {"soil", 1},
        {"fert", 2},
        {"wate", 3},
        {"ligh", 4},
        {"temp", 5},
        {"humi", 6},
    };

    for (int idx_line = 1; idx_line < lines.size(); idx_line++) {
        auto& line = lines[idx_line];
        if (line.empty()) {
            read_numbers = false;
        }
        if (read_numbers) {
            auto ns = split_string(line, " ");
            auto dst_offset = std::atoll(ns[0].c_str());
            auto src_offset = std::atoll(ns[1].c_str());
            auto cnt = std::atoll(ns[2].c_str());
            auto map_offset = dst_offset - src_offset;
            maps[cur_type].push_back({ cur_type, src_offset, cnt, map_offset });
        }
        if (!line.empty() && !read_numbers) {
            auto ss = line.substr(0, 4);
            if (label_to_type.count(ss) > 0) {
                cur_type = label_to_type[ss];
                read_numbers = true;
            }
            else {
                std::cout << "Could not find label starting with " << ss << std::endl;
            }
        }
    }

    // Checks for overlap. Result: there are no overlap between maps on the same level
    //for (int map_type = 0; map_type < 7; map_type++) {
    //    for (int i = 0; i < maps[map_type].size(); i++) {
    //        auto map1 = maps[map_type][i];
    //        auto xs1 = map1.src_offset;
    //        auto xe1 = map1.src_offset + map1.cnt - 1;
    //        for (int j = i + 1; j < maps[map_type].size(); j++) {
    //            auto map2 = maps[map_type][j];
    //            auto xs2 = map2.src_offset;
    //            auto xe2 = map2.src_offset + map2.cnt - 1;
    //            bool is_overlap = false;
    //            if (xs2 <= xe1 && xe2 >= xs1) {
    //                is_overlap = true;
    //            }
    //            if (xs1 <= xe2 && xe1 >= xs2) {
    //                is_overlap = true;
    //            }
    //            if (is_overlap) {
    //                std::cout << std::format("{} - {}:{} {}:{}", map_type, xs1, xe1, xs2, xe2) << std::endl;
    //            }
    //        }
    //    }
    //}

    for (int map_level = 0; map_level < 7; map_level++) {
        std::sort(maps[map_level].begin(), maps[map_level].end(), [](Resource_map& a, Resource_map& b) { return a.range.offset < b.range.offset; });
    }

    struct Range_track {
        long long offset;
        long long cnt;
        long long offset_original;
        int moved_at_level;
    };
    std::vector<Range_track> ranges_to_handle = { {0, std::numeric_limits<long long>::max(), 0, -1 }};

    for (int map_level = 0; map_level < 7; map_level++) {
        std::vector<Range_track> new_ranges_to_handle = {};
        for (int idx_range = 0; idx_range < ranges_to_handle.size(); idx_range++) {
            std::vector<Range_track> r_splits = { ranges_to_handle[idx_range] };
            for (auto& m : maps[map_level]) {
                for (int idx_rs = 0; idx_rs < r_splits.size(); idx_rs++) {
                    auto r = r_splits[idx_rs];
                    if (r.moved_at_level == map_level) {
                        continue;
                    }
                    auto xs1 = r.offset;
                    auto xe1 = r.offset + r.cnt - 1;
                    auto xs2 = m.range.offset;
                    auto xe2 = m.range.offset + m.range.cnt - 1;
                    if ((xs2 <= xe1 && xe2 >= xs1) ||
                        (xs1 <= xe2 && xe1 >= xs2)) {
                        r_splits.erase(r_splits.begin() + idx_rs);
                        std::vector<Range_track> new_parts = {};
                        if (xs1 < xs2) {
                            new_parts.push_back({ xs1,xs2 - xs1, r.offset_original, r.moved_at_level });
                        }
                        auto max_s = std::max(xs1, xs2);
                        auto min_e = std::min(xe1, xe2);
                        // We want to offset this offset! The source is mapped to another destination
                        new_parts.push_back({ max_s + m.map_offset, min_e - max_s + 1, r.offset_original + m.map_offset, map_level });
                        if (xe1 > xe2) {
                            new_parts.push_back({ xe2 + 1,xe1 - xe2, r.offset_original, r.moved_at_level });
                        }

                        for (auto p : new_parts) {
                            r_splits.push_back(p);
                        }
                    }
                }
            }
            for (auto& x : r_splits) {
                new_ranges_to_handle.push_back(x);
            }
        }
        ranges_to_handle = new_ranges_to_handle;
    }

    //for (auto& r : ranges_to_handle) {
    //    std::cout << r.offset - r.offset_original << " - " << r.offset - r.offset_original + r.cnt - 1 << ": " << r.offset_original << std::endl;
    //}

    auto seed_split = split_string(lines[0], ": ");
    auto seed_strings = split_string(seed_split[1], " ");

    for (int idx_pt = 0; idx_pt < 2; idx_pt++) {
        std::vector<Range> seed_ranges = {};
        long long min_location = std::numeric_limits<long long>::max();

        if (idx_pt == 0) {
            for (int i = 0; i < seed_strings.size(); i ++) {
                auto offset_s = seed_strings[i];
                seed_ranges.push_back({ std::atoll(offset_s.c_str()), 1 });
            }
        }
        if (idx_pt == 1) {
            for (int i = 0; i < seed_strings.size(); i += 2) {
                auto offset_s = seed_strings[i];
                auto cnt_s = seed_strings[i + 1];
                seed_ranges.push_back({ std::atoll(offset_s.c_str()), std::atoll(cnt_s.c_str()) });
            }
        }

        long long min_val = std::numeric_limits<long long>::max();
        for (auto& sr : seed_ranges) {
            for (auto& r : ranges_to_handle) {
                long long xs1 = sr.offset;
                long long xe1 = sr.offset + sr.cnt - 1;
                long long xs2 = r.offset - r.offset_original;
                long long xe2 = r.offset - r.offset_original + r.cnt - 1;
                if ((xs2 <= xe1 && xe2 >= xs1) ||
                    (xs1 <= xe2 && xe1 >= xs2)) {
                    auto max_s = std::max(xs1, xs2);
                    auto cur_min = max_s + r.offset_original;
                    if (cur_min < min_val) {
                        min_val = cur_min;
                    }
                }
            }
        }
        std::cout << std::format("AOC05-{}: {}", idx_pt + 1, min_val) << std::endl;
    }
}

void aoc06() {
    auto lines = read_file("aoc06_real.txt");

    auto time_row_split = split_string(lines[0], ": ");
    auto distance_row_split = split_string(lines[1], ": ");
    auto time_string = replace_all(time_row_split[1], "  ", " ");
    time_string = time_string.substr(1);
    auto distance_string = replace_all(distance_row_split[1], "  ", " ");
    distance_string = distance_string.substr(1);
    auto time_split = split_string(time_string, " ");
    auto distance_split = split_string(distance_string, " ");

    struct Run {
        int time;
        int distance;
    };

    std::vector<Run> runs = {};
    for (int i = 0; i < time_split.size(); i++) {
        runs.push_back({ std::atoi(time_split[i].c_str()), std::atoi(distance_split[i].c_str()) });
    }

    auto eval_run = [](long long time, long long distance) {
        long long num_ways = 0;
        for (long long hold_time = 1; hold_time < time; hold_time++) {
            long long time_to_finish = hold_time + distance / hold_time;
            if (time_to_finish < time) {
                num_ways++;
            }
        }
        return num_ways;
    };

    int ans_pt1 = 1;
    for (auto& run : runs) {
        int num_ways = 0;

        ans_pt1 *= eval_run(run.time, run.distance);
    }

    std::string time_combined_string = {};
    std::string distance_combined_string = {};
    for (auto& run : runs) {
        time_combined_string += std::to_string(run.time);
        distance_combined_string += std::to_string(run.distance);
    }

    long long time_combined = std::atoll(time_combined_string.c_str());
    long long distance_combined = std::atoll(distance_combined_string.c_str());

    auto ans_pt2 = eval_run(time_combined, distance_combined);

    std::cout << std::format("AOC06-{}: {}", 1, ans_pt1) << std::endl;
    std::cout << std::format("AOC06-{}: {}", 2, ans_pt2) << std::endl;
}

void aoc07() {
    auto lines = read_file("aoc07_real.txt");
    struct Hand {
        long long type;
        long long bid;
        long long rank_val;
        long long rank;
        std::vector<long long> card_vals;
    };

    std::map<char, int> card_map = {
        {'2', 2},
        {'3', 3},
        {'4', 4},
        {'5', 5},
        {'6', 6},
        {'7', 7},
        {'8', 8},
        {'9', 9},
        {'T', 10},
        {'J', 11},
        {'Q', 12},
        {'K', 13},
        {'A', 14},
    };
    
    std::vector<Hand> hands = {};

    for (auto& line : lines) {
        auto parts = split_string(line, " ");
        Hand hand = {};
        hand.card_vals.resize(parts[0].size());
        for (int i = 0; i < parts[0].size(); i++) {
            hand.card_vals[i] = card_map[parts[0][i]];
        }
        hand.bid = std::atoll(parts[1].c_str());
        hands.push_back(hand);
    }

    auto get_ranks = [&hands, &card_map](bool use_joker) {
        for (auto& hand : hands) {
            std::map<int, int> m = {};
            int max_cnt = 0;
            for (auto& c : hand.card_vals) {
                bool is_joker = (c == card_map['J']);
                if (m.count(c) > 0) {
                    m[c]++;
                }
                else {
                    m[c] = 1;
                }
                if (!(is_joker && use_joker) && (m[c] > max_cnt)) {
                    max_cnt = m[c];
                }
            }

            auto num_different = m.size();
            if (use_joker && m.count(card_map['J']) > 0) {
                num_different--;
                max_cnt += m[card_map['J']];
            }

            if (use_joker && m.count(card_map['J']) > 0 && m[card_map['J']] == 5) {
                num_different = 1;
                max_cnt = 5;
            }

            long long type = 0;
            switch (num_different) {
            case 1: type = 6; break;
            case 2: max_cnt == 4 ? type = 5 : type = 4; break;
            case 3: max_cnt == 3 ? type = 3 : type = 2; break;
            case 4: type = 1; break;
            case 5: type = 0; break;
            }
            hand.type = type;
            hand.rank_val = hand.type * 1'000'000'000'000;
            long long mult = 1;
            for (int i = 4; i >= 0; i--) {
                long long card_val = hand.card_vals[i];
                if (use_joker && (card_val == card_map['J'])) {
                    card_val = 1;
                }
                hand.rank_val += mult * card_val;
                mult *= 100;
            }
        }
    };

    for (int idx_pt = 1; idx_pt < 3; idx_pt++) {
        long long ans = 0;

        get_ranks(idx_pt == 1 ? false : true);
        std::sort(hands.begin(), hands.end(), [](Hand& a, Hand& b) {return a.rank_val > b.rank_val; });

        for (int i = 0; i < hands.size(); i++) {
            hands[i].rank = hands.size() - i;
            ans += hands[i].rank * hands[i].bid;
        }

        std::cout << std::format("AOC07-{}: {}", idx_pt, ans) << std::endl;
    }
}

void aoc08() {
    auto lines = read_file("aoc08_real.txt");

    struct Node {
        int id;
        std::string name;
        Node* left;
        Node* right;
    };

    std::vector<Node> nodes = {};
    std::string moves = lines[0];

    for (int i = 2; i < lines.size(); i++) {
        Node n = {};
        n.id = i - 2;
        n.name = lines[i].substr(0, 3);
        nodes.push_back(n);
    }

    for (int i = 2; i < lines.size(); i++) {
        auto name_left = lines[i].substr(7, 3);
        auto name_right = lines[i].substr(12, 3);
        for (auto& n : nodes) {
            if (n.name == name_left) {
                nodes[i - 2].left = &n;
            }
            if (n.name == name_right) {
                nodes[i - 2].right = &n;
            }
        }
    }

    for (int idx_part = 1; idx_part < 3; idx_part++) {
        std::vector<Node*> cur_nodes = {};
        std::vector<Node*> end_nodes = {};

        if (idx_part == 1) {
            for (auto& n : nodes) {
                if (n.name == "AAA") {
                    cur_nodes.push_back(&n);
                }
                if (n.name == "ZZZ") {
                    end_nodes.push_back(&n);
                }
            }
        }
        if (idx_part == 2) {
            for (auto& n : nodes) {
                if (n.name[2] == 'A') {
                    cur_nodes.push_back(&n);
                }
                if (n.name[2] == 'Z') {
                    end_nodes.push_back(&n);
                }
            }
        }

        long long idx_cur_move = 0;
        long long num_steps = 0;
        bool done = false;

        long long last_step = 0;
        std::vector<long long> intervals(cur_nodes.size());
        int num_intervals_found = 0;

        while (!done) {
            if (idx_part == 2) {
                for (int i = 0; i < cur_nodes.size(); i++) {
                    if (intervals[i] > 0) {
                        continue;
                    }
                    for (auto e : end_nodes) {
                        if (cur_nodes[i] == e) {
                            intervals[i] = num_steps;
                            num_intervals_found++;
                        }
                    }
                }
                if (num_intervals_found == cur_nodes.size()) {
                    num_steps = lcm(intervals);
                    done = true;
                    break;
                }
            }

            for (int idx_node = 0; idx_node < cur_nodes.size(); idx_node++) {
                if (moves[idx_cur_move] == 'L') {
                    cur_nodes[idx_node] = cur_nodes[idx_node]->left;
                }
                else {
                    cur_nodes[idx_node] = cur_nodes[idx_node]->right;
                }
            }

            idx_cur_move = (idx_cur_move + 1) % moves.size();
            num_steps++;

            bool all_done = true;

            for (auto c : cur_nodes) {
                bool cur_done = false;
                for (auto e : end_nodes) {
                    if (c == e) {
                        cur_done = true;
                    }
                }
                all_done = all_done && cur_done;
            }

            done = all_done;
        }
        std::cout << std::format("AOC08-{}: {}", idx_part, num_steps) << std::endl;
    }
}
void aoc09() {
    auto lines = read_file("aoc09_real.txt");

    struct Row {
        long long score;
        std::vector<long long> vals;
    };

    std::vector<Row> rows = {};

    for (auto& line : lines) {
        auto parts = split_string(line, " ");
        Row row = {};
        for (auto& p : parts) {
            row.vals.push_back(std::atoll(p.c_str()));
        }
        rows.push_back(row);
    }

    long long tot_score_beginning = 0;
    long long tot_score_end = 0;

    for (auto& row : rows) {
        std::vector<std::vector<long long>> diffs = {};
        std::vector<long long>* src = &row.vals;

        bool all_zeros = false;
        while (!all_zeros) {
            all_zeros = true;
            for (auto& x : *src) {
                if (x != 0) {
                    all_zeros = false;
                }
            }
            if (!all_zeros) {
                std::vector<long long> cur_diff(src->size() - 1);
                for (int i = 0; i < src->size() - 1; i++) {
                    cur_diff[i] = (*src)[i + 1] - (*src)[i];
                }
                diffs.push_back(cur_diff);
                src = &diffs.back();
            }
        }
        long long cur_score_beginning = 0;
        long long cur_score_end = 0;
        for (int i = diffs.size() - 2; i >= 0; i--) {
            cur_score_beginning = diffs[i].front() - cur_score_beginning;
            cur_score_end = diffs[i].back() + cur_score_end;
        }
        cur_score_beginning = row.vals.front() - cur_score_beginning;
        cur_score_end = row.vals.back() + cur_score_end;
        tot_score_beginning += cur_score_beginning;
        tot_score_end += cur_score_end;

    }

    std::cout << std::format("AOC09-{}: {}", 1, tot_score_end) << std::endl;
    std::cout << std::format("AOC09-{}: {}", 2, tot_score_beginning) << std::endl;
}

void aoc10() {
    auto lines = read_file("aoc10_real.txt");

    struct Tile {
        int row;
        int col;
        bool con_left;
        bool con_right;
        bool con_up;
        bool con_down;
    };

    struct Pos {
        int row;
        int col;
    };

    int num_rows = lines.size();
    int num_cols = lines[0].size();

    Pos start_pos = {};
    std::vector<std::vector<Tile>> tiles(num_rows, std::vector<Tile>(num_cols));
    std::set<char> set_left = { '-', 'J', '7' };
    std::set<char> set_right = { '-', 'L', 'F' };
    std::set<char> set_up = { '|', 'L', 'J' };
    std::set<char> set_down = { '|', '7', 'F' };
    std::map<char, Tile*> tile_ref = {};

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            char c = lines[row][col];
            if (c == 'S') {
                start_pos = { row,col };
            }
            Tile tile = { };
            tile.row = row;
            tile.col = col;
            tile.con_left = set_left.count(c) > 0;
            tile.con_right = set_right.count(c) > 0;
            tile.con_up = set_up.count(c) > 0;
            tile.con_down = set_down.count(c) > 0;
            tiles[row][col] = tile;
            tile_ref[c] = &tiles[row][col];
        }
    }

    std::set<char> candidates[2] = {};
    int sr = start_pos.row;
    int sc = start_pos.col;
    int idx_c = 0;
    if ((sr > 0) && tiles[sr - 1][sc].con_down) {
        candidates[idx_c++] = set_up;
    }
    if ((sr < num_rows - 1) && tiles[sr + 1][sc].con_up) {
        candidates[idx_c++] = set_down;
    }
    if ((sc > 0) && tiles[sr][sc - 1].con_right) {
        candidates[idx_c++] = set_left;
    }
    if ((sc < num_cols - 1) && tiles[sr][sc + 1].con_left) {
        candidates[idx_c++] = set_right;
    }

    std::set<char> intersection;
    std::set_intersection(candidates[0].begin(), candidates[0].end(), candidates[1].begin(), candidates[1].end(),
        std::inserter(intersection, intersection.begin()));
    if (intersection.size() != 1) {
        std::cout << "ERROR intersection has wrong amount of elements: " << intersection.size() << std::endl;
    }

    Tile ref = *tile_ref[*intersection.begin()];
    ref.row = sr;
    ref.col = sc;
    tiles[sr][sc] = ref;
    Tile* start_tile = &tiles[sr][sc];

    Tile* last_tile = nullptr;
    Tile* cur_tile = start_tile;
    int loop_size = 0;

    std::set<Tile*> loop_tiles = {};

    while (cur_tile != start_tile || last_tile == nullptr) {
        loop_tiles.insert(cur_tile);
        Tile* ignored_tile = last_tile;
        last_tile = cur_tile;
        int cur_row = cur_tile->row;
        int cur_col = cur_tile->col;
        if (cur_tile->con_right && &tiles[cur_row][cur_col + 1] != ignored_tile) {
            cur_tile = &tiles[cur_row][cur_col + 1];
        }
        else if (cur_tile->con_left && &tiles[cur_row][cur_col - 1] != ignored_tile) {
            cur_tile = &tiles[cur_row][cur_col - 1];
        }
        else if (cur_tile->con_up && &tiles[cur_row - 1][cur_col] != ignored_tile) {
            cur_tile = &tiles[cur_row - 1][cur_col];
        }
        else if (cur_tile->con_down && &tiles[cur_row + 1][cur_col] != ignored_tile) {
            cur_tile = &tiles[cur_row + 1][cur_col];
        }
        loop_size++;
    }

    int num_enclosed_tiles = 0;
    for (int row = 0; row < tiles.size(); row++) {
        Tile* last_tile = nullptr;
        int before = num_enclosed_tiles;
        int line_passes = 0;
        Tile* edge_start = nullptr;
        Tile* edge_end = nullptr;
        for (int col = 0; col < tiles[row].size(); col++) {
            Tile* cur_tile = &tiles[row][col];
            bool current_is_loop_tile = loop_tiles.count(cur_tile) > 0;
            bool count_the_edge = false;
            if (current_is_loop_tile && cur_tile->con_right && (cur_tile->con_down || cur_tile->con_up)) {
                edge_start = cur_tile;
            }
            if (current_is_loop_tile && cur_tile->con_left && (cur_tile->con_down || cur_tile->con_up)) {
                edge_end = cur_tile;
                count_the_edge = edge_start->con_down != edge_end->con_down;
                edge_start = nullptr;
                edge_end = nullptr;
            }
            bool is_vert = cur_tile->con_down && !cur_tile->con_left && !cur_tile->con_right;
            if (current_is_loop_tile && (is_vert || count_the_edge)) {
                line_passes++;
            }
            if (!current_is_loop_tile && (line_passes % 2 == 1)) {
                num_enclosed_tiles++;
            }
            last_tile = cur_tile;
        }
    }

    std::cout << std::format("AOC10-{}: {}", 1, loop_size / 2) << std::endl;
    std::cout << std::format("AOC10-{}: {}", 2, num_enclosed_tiles) << std::endl;
}

void aoc12() {
    auto lines = read_file("aoc12_real.txt");

    struct Row {
        std::vector<unsigned int> pieces;
        unsigned int bit_possible;
        unsigned int bit_must;
        unsigned int len;
    };

    for (int idx_part = 1; idx_part < 3; idx_part++) {
        std::vector<Row> rows = {};
        for (auto& line : lines) {
            auto parts = split_string(line, " ");
            if (idx_part == 2) {
                std::string new_parts0 = parts[0];
                std::string new_parts1 = parts[1];
                for (int i = 0; i < 4; i++) {
                    new_parts0 += "?";
                    new_parts0 += parts[0];
                    new_parts1 += ",";
                    new_parts1 += parts[1];
                }
                parts[0] = new_parts0;
                parts[1] = new_parts1;
            }
            auto pieces_s = split_string(parts[1], ",");
            unsigned int row_possible = 0;
            unsigned int row_must = 0;
            unsigned int steps = parts[0].size() - 1;
            std::vector<unsigned int> pieces = {};
            for (auto& s : pieces_s) {
                pieces.push_back(std::atoi(s.c_str()));
            }
            for (int i = 0; i < parts[0].size(); i++) {
                bool must = parts[0][i] == '#';
                bool unknown = parts[0][i] == '?';
                if (must) {
                    row_must |= (1u << steps);
                }
                if (must || unknown) {
                    row_possible |= (1u << steps);
                }
                steps--;
            }
            Row row = {};
            row.bit_possible = row_possible;
            row.bit_must = row_must;
            row.pieces = pieces;
            row.len = parts[0].size();
            rows.push_back(row);
        }

        struct Node {
            Node* parent;
            unsigned int bit_merged;
            unsigned int bit_this;
        };

        long long num_arrangements = 0;
        unsigned int mem_elements = 1'500'000'000;
        std::vector<Node> buffer(mem_elements);
        for (int idx_row = 0; idx_row < rows.size(); idx_row++) {
            std::cout << "Working on row " << idx_row << std::endl;
            auto& row = rows[idx_row];
            std::vector<std::vector<unsigned int>> bit_candidates(row.pieces.size());
            unsigned int lengths_before = 0;
            unsigned int idx_end_exclusive = row.len;
            for (int idx_piece = 0; idx_piece < row.pieces.size(); idx_piece++) {
                unsigned int bit_piece = 1;
                for (unsigned int i = 1; i < row.pieces[idx_piece]; i++) {
                    bit_piece |= (1u << i);
                }
                unsigned int idx_start = 0;
                for (int i = idx_piece + 1; i < row.pieces.size(); i++) {
                    // +1 to allow for space between pieces
                    idx_start += row.pieces[i] + 1;
                }

                // Check from right to left. Eg. with a piece of size 3:
                //  00000111
                //  00001110
                //  00011100
                //  etc.
                unsigned max_possible_shifts = 0;
                for (unsigned int i = idx_start; i < idx_end_exclusive - row.pieces[idx_piece] + 1; i++) {
                    unsigned int bit_shifted_piece = bit_piece << i;
                    if ((bit_shifted_piece | row.bit_possible) == row.bit_possible) {
                        // The piece is a candidate to fit here
                        bit_candidates[idx_piece].push_back(bit_shifted_piece);
                        max_possible_shifts = i;
                    }
                }
                // Add -1 so we make space for an empty square between
                idx_end_exclusive = max_possible_shifts - 1;
                lengths_before += row.pieces[idx_piece];
            }
            long long row_score = 0;
            int num_nodes = 0;
            int idx_start_node = 0;
            int idx_end_node_exclusive = 0;
            for (int idx_piece = 0; idx_piece < row.pieces.size(); idx_piece++) {
                for (int idx_candidate = 0; idx_candidate < bit_candidates[idx_piece].size(); idx_candidate++) {
                    auto& candidate = bit_candidates[idx_piece][idx_candidate];
                    if (idx_piece == 0) {
                        buffer[num_nodes++] = { nullptr,candidate,candidate };
                    }
                    if (idx_piece > 0) {
                        for (int idx_parent = idx_start_node; idx_parent < idx_end_node_exclusive; idx_parent++) {
                            Node* parent = &buffer[idx_parent];
                            // Current candidate is smaller (= lower significant bit) and there is no overlap
                            //  Also, check that there is at least one empty bit in between
                            if ((candidate < parent->bit_this) && ((candidate & (parent->bit_this | (parent->bit_this >> 1))) == 0)) {
                                auto bit_merged = parent->bit_merged | candidate;
                                if (idx_piece == row.pieces.size() - 1) {
                                    if ((bit_merged & row.bit_must) == row.bit_must) {
                                        row_score++;
                                    }

                                }
                                if (idx_piece < row.pieces.size() - 1) {
                                    buffer[num_nodes++] = { parent, bit_merged, candidate };
                                    if (num_nodes == mem_elements - 1) {
                                        std::cout << "Memory about to run out" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
                idx_start_node = idx_end_node_exclusive;
                idx_end_node_exclusive = num_nodes;
            }
            num_arrangements += row_score;
        }

        std::cout << std::format("AOC12-{}: {}", idx_part, num_arrangements) << std::endl;
    }

}

void aoc13() {
    auto lines = read_file("aoc13_real.txt");

    struct Line_section {
        unsigned int idx_start;
        unsigned int idx_end_excl;
        std::vector<unsigned int> col_hashes;
        std::vector<unsigned int> row_hashes;
    };

    std::vector<Line_section> line_sections = {};
    unsigned int idx_start = 0;
    unsigned int idx_end_excl = 0;
    for (unsigned int idx_line = 0; idx_line < lines.size(); idx_line++) {
        if (lines[idx_line].empty()) {
            idx_end_excl = idx_line;
            line_sections.push_back({ idx_start,idx_end_excl });
            idx_start = idx_end_excl + 1;
        }
    }
    line_sections.push_back({ idx_start, (unsigned int)lines.size() });

    for (auto& ls : line_sections) {
        int num_rows = ls.idx_end_excl - ls.idx_start;
        int num_cols = lines[ls.idx_start].size();
        ls.row_hashes.resize(num_rows);
        ls.col_hashes.resize(num_cols);
        for (unsigned int idx_line = ls.idx_start; idx_line < ls.idx_end_excl; idx_line++) {
            for (unsigned int idx_char = 0; idx_char < num_cols; idx_char++) {
                if (lines[idx_line][idx_char] == '#') {
                    ls.row_hashes[idx_line - ls.idx_start] |= (1u << idx_char);
                    ls.col_hashes[idx_char] |= (1u << (idx_line - ls.idx_start));
                }
            }
        }
    }

    std::vector<unsigned int> excl_pos_col(line_sections.size(), 10000);
    std::vector<unsigned int> excl_pos_row(line_sections.size(), 10000);

    for (int idx_part = 1; idx_part < 3; idx_part++) {
        unsigned int tot_score = 0;
        for (int idx_section = 0; idx_section < line_sections.size(); idx_section++) {
            auto& ls = line_sections[idx_section];
            int num_rows = ls.idx_end_excl - ls.idx_start;
            int num_cols = lines[ls.idx_start].size();
            int max_val_col = 0;
            int max_pos_col = 0;
            int max_val_row = 0;
            int max_pos_row = 0;

            for (int idx_start = 0; idx_start < num_cols - 1; idx_start++) {
                int idx_end_excl = std::min(idx_start + 1, num_cols - idx_start - 1);
                int cnt = 0;
                bool used_modified = false;
                for (int i = 0; i < idx_end_excl; i++) {
                    auto left = ls.col_hashes[idx_start - i];
                    auto right = ls.col_hashes[idx_start + i + 1];
                    if (left == right) {
                        cnt++;
                    }
                    else if (idx_part == 2 && !used_modified && std::popcount(left ^ right) == 1) {
                        used_modified = true;
                        cnt++;
                    }
                    else {
                        break;
                    }
                }
                if (cnt > max_val_col && cnt == idx_end_excl && excl_pos_col[idx_section] != idx_start) {
                    max_val_col = cnt;
                    max_pos_col = idx_start;
                }
            }
            for (int idx_start = 0; idx_start < num_rows - 1; idx_start++) {
                int idx_end_excl = std::min(idx_start + 1, num_rows - idx_start - 1);
                int cnt = 0;
                bool used_modified = false;
                for (int i = 0; i < idx_end_excl; i++) {
                    auto above = ls.row_hashes[idx_start - i];
                    auto below = ls.row_hashes[idx_start + i + 1];
                    if (above == below) {
                        cnt++;
                    }
                    else if (idx_part == 2 && !used_modified && std::popcount(above ^ below) == 1) {
                        used_modified = true;
                        cnt++;
                    }
                    else {
                        break;
                    }
                }
                if (cnt > max_val_row && cnt == idx_end_excl && excl_pos_row[idx_section] != idx_start) {
                    max_val_row = cnt;
                    max_pos_row = idx_start;
                }
            }

            if (max_val_col > 0 || max_val_row > 0) {
                if (max_val_col == max_val_row) {
                    std::cout << "Same" << std::endl;
                }
                bool do_split_vert = max_val_col > max_val_row;
                if (do_split_vert) {
                    unsigned int ans = max_pos_col + 1;
                    excl_pos_col[idx_section] = max_pos_col;
                    tot_score += ans;
                }
                else {
                    unsigned int ans = max_pos_row + 1;
                    excl_pos_row[idx_section] = max_pos_row;
                    tot_score += 100 * ans;
                }
            }
        }

        std::cout << std::format("AOC13-{}: {}", idx_part, tot_score) << std::endl;
    }
}

void aoc14() {
    auto lines = read_file("aoc14_real.txt");

    unsigned int num_rows = lines.size();
    unsigned int num_cols = lines[0].size();
    std::vector<unsigned int> cnt_per_row(num_rows, 0);
    std::vector<unsigned int> best_pos(lines[0].size(), 0);
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            auto tile = lines[row][col];
            switch (tile) {
            case 'O': cnt_per_row[best_pos[col]]++; best_pos[col]++; break;
            case '#': best_pos[col] = row + 1; break;
            }
        }
    }

    unsigned int ans = 0;
    for (unsigned int i = 0; i < num_rows; i++) {
        auto row_factor = num_rows - i;
        ans += cnt_per_row[i] * row_factor;
    }

    std::cout << std::format("AOC14-{}: {}", 1, ans) << std::endl;

    enum class Move_dir { north, east, south, west };
    auto move_rocks = [&lines, &num_rows, &num_cols](Move_dir dir) {
        auto num_best_pos = dir == Move_dir::north || dir == Move_dir::south ? num_cols : num_rows;
        auto best_pos_val = 0;
        best_pos_val = dir == Move_dir::south ? num_rows - 1 : best_pos_val;
        best_pos_val = dir == Move_dir::east ? num_cols - 1 : best_pos_val;

        std::vector<unsigned int> best_pos(num_best_pos, best_pos_val);
        for (int row = 0; row < num_rows; row++) {
            for (int col = 0; col < num_cols; col++) {
                char noop = ' '; // Can be any char, really.
                char tile = 0;
                int idx_col = 0;
                int idx_row = 0;
                switch (dir) {
                case Move_dir::north:
                    tile = lines[row][col];
                    switch (tile) {
                    case 'O': lines[best_pos[col]][col] = 'O'; best_pos[col] == row ? noop : lines[row][col] = '.'; best_pos[col]++; break;
                    case '#': best_pos[col] = row + 1; break;
                    }
                    break;
                case Move_dir::west:
                    tile = lines[num_rows - row - 1][col];
                    switch (tile) {
                    case 'O': lines[num_rows - row - 1][best_pos[row]] = 'O'; best_pos[row] == col ? noop : lines[num_rows - row - 1][col] = '.'; best_pos[row]++; break;
                    case '#': best_pos[row] = col + 1; break;
                    }
                    break;
                case Move_dir::south:
                    idx_col = num_cols - col - 1;
                    idx_row = num_rows - row - 1;
                    tile = lines[idx_row][idx_col];
                    switch (tile) {
                    case 'O': lines[best_pos[idx_col]][idx_col] = 'O'; best_pos[idx_col] == idx_row ? noop : lines[idx_row][idx_col] = '.'; best_pos[idx_col]--; break;
                    case '#': best_pos[idx_col] = idx_row - 1; break;
                    }
                    break;
                case Move_dir::east:
                    idx_col = num_cols - col - 1;
                    tile = lines[row][idx_col];
                    switch (tile) {
                    case 'O': lines[row][best_pos[row]] = 'O'; best_pos[row] == idx_col ? noop : lines[row][idx_col] = '.'; best_pos[row]--; break;
                    case '#': best_pos[row] = idx_col - 1; break;
                    }
                    break;
                }
            }
        }
        };

    auto calc_score = [&lines]() {
        unsigned int num_rows = lines.size();
        unsigned int score = 0;
        for (unsigned int i = num_rows; i > 0; i--) {
            unsigned int cnt = 0;
            for (auto& c : lines[num_rows - i]) {
                if (c == 'O') {
                    cnt++;
                }
            }
            score += i * cnt;
        }
        return score;
        };

    size_t max_num_hashes = 100'000;
    std::vector<size_t> hashes(max_num_hashes);
    int offset = 0;
    int interval = 0;
    bool done = false;
    for (int i = 0; i < max_num_hashes; i++) {
        move_rocks(Move_dir::north);
        move_rocks(Move_dir::west);
        move_rocks(Move_dir::south);
        move_rocks(Move_dir::east);
        std::string s = {};
        for (auto& line : lines) {
            s += line;
        }
        auto state_hash = std::hash<std::string>{}(s);
        hashes[i] = state_hash;
        for (int j = 0; j < i; j++) {
            if (state_hash == hashes[j]) {
                offset = j;
                interval = i - j;
                done = true;
                break;
            }
        }
        if (done) {
            break;
        }
    }

    std::vector<unsigned int> scores(interval);
    for (int i = 0; i < interval; i++) {
        scores[i] = calc_score();
        move_rocks(Move_dir::north);
        move_rocks(Move_dir::west);
        move_rocks(Move_dir::south);
        move_rocks(Move_dir::east);
    }

    unsigned long num_cycles = 1'000'000'000;

    auto s = scores[(num_cycles - offset - 1) % interval];

    std::cout << std::format("AOC14-{}: {}", 2, s) << std::endl;
}

void aoc15() {
    auto lines = read_file("aoc15_real.txt");

    unsigned int cnt = 0;
    unsigned int tot_score = 0;
    unsigned int cur_score = 0;
    lines[0] += ",";

    for (int i = 0; i < lines[0].size(); i++) {
        char c = lines[0][i];
        if (c == ',') {
            tot_score += cur_score;
            cnt++;
            cur_score = 0;
        }
        else {
            cur_score += (unsigned int)c;
            cur_score *= 17;
            cur_score = cur_score % 256;
        }
    }

    std::cout << std::format("AOC15-{}: {}", 1, tot_score) << std::endl;

    struct Node {
        std::string label;
        unsigned int focal_length;
        Node* prev;
        Node* next;
    };

    std::vector<Node> root_nodes(256);
    for (int i = 0; i < 256; i++) {
        std::string label = "Box" + std::to_string(i);
        root_nodes[i] = { label,0,nullptr,nullptr };
    }

    std::vector<Node> other_nodes(10000);

    cur_score = 0;
    std::string label = {};
    int next_node = 0;
    for (int i = 0; i < lines[0].size(); i++) {
        char c = lines[0][i];
        if (c == ',') {
            cur_score = 0;
            label = {};
        }
        else if (c == '-') {
            auto node = &root_nodes[cur_score];
            while (node->label != label && node->next != nullptr) {
                node = node->next;
            }
            if (node->label == label) {
                if (node->next != nullptr) {
                    node->next->prev = node->prev;
                    node->prev->next = node->next;
                }
                else {
                    if (node->prev != nullptr) {
                        node->prev->next = nullptr;
                    }
                }
            }
        }
        else if (c == '=') {
            auto focal_length = ((unsigned int)lines[0][i + 1]) - '0';
            auto node = &root_nodes[cur_score];
            while (node->label != label && node->next != nullptr) {
                node = node->next;
            }
            if (node->label == label) {
                node->focal_length = focal_length;
            }
            else {
                other_nodes[next_node++] = { label,focal_length,node,nullptr };
                node->next = &other_nodes[next_node - 1];
            }
        }
        else {
            label += c;
            cur_score += (unsigned int)c;
            cur_score *= 17;
            cur_score = cur_score % 256;
        }
    }

    tot_score = 0;
    for (unsigned int idx_box = 0; idx_box < 256; idx_box++) {
        Node* node = &root_nodes[idx_box];
        unsigned int idx_slot = 1;
        unsigned int added_score = 0;
        while (node->next != nullptr) {
            node = node->next;
            added_score = (idx_box + 1) * (idx_slot++) * node->focal_length;
            tot_score += added_score;
        }
    }

    std::cout << std::format("AOC15-{}: {}", 2, tot_score) << std::endl;
}

void aoc16() {
    auto lines = read_file("aoc16_real.txt");

    struct Tile {
        char type;
        bool hit_from_left;
        bool hit_from_right;
        bool hit_from_above;
        bool hit_from_below;
    };

    int num_rows = lines.size();
    int num_cols = lines[0].size();
    std::vector<std::vector<Tile>> tiles(num_rows, std::vector<Tile>(num_cols));

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            char c = lines[row][col];
            tiles[row][col] = { c,false,false,false,false };
        }
    }

    struct Beam {
        int row;
        int col;
        int row_d;
        int col_d;
        bool alive;
    };

    auto simulate = [&num_rows, &num_cols](std::vector<std::vector<Tile>>& tiles, Beam start_beam) {
        auto r = tiles.size();
        auto c = tiles[0].size();
        // Cleaning up is faster than copying tiles each iteration
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                tiles[i][j].hit_from_above = false;
                tiles[i][j].hit_from_below = false;
                tiles[i][j].hit_from_left = false;
                tiles[i][j].hit_from_right = false;
            }
        }
        std::vector<std::vector<bool>> visited(num_rows, std::vector<bool>(num_cols, false));
        int max_beams = 800;
        std::vector<Beam> beams(max_beams);
        int num_beams = 0;
        beams[num_beams++] = start_beam;
        int num_alive = 1;

        while (num_alive > 0) {
            for (int idx_beam = 0; idx_beam < num_beams; idx_beam++) {
                auto& beam = beams[idx_beam];
                if (beam.alive) {
                    if (beam.row < 0 || beam.row >= num_rows || beam.col < 0 || beam.col >= num_cols) {
                        beam.alive = false;
                        num_alive--;
                    }
                    if (beam.alive) {
                        visited[beam.row][beam.col] = true;
                        int row = beam.row;
                        int col = beam.col;
                        bool moving_left = beam.col_d < 0;
                        bool moving_right = beam.col_d > 0;
                        bool moving_up = beam.row_d < 0;
                        bool moving_down = beam.row_d > 0;
                        Tile& tile = tiles[row][col];
                        bool cycling = false;
                        if (
                            (tile.hit_from_above && moving_down) ||
                            (tile.hit_from_below && moving_up) ||
                            (tile.hit_from_right && moving_left) ||
                            (tile.hit_from_left && moving_right))
                        {
                            cycling = true;
                            beam.alive = false;
                            num_alive--;
                        }
                        if (!cycling) {
                            auto cur_row_d = beam.row_d;
                            auto cur_col_d = beam.col_d;
                            tile.hit_from_above = moving_down;
                            tile.hit_from_below = moving_up;
                            tile.hit_from_left = moving_right;
                            tile.hit_from_right = moving_left;
                            switch (tile.type) {
                            case '/':  beam.col_d = -cur_row_d; beam.row_d = -cur_col_d; break;
                            case '\\': beam.col_d = cur_row_d; beam.row_d = cur_col_d; break;
                            case '|':
                                if (moving_left || moving_right) {
                                    beams[num_beams++] = { row,col,1,0, true };
                                    beams[num_beams++] = { row,col,-1,0, true };
                                    num_alive += 1;
                                    beam.alive = false;
                                }
                                break;
                            case '-':
                                if (moving_up || moving_down) {
                                    beams[num_beams++] = { row,col,0,1, true };
                                    beams[num_beams++] = { row,col,0,-1, true };
                                    num_alive += 1;
                                    beam.alive = false;
                                }
                                break;
                            }
                        }
                        beam.row += beam.row_d;
                        beam.col += beam.col_d;
                    }
                }
            }
        }

        int num_visited = 0;
        for (auto& r : visited) {
            for (auto b : r) {
                if (b) {
                    num_visited++;
                }
            }
        }

        return num_visited;
    };
    
    auto best_simulation = [&simulate](std::vector<std::vector<Tile>>* tiles, std::vector<Beam> start_beams) {
        int max_num_visited = 0;
        for (auto& b : start_beams) {
            auto s = simulate(*tiles, b);
            max_num_visited = s > max_num_visited ? s : max_num_visited;
        }
        return max_num_visited;
        };


    Beam beam = { 0,0,0,1, true };
    auto num_visited = best_simulation(&tiles, { beam });
    std::cout << std::format("AOC16-{}: {}", 1, num_visited) << std::endl;

    std::vector<Beam> beams(2 * num_rows + 2 * num_cols);
    int idx_beam = 0;
    for (int i = 0; i < num_rows; i++) {
        beams[idx_beam++] = { i,0,0,1,true };
        beams[idx_beam++] = { i,num_cols - 1,0,-1,true };
    }
    for (int i = 0; i < num_cols; i++) {
        beams[idx_beam++] = { 0,i,1,0,true };
        beams[idx_beam++] = { num_rows - 1,i,-1,0,true };
    }

    int max_num_visited = best_simulation(&tiles, beams);

    std::cout << std::format("AOC16-{}: {}", 2, max_num_visited) << std::endl;
}

void aoc17() {
    auto lines = read_file("aoc17_test.txt");

    struct Tile {
        unsigned int val;
    };

    int num_rows = lines.size();
    int num_cols = lines[0].size();
    std::vector<std::vector<Tile>> tiles(num_rows, std::vector<Tile>(num_cols));

    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_cols; col++) {
            unsigned int val = lines[row][col] - '0';
            tiles[row][col] = { val };
        }
    }

    struct Node {
        unsigned int row;
        unsigned int col;
        unsigned int last_moves;
        unsigned int val;
        unsigned int steps;
        Node* prev;
    };

    struct Node_diff {
        int row_d;
        int col_d;
        unsigned int bit;
    };

    int max_nodes = 100'000'000;
    std::vector<Node> nodes(max_nodes);
    std::cout << "Done allocating memory" << std::endl;

    for (int idx_part = 1; idx_part < 3; idx_part++) {
        unsigned int min_moves = idx_part == 1 ? 1 : 4;
        unsigned int max_moves = idx_part == 1 ? 3 : 10;
        unsigned int max_move_mask = 0;
        unsigned int min_move_mask = 0;

        for (unsigned int i = 0; i < max_moves; i++) {
            max_move_mask |= 1 << i;
        }

        for (unsigned int i = 0; i < min_moves; i++) {
            min_move_mask |= 1 << i;
        }

        // Depends on where you are coming from when you visit this tile
        std::vector<std::vector<std::vector<unsigned int>>> visited(num_rows, std::vector<std::vector<unsigned int>>(num_cols, std::vector<unsigned int>(1 << max_moves, 10000000)));

        int num_nodes = 0;
        bool done = false;
        nodes[num_nodes++] = { 0, 0, 0, 0, 0, nullptr };

        int idx_start = 0;
        int idx_end_exclusive = num_nodes;
        unsigned int best_score = 10000000;
        while (!done) {
            auto num_nodes_before = num_nodes;
            for (int idx_node = idx_start; idx_node < idx_end_exclusive; idx_node++) {
                auto& node = nodes[idx_node];
                bool straight_line = ((node.last_moves & min_move_mask) == min_move_mask)
                    || ((node.last_moves & min_move_mask) == 0);
                if (straight_line && (node.row == num_rows - 1) && (node.col == num_cols - 1)) {
                    if (node.val < best_score) {
                        //std::cout << "Reached the goal. Score: " << node.val << std::endl;
                        //std::vector<std::vector<char>> x(num_rows, std::vector<char>(num_cols, '.'));
                        //Node* n = &node;
                        //while (n != nullptr) {
                        //    x[n->row][n->col] = '#';
                        //    n = n->prev;
                        //}
                        //for (int row = 0; row < num_rows; row++) {
                        //    for (int col = 0; col < num_cols; col++) {
                        //        std::cout << x[row][col];
                        //    }
                        //    std::cout << std::endl;
                        //}
                        best_score = node.val;
                    }
                }
                bool can_move_vert = (node.steps < 1)
                    || (((node.last_moves & 0b1) == 0b1) && ((node.steps < max_moves) || ((node.last_moves & max_move_mask) != max_move_mask)))
                    || (((node.steps >= min_moves) && (node.last_moves & min_move_mask) == 0));
                bool can_move_hor = (node.steps < 1)
                    || (((node.last_moves & 0b1) == 0b0) && ((node.steps < max_moves) || ((node.last_moves & max_move_mask) != 0)))
                    || (((node.steps >= min_moves) && (node.last_moves & min_move_mask) == min_move_mask));
                std::vector<Node_diff> node_diffs = {};
                if (can_move_vert) {
                    bool can_move_down = true;
                    bool can_move_up = true;
                    if (can_move_up && node.row > 0) {
                        node_diffs.push_back({ -1,0,1 });
                    }
                    if (can_move_down && node.row + 1 < num_rows) {
                        node_diffs.push_back({ 1,0,1 });
                    }
                }
                if (can_move_hor) {
                    bool can_move_left = true;
                    bool can_move_right = true;
                    if (can_move_left && node.col > 0) {
                        node_diffs.push_back({ 0,-1,0 });
                    }
                    if (can_move_right && node.col + 1 < num_cols) {
                        node_diffs.push_back({ 0,1,0 });
                    }
                }
                for (auto& diff : node_diffs) {
                    auto row = node.row + diff.row_d;
                    auto col = node.col + diff.col_d;
                    //Node* prev = node.prev;
                    //bool is_cycling = false;
                    //while (prev != nullptr) {
                    //    if (prev->row == row && prev->col == col) {
                    //        is_cycling = true;
                    //        break;
                    //    }
                    //    prev = prev->prev;
                    //}
                    //if (is_cycling) {
                    //    continue;
                    //}
                    auto val = node.val + tiles[row][col].val;
                    auto move_bit = ((node.last_moves << 1u) & max_move_mask) | diff.bit;
                    bool moving_back = (node.prev != nullptr) && (row == node.prev->row && col == node.prev->col);
                    if (val < best_score && !moving_back && visited[row][col][move_bit] > val) {
                        nodes[num_nodes++] = { row, col, move_bit, val, node.steps + 1, &node };
                        visited[row][col][move_bit] = val;
                    }
                }
            }
            idx_start = idx_end_exclusive;
            idx_end_exclusive = num_nodes;
            if (num_nodes == num_nodes_before) {
                done = true;
            }
        }

        std::cout << "Num. nodes used: " << num_nodes << std::endl;
        std::cout << std::format("AOC17-{}: {}", idx_part, best_score) << std::endl;
    }

    // 1181 too high for pt2
}

void aoc18() {
    auto lines = read_file("aoc18_real.txt");

    struct Plan_part {
        int vert;
        int hor;
        unsigned int cnt;
        int start_x;
        int start_y;
        int end_x;
        int end_y;
    };

    std::vector<Plan_part> plan_parts(lines.size());

    auto hex2uint = [](std::string hex) {
        unsigned int ret = 0;
        unsigned int hex_len = hex.size();
        for (unsigned int i = 0; i < hex_len; i++) {
            int n = hex[i] - '0';
            if (n > 9) {
                n = (hex[i] - 'a') + 10;
            }
            ret |= n << ((hex_len - i - 1) * 4u);
        }

        return ret;
        };

    for (int idx_part = 1; idx_part < 3; idx_part++) {
        for (int idx_line = 0; idx_line < lines.size(); idx_line++) {
            auto& line = lines[idx_line];
            auto lines_parts = split_string(line, " ");
            Plan_part part = {};
            int dir = 0;
            int cnt = 0;
            if (idx_part == 1) {
                switch (lines_parts[0][0]) {
                case 'R': dir = 0; break;
                case 'D': dir = 1; break;
                case 'L': dir = 2; break;
                case 'U': dir = 3; break;
                }
                cnt = std::atoi(lines_parts[1].c_str());
            }
            else if (idx_part == 2) {
                auto trimmed = lines_parts[2].substr(2, 6);
                dir = hex2uint(trimmed.substr(5));
                cnt = hex2uint(trimmed.substr(0, 5));
            }
            switch (dir) {
            case 0: part.vert = 0; part.hor = 1; break;
            case 1: part.vert = 1; part.hor = 0; break;
            case 2: part.vert = 0; part.hor = -1; break;
            case 3: part.vert = -1; part.hor = 0; break;
            }
            part.cnt = cnt;
            plan_parts[idx_line] = part;
        }

        int min_x = std::numeric_limits<int>::max();
        int max_x = std::numeric_limits<int>::min();
        int min_y = std::numeric_limits<int>::max();
        int max_y = std::numeric_limits<int>::min();
        int cur_x = 0;
        int cur_y = 0;

        for (auto& part : plan_parts) {
            part.start_x = cur_x;
            part.start_y = cur_y;
            cur_x += part.hor * part.cnt;
            cur_y += part.vert * part.cnt;
            part.end_x = cur_x;
            part.end_y = cur_y;
            min_x = cur_x < min_x ? cur_x : min_x;
            min_y = cur_y < min_y ? cur_y : min_y;
            max_x = cur_x > max_x ? cur_x : max_x;
            max_y = cur_y > max_y ? cur_y : max_y;
        }

        for (auto& part : plan_parts) {
            part.start_x -= min_x;
            part.start_y -= min_y;
            part.end_x -= min_x;
            part.end_y -= min_y;
        }

        max_x -= min_x;
        max_y -= min_y;
        min_x = 0;
        min_y = 0;

        unsigned int w = max_x + 1;
        unsigned int h = max_y + 1;
        std::cout << std::format("Part {}\n  Num. edges: {}\n  X/Y max: {} / {}", idx_part, plan_parts.size(), max_x, max_y) << std::endl;

        // TODO: Ta bort alla horisontella linjer. I de fall som de horisontella linjerna är som "lock" på en u-form: kapa bort den 
        //  överlappande punkten från de två intilliggande vertikala linjerna. Då kommer alla vertikala linjer bidra till en "crossing".
        //  Vi kan t ex ha en buffer som vet när det finns crossings för varje vertikal linje (rad). Eller så beräknar vi intersection
        //  med alla edges för varje rad. Fast det är långsammare.
    }


    //std::vector<std::vector<char>> output(h, std::vector<char>(w, '.'));
    //std::vector<std::vector<bool>> is_crossing(h, std::vector<bool>(w, false));

    //for (int idx_part = 0; idx_part < plan_parts.size(); idx_part++) {
    //    auto& p = plan_parts[idx_part];
    //    for (int i = 0; i < p.cnt; i++) {
    //        output[p.start_y + p.vert * i][p.start_x + p.hor * i] = '#';
    //    }
    //    auto idx_prev = (idx_part + plan_parts.size() - 1) % plan_parts.size();
    //    auto idx_next = (idx_part + plan_parts.size() + 1) % plan_parts.size();
    //    auto& p_prev = plan_parts[idx_prev];
    //    auto& p_next = plan_parts[idx_next];
    //    if (p.vert == 0 && (p_prev.vert != 0 && p_next.vert != 0) && (p_prev.vert == p_next.vert)) {
    //        auto p_max_x = p.start_x > p.end_x ? p.start_x : p.end_x;
    //        is_crossing[p.start_y][p_max_x] = true;
    //    }
    //    if (p.vert != 0) {
    //        for (int i = 1; i < p.cnt; i++) {
    //            is_crossing[p.start_y + p.vert * i][p.start_x] = true;
    //        }
    //    }
    //}

    //for (int row = 0; row < h; row++) {
    //    int crossings = 0;
    //    for (int col = 0; col < w; col++) {
    //        if (is_crossing[row][col]) {
    //            crossings++;
    //        }
    //        else if (crossings % 2 == 1) {
    //            output[row][col] = '#';
    //        }
    //    }
    //}

    //int added = 0;

    //for (int row = 0; row < output.size(); row++) {
    //    for (auto c : output[row]) {
    //        if (c == '#') {
    //            added++;
    //        }
    //    }
    //}

    //std::cout << std::format("AOC18-{}: {}", 1, added) << std::endl;
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
        bool operator==(const Cube_extension& a) const {
            return a.id == id;
        }
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

    auto occupied_z_original = occupied_z;
    auto cube_extensions_original = cube_extensions;
    int tot_moved = 0;
    for (int idx_remove = 0; idx_remove < cube_extensions.size();idx_remove++) {
        std::set<int> ids_moved = {};
        occupied_z = occupied_z_original;
        for (int i = 0; i < cube_extensions.size(); i++) {
            cube_extensions[i].cubes = cube_extensions_original[i].cubes;
        }
        auto& to_remove = cube_extensions[idx_remove];
        done = false;
        steps_left = 100000;
        while (!done && steps_left > 0) {
            bool something_moved = false;
            for (auto& el : cube_extensions) {
                bool can_move = true;
                for (auto& cube : el.cubes) {
                    if (occupied_z[cube.x][cube.y][cube.z - 1] != nullptr && occupied_z[cube.x][cube.y][cube.z - 1] != &el && occupied_z[cube.x][cube.y][cube.z - 1] != &to_remove) {
                        can_move = false;
                    }
                }
                if (can_move) {
                    ids_moved.insert(el.id);
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
        tot_moved += ids_moved.size();
    }
    std::cout << std::endl;

    std::cout << "AOC22-1: " << cnt_possible_remove << std::endl;
    std::cout << "AOC22-2: " << tot_moved << std::endl;
}

void aoc23() {
    auto lines = read_file("aoc23_real.txt");
    int num_cols = lines[0].size();
    int num_rows = lines.size();
    std::vector<std::vector<char>> tiles(num_rows, std::vector<char>(num_cols, 0));

    for (int idx_line = 0; idx_line < lines.size(); idx_line++) {
        for (int idx_char = 0; idx_char < lines[idx_line].size(); idx_char++) {
            tiles[idx_line][idx_char] = lines[idx_line][idx_char];
        }
    }

    struct Graph_edge;

    struct Graph_node {
        int id;
        int row;
        int col;
        std::vector<Graph_edge*> edges;
    };

    std::vector<Graph_node> graph_nodes = {};
    int idx_graph_node = 0;
    // Manually add start node
    graph_nodes.push_back({ idx_graph_node++, 0,1 });
    // Look for decision points
    for (int idx_row = 1; idx_row < num_rows - 1;  idx_row++) {
        for (int idx_col = 1; idx_col < num_cols - 1; idx_col++) {
            auto c = tiles[idx_row][idx_col];
            char neighborhood[4] = {
                tiles[idx_row][idx_col + 1],
                tiles[idx_row][idx_col - 1],
                tiles[idx_row + 1][idx_col],
                tiles[idx_row - 1][idx_col]
            };
            if (c == '.') {
                int num_free = 0;
                for (int i = 0; i < 4; i++) {
                    auto cn = neighborhood[i];
                    if (cn == '.' || cn == '>' || cn == '<' || cn == '^' || cn == 'v') {
                        num_free++;
                    }
                }
                if (num_free > 2) {
                    graph_nodes.push_back({ idx_graph_node++,idx_row,idx_col });
                }
            }
        }
    }
    // Manually add end node
    graph_nodes.push_back({ idx_graph_node++, num_rows - 1,num_cols - 2 });

    struct Graph_edge {
        Graph_node* src;
        Graph_node* dst;
        int dist;
    };

    struct Coord2i {
        int row;
        int col;
    };

    std::vector<Graph_edge> graph_edges(1000);
    int num_graph_edges = 0;

    for (int idx_node = 0; idx_node < graph_nodes.size(); idx_node++) {
        Graph_node* src = &graph_nodes[idx_node];
        bool done_with_node = false;
        std::vector<Coord2i> heads = { };
        std::vector<Coord2i> cur_last_positions = {};
        std::vector<int> edge_lengths = {};
        if (src->row - 1 >= 0 && tiles[src->row - 1][src->col] != 'v' && tiles[src->row - 1][src->col] != '#') {
            heads.push_back({ src->row - 1,src->col });
            cur_last_positions.push_back({ src->row,src->col });
            edge_lengths.push_back(1);
        }
        if (src->row + 1 < num_rows && tiles[src->row + 1][src->col] != '^' && tiles[src->row + 1][src->col] != '#') {
            heads.push_back({ src->row + 1,src->col });
            cur_last_positions.push_back({ src->row,src->col });
            edge_lengths.push_back(1);
        }
        if (src->col - 1 >= 0 && tiles[src->row][src->col - 1] != '>' && tiles[src->row][src->col - 1] != '#') {
            heads.push_back({ src->row, src->col - 1 });
            cur_last_positions.push_back({ src->row,src->col });
            edge_lengths.push_back(1);
        }
        if (src->col + 1 >= 0 && tiles[src->row][src->col + 1] != '<' && tiles[src->row][src->col + 1] != '#') {
            heads.push_back({ src->row, src->col + 1 });
            cur_last_positions.push_back({ src->row,src->col });
            edge_lengths.push_back(1);
        }
        for (int idx_head = 0; idx_head < heads.size(); idx_head++) {
            bool head_done = false;
            auto cur_head = &heads[idx_head];
            auto cur_last_pos = &cur_last_positions[idx_head];
            auto cur_length = &edge_lengths[idx_head];
            while (!head_done) {
                Coord2i neighborhood_coord[4] = {
                    {cur_head->row,      cur_head->col + 1},
                    {cur_head->row,      cur_head->col - 1},
                    {cur_head->row + 1,  cur_head->col},
                    {cur_head->row - 1,  cur_head->col}
                };
                Coord2i new_head = {};
                for (int idx_n = 0; idx_n < 4; idx_n++) {
                    auto row_check = neighborhood_coord[idx_n].row;
                    auto col_check = neighborhood_coord[idx_n].col;
                    if (row_check < 0 || row_check >= num_rows || col_check < 0 || col_check >= num_cols) {
                        continue;
                    }
                    if (tiles[row_check][col_check] != '#' && ((row_check != cur_last_pos->row) || (col_check != cur_last_pos->col))) {
                        // We assume there is no opposite-pointing slope on the path between two adjacent nodes (= crossroads)
                        new_head = { row_check,col_check };
                        break;
                    }
                }
                cur_last_pos->row = cur_head->row;
                cur_last_pos->col = cur_head->col;
                cur_head->row = new_head.row;
                cur_head->col = new_head.col;
                (*cur_length)++;
                for (int idx_graph_node = 0; idx_graph_node < graph_nodes.size(); idx_graph_node++) {
                    if (idx_graph_node == src->id) {
                        continue;
                    }
                    if (new_head.row == graph_nodes[idx_graph_node].row &&
                        new_head.col == graph_nodes[idx_graph_node].col) {
                        graph_edges[num_graph_edges++] = { src,&graph_nodes[idx_graph_node] ,*cur_length };
                        head_done = true;
                    }
                }
            }
        }
    }

    for (int idx_edge = 0; idx_edge < num_graph_edges; idx_edge++) {
        graph_edges[idx_edge].src->edges.push_back(&graph_edges[idx_edge]);
    }

    struct Search_node {
        Graph_node* node;
        Search_node* parent;
        int tot_walk_length;
    };

    for (int idx_challenge_parts = 0; idx_challenge_parts < 2; idx_challenge_parts++) {
        if (idx_challenge_parts == 1) {
            // Make graph bidirectional
            for (auto& n : graph_nodes) {
                for (auto& src_edge : n.edges) {
                    bool has_edge = false;
                    // The edge to match should mirror the source/destination part
                    Graph_edge edge_to_match = { src_edge->dst,src_edge->src,src_edge->dist };
                    for (auto& dst_edge : src_edge->dst->edges) {
                        if (dst_edge->src == edge_to_match.src && dst_edge->dst == edge_to_match.dst) {
                            has_edge = true;
                        }
                    }
                    if (has_edge == false) {
                        graph_edges[num_graph_edges++] = edge_to_match;
                        src_edge->dst->edges.push_back(&graph_edges[num_graph_edges - 1]);
                    }
                }
            }
        }
        std::vector<Search_node> search_nodes(100000000);
        search_nodes[0] = { &graph_nodes[0], nullptr, 0 };
        int num_nodes = 1;
        int max_walk_length = 0;

        for (int idx_node = 0; idx_node < num_nodes; idx_node++) {
            if (search_nodes[idx_node].node == &graph_nodes.back()) {
                if (search_nodes[idx_node].tot_walk_length > max_walk_length) {
                    max_walk_length = search_nodes[idx_node].tot_walk_length;
                }
                continue;
            }
            for (int idx_edge = 0; idx_edge < search_nodes[idx_node].node->edges.size(); idx_edge++) {
                bool do_use = true;
                Graph_node* current_node = search_nodes[idx_node].node->edges[idx_edge]->dst;
                auto search_parent = &search_nodes[idx_node];
                while (search_parent != nullptr) {
                    if (search_parent->node == current_node) {
                        do_use = false;
                    }
                    search_parent = search_parent->parent;
                }
                if (do_use) {
                    Search_node* parent = &search_nodes[idx_node];
                    int new_tot_walk_length = search_nodes[idx_node].tot_walk_length + search_nodes[idx_node].node->edges[idx_edge]->dist;
                    search_nodes[num_nodes++] = { current_node, parent,  new_tot_walk_length };
                }
            }
        }
        std::cout << "AOC23-" << idx_challenge_parts + 1 << ": " << max_walk_length << std::endl;
    }
}

void aoc24() {
    bool use_test = false;
    std::string fn = use_test ? "aoc24_test.txt" : "aoc24_real.txt";
    auto lines = read_file(fn);

    struct Particle3d {
        int id;
        double x;
        double y;
        double z;
        double x_d;
        double y_d;
        double z_d;
        void print() {
            std::cout << std::format("x: {} y: {} z: {} xd: {} yd: {} zd: {}", x, y, z, x_d, y_d, z_d) << std::endl;
        }
    };
    std::vector<Particle3d> hails = {};
    int num_hails = 0;

    for (auto& line : lines) {
        auto the_split = split_string(line, " @ ");
        auto parts_pos = split_string(the_split[0], ", ");
        auto parts_dir = split_string(the_split[1], ", ");
        hails.push_back({
            num_hails++,
            std::atof(parts_pos[0].c_str()),
            std::atof(parts_pos[1].c_str()),
            std::atof(parts_pos[2].c_str()),
            std::atof(parts_dir[0].c_str()),
            std::atof(parts_dir[1].c_str()),
            std::atof(parts_dir[2].c_str())
            });
    }

    int intersections_within = 0;
    double boundary_min = use_test ? 7 : 200'000'000'000'000;
    double boundary_max = use_test ? 27 : 400'000'000'000'000;
    for (int idx_hail_src = 0; idx_hail_src < hails.size(); idx_hail_src++) {
        for (int idx_hail_dst = idx_hail_src + 1; idx_hail_dst < hails.size(); idx_hail_dst++) {
            double x1 = hails[idx_hail_src].x;
            double y1 = hails[idx_hail_src].y;
            double x3 = hails[idx_hail_dst].x;
            double y3 = hails[idx_hail_dst].y;

            // Hails might start outside the test area but collisions must occur inside
            double x[2] = { hails[idx_hail_src].x, hails[idx_hail_dst].x };
            double y[2] = { hails[idx_hail_src].y, hails[idx_hail_dst].y };
            double x_d[2] = { hails[idx_hail_src].x_d, hails[idx_hail_dst].x_d };
            double y_d[2] = { hails[idx_hail_src].y_d, hails[idx_hail_dst].y_d };
            double num_steps[2] = {};
            for (int i = 0; i < 2; i++) {
                if (x_d[i] < 0 && x[i] > boundary_min) {
                    num_steps[i] = (boundary_min - x[i]) / x_d[i];
                }
                if (x_d[i] > 0 && x[i] < boundary_max) {
                    auto cur_steps = (boundary_max - x[i]) / x_d[i];
                    if (num_steps[i] == 0 || cur_steps < num_steps[i]) {
                        num_steps[i] = cur_steps;
                    }
                }
                if (y_d[i] < 0 && y[i] > boundary_min) {
                    auto cur_steps = (boundary_min - y[i]) / y_d[i];
                    if (num_steps[i] == 0 || cur_steps < num_steps[i]) {
                        num_steps[i] = cur_steps;
                    }
                }
                if (y_d[i] > 0 && y[i] < boundary_max) {
                    auto cur_steps = (boundary_max - y[i]) / y_d[i];
                    if (num_steps[i] == 0 || cur_steps < num_steps[i]) {
                        num_steps[i] = cur_steps;
                    }
                }
                if (num_steps == 0) {
                    std::cout << "ERROR num_steps=0" << std::endl;
                    continue;
                }
            }

            double x2 = x1 + num_steps[0] * x_d[0];
            double y2 = y1 + num_steps[0] * y_d[0];
            double x4 = x3 + num_steps[1] * x_d[1];
            double y4 = y3 + num_steps[1] * y_d[1];

            double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
            if (std::abs(denominator) < 0.000001) {
                continue;
            }

            auto nominator_1 = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
            double t = (nominator_1 / denominator);
            auto nominator_2 = (x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2);
            double u = nominator_2 / denominator;
            if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
                double check_x = x1 + t * (x2 - x1);
                double check_y = y1 + t * (y2 - y1);
                if (check_x >= boundary_min && check_x <= boundary_max && check_y >= boundary_min && check_y <= boundary_max) {
                    intersections_within++;
                }
            }
        }
    }

    std::cout << "AOC24-1: " << intersections_within << std::endl;

    // Part two is solved by building an equation system and solving.
    // Subtract the stone's position (x0, y0, z0) and direction (xd0, yd0, zd0) from all hails.
    // Now, the position vector and the direction vector are rooted in the origin.
    // They give a solution when these two vectors are colinear, meaning the go in the opposite direction.
    // This happens when their cross-product is the null vector.
    // After some calculations we get equations:
    //
    //  x * (-yd1 + yd2) + y * (xd1 + xd2) + xd * (y1 - y2) + yd * (-x1 + x2) = y1*xd1 - x1*yd1 + x2*yd2 - y2*xd2
    //  y * (-zd1 + zd2) + z * (yd1 + yd2) + yd * (z1 - z2) + zd * (-y1 + y2) = z1*yd1 - y1*zd1 + y2*zd2 - z2*yd2
    // 
    // Solve these to get our position and direction
    Particle3d stone = {};
    double avg_factor = 10;

    for (int j = 0; j < avg_factor; j++) {
        std::vector<std::vector<double>> to_solve_1 = {};
        for (int i = j * 4; i < (j + 1) * 4; i++) {
            auto y_factor = -hails[i].z_d + hails[i + 1].z_d;
            auto z_factor = hails[i].y_d - hails[i + 1].y_d;
            auto u_factor = hails[i].z - hails[i + 1].z;
            auto v_factor = -hails[i].y + hails[i + 1].y;
            auto scalar = hails[i].z * hails[i].y_d - hails[i].y * hails[i].z_d + hails[i + 1].y * hails[i + 1].z_d - hails[i + 1].z * hails[i + 1].y_d;
            to_solve_1.push_back({ y_factor,z_factor,u_factor,v_factor,scalar });
        }

        auto ret = linear_solver(to_solve_1);
        stone.y += ret[0];
        stone.z += ret[1];
        stone.y_d += ret[2];
        stone.z_d += ret[3];

        std::vector<std::vector<double>> to_solve_2 = {};

        for (int i = j * 4; i < (j + 1) * 4; i++) {
            auto x_factor = -hails[i].y_d + hails[i + 1].y_d;
            auto y_factor = hails[i].x_d - hails[i + 1].x_d;
            auto t_factor = hails[i].y - hails[i + 1].y;
            auto u_factor = -hails[i].x + hails[i + 1].x;
            auto scalar = hails[i].y * hails[i].x_d - hails[i].x * hails[i].y_d + hails[i + 1].x * hails[i + 1].y_d - hails[i + 1].y * hails[i + 1].x_d;
            to_solve_2.push_back({ x_factor,y_factor,t_factor,u_factor,scalar });
        }

        ret = linear_solver(to_solve_2);
        stone.x += ret[0];
        stone.x_d += ret[2];
    }

    stone.x /= avg_factor;
    stone.y /= avg_factor;
    stone.z /= avg_factor;
    stone.x_d /= avg_factor;
    stone.y_d /= avg_factor;
    stone.z_d /= avg_factor;

    auto ans = std::round(stone.x) + std::round(stone.y) + std::round(stone.z);

    std::cout << "AOC24-2: " << std::setprecision(15) << ans << std::endl;
}

void aoc25() {
    bool use_test = false;
    std::string fn = use_test ? "aoc25_test.txt" : "aoc25_real.txt";
    auto lines = read_file(fn);

    std::vector<Graph_node> graph_nodes = {};
    int cnt_nodes = 0;
    for (auto& line : lines) {
        auto split = split_string(line, ": ");
        auto neighbor_names = split_string(split[1], " ");
        std::vector<std::string> node_names = { split[0] };
        for (auto& nn : neighbor_names) {
            node_names.push_back(nn);
        }
        for (auto& nn : node_names) {
            bool found = false;
            for (auto& gn : graph_nodes) {
                if (gn.name == nn) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                graph_nodes.push_back({ cnt_nodes++,nn });
            }
        }
    }

    for (int idx_line = 0; idx_line < lines.size(); idx_line++) {
        auto split = split_string(lines[idx_line], ": ");
        auto neighbor_names = split_string(split[1], " ");
        for (auto& gn : graph_nodes) {
            if (gn.name == split[0]) {
                for (auto& nn : neighbor_names) {
                    for (int i = 0; i < graph_nodes.size(); i++) {
                        if (graph_nodes[i].name == nn) {
                            gn.neighbors.push_back(&graph_nodes[i]);
                            break;
                        }
                    }
                }
            }
        }
    }

    for (auto& n : graph_nodes) {
        for (auto& nn : n.neighbors) {
            bool is_ok = false;
            for (auto& nnn : nn->neighbors) {
                if (nnn == &n) {
                    is_ok = true;
                    break;
                }
            }
            if (!is_ok) {
                nn->neighbors.push_back(&n);
            }
        }
    }

    std::vector<int> src_indices = {};
    std::vector<int> dst_indices = {};

    int num_indices = 1000;

    for (int i = 0; i < num_indices; i++) {
        src_indices.push_back(std::rand() % graph_nodes.size());
        dst_indices.push_back(std::rand() % graph_nodes.size());
    }
    /*
    1 000 iter:
      nsk -> rsg: 237
      zcp -> zjm: 157
      jks -> rfg: 111

    10 000 iter:
      nsk -> rsg: 2299
      zcp -> zjm: 1567
      jks -> rfg: 1069
    */

    std::vector<int> cnt_passage(graph_nodes.size() * graph_nodes.size(), 0);

    for (int i = 0; i < src_indices.size(); i++) {
        int idx_src = src_indices[i];
        int idx_dst = dst_indices[i];
        auto shortest = shortest_path(&graph_nodes[idx_src], &graph_nodes[idx_dst], graph_nodes.size());
        for (int idx_node = 0; idx_node < (int)shortest.size() - 1; idx_node++) {
            int id1 = shortest[idx_node]->id;
            int id2 = shortest[idx_node + 1]->id;
            int idx1 = id1 + id2 * graph_nodes.size();
            int idx2 = id2 + id1 * graph_nodes.size();
            cnt_passage[idx1]++;
            cnt_passage[idx2]++;
        }
    }

    int num_largest = 5;
    std::vector<int> largest_val(num_largest);
    std::vector<Graph_node*> largest_src(num_largest);
    std::vector<Graph_node*> largest_dst(num_largest);
    for (int row = 0; row < graph_nodes.size(); row++) {
        for (int col = row + 1; col < graph_nodes.size(); col++) {
            int val = cnt_passage[row * graph_nodes.size() + col];
            if (val == 0) {
                continue;
            }
            int idx_smallest = 0;
            for (int i = 1; i < num_largest; i++) {
                if (largest_val[i] < largest_val[idx_smallest]) {
                    idx_smallest = i;
                }
            }
            if (largest_val[idx_smallest] < val) {
                largest_val[idx_smallest] = val;
                largest_src[idx_smallest] = &graph_nodes[col];
                largest_dst[idx_smallest] = &graph_nodes[row];
            }
        }
    }

    auto largest_val_sorted = largest_val;
    std::set<int> s(largest_val_sorted.begin(), largest_val_sorted.end());
    largest_val_sorted.assign(s.begin(), s.end());
    std::sort(largest_val_sorted.begin(), largest_val_sorted.end());

    std::vector<int> val_max;
    std::vector<Graph_node*> src_max;
    std::vector<Graph_node*> dst_max;

    for (int i = largest_val_sorted.size() - 1; i >= 0; i--) {
        int val = largest_val_sorted[i];
        for (int j = 0; j < largest_val.size(); j++) {
            if (largest_val[j] == val) {
                val_max.push_back(largest_val[j]);
                src_max.push_back(largest_src[j]);
                dst_max.push_back(largest_dst[j]);
            }
        }
        if (val_max.size() >= 3) {
            break;
        }
    }

    for (int i = 0; i < src_max.size(); i++) {
        auto to_remove = std::find(src_max[i]->neighbors.begin(), src_max[i]->neighbors.end(), dst_max[i]);
        src_max[i]->neighbors.erase(to_remove);

        to_remove = std::find(dst_max[i]->neighbors.begin(), dst_max[i]->neighbors.end(), src_max[i]);
        dst_max[i]->neighbors.erase(to_remove);
    }
    
    auto con1 = connected_nodes(graph_nodes, &graph_nodes[0], graph_nodes.size());
    std::vector<Graph_node*> con2 = {};
    for (auto& gn : graph_nodes) {
        bool found = false;
        for (auto n : con1) {
            if (gn.id == n->id) {
                found = true;
                break;
            }
        }
        if (!found) {
            con2.push_back(&gn);
        }
    }

    int ans_1 = con1.size() * con2.size();

    std::cout << "AOC25-1: " << ans_1 << std::endl;
}

int main() {
    auto t_start = std::chrono::high_resolution_clock::now();
    //aoc01();
    //aoc02();
    //aoc04();
    //aoc05();
    //aoc06();
    //aoc07();
    //aoc08();
    //aoc09();
    //aoc10();
    //aoc12();
    //aoc13();
    //aoc14();
    //aoc15();
    //aoc16();
    //aoc17();
    aoc18();
	//aoc19();
    //aoc20();
    //aoc21();
    //aoc22();
    //aoc23();
    //aoc24();
    //aoc25();
    auto t_end = std::chrono::high_resolution_clock::now();

    auto duration = duration_cast<std::chrono::milliseconds>(t_end - t_start);
    std::cout << "Duration: " << duration.count() << "ms" << std::endl;

	return 0;

}