#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <stdexcept>
using namespace std;

//forward declaration
class User;
class Group;
class ExpenseManager;

enum class SplitType{
    EQUAL,
    EXACT,
    PERCENTAGE
};

class Split{
public:
    string userId;
    double amount;

    Split(const string& uid, double a){
        this->userId = uid;
        this->amount = a;
    }
};

//Observer Pattern :Notification interface
class IObserver{
public:    virtual ~IObserver() = default;
    virtual void update(const string& message) = 0;
};

//Strategy Pattern: split strategy interface
class ISplitStrategy{
public:
    virtual ~ISplitStrategy() = default;
    virtual vector<Split> calculateSplits(double totalAmount, const vector<string>& userIds, const vector<double>& values) = 0;
};

//Equal Split Strategy
class EqualSplitStrategy : public ISplitStrategy{
public:
    vector<Split> calculateSplits(double totalAmount, const vector<string>& userIds, const vector<double>& values) override{
        vector<Split> splits;
        int numUsers = userIds.size();
        double splitAmount = totalAmount / numUsers;

        for(const string& uid : userIds){
            splits.push_back(Split(uid, splitAmount));
        }
        return splits;
    }
};

class ExactSplitStrategy : public ISplitStrategy{
public:
    vector<Split> calculateSplits(double totalAmount, const vector<string>& userIds, const vector<double>& values) override{
        if(userIds.size() != values.size()){
            throw invalid_argument("Number of users does not match number of split values.");
        }
        vector<Split> splits;
        double sum = 0;
        for(double val : values){
            sum += val;
        }
        if(abs(sum - totalAmount) > 1e-6){
            throw invalid_argument("Exact split amounts do not sum up to total amount.");
        }
        for(size_t i = 0; i < userIds.size(); i++){
            splits.push_back(Split(userIds[i], values[i]));
        }
        return splits;
    }
};

class PercentageSplitStrategy : public ISplitStrategy{
public:
    vector<Split> calculateSplits(double totalAmount, const vector<string>& userIds, const vector<double>& values) override{
        if(userIds.size() != values.size()){
            throw invalid_argument("Number of users does not match number of split values.");
        }
        vector<Split> splits;
        double sum = 0;
        for(double val : values){
            sum += val;
        }
        if(abs(sum - 100.0) > 1e-6){
            throw invalid_argument("Percentage values do not sum up to 100.");
        }
        for(size_t i = 0; i < userIds.size(); i++){
            double splitAmount = (values[i] / 100) * totalAmount;
            splits.push_back(Split(userIds[i], splitAmount));
        }
        return splits;
    }
};

//Factory Pattern: Split strategy factory
class SplitFactory{
public:
    static ISplitStrategy* createSplitStrategy(SplitType type){
        switch(type){
            case SplitType::EQUAL:
                return new EqualSplitStrategy();
            case SplitType::EXACT:
                return new ExactSplitStrategy();
            case SplitType::PERCENTAGE:
                return new PercentageSplitStrategy();
            default:
                cout << "Invalid split type. Defaulting to Equal Split." << endl;
                return new EqualSplitStrategy();
        }
    }
};

//ConcreteObserver: User class
class User : public IObserver{
public:
    static int nextUserId;
    string userId;
    string name;
    string email;
    map<string, double> balances; 

    User(string name, string email){
        this->userId = "U" + to_string(++nextUserId);
        this->name = name;
        this->email = email;
    }
    void update(const string& message) override{
        cout << "NOTIFICATION: message for " << name << ": " << message << endl;
    }

    void updateBalance(const string& otherUserId, double amount){
        balances[otherUserId] += amount;
        if(abs(balances[otherUserId]) < 0.01){
            balances.erase(otherUserId);
        }
    }
    
    double getTotalOwed(){
        double total = 0;
        for(const auto& entry : balances){
            if(entry.second > 0){
                total += entry.second;
            }
        }
        return total;
    }

