#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <ctime>

using namespace std;

class Project {
   private:
        string title;
        string desc;
        int priority = 0;
        string status = "pending";  // Set default status to "pending"
        int day = 0, month = 0, year = 0;

    public:
        // Constructor to initialize status to "pending"
        Project() {
            status = "pending";  // Automatically set the status to "pending"
        }

        void add_title(string t){
            if (t.empty()){
                cout << "There was no title input" << endl;
            }
            else{
                title = t;
            }
        }

        void add_desc(string a){
            if(a.empty()){
                cout << "There was no description entered" << endl;
            }
            else{
                desc = a;
            }
        }

        int set_priority(int s){
            if (s==0){
                cout << "There was no priority number input" << endl;
            }
            else{
                if (s<1 || s>10){
                    cout << "Invalid priority declaration" << endl;
                }
                else{
                    priority = s;
                }
            }
            return priority;
        }

        int get_priority() const {
            return priority;
        }

        bool set_date(int d, int m, int y) {
            time_t t = time(nullptr);
            tm* now = localtime(&t);
            int current_year = now->tm_year + 1900;
            int current_month = now->tm_mon + 1;
            int current_day = now->tm_mday;

            if (y < current_year || (y == current_year && m < current_month) || (y == current_year && m == current_month && d <= current_day)) {
                cout << "Invalid date! Due date must be in the future." << endl;
                return false;
            }
            
            if (m < 1 || m > 12 || d < 1 || d > 31) {
                cout << "Invalid date! Please enter a valid day, month, and year." << endl;
                return false;
            }

            day = d;
            month = m;
            year = y;
            return true;
        }

        void set_status(const string& new_status) {
            status = new_status;
        }

        string get_status() const {
            return status;
        }

        void save_to_file(ofstream& file) const {
            file << title << endl;
            file << desc << endl;
            file << priority << endl;
            file << day << "-" << month << "-" << year << endl;  // Correct date format
            file << status << endl;
            file << "////////////////////////////////////" << endl;
        }

        static Project load_from_file(ifstream& file) {
            Project task;
            string line;

            // Read the title
            if (!getline(file, task.title) || task.title.empty()) return task;

            // Read the description
            getline(file, task.desc);

            // Read priority
            file >> task.priority;

            // Read the date (correctly parse the format dd-mm-yyyy)
            char dash1, dash2;  // To handle the dash characters between the date components
            file >> task.day >> dash1 >> task.month >> dash2 >> task.year;
            file.ignore();  // Ignore newline left by >> operator

            // Read the status
            getline(file, task.status);

            // Skip the separator line
            getline(file, line);  // This will read the "////////////////////////////////////"

            return task;
        }

        void print_from_file() const {
            cout << "Title: " << title << endl;
            cout << "Description: " << desc << endl;
            cout << "Priority: " << priority << endl;
            cout << "Due Date: " << day << "-" << month << "-" << year << endl;
            cout << "Status: " << status << endl;
            cout << "///////////////////////////////////" << endl;
        }

        string get_title() const {
            return title;
        }

        int get_day() const { return day; }
        int get_month() const { return month; }
        int get_year() const { return year; }
};

