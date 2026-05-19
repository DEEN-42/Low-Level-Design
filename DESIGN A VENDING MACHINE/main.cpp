#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;

#define endl '\n'

//enums for Coin
enum Coin{
    smallcoin = 1,
    mediumcoin = 2,
    largecoin = 5,
    xlargecoin = 10
};

//Item
class Item{
public:
    string itemName;
    double itemPrice;

    //constructor
    Item(string name = "Not Found", double price = 0.0){
        itemName = name;
        itemPrice = price;
    }
};

//Inventory
class Inventory{
private:
    unordered_map<string, pair<Item, int>> items; //map of itemName to pair<Item, quantity>

public:
    //constructor
    Inventory(){}

    //add item to inventory
    void addItem(Item item, int quantity){
        items[item.itemName] = make_pair(item, quantity);
    }
    //get item by name
    Item getItem(string name){
        if(items.find(name) != items.end()){
            return items[name].first;
        }
        return Item("Not Found", 0.0);
    }
    //get quantity of item by name
    int getQuantity(string name){
        auto it = items.find(name);
        if(it != items.end()){
            return it->second.second;
        }
        return 0; // Return 0 if not found
    }
    
    //get list of items
    vector<Item> getItems(){
        vector<Item> itemList;
        for(auto& pair : items){
            itemList.push_back(pair.second.first);
        }
        return itemList;
    }

    //reduce quantity of item by name
    void reduceQuantity(string name){
        auto it = items.find(name);
        if(it != items.end() && it->second.second > 0){
            it->second.second--;
        }
    }

    //update quantity of item by name
    void updateQuantity(string name, int quantity){
        auto it = items.find(name);
        if(it != items.end()){
            it->second.second = quantity;
        }
    }

    //check if all out of stock
    bool isOutOfStock(){
        for(auto& pair : items){
            if(pair.second.second > 0){
                return false;
            }
        }
        return true;
    }
};

//Vending Machine State Enum
enum VendingStateEnum{
    NoCoinStateEnum,
    HasCoinStateEnum,
    DispensingStateEnum,
    OutOfStockStateEnum,
    MaintenanceStateEnum
};

//forward declaration of VendingMachine so states can use it
class VendingMachine;

//state interface
class VendingMachineState{
public:
    virtual ~VendingMachineState() = default;
    virtual void insertCoin(VendingMachine* machine, Coin coin) = 0;
    virtual void selectItem(VendingMachine* machine, string itemName) = 0;
    virtual void dispenseItem(VendingMachine* machine) = 0;
    virtual void refund(VendingMachine* machine) = 0;
    virtual void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) = 0;
    virtual void openPanel(VendingMachine* machine) = 0;
    virtual void closePanel(VendingMachine* machine) = 0;
};

//states 
class NoCoinState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
};

class HasCoinState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
};

class DispensingState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
};

class OutOfStockState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
};

class MaintenanceState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void refund(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
};

//Vending Machine(context class)
class VendingMachine{
private:
    Inventory inventory;
    double currentBalance;
    string currentItem;
    VendingMachineState* state;

    unique_ptr<NoCoinState> noCoinState;
    unique_ptr<HasCoinState> hasCoinState;
    unique_ptr<DispensingState> dispensingState;
    unique_ptr<OutOfStockState> outOfStockState;
    unique_ptr<MaintenanceState> maintenanceState;

public:
    //constructor
    VendingMachine();
    ~VendingMachine() = default;

    //getters and setters
    double getBalance(){ return currentBalance; }
    void updateBalance(double amount){ currentBalance += amount; }
    void resetBalance(){ currentBalance = 0.0; }
    
    Inventory& getInventory() { return inventory; }
    string getCurrentItem() { return currentItem; }
    void setCurrentItem(string item) { currentItem = item; }
    
    // Changing states
    void setState(VendingMachineState* newState) { state = newState; }

    VendingMachineState* getNoCoinState() { return noCoinState.get(); }
    VendingMachineState* getHasCoinState() { return hasCoinState.get(); }
    VendingMachineState* getDispensingState() { return dispensingState.get(); }
    VendingMachineState* getOutOfStockState() { return outOfStockState.get(); }
    VendingMachineState* getMaintenanceState() { return maintenanceState.get(); }