    double getTotalOwing(){
        double total = 0;
        for(const auto& entry : balances){
            if(entry.second < 0){
                total += abs(entry.second);
            }
        }
        return total;
    }

};
int User::nextUserId = 0;

//Expense Model class
class Expense{
public:
    static int nextExpenseId;
    string expenseId;
    string description;
    double amount;
    string paidBy;
    vector<Split> splits;
    string groupId;

    Expense(const string& desc, double amt, const string& paidBy, const vector<Split>& splits, const string& groupId)
        : description(desc), amount(amt), paidBy(paidBy), splits(splits), groupId(groupId) {
        this->expenseId = "E" + to_string(++nextExpenseId);
    }

};
int Expense::nextExpenseId = 0;

//IMPORTANT: for dsa+lld interviews
//Minimum number of transactions to settle debts(Greedy Algorithm)
class DebtSimplification{
public:
    static map<string, map<string, double>> simplify(const map<string, map<string, double>>& groupBalances){
        map<string, double> netBalances;
        for(const auto& entry : groupBalances){
            string userId = entry.first;
            double netBalance = 0;
            for(const auto& balanceEntry : entry.second){
                netBalance += balanceEntry.second;
            }
            netBalances[userId] = netBalance;
        }
        vector<pair<string, double>> creditors;
        vector<pair<string, double>> debtors;
        for(const auto& entry : netBalances){
            if(entry.second > 0){
                creditors.push_back(entry);
            } else if(entry.second < 0){
                debtors.push_back(entry);
            }
        }
        sort(creditors.begin(), creditors.end(), [](const pair<string, double>& a, const pair<string, double>& b){
            return a.second > b.second;
        });
        sort(debtors.begin(), debtors.end(), [](const pair<string, double>& a, const pair<string, double>& b){
            return a.second < b.second;
        });
        
        map<string, map<string, double>> res;
        for(const auto& entry : groupBalances){
            res[entry.first] = map<string, double>();
        }
        
        for(auto& creditor : creditors){
            string creditorId = creditor.first;
            
            for(auto& debtor : debtors){
                string debtorId = debtor.first;
                
                if(creditor.second <= 0) break;
                if(debtor.second >= 0) continue;

                double settlementAmount = min(creditor.second, -debtor.second);
                
                res[debtorId][creditorId] -= settlementAmount;
                res[creditorId][debtorId] += settlementAmount;

                creditor.second -= settlementAmount;
                debtor.second += settlementAmount;
            }
        }

        return res;
    }
};

//Group class-> concreteObservable
class Group{
private:
    User* getUserById(const string& userId){
        for(User* user : members){
            if(user->userId == userId){
                return user;
            }
        }
        return nullptr;
    }
public:
    static int nextGroupId;
    string groupId;
    string groupName;
    vector<User*> members;
    map<string, Expense*> groupExpenses; //history of expenses in the group
    map<string, map<string, double>> groupBalances; //userId -> (otherUserId -> balance)

    Group(const string& name){
        this->groupId = "Group:" + to_string(++nextGroupId);
        this->groupName = name;
    }

    ~Group(){
        for(auto& pair: groupExpenses){
            delete pair.second;
        }
    }

    bool addMember(User* user){
        members.push_back(user);
        groupBalances[user->userId] = map<string, double>();
        
        //notify existing members
        notifyMembers("User " + user->name + " has joined the group " + groupName);
        return true;
    }

    bool removeMember(const string& userId){
        //check if user can be removed (no outstanding balances)
        if(!canUserLeaveGrpoup(userId)){
            cout << "LOG: Cannot remove user. Outstanding balances exist." << endl;
            return false;
        }
        //get user object
        User* user = getUserById(userId);
        if(user == nullptr){
            cout << "LOG: User not found in group." << endl;
            return false;
        }
        //Remove from observers
        for(User* member : members){
            if(userId == member->userId){
                members.erase(remove(members.begin(), members.end(), member), members.end());
                break;
            }
        }
        //Remove from grp balances
        groupBalances.erase(userId);

        //Remove from othr members grp balances
        for(auto& entry : groupBalances){
            entry.second.erase(userId);
        }
        //notify other members
        notifyMembers("User " + user->name + " has left the group " + groupName);
        return true;
    }