int main(){
    int num;
    Project task;
    vector<Project> tasks;
    string title, description, status;
    int priority, day, month, year;

    do{
        cout << "Hello, press the number that fits the criteria" << endl;
        cout << "1. Create new task" << endl;
        cout << "2. Update task status" << endl;
        cout << "3. View task list"  << endl;
        cin >> num;
        cin.ignore();
        if (num == 0 || num > 4 || cin.fail()){
            cout << "No actions were given" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
        else if (num == 1) {
            do {
                cout << "Please provide the task title" << endl;
                getline(cin, title);
                task.add_title(title);
            } while (title.empty());

            do {
                cout << "Please provide the task description" << endl;
                getline(cin, description);
                task.add_desc(description);
            } while (description.empty());

            do {
                cout << "Please provide the priority number of the document" << endl;
                cin >> priority;
                task.set_priority(priority);
            } while (priority < 1 || priority > 10);

            bool valid_date = false;
            while (!valid_date) {
                cout << "Please enter the due date separately:" << endl;
                cout << "Day: "; cin >> day;
                cout << "Month: "; cin >> month;
                cout << "Year: "; cin >> year;
                valid_date = task.set_date(day, month, year);
            }
            cin.ignore();

            // Status is automatically set to "pending"
            cout << "The task has been created succesfully." << endl;

            // Save task to file
            ofstream file("tasks.txt", ios::app);
            task.save_to_file(file);
            file.close();
        }
        else if (num == 2) {
            ifstream file("tasks.txt");
            if (!file.is_open()) {
                cout << "No tasks found!" << endl;
            } else {
                tasks.clear();
                while (file) {
                    Project task = Project::load_from_file(file);
                    if (!file.eof()) {
                        tasks.push_back(task);
                    }
                }
                file.close();

                if (tasks.empty()) {
                    cout << "No tasks available to update!" << endl;
                    continue;
                }

                // Print task titles for user to choose from
                cout << "Select a task to update its status:" << endl;
                for (int i = 0; i < tasks.size(); ++i) {
                    cout << i + 1 << ". " << tasks[i].get_title() << endl;
                }

                int choice;
                cout << "Enter the task number to update its status: ";
                cin >> choice;
                cin.ignore();  // To discard any leftover newline character

                if (choice < 1 || choice > tasks.size()) {
                    cout << "Invalid choice!" << endl;
                    continue;
                }

                // Get selected task
                Project& selected_task = tasks[choice - 1];

                // Show the current status
                cout << "Current status: " << selected_task.get_status() << endl;

                // Ask the user if they want to toggle the status
                string current_status = selected_task.get_status();
                string new_status = (current_status == "pending") ? "completed" : "pending";

                cout << "Do you want to change the status from '" << current_status << "' to '" << new_status << "'? (y/n): ";
                char confirm;
                cin >> confirm;

                if (confirm == 'y' || confirm == 'Y') {
                    selected_task.set_status(new_status);
                    cout << "Task status updated to '" << new_status << "'!" << endl;
                } else {
                    cout << "Status change canceled." << endl;
                    continue;
                }

                // Rewrite tasks back to file
                ofstream file("tasks.txt", ios::trunc);
                for (const auto& task : tasks) {
                    task.save_to_file(file);
                }
                file.close();
            }
        }
        else if (num == 3) {
            ifstream file("tasks.txt");
            if (!file.is_open()) {
                cout << "No tasks found!" << endl;
            } else {
                tasks.clear();
                while (file) {
                    Project task = Project::load_from_file(file);
                    if (!file.eof()) {
                        tasks.push_back(task);
                    }
                }
                file.close();

                if (tasks.empty()) {
                    cout << "No tasks available!" << endl;
                    continue;
                }

                int sort_choice;
                cout << "Choose sorting order: " << endl;
                cout << "1. By Priority" << endl;
                cout << "2. By Due Date" << endl;
                cin >> sort_choice;

                if (sort_choice == 1) {
                    sort(tasks.begin(), tasks.end(), [](const Project& a, const Project& b) {
                        return a.get_priority() > b.get_priority();
                    });
                } else if (sort_choice == 2) {
                    sort(tasks.begin(), tasks.end(), [](const Project& a, const Project& b) {
                        if (a.get_year() != b.get_year()) return a.get_year() < b.get_year();
                        if (a.get_month() != b.get_month()) return a.get_month() < b.get_month();
                        return a.get_day() < b.get_day();
                    });
                }

                cout << "Sorted List of Tasks:\n";
                for (const auto &task : tasks) {
                    task.print_from_file();
                }
            }
        }
    } while (num < 1 || num > 3);

    return 0;
}