    void insertCoin(Coin coin){ state->insertCoin(this, coin); }
    void selectItem(string itemName){ state->selectItem(this, itemName); }
    void dispenseItem(){ state->dispenseItem(this); }
    void refund(){ state->refund(this); }
    void restockItems(vector<pair<Item, int>> newItems){ state->restockItems(this, newItems); }
    void openPanel(){ state->openPanel(this); }
    void closePanel(){ state->closePanel(this); }
};

//Vending Machine Constructor
VendingMachine::VendingMachine(){
    currentBalance = 0.0;
    currentItem = "";
    noCoinState = make_unique<NoCoinState>();
    hasCoinState = make_unique<HasCoinState>();
    dispensingState = make_unique<DispensingState>();
    outOfStockState = make_unique<OutOfStockState>();
    maintenanceState = make_unique<MaintenanceState>();
    
    if(inventory.isOutOfStock()){
        state = outOfStockState.get();
    }else{
        state = noCoinState.get();
    }
}

//NoCoinState methods
void NoCoinState::insertCoin(VendingMachine* machine, Coin coin){
    machine->updateBalance(coin);
    cout << "Inserted coin: " << coin << ". Current balance: " << machine->getBalance() << endl;
    machine->setState(machine->getHasCoinState());
}
void NoCoinState::selectItem(VendingMachine* machine, string itemName){
    cout << "Please insert coin first." << endl;
}
void NoCoinState::dispenseItem(VendingMachine* machine){
    cout << "Please insert coin first." << endl;
}
void NoCoinState::refund(VendingMachine* machine){
    cout << "No coins to refund." << endl;
}
void NoCoinState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Restocking can only happen during Maintenance state." << endl;
}
void NoCoinState::openPanel(VendingMachine* machine){
    cout << "Opening maintenance panel. Entering Maintenance State." << endl;
    machine->setState(machine->getMaintenanceState());
}
void NoCoinState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}

//HasCoinState methods
void HasCoinState::insertCoin(VendingMachine* machine, Coin coin){
    machine->updateBalance(coin);
    cout << "Inserted Additional coin: " << coin << ". Current balance: " << machine->getBalance() << endl;
}
void HasCoinState::selectItem(VendingMachine* machine, string itemName){
    Item item = machine->getInventory().getItem(itemName);
    if(item.itemName == "Not Found"){
        cout << "Item not found." << endl;
        return;
    }
    if(machine->getInventory().getQuantity(itemName) <= 0){
        cout << "Item out of stock." << endl;
        return;
    }
    if(machine->getBalance() < item.itemPrice){
        cout << "Insufficient balance. Please insert more coins." << endl;
        return;
    }

    machine->setCurrentItem(itemName);
    machine->setState(machine->getDispensingState());
    machine->dispenseItem();
}
void HasCoinState::dispenseItem(VendingMachine* machine){
    cout << "Please select item first." << endl;
}
void HasCoinState::refund(VendingMachine* machine){
    cout << "Refunding: " << machine->getBalance() << " coins." << endl;
    machine->resetBalance();
    machine->setState(machine->getNoCoinState());
}
void HasCoinState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Restocking can only happen during Maintenance state." << endl;
}
void HasCoinState::openPanel(VendingMachine* machine){
    cout << "Cannot open maintenance panel while money is inserted. Please complete transaction or refund." << endl;
}
void HasCoinState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}

//DispensingState methods
void DispensingState::insertCoin(VendingMachine* machine, Coin coin){
    cout << "Please wait, dispensing item." << endl;
}
void DispensingState::selectItem(VendingMachine* machine, string itemName){
    cout << "Please wait, dispensing item." << endl;
}
void DispensingState::dispenseItem(VendingMachine* machine){
    string curItem = machine->getCurrentItem();
    cout << "Dispensing item: " << curItem << endl;
    machine->getInventory().reduceQuantity(curItem);

    // Calculate change
    Item item = machine->getInventory().getItem(curItem);
    double change = machine->getBalance() - item.itemPrice;
    if(change > 0){
        cout << "Returning change: " << change << endl;
    }
    machine->resetBalance();
    machine->setCurrentItem("");

    if(machine->getInventory().isOutOfStock()){
        cout << "Machine is now out of stock." << endl;
        machine->setState(machine->getOutOfStockState());
    }else{
        machine->setState(machine->getNoCoinState());
    }
}