    void notifyMembers(const string& message){
        for(User* member : members){
            member->update(message);
        }
    }

    bool isMember(const string& userId){
        return groupBalances.find(userId) != groupBalances.end();
    }

    //update grpbalances 
    void updateGroupBalance(const string& fromUserId, const string& toUserId, double amount){
        groupBalances[fromUserId][toUserId] -= amount;
        groupBalances[toUserId][fromUserId] += amount;

        //remove if balance is close to zero
        if(abs(groupBalances[fromUserId][toUserId]) < 0.01){
            groupBalances[fromUserId].erase(toUserId);
        }
        if(abs(groupBalances[toUserId][fromUserId]) < 0.01){
            groupBalances[toUserId].erase(fromUserId);
        }

    }

    //check if user can leave group (no outstanding balances)
    bool canUserLeaveGrpoup(const string& userId){
        if(groupBalances.find(userId) == groupBalances.end()){
            return true; //not a member
        }
        for(const auto& entry : groupBalances[userId]){
            if(abs(entry.second) > 0.01){
                return false; //outstanding balance exists
            }
        }
        return true;
    }

    //get user balance in group
    double getUserBalance(const string& userId){
        double balance = 0;
        if(groupBalances.find(userId) != groupBalances.end()){
            for(const auto& entry : groupBalances[userId]){
                balance += entry.second;
            }
        }
        return balance;
    }

    //add expense to group
    bool addExpense(const string& description, double amount, const string& paidBy, SplitType splitType, const vector<string>& userIds, const vector<double>& values){
        if(groupBalances.find(paidBy) == groupBalances.end()){
            cout << "ERROR: User who paid is not a member of the group." << endl;
            return false;
        }

        // Validate that all participants are also in the group
        for(const string& uid : userIds) {
            if(groupBalances.find(uid) == groupBalances.end()) {
                cout << "ERROR: User " << uid << " is not a member of the group." << endl;
                return false;
            }
        }

        ISplitStrategy* strategy = SplitFactory::createSplitStrategy(splitType);
        vector<Split> splits;
        try{
            splits = strategy->calculateSplits(amount, userIds, values);
        } catch(const invalid_argument& e){
            cout << "ERROR: Error calculating splits: " << e.what() << endl;
            delete strategy;
            return false;
        }
        delete strategy;

        Expense* expense = new Expense(description, amount, paidBy, splits, groupId);
        groupExpenses[expense->expenseId] = expense;

        //update balances
        for(const Split& split : splits){
            // Skip updating balance if the person owes themselves
            if(paidBy == split.userId) continue;

            updateGroupBalance(split.userId, paidBy, split.amount);
            User* paidByUser = getUserById(paidBy);
            User* owedUser = getUserById(split.userId);
            if(paidByUser && owedUser){
                paidByUser->updateBalance(split.userId, split.amount);
                owedUser->updateBalance(paidBy, -split.amount);
            }
        }
        notifyMembers("New expense added: " + description + " Amount: " + to_string(amount) + " Paid by: " + paidBy);
        
        // Automatically simplify debts in the group after adding an expense
        simplifyDebts();
        
        return true;
    }

