#include <bits/stdc++.h>
using namespace std;

struct Expense {
    string payer;
    double amount;
    vector<string> participants;
};

unordered_map<string, unordered_map<string, double>> graph;
unordered_map<string, double> balances;
vector<Expense> expenses;

void addUser() {
    string name;
    cout << "Enter new user name: ";
    cin >> name;

    if (balances.find(name) != balances.end()) {
        cout << "User " << name << " already exists!" << endl;
    } else {
        balances[name] = 0.0;
        cout << "User " << name << " added successfully!" << endl;
    }
}

void addExpense() {
    string payer;
    double amount;
    int numParticipants;
    vector<string> participants;

    cout << "Enter name of the payer: ";
    cin >> payer;
    cout << "Enter amount: ";
    cin >> amount;
    cout << "Enter number of participants: ";
    cin >> numParticipants;

    for (int i = 0; i < numParticipants; ++i) {
        string participant;
        cout << "Enter participant " << i + 1 << ": ";
        cin >> participant;
        participants.push_back(participant);
    }

    double share = amount / numParticipants;
    for (const string& participant : participants) {
        if (participant != payer) {
            graph[payer][participant] += share;
            balances[participant] -= share;
            balances[payer] += share;
        }
    }

    expenses.push_back({payer, amount, participants});
    cout << "Expense added successfully!" << endl;
}

void dfs(const string& node, unordered_map<string, bool>& visited, vector<string>& cycle) {
    visited[node] = true;
    cycle.push_back(node);

    for (const auto& neighbor : graph[node]) {
        if (!visited[neighbor.first] && neighbor.second > 0) {
            dfs(neighbor.first, visited, cycle);
            if (!cycle.empty() && cycle.back() == neighbor.first) {
                return;
            }
        }
    }

    if (!cycle.empty() && cycle.front() == cycle.back()) {
        return;
    }

    cycle.pop_back();
}

void settleCycle(vector<string>& cycle) {
    double minDebt = numeric_limits<double>::max();
    for (size_t i = 0; i < cycle.size() - 1; ++i) {
        minDebt = min(minDebt, graph[cycle[i]][cycle[i + 1]]);
    }

    for (size_t i = 0; i < cycle.size() - 1; ++i) {
        graph[cycle[i]][cycle[i + 1]] -= minDebt;
        if (graph[cycle[i]][cycle[i + 1]] == 0) {
            graph[cycle[i]].erase(cycle[i + 1]);
        }
    }
}

void simplifyDebts() {
    unordered_map<string, bool> visited;
    vector<string> cycle;

    for (const auto& node : graph) {
        if (!visited[node.first]) {
            dfs(node.first, visited, cycle);
            if (!cycle.empty()) {
                cycle.push_back(cycle.front());
                settleCycle(cycle);
                cycle.clear();
                visited.clear();
            }
        }
    }
}

void showDebts() {
    simplifyDebts();

    vector<pair<string, double>> positiveBalances;
    vector<pair<string, double>> negativeBalances;

    for (const auto& pair : balances) {
        if (pair.second > 0) {
            positiveBalances.push_back(pair);
        } else if (pair.second < 0) {
            negativeBalances.push_back({pair.first, -pair.second});
        }
    }

    if (positiveBalances.empty() && negativeBalances.empty()) {
        cout << "All good! No debts to settle." << endl;
        return;
    }

    sort(positiveBalances.begin(), positiveBalances.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
        return a.second > b.second;
    });

    sort(negativeBalances.begin(), negativeBalances.end(), [](const pair<string, double>& a, const pair<string, double>& b) {
        return a.second > b.second;
    });

    int i = 0, j = 0;
    while (i < positiveBalances.size() && j < negativeBalances.size()) {
        double amount = min(positiveBalances[i].second, negativeBalances[j].second);
        cout << negativeBalances[j].first << " should pay " << positiveBalances[i].first << " " << fixed << setprecision(2) << amount << endl;
        positiveBalances[i].second -= amount;
        negativeBalances[j].second -= amount;

        if (positiveBalances[i].second == 0) {
            ++i;
        }
        if (negativeBalances[j].second == 0) {
            ++j;
        }
    }
}

void settleDebt() {
    string user;
    cout << "Enter name of the user: ";
    cin >> user;

    if (balances[user] > 0) {
        for (const auto& pair : balances) {
            if (pair.second < 0) {
                double amountOwed = min(balances[user], -pair.second);
                cout << pair.first << " owes " << user << " " << fixed << setprecision(2) << amountOwed << endl;
                cout << "Clear debt? 1. Yes 2. No: ";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    balances[user] -= amountOwed;
                    balances[pair.first] += amountOwed;
                    cout << "Debt cleared!" << endl;
                } else {
                    cout << "Debt not cleared." << endl;
                }
                return;
            }
        }
    } else if (balances[user] < 0) {
        for (const auto& pair : balances) {
            if (pair.second > 0) {
                double amountOwed = min(pair.second, -balances[user]);
                cout << user << " owes " << pair.first << " " << fixed << setprecision(2) << amountOwed << endl;
                cout << "Clear debt? 1. Yes 2. No: ";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    balances[user] += amountOwed;
                    balances[pair.first] -= amountOwed;
                    cout << "Debt cleared!" << endl;
                } else {
                    cout << "Debt not cleared." << endl;
                }
                return;
            }
        }
    } else {
        cout << user << " has no debts." << endl;
    }
}

int main() {
    while (true) {
        cout << "\nOptions:\n";
        cout << "1. Enter new user\n";
        cout << "2. Enter new expense\n";
        cout << "3. Show current debts\n";
        cout << "4. Settle debt\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        switch (choice) {
            case 1:
                addUser();
                break;
            case 2:
                addExpense();
                break;
            case 3:
                showDebts();
                break;
            case 4:
                settleDebt();
                break;
            case 5:
                cout << "Exiting..." << endl;
                return 0;
            default:
                cout << "Invalid choice, please try again." << endl;
        }
    }
    return 0;
}
