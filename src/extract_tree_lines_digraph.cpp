// extract_tree_lines.coo
// g++ -std=c++11 -o extract_tree_lines_digraph extract_tree_lines_digraph.cpp
// ./extract_tree_lines_digraph ./digraph.gv

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <map>

typedef std::map<int, std::vector<int>> graph_type;
typedef std::map<std::pair<int, int>, std::vector<std::string>> edges_type;

void read_gv_files(const char* filename, int &number_v, edges_type &edges, graph_type &g)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        std::string arrow = " -> ";
        std::string label = " [label=\"";
        std::string newline = "\\n";
        std::string end = "\"]";
        std::string minus = "--";
        
        std::unordered_set<int> all_vertices;
        while (getline(file, line))
        {            
            std::size_t arrow_pos = line.find(arrow);
            std::size_t label_pos = line.find(label);
            if (arrow_pos == std::string::npos || label_pos == std::string::npos)
                continue;

            std::string token = line.substr(0, arrow_pos);
            int first = std::stoi(token);
            line.erase(0, arrow_pos+arrow.length());
            
            label_pos = line.find(label);
            token = line.substr(0, label_pos);
            int second = std::stoi(token);
            line.erase(0, label_pos+label.length());

            std::vector<std::string> labels;
            std::size_t newline_pos = line.find(newline);
            while (newline_pos != std::string::npos) 
            {
                token = line.substr(0, newline_pos);
                if (token.find(minus) == std::string::npos)
                    labels.push_back(token);
                line.erase(0, newline_pos+newline.length());
                newline_pos = line.find(newline);
            }

            std::size_t end_pos = line.find(end);
            token = line.substr(0, end_pos);
            if (token.find(minus) == std::string::npos)
                labels.push_back(token);
            
            std::pair<int, int> edge = std::make_pair(first, second);
            edges[edge] = labels;
            g[first].push_back(second);
            
            if (all_vertices.find(first) == all_vertices.end())
            {
                number_v++;
                all_vertices.insert(first);
            }
            if (all_vertices.find(second) == all_vertices.end())
            {
                number_v++;
                all_vertices.insert(second);
            }
        }
        
        file.close();
    }
    else
    {
        std::cout << "Error: Cannot open input file given. " << std::endl;
        return;
    }
}

void output_branch_to_file(const char* filename, int root_id, std::vector<std::vector<int>> &all_branches)
{
    std::ofstream myfile(filename);
    if(myfile.is_open())
    {
        for (auto branch : all_branches)
        {
            myfile<<root_id+1<<" ";
            for (auto v : branch)
            {
                myfile << v+1 << " ";
            }
            myfile<<std::endl;
        }
        myfile.close();
    }
    else
    {
        std::cout << "Error: Cannot open output file given. " << std::endl;
    }
}

void output_same_level_labels_to_file(const char* filename, edges_type &edges)
{
    std::string same_level_output_filename = std::string(filename) + ".same_level.gv";
    std::ofstream myfile(same_level_output_filename);
    if(myfile.is_open())
    {
        for (auto edge : edges)
        {
            if (edge.second.size() == 0) 
                continue;

            for (auto mutation : edge.second)
            {
                myfile << mutation << ", ";
            }
            myfile<<std::endl;
        }
        myfile.close();
    }
    else
    {
        std::cout << "Error: Cannot open output file given. " << std::endl;
    }
}

void output_next_edge_labels(int first_vid, std::ofstream &myfile, edges_type &edges, graph_type &g)
{
    for (auto next_vid : g[first_vid]) 
    {
        std::pair<int, int> edge = std::make_pair(first_vid, next_vid);
        std::vector<std::string> labels = edges[edge];
        for (auto next_next_vid : g[next_vid]) 
        {
            std::pair<int, int> edge = std::make_pair(next_vid, next_next_vid);
            std::vector<std::string> next_labels = edges[edge];
            for (auto cur_label : labels)
            {
                for (auto nxt_label : next_labels)
                {
                    myfile << cur_label << ", " << nxt_label << std::endl;
                }
            }
        }
        output_next_edge_labels(next_vid, myfile, edges, g);
    }
}

void output_adjacency_edge_to_file(const char* filename, edges_type &edges, graph_type &g)
{
    std::string adjancecy_output_filename = std::string(filename) + ".adjancecy.gv";
    std::ofstream myfile(adjancecy_output_filename);
    if(myfile.is_open())
    {
        int first_vid = 0; // first vertex id
        output_next_edge_labels(first_vid, myfile, edges, g);
        myfile.close();
    }
    else
    {
        std::cout << "Error: Cannot open output file given. " << std::endl;
    }
}

void output_all_next_edge_labels(std::string cur_label, int next_vid, std::ofstream &myfile, edges_type &edges, graph_type &g)
{
    for (auto next_next_vid : g[next_vid]) 
    {
        std::pair<int, int> edge = std::make_pair(next_vid, next_next_vid);
        std::vector<std::string> next_labels = edges[edge];
        for (auto nxt_label : next_labels)
        {
            myfile << cur_label << ", " << nxt_label << std::endl;
        }
        output_all_next_edge_labels(cur_label, next_next_vid, myfile, edges, g);
    }
}

void output_all_edge_labels(int first_vid, std::ofstream &myfile, edges_type &edges, graph_type &g)
{
    for (auto next_vid : g[first_vid]) 
    {
        std::pair<int, int> edge = std::make_pair(first_vid, next_vid);
        std::vector<std::string> labels = edges[edge];
        for (std::string label : labels)
        {  
            output_all_next_edge_labels(label, next_vid, myfile, edges, g);
        }
        output_all_edge_labels(next_vid, myfile, edges, g);
    }
}

void output_all_edge_to_file(const char* filename, edges_type &edges, graph_type &g)
{
    std::string adjancecy_output_filename = std::string(filename) + ".all.gv";
    std::ofstream myfile(adjancecy_output_filename);
    if(myfile.is_open())
    {
        int first_vid = 0; // first vertex id
        output_all_edge_labels(first_vid, myfile, edges, g);
        myfile.close();
    }
    else
    {
        std::cout << "Error: Cannot open output file given. " << std::endl;
    }
}

int main(int argc, char** argv) {
   
    std::cout << "extract tree lines for gv " << std::endl;

    if (argc <= 1)
    {
        std::cout << "No input file given. " << std::endl;
        return 1;
    }
    
    std::map<std::pair<int, int>, std::vector<std::string>> edges;
    graph_type g;
    int num_v = 0;
    read_gv_files(argv[1], num_v, edges, g);

    output_same_level_labels_to_file(argv[1], edges);

    output_adjacency_edge_to_file(argv[1], edges, g);

    output_all_edge_to_file(argv[1], edges, g);

   return 0;
}

