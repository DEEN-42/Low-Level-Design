#include <bits/stdc++.h>
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
    int itemPrice;

    //constructor
    Item(string name, int price){
        itemName = name;
        itemPrice = price;
    }
};

//Inventory
class Inventory{
public:
    vector<pair<Item, int>> items;

    //constructor
    Inventory(){
        items = vector<pair<Item, int>>();
    }
    //add item to inventory
    void addItem(Item item, int quantity){
        items.push_back(make_pair(item, quantity));
    }
    //get item by name
    Item getItem(string name){
        for(auto& item : items){
            if(item.first.itemName == name){
                return item.first;
            }
        }
        // Return a default Item if not found
        return Item("Not Found", 0);
    }
    //get quantity of item by name
    int getQuantity(string name){
        for(auto& item : items){
            if(item.first.itemName == name){
                return item.second;
            }
        }
        return 0; // Return 0 if not found
    }
    
    //get list of items
    vector<Item> getItems(){
        vector<Item> itemList;
        for(auto& item : items){
            itemList.push_back(item.first);
        }
        return itemList;
    }

    //reduce quantity of item by name
    void reduceQuantity(string name){
        for(auto& item : items){
            if(item.first.itemName == name){
                item.second--;
                return;
            }
        }
    }

    //update quantity of item by name
    void updateQuantity(string name, int quantity){
        for(auto& item : items){
            if(item.first.itemName == name){
                item.second = quantity;
                return;
            }
        }
    }

    //check if all out of stock
    bool isOutOfStock(){
        for(auto& item : items){
            if(item.second > 0){
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
    virtual void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) = 0;
    virtual void openPanel(VendingMachine* machine) = 0;
    virtual void closePanel(VendingMachine* machine) = 0;
    virtual void cancelTransaction(VendingMachine* machine) = 0;
};

//states 
class NoCoinState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
    void cancelTransaction(VendingMachine* machine) override;
};

class HasCoinState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
    void cancelTransaction(VendingMachine* machine) override;
};

class DispensingState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
    void cancelTransaction(VendingMachine* machine) override;
};

class OutOfStockState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
    void cancelTransaction(VendingMachine* machine) override;
};

class MaintenanceState : public VendingMachineState{
public:
    void insertCoin(VendingMachine* machine, Coin coin) override;
    void selectItem(VendingMachine* machine, string itemName) override;
    void dispenseItem(VendingMachine* machine) override;
    void restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems) override;
    void openPanel(VendingMachine* machine) override;
    void closePanel(VendingMachine* machine) override;
    void cancelTransaction(VendingMachine* machine) override;
};

//Vending Machine(context class)
class VendingMachine{
public:
    Inventory inventory;
    int currentBalance;
    string currentItem;
    VendingMachineState* state;

    //to be initialized later
    VendingMachineState* noCoinState ;
    VendingMachineState* hasCoinState ;
    VendingMachineState* dispensingState ;
    VendingMachineState* outOfStockState ;
    VendingMachineState* maintenanceState ;
    //constructor
    VendingMachine();
    ~VendingMachine();

    //get balance
    int getBalance(){
        return currentBalance;
    }

    //update balance
    void updateBalance(int amount){
        currentBalance += amount;
    }

    //reset balance
    void resetBalance(){
        currentBalance = 0;
    }

    void insertCoin(Coin coin){
        state->insertCoin(this, coin);
    }
    void selectItem(string itemName){
        state->selectItem(this, itemName);
    }
    void dispenseItem(){
        state->dispenseItem(this);
    }
    void restockItems(vector<pair<Item, int>> newItems){
        state->restockItems(this, newItems);
    }
    void openPanel(){
        state->openPanel(this);
    }
    void closePanel(){
        state->closePanel(this);
    }
    void cancelTransaction(){
        state->cancelTransaction(this);
    }
};

//Vending Machine Constructor & Destructor
VendingMachine::VendingMachine(){
    currentBalance = 0;
    currentItem = "";
    noCoinState = (VendingMachineState*) new class NoCoinState();
    hasCoinState = (VendingMachineState*) new class HasCoinState();
    dispensingState = (VendingMachineState*) new class DispensingState();
    outOfStockState = (VendingMachineState*) new class OutOfStockState();
    maintenanceState = (VendingMachineState*) new class MaintenanceState();
    if(inventory.isOutOfStock()){
        state = outOfStockState;
    }else{
        state = noCoinState;
    }
}

VendingMachine::~VendingMachine(){
    delete noCoinState;
    delete hasCoinState;
    delete dispensingState;
    delete outOfStockState;
    delete maintenanceState;
}

//NoCoinState methods
void NoCoinState::insertCoin(VendingMachine* machine, Coin coin){
    machine->updateBalance(coin);
    cout << "Inserted coin: " << coin << ". Current balance: " << machine->getBalance() << endl;
    machine->state = machine->hasCoinState;
}
void NoCoinState::selectItem(VendingMachine* machine, string itemName){
    cout << "Please insert coin first." << endl;
}
void NoCoinState::dispenseItem(VendingMachine* machine){
    cout << "Please insert coin first." << endl;
}
void NoCoinState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Restocking can only happen during Maintenance state." << endl;
}
void NoCoinState::openPanel(VendingMachine* machine){
    cout << "Opening maintenance panel. Entering Maintenance State." << endl;
    machine->state = machine->maintenanceState;
}
void NoCoinState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}
void NoCoinState::cancelTransaction(VendingMachine* machine){
    cout << "No transaction to cancel." << endl;
}

