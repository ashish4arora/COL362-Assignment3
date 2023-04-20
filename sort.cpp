#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <deque>
#include <bits/stdc++.h>
#include <string>
#include <sys/mman.h>
using namespace std;




// Function to merge k sorted runs and save to output file
// Currently using a min-heap for sorting purpose 
// Allocate memory for the input files, and read them using getline to the allocated memory 
// Allocate memory for the output buffer and finally write the output buffer to the output file
// Clear the allocated memory after the merge is completed

long long mergeruns_clock=0;
long long input_clock = 0;
long long output_clock = 0;
long long merge_clock = 0;

void merge_runs(const vector<string>& inputs, const string& output) {
    cout << "outfile: " << output << endl;
    auto start_1 = chrono::steady_clock::now();
    vector<vector<string>> files(inputs.size());
    vector<int> index(inputs.size(), 0);
    string line;

    auto input_start = chrono::steady_clock::now();
    for (size_t i = 0; i < inputs.size(); ++i) {
        
        ifstream file(inputs[i]);
        while(getline(file, line)){
            files[i].push_back(line);
        }
        file.close();
    }
    auto input_end = chrono::steady_clock::now();
    input_clock += chrono::duration_cast<chrono::milliseconds>(input_end - input_start).count();

    ofstream outfile(output);
    // outfile.rdbuf()->pubsetbuf(0, 1024);
    
    auto merge_start = chrono::steady_clock::now();
    priority_queue<pair<string, int>, vector<pair<string, int>>, greater<pair<string, int>>> min_heap;
    for (size_t i = 0; i < files.size(); ++i) {
        string key;
        if (index[i] < files[i].size()){
            min_heap.push({files[i][index[i]], static_cast<int>(i)});
            index[i]++;
        }
    }

    while (!min_heap.empty()) {
        pair<string, int> min_pair = min_heap.top();
        min_heap.pop();
        // auto output_start = chrono::steady_clock::now();
        outfile << min_pair.first << "\n";
        // auto output_end = chrono::steady_clock::now();
        // output_clock += chrono::duration_cast<chrono::milliseconds>(output_end - output_start).count();

        string key;
        if (index[min_pair.second] < files[min_pair.second].size()) {
            min_heap.push({files[min_pair.second][index[min_pair.second]], min_pair.second});
            index[min_pair.second]++;
        }
    }

    auto merge_end = chrono::steady_clock::now();
    merge_clock += chrono::duration_cast<chrono::milliseconds>(merge_end - merge_start).count();
    outfile.flush();
    outfile.close();

    for (size_t i = 0; i < files.size(); ++i) {
        vector<string>().swap(files[i]);
    }
    vector<vector<string>>().swap(files);
    auto end_1 = chrono::steady_clock::now();
    mergeruns_clock += chrono::duration_cast<chrono::milliseconds>(end_1 - start_1).count();
    
}



/*
 The function is expected to implement a k−way external merge−sort
 on a file consisting of string values. The description of arguments
 is given in the assignment document.
*/
int externalmergesortwithstop ( const char* input , const char* output ,
const long key_count , const int k = 2 , const int num_merges = 0 ) {

    int size_run=100000;
    vector<string> run_files;
    
    ifstream input_stream(input);
    long keys_read = 0;
    int run_num = 0;
    while (keys_read < key_count && !input_stream.eof()) {
        // Read size_run keys into memory and sort them
        vector<string> keys;
        keys.reserve(size_run);
        for (int i = 0; i < size_run && !input_stream.eof(); i++) {
            string key;
            getline(input_stream, key);
            keys.push_back(key);
            keys_read++;
        }
        sort(keys.begin(), keys.end());
        // Write sorted keys to a run file
        run_num++;
        string run_file = "temp.0." + to_string(run_num);
        ofstream run_stream(run_file);
        // run_stream.rdbuf()->pubsetbuf(0, 1024);
        for (const auto& key : keys) {
            run_stream << key << '\n';
        }
        vector<string>().swap(keys);
        run_files.push_back(run_file);
        run_stream.flush();
        run_stream.close();
    }
    input_stream.close();
    

    // Step 2: Merge sorted runs until only one run remains
    int merge_steps = 0;
    vector<string> temp;
    int merge_count=0;
    int l=1;

    while (run_files.size() > 1) {
        merge_count=0;
          while (run_files.size() > 0) {
        // Merge k runs into one
        vector<string> input_files;
        input_files.reserve(k);
        for (int i = 0; i < k && !run_files.empty(); ++i) {
            input_files.push_back(run_files.front());
            run_files.erase(run_files.begin());
        }
        merge_count++;
        string output_file = "temp." + to_string(l) + "." + to_string(merge_count);
        merge_runs(input_files, output_file);
        temp.push_back(output_file);
        ++merge_steps;

        // Check if we've completed the requested number of merge steps
        if (num_merges > 0 && merge_steps >= num_merges) {
            break;
        }
    }
    while(!temp.empty()){
        run_files.push_back(temp.back());
        temp.pop_back();
    }
    reverse(run_files.begin(), run_files.end());
    l++;
    if (num_merges > 0 && merge_steps >= num_merges) {
            break;
        }
    }

    // Step 3: Rename final run file to output file
    if (run_files.size() != 1) {
        return merge_steps;  // Something went wrong, we should have exactly one run file
    }
    else rename(run_files[0].c_str(), output) != 0;
        


    return merge_steps;
}


int main() {
    auto start = chrono::steady_clock::now();
      externalmergesortwithstop ( "input.txt" , "output.txt" , 1000000 , 2 , 0 );
    auto end = chrono::steady_clock::now();
    cout<<chrono::duration_cast<chrono::milliseconds>(end-start).count()<< " milliseconds" << endl;
    cout << mergeruns_clock << " milliseconds" << endl;
    cout << "input time: " << input_clock << " milliseconds" << endl;
    cout << "output time: " << output_clock << " milliseconds" << endl;
    cout << "merge time: " << merge_clock << " milliseconds" << endl;
}