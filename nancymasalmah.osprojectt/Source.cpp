#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib> 

using namespace std;

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;

    bool operator<(const Process& other) const {
        return remaining_time > other.remaining_time;
    }
};

void readProcessesFromFile(const string& filename, vector<Process>& processes) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "File " << filename << " not found. Creating a new file." << endl;
        // Create a new file if it doesn't exist
        ofstream newFile(filename);
        if (!newFile.is_open()) {
            cerr << "Error: Unable to create file " << filename << endl;
            exit(1);
        }
        // Write default values to the file
       
        newFile << "4 0 7\n";
        newFile << "2 2 3\n";
        newFile << "3 4 6\n";
        newFile.close();
        cout << "Empty file " << filename << " created. Please add process data to the file." << endl;
        return;
    }
    cout << "File " << filename << " opened successfully." << endl;

    int pid, arrival_time, burst_time;
    while (file >> pid >> arrival_time >> burst_time) {
        processes.push_back({ pid, arrival_time, burst_time, burst_time, 0, 0, 0, 0 });
    }
    file.close();
}

void calculateWaitingTurnaround(vector<Process>& processes) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    for (auto& process : processes) {
        process.turnaround_time = process.finish_time - process.arrival_time;
        process.waiting_time = process.turnaround_time - process.burst_time;
        total_waiting_time += process.waiting_time;
        total_turnaround_time += process.turnaround_time;
    }
    cout << "\nTotal Waiting Time: " << total_waiting_time << endl;
    cout << "Total Turnaround Time: " << total_turnaround_time << endl;
}


void displayGanttChart(const vector<Process>& processes) {
    cout << "\nGantt Chart:" << endl;
    cout << " ";
    for (const auto& process : processes) {
        cout << "-------";
    }
    cout << "-" << endl;
    cout << "|";
    for  (const auto& process : processes) {
        cout << "  P" << process.pid << "   |";
    }
    cout << endl;
    cout << " ";
    int last_finish_time = processes.back().finish_time;
    for (int time = 0; time < last_finish_time; ++time) {
        cout << "   " << time << "   ";
    }
    cout << last_finish_time << endl;
    cout << " ";
    for (const auto& process : processes) {
        cout << "-------";
    }
    cout << "-" << endl;
}

void fcfs(vector<Process>& processes) {
    int current_time = 0;
    for (auto& process : processes) {
        process.start_time = max(current_time, process.arrival_time);
        process.finish_time = process.start_time + process.burst_time;
        current_time = process.finish_time;
    }
    displayGanttChart(processes);
    calculateWaitingTurnaround(processes);
}


void srt(vector<Process>& processes) {
    int current_time = 0;
    auto remaining_time_compare = [](const Process& p1, const Process& p2) {
        return p1.remaining_time > p2.remaining_time;
    };
    priority_queue<Process, vector<Process>, decltype(remaining_time_compare)> ready_queue(remaining_time_compare);

    while (!ready_queue.empty() || current_time < INT_MAX) { // Modified the loop condition
        for (int i = 0; i < processes.size(); ++i) { // Iterate through all processes
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                ready_queue.push(processes[i]);
            }
        }

        if (ready_queue.empty()) {
            ++current_time;
            continue;
        }

        Process current_process = ready_queue.top();
        ready_queue.pop();

        current_process.start_time = current_time;
        current_process.finish_time = current_time + 1;
        --current_process.remaining_time;
        current_time = current_process.finish_time;

        if (current_process.remaining_time > 0) {
            ready_queue.push(current_process);
        }
        else {
            processes[current_process.pid - 1] = current_process;
        }
    }
    displayGanttChart(processes);
    calculateWaitingTurnaround(processes);
}

void rr(vector<Process>& processes, int quantum) {
    int current_time = 0;
    queue<Process> ready_queue;
    int index = 0;

    while (!ready_queue.empty() || index < processes.size()) {
        while (index < processes.size() && processes[index].arrival_time <= current_time) {
            ready_queue.push(processes[index]);
            ++index;
        }

        if (ready_queue.empty()) {
            current_time = processes[index].arrival_time;
            continue;
        }

        Process current_process = ready_queue.front();
        ready_queue.pop();

        current_process.start_time = current_time;
        int remaining_burst_time = min(quantum, current_process.remaining_time);
        current_process.finish_time = current_time + remaining_burst_time;
        current_process.remaining_time -= remaining_burst_time;
        current_time = current_process.finish_time;

        if (current_process.remaining_time > 0) {
            ready_queue.push(current_process);
        }
        else {
            // Fixing indexing here
            processes[current_process.pid - 1] = current_process;
        }
    }
    displayGanttChart(processes);
    calculateWaitingTurnaround(processes);
}


int main() {
    vector<Process> processes;
    string filename;
    int algorithm_choice, quantum;

    cout << "Enter the filename containing the processes: ";
    cin >> filename;

    readProcessesFromFile(filename, processes);

    cout << "Choose the scheduling algorithm:\n";
    cout << "1. FCFS\n";
    cout << "2. SRT\n";
    cout << "3. RR\n";
    cout << "Enter your choice: ";
    cin >> algorithm_choice;

    switch (algorithm_choice) {
    case 1:
        fcfs(processes);
        break;
    case 2:
        srt(processes);
        break;
    case 3:
        cout << "Enter the time quantum for RR: ";
        cin >> quantum;
        rr(processes, quantum);
        break;
    default:
        cout << "Invalid choice. Exiting...\n";
        exit(1);
    }

    return 0;
}