void DispensingState::refund(VendingMachine* machine){
    cout << "Cannot refund while dispensing." << endl;
}
void DispensingState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Restocking can only happen during Maintenance state." << endl;
}
void DispensingState::openPanel(VendingMachine* machine){
    cout << "Cannot open maintenance panel while dispensing." << endl;
}
void DispensingState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}


//OutOfStockState methods
void OutOfStockState::insertCoin(VendingMachine* machine, Coin coin){
    cout << "Machine is out of stock. Please wait for restocking." << endl;
}
void OutOfStockState::selectItem(VendingMachine* machine, string itemName){
    cout << "Machine is out of stock. Please wait for restocking." << endl;
}
void OutOfStockState::dispenseItem(VendingMachine* machine){
    cout << "Machine is out of stock. Please wait for restocking." << endl;
}
void OutOfStockState::refund(VendingMachine* machine){
    cout << "No coins to refund." << endl;
}
void OutOfStockState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Please open the maintenance panel first to restock." << endl;
}
void OutOfStockState::openPanel(VendingMachine* machine){
    cout << "Opening maintenance panel. Entering Maintenance State." << endl;
    machine->setState(machine->getMaintenanceState());
}
void OutOfStockState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}

//MaintenanceState methods
void MaintenanceState::insertCoin(VendingMachine* machine, Coin coin){
    cout << "Temporarily disabled. Machine is in Maintenance mode." << endl;
}
void MaintenanceState::selectItem(VendingMachine* machine, string itemName){
    cout << "Temporarily disabled. Machine is in Maintenance mode." << endl;
}
void MaintenanceState::dispenseItem(VendingMachine* machine){
    cout << "Temporarily disabled. Machine is in Maintenance mode." << endl;
}
void MaintenanceState::refund(VendingMachine* machine){
    cout << "No transaction to refund in Maintenance mode." << endl;
}
void MaintenanceState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    for(auto& pair : newItems){
        Item existingItem = machine->getInventory().getItem(pair.first.itemName);
        if(existingItem.itemName != "Not Found"){
            // Item exists, update quantity
            int previousQty = machine->getInventory().getQuantity(pair.first.itemName);
            machine->getInventory().updateQuantity(pair.first.itemName, previousQty + pair.second);
            cout << "Restocked existing item: " << pair.first.itemName << " (Added: " << pair.second << " unit(s))" << endl;
        } else {
            // New item
            machine->getInventory().addItem(pair.first, pair.second);
            cout << "Added new item: " << pair.first.itemName << " (Quantity: " << pair.second << ")" << endl;
        }
    }
}
void MaintenanceState::openPanel(VendingMachine* machine){
    cout << "Panel is already open." << endl;
}
void MaintenanceState::closePanel(VendingMachine* machine){
    cout << "Closing maintenance panel." << endl;
    if(machine->getInventory().isOutOfStock()){
        machine->setState(machine->getOutOfStockState());
    } else {
        machine->setState(machine->getNoCoinState());
    }
}

int main() {
    VendingMachine machine;

    // Adding some initial items to the machine
    machine.openPanel();
    machine.restockItems({{Item("Soda", 2.0), 10}, {Item("Chips", 1.5), 5}, {Item("Candy", 0.5), 20}});
    machine.closePanel();

    // Simulating user interactions
    machine.insertCoin(Coin::mediumcoin);
    machine.insertCoin(Coin::smallcoin);
    machine.selectItem("Soda"); // Not enough money

    machine.refund(); // New refund feature!

    machine.insertCoin(Coin::largecoin);
    machine.selectItem("Chips");

    return 0;
}