    bool settleExpense(const string& fromUserId, const string& toUserId, double amount){
        if(groupBalances.find(fromUserId) == groupBalances.end() || groupBalances[fromUserId].find(toUserId) == groupBalances[fromUserId].end()){
            cout << "ERROR: No outstanding balance between users." << endl;
            return false;
        }
        if(abs(groupBalances[fromUserId][toUserId]) < 0.01){
            cout << "ERROR: No outstanding balance between users." << endl;
            return false;
        }
        if(amount <= 0 || amount > abs(groupBalances[fromUserId][toUserId])){
            cout << "ERROR: Invalid settlement amount." << endl;
            return false;
        }
        updateGroupBalance(toUserId, fromUserId, amount);
        User* fromUser = getUserById(fromUserId);
        User* toUser = getUserById(toUserId);
        if(fromUser && toUser){
            fromUser->updateBalance(toUserId, amount);
            toUser->updateBalance(fromUserId, -amount);
        }
        notifyMembers("Expense settled: " + fromUser->name + " paid " + toUser->name + " amount: " + to_string(amount));
        //log settlement
        cout << "LOG: Settled expense between " << fromUser->name << " and " << toUser->name << " Amount: " << amount << endl;
        return true;
    }

    void simplifyDebts(){
        for(const auto& entry : groupBalances){
            string userId = entry.first;
            User* user = getUserById(userId);
            if(user){
                for(const auto& balanceEntry : entry.second){
                    user->updateBalance(balanceEntry.first, -balanceEntry.second);
                }
            }
        }
        map<string, map<string, double>> simplifiedBalances = DebtSimplification::simplify(groupBalances);
        groupBalances = simplifiedBalances;
        for(const auto& entry : groupBalances){
            string userId = entry.first;
            User* user = getUserById(userId);
            if(user){
                for(const auto& balanceEntry : entry.second){
                    user->updateBalance(balanceEntry.first, balanceEntry.second);
                }
            }
        }
        notifyMembers("Debts have been simplified among group members.");
        //log debt simplification
        cout << "LOG: Simplified debts among group members in group " << groupName << endl;
    }

    void showGroupBalances(){
        cout << "\n=========================================\n";
        cout << "     Balances for group: " << groupName << "\n";
        cout << "=========================================\n";
        bool hasBalances = false;
        
        for(const auto& entry : groupBalances){
            string userId = entry.first;
            User* user = getUserById(userId);
            if(user){
                bool userHasBalances = false;
                for(const auto& balanceEntry : entry.second){
                    if(abs(balanceEntry.second) >= 0.01){
                        userHasBalances = true;
                        hasBalances = true;
                        break;
                    }
                }
                
                if (userHasBalances) {
                    cout << "  [" << user->name << "]" << endl;
                    for(const auto& balanceEntry : entry.second){
                        string otherUserId = balanceEntry.first;
                        double balance = balanceEntry.second;
                        if(abs(balance) < 0.01) continue;
                        
                        User* otherUser = getUserById(otherUserId);
                        if(otherUser){
                            if(balance > 0){
                                cout << "    => Is owed by " << otherUser->name << ": $" << fixed << setprecision(2) << balance << endl;
                            } else {
                                cout << "    => Owes " << otherUser->name << ": $" << fixed << setprecision(2) << abs(balance) << endl;
                            }
                        }
                    }
                    cout << endl;
                }
            }
        }
        
        if (!hasBalances) {
            cout << "  No outstanding balances. Everyone is settled up!\n";
        }
        cout << "=========================================\n\n";
    }
};
int Group::nextGroupId = 0;

class SplitWise {
private:
    map<string, User*> users;
    map<string, Group*> groups;
    map<string, Expense*> expenses;
    static SplitWise* instance;
    SplitWise() {}
public:
    static SplitWise* getInstance(){
        if(instance == nullptr){
            instance = new SplitWise();
        }
        return instance;
    }

    //User management
    User* createUser(const string& name, const string& email){
        User* user = new User(name, email);
        users[user->userId] = user;
        cout << "LOG: Created user " << name << " with email " << email << endl;
        return user;
    }

    User* getUser(const string& userId){
        if(users.find(userId) != users.end()){
            return users[userId];
        }
        return nullptr;
    }

    //Group management
    Group* createGroup(const string& groupName){
        Group* group = new Group(groupName);
        groups[group->groupId] = group;
        cout << "LOG: Created group " << groupName << endl;
        return group;
    }

    Group* getGroup(const string& groupId){
        if(groups.find(groupId) != groups.end()){
            return groups[groupId];
        }
        return nullptr;
    }