//HasCoinState methods
void HasCoinState::insertCoin(VendingMachine* machine, Coin coin){
    machine->updateBalance(coin);
    cout << "Inserted Additional coin: " << coin << ". Current balance: " << machine->getBalance() << endl;
}
void HasCoinState::selectItem(VendingMachine* machine, string itemName){
    Item item = machine->inventory.getItem(itemName);
    if(item.itemName == "Not Found"){
        cout << "Item not found." << endl;
        return;
    }
    if(machine->inventory.getQuantity(itemName) <= 0){
        cout << "Item out of stock." << endl;
        return;
    }
    if(machine->getBalance() < item.itemPrice){
        cout << "Insufficient balance. Please insert more coins." << endl;
        return;
    }

    machine->currentItem = itemName;
    machine->state = machine->dispensingState;
    machine->dispenseItem();
}
void HasCoinState::dispenseItem(VendingMachine* machine){
    cout << "Please select item first." << endl;
}
void HasCoinState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Restocking can only happen during Maintenance state." << endl;
}
void HasCoinState::openPanel(VendingMachine* machine){
    cout << "Cannot open maintenance panel while money is inserted. Please complete transaction or cancel." << endl;
}
void HasCoinState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}
void HasCoinState::cancelTransaction(VendingMachine* machine){
    cout << "Transaction cancelled. Refunding balance: " << machine->getBalance() << endl;
    machine->resetBalance();
    machine->state = machine->noCoinState;
}

//DispensingState methods
void DispensingState::insertCoin(VendingMachine* machine, Coin coin){
    cout << "Please wait, dispensing item." << endl;
}
void DispensingState::selectItem(VendingMachine* machine, string itemName){
    cout << "Please wait, dispensing item." << endl;
}
void DispensingState::dispenseItem(VendingMachine* machine){
    cout << "Dispensing item: " << machine->currentItem << endl;
    machine->inventory.reduceQuantity(machine->currentItem);

    // Calculate change
    Item item = machine->inventory.getItem(machine->currentItem);
    int change = machine->getBalance() - item.itemPrice;
    if(change > 0){
        cout << "Returning change: " << change << endl;
    }
    machine->resetBalance();
    machine->currentItem = "";

    if(machine->inventory.isOutOfStock()){
        cout << "Machine is now out of stock." << endl;
        machine->state = machine->outOfStockState;
    }else{
        machine->state = machine->noCoinState;
    }
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
void DispensingState::cancelTransaction(VendingMachine* machine){
    cout << "Cannot cancel. Already dispensing." << endl;
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
void OutOfStockState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    cout << "Denied: Please open the maintenance panel first to restock." << endl;
}
void OutOfStockState::openPanel(VendingMachine* machine){
    cout << "Opening maintenance panel. Entering Maintenance State." << endl;
    machine->state = machine->maintenanceState;
}
void OutOfStockState::closePanel(VendingMachine* machine){
    cout << "Panel is already closed." << endl;
}
void OutOfStockState::cancelTransaction(VendingMachine* machine){
    cout << "No transaction to cancel." << endl;
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
void MaintenanceState::restockItems(VendingMachine* machine, vector<pair<Item, int>> newItems){
    for(auto& pair : newItems){
        Item existingItem = machine->inventory.getItem(pair.first.itemName);
        if(existingItem.itemName != "Not Found"){
            // Item exists, update quantity
            int previousQty = machine->inventory.getQuantity(pair.first.itemName);
            machine->inventory.updateQuantity(pair.first.itemName, previousQty + pair.second);
            cout << "Restocked existing item: " << pair.first.itemName << " (Added: " << pair.second << " unit(s))" << endl;
        } else {
            // New item
            machine->inventory.addItem(pair.first, pair.second);
            cout << "Added new item: " << pair.first.itemName << " (Quantity: " << pair.second << ")" << endl;
        }
    }
}
void MaintenanceState::openPanel(VendingMachine* machine){
    cout << "Panel is already open." << endl;
}
void MaintenanceState::closePanel(VendingMachine* machine){
    cout << "Closing maintenance panel." << endl;
    if(machine->inventory.isOutOfStock()){
        machine->state = machine->outOfStockState;
    } else {
        machine->state = machine->noCoinState;
    }
}
void MaintenanceState::cancelTransaction(VendingMachine* machine){
    cout << "No transaction to cancel in Maintenance mode." << endl;
}

int main() {
    VendingMachine machine;

    // Adding some initial items to the machine
    machine.openPanel();
    machine.restockItems({{Item("Soda", 2), 10}, {Item("Chips", 10), 5}, {Item("Candy", 5), 20}});
    machine.closePanel();

    // Simulating user interactions
    machine.insertCoin(Coin::mediumcoin);
    machine.insertCoin(Coin::smallcoin);
    machine.selectItem("Soda");

    machine.insertCoin(Coin::largecoin);
    machine.selectItem("Chips");

    machine.insertCoin(Coin::xlargecoin);
    machine.selectItem("Candy");

    // Simulating maintenance
    machine.openPanel();
    machine.restockItems({{Item("Soda", 1.5), 5}, {Item("Chips", 1.0), 10}});
    machine.closePanel();

    return 0;
}