    void addUserToGroup(const string& userId, const string& groupId){
        User* user = getUser(userId);
        Group* group = getGroup(groupId);
        if(user && group){
            bool result= group->addMember(user);
            if(result) cout << "LOG: Added user " << user->name << " to group " << group->groupName << ". Success: " << (result ? "Yes" : "No") << endl;

        } else {
            cout << "ERROR: Invalid user or group ID." << endl;
        }
    }

    bool removeUserFromGroup(const string& userId, const string& groupId){
        Group* group = getGroup(groupId);
        if(!group){
            cout << "ERROR: Invalid group ID." << endl;
            return false;
        }
        User* user = getUser(userId);
        if(!user){
            cout << "ERROR: Invalid user ID." << endl;
            return false;
        }
        bool result = group->removeMember(userId);
        if(result) cout << "LOG: Removed user " << user->name << " from group " << group->groupName << ". Success: " << (result ? "Yes" : "No") << endl;
        return result;
    }

    //Expense management
    void addExpenseToGroup(const string& groupId, const string& description, double amount, const string& paidBy, SplitType splitType, const vector<string>& userIds, const vector<double>& values){
        Group* group = getGroup(groupId);
        if(group){
            bool result = group->addExpense(description, amount, paidBy, splitType, userIds, values);
            if(result) cout << "LOG: Added expense " << description << " to group " << group->groupName << ". Success: " << (result ? "Yes" : "No") << endl;
        } else {
            cout << "ERROR: Invalid group ID." << endl;
        }
    }

    //Settlement management
    void settleExpense(const string& groupId, const string& fromUserId, const string& toUserId, double amount){
        Group* group = getGroup(groupId);
        if(group){
            bool result = group->settleExpense(fromUserId, toUserId, amount);
            if(result) cout << "LOG: Settled expense in group " << group->groupName << " between " << fromUserId << " and " << toUserId << ". Success: " << (result ? "Yes" : "No") << endl;
        } else {
            cout << "ERROR: Invalid group ID." << endl;
        }
    }

    //Individual user payment and expense
    void settleIndividualExpense(const string& fromUserId, const string& toUserId, double amount){
        User* fromUser = getUser(fromUserId);
        User* toUser = getUser(toUserId);
        if(fromUser && toUser){
            fromUser->updateBalance(toUserId, amount);
            toUser->updateBalance(fromUserId, -amount);
            cout << "LOG: Settled individual expense between " << fromUser->name << " and " << toUser->name << " Amount: " << amount << endl;
        } else {
            cout << "ERROR: Invalid user IDs for settlement." << endl;
        }
    }

    void addIndividualExpense(const string& description, double amount, string paidByUserId, string toUserId, SplitType splitType, const vector<double>& SplitValues ={}){
        User* paidByUser = getUser(paidByUserId);
        User* toUser = getUser(toUserId);
        if(!paidByUser || !toUser){
            cout << "ERROR: Invalid user IDs for expense." << endl;
            return;
        }
        ISplitStrategy* strategy = SplitFactory::createSplitStrategy(splitType);
        vector<Split> splits;
        try{
            splits = strategy->calculateSplits(amount, {paidByUserId, toUserId}, SplitValues);
        } catch(const invalid_argument& e){
            cout << "ERROR: Error calculating splits: " << e.what() << endl;
            delete strategy;
            return;
        }
        delete strategy;

        Expense* expense = new Expense(description, amount, paidByUserId, splits, "Individual");
        expenses[expense->expenseId] = expense;

        for(const Split& split : splits){
            if(split.userId == paidByUserId) continue; 

            User* owedUser = getUser(split.userId);
            if(paidByUser && owedUser){
                paidByUser->updateBalance(split.userId, split.amount);
                owedUser->updateBalance(paidByUserId, -split.amount);
            }
        }
        cout << "LOG: Added individual expense " << description << " by " << paidByUser->name << " Amount: " << amount << endl;
    }

    //Display functions
    void showUserBalances(const string& userId){
        User* user = getUser(userId);
        if(user){
            cout << "Balances for " << user->name << ":" << endl;
            cout<< "Total Owed: " << fixed << setprecision(2) << user->getTotalOwed() << endl;
            cout<< "Total Owing: " << fixed << setprecision(2) << user->getTotalOwing() << endl;

            cout<< "Detailed Balances:" << endl;
            for(const auto& entry : user->balances){
                string otherUserId = entry.first;
                double balance = entry.second;
                User* otherUser = getUser(otherUserId);
                if(otherUser){
                    if(balance > 0){
                        cout << "  " << otherUser->name << ": " << fixed << setprecision(2) << balance << endl;
                    } else if(balance < 0){
                        cout << "  " << otherUser->name << ": " << fixed << setprecision(2) << balance << endl;
                    }
                }
            }
        } else {
            cout << "ERROR: Invalid user ID." << endl;
        }
    }

    void showGroupBalances(const string& groupId){
        Group* group = getGroup(groupId);
        if(group){
            group->showGroupBalances();
        } else {
            cout << "ERROR: Invalid group ID." << endl;
        }
    }

    void simplifyGroupDebts(const string& groupId){
        Group* group = getGroup(groupId);
        if(group){
            group->simplifyDebts();
        } else {
            cout << "ERROR: Invalid group ID." << endl;
        }
    }
};

SplitWise* SplitWise::instance = nullptr;

int main() {
    SplitWise* app = SplitWise::getInstance();

    cout << "\n--- 1. CREATING USERS ---\n";
    User* alice = app->createUser("Alice", "alice@example.com");
    User* bob = app->createUser("Bob", "bob@example.com");
    User* charlie = app->createUser("Charlie", "charlie@example.com");
    User* david = app->createUser("David", "david@example.com");

    cout << "\n--- 2. CREATING GROUP & ADDING MEMBERS ---\n";
    Group* trip = app->createGroup("Goa Trip");
    app->addUserToGroup(alice->userId, trip->groupId);
    app->addUserToGroup(bob->userId, trip->groupId);
    app->addUserToGroup(charlie->userId, trip->groupId);
    app->addUserToGroup(david->userId, trip->groupId);

    cout << "\n--- 3. ADDING GROUP EXPENSES ---\n";
    
    // Equal Split: Alice paid $400 for everyone ($100 each)
    vector<string> allUsers = {alice->userId, bob->userId, charlie->userId, david->userId};
    app->addExpenseToGroup(trip->groupId, "Dinner", 400.0, alice->userId, SplitType::EQUAL, allUsers, {});

    // Exact Split: Bob paid $300 for Alice and Charlie
    vector<string> specificUsers = {alice->userId, charlie->userId, bob->userId};
    app->addExpenseToGroup(trip->groupId, "Cab", 300.0, bob->userId, SplitType::EXACT, specificUsers, {100.0, 150.0, 50.0});

    // Percentage Split: Charlie paid $500 for Alice(40%) and David(60%)
    vector<string> percUsers = {alice->userId, david->userId};
    app->addExpenseToGroup(trip->groupId, "Hotel", 500.0, charlie->userId, SplitType::PERCENTAGE, percUsers, {40.0, 60.0});

    cout << "\n--- 4. SHOWING GROUP BALANCES ---\n";
    // This will print the beautiful new display function
    app->showGroupBalances(trip->groupId);

    cout << "\n--- 5. SHOWING INDIVIDUAL BALANCES (Alice) ---\n";
    app->showUserBalances(alice->userId);

    cout << "\n--- 6. SETTLING EXPENSES ---\n";
    // David owes Charlie $300 (60% of Hotel which was $500)
    app->settleExpense(trip->groupId, david->userId, charlie->userId, 300.0);
    
    cout << "\n--- 7. SHOWING GROUP BALANCES AFTER SETTLEMENT ---\n";
    app->showGroupBalances(trip->groupId);

    return 0;
}