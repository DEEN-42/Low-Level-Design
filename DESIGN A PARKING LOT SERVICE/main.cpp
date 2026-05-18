#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
#define int long long

enum class VehicleType {
    Car,
    Bike,
    Truck
};
enum class ParkingFeeStrategyType {
    Standard,
    Premium
};

//abstract vehicle class
class IVehicle {
public:
    string licensePlate;
    VehicleType type;
    ParkingFeeStrategyType parkingFeeStrategyType;
    virtual ~IVehicle() {};
};    

class Car : public IVehicle {
public:
    Car(const string& plate, ParkingFeeStrategyType strategyType = ParkingFeeStrategyType::Standard) {
        licensePlate = plate;
        type = VehicleType::Car;
        parkingFeeStrategyType = strategyType;
    }
};
class Bike : public IVehicle {
public:    
    Bike(const string& plate, ParkingFeeStrategyType strategyType = ParkingFeeStrategyType::Standard) {
        licensePlate = plate;
        type = VehicleType::Bike;
        parkingFeeStrategyType = strategyType;
    }
};

class Truck : public IVehicle {
public:
    Truck(const string& plate, ParkingFeeStrategyType strategyType = ParkingFeeStrategyType::Standard) {
        licensePlate = plate;
        type = VehicleType::Truck;
        parkingFeeStrategyType = strategyType;
    }
};

//Parking spot interface
class IParkingSpot {
public:
    int spotId;
    bool isOccupied;
    string vehicleType;
    virtual ~IParkingSpot() {};
};

class BikeParkingSpot : public IParkingSpot {
public:
    BikeParkingSpot(int id) {
        spotId = id;
        isOccupied = false;
        vehicleType = "Bike";
    }
};

class CarParkingSpot : public IParkingSpot {
public:
    CarParkingSpot(int id) {
        spotId = id;
        isOccupied = false;
        vehicleType = "Car";
    }
};
class TruckParkingSpot : public IParkingSpot {
public:
    TruckParkingSpot(int id) {
        spotId = id;
        isOccupied = false;
        vehicleType = "Truck";
    }
};

class Ticket{
public:
    string TicketId;
    IVehicle* vehicle;
    IParkingSpot* parkingSpot;

    Ticket(IVehicle* v, IParkingSpot* spot) : TicketId("TICKET_" + to_string(rand())), vehicle(v), parkingSpot(spot) {}
};



//parking strategy interface
class IParkingFeeStrategy {
public:
    virtual double calculateFee(const IVehicle& vehicle, int hours) = 0;
    virtual ~IParkingFeeStrategy() = default;
};

class StandardParkingFeeStrategy : public IParkingFeeStrategy {
public:
    double calculateFee(const IVehicle& vehicle, int hours) override {
        if (vehicle.type == VehicleType::Car) {
            return hours * 2.0;
        } else if (vehicle.type == VehicleType::Bike) {
            return hours * 1.0;
        } else if (vehicle.type == VehicleType::Truck) {
            return hours * 3.0;
        }
        return 0.0;
    }
};

class PremiumParkingFeeStrategy : public IParkingFeeStrategy {
public:
    double calculateFee(const IVehicle& vehicle, int hours) override {
        if (vehicle.type == VehicleType::Car) {
            return hours * 3.0;
        } else if (vehicle.type == VehicleType::Bike) {
            return hours * 1.5;
        } else if (vehicle.type == VehicleType::Truck) {
            return hours * 4.0;
        }
        return 0.0;
    }
};

//payment strategy interface
class IPaymentStrategy {
public:
    virtual void pay(double amount) = 0;
    virtual ~IPaymentStrategy() = default;
};

class CreditCardPayment : public IPaymentStrategy {
private: string cardNumber;
public:
    CreditCardPayment(const string& card) : cardNumber(card) {}

    void pay(double amount) override {
        cout << "Paying $" << amount << " with credit card: " << cardNumber << endl;
    }
};

class UPIPayment : public IPaymentStrategy {
private: string upiId;
public:
    UPIPayment(const string& upi) : upiId(upi) {}

    void pay(double amount) override {
        cout << "Paying $" << amount << " with UPI payment: " << upiId << endl;
    }
};

class CashPayment : public IPaymentStrategy {
public:
    void pay(double amount) override {
        cout << "Paying $" << amount << " with cash." << endl;
    }
};

//vehicle factory for creating different types of vehicles
class VehicleFactory {
public:
    static IVehicle* createVehicle(const string& type, const string& licensePlate, ParkingFeeStrategyType strategyType = ParkingFeeStrategyType::Standard) {
        if (type == "Car") {
            return new Car(licensePlate, strategyType);
        } else if (type == "Bike") {
            return new Bike(licensePlate, strategyType);
        } else if (type == "Truck") {
            return new Truck(licensePlate, strategyType);
        }
        return nullptr;
    }
};

//Parking Lot class(singleton class)
class ParkingLot{
private:
    static ParkingLot* instance;
    vector<IParkingSpot*> parkingSpots;
public:

    //create parking lot with given number of spots for each type of vehicle
    static void createParkingLot(int carSpots, int bikeSpots, int truckSpots) {
        if (instance == nullptr) {
            instance = new ParkingLot();
            for (int i = 0; i < carSpots; i++) {
                instance->parkingSpots.push_back(new CarParkingSpot(i));
            }
            for (int i = 0; i < bikeSpots; i++) {
                instance->parkingSpots.push_back(new BikeParkingSpot(i + carSpots));
            }
            for (int i = 0; i < truckSpots; i++) {
                instance->parkingSpots.push_back(new TruckParkingSpot(i + carSpots + bikeSpots));
            }
        }
    }

    //check if parking available for a given type of vehicle
    bool isParkingAvailable(VehicleType type) {
        for (auto spot : parkingSpots) {
            if (!spot->isOccupied && spot->vehicleType == (type == VehicleType::Car ? "Car" : type == VehicleType::Bike ? "Bike" : "Truck")) {
                return true;
            }
        }
        return false;
    }

    //park a vehicle in the parking lot
    Ticket* parkVehicle(IVehicle* vehicle) {
        for (auto spot : parkingSpots) {
            if (!spot->isOccupied && spot->vehicleType == (vehicle->type == VehicleType::Car ? "Car" : vehicle->type == VehicleType::Bike ? "Bike" : "Truck")) {
                spot->isOccupied = true;
                cout << "Vehicle with license plate " << vehicle->licensePlate << " parked in spot " << spot->spotId << endl;
                return new Ticket(vehicle, spot);
            }
        }
        cout << "No parking available for vehicle with license plate " << vehicle->licensePlate << endl;
        return nullptr;
    }

    //unpark a vehicle from the parking lot
    bool unparkVehicle(Ticket* ticket,int hoursParked, IPaymentStrategy* paymentStrategy) {
        //check ticket
        if (ticket == nullptr) {
            cout << "Invalid ticket." << endl;
            return false;
        }
        //if already unparked
        if (!ticket->parkingSpot->isOccupied) {
            cout << "Vehicle with license plate " << ticket->vehicle->licensePlate << " is already unparked." << endl;
            return false;
        }
        
        //calculate parking fee
        IParkingFeeStrategy* feeStrategy = nullptr;
        if (ticket->vehicle->parkingFeeStrategyType == ParkingFeeStrategyType::Standard) {
            feeStrategy = new StandardParkingFeeStrategy();
        } else if (ticket->vehicle->parkingFeeStrategyType == ParkingFeeStrategyType::Premium) {
            feeStrategy = new PremiumParkingFeeStrategy();
        }
        double parkingFee = feeStrategy->calculateFee(*(ticket->vehicle), hoursParked);
        
        //process payment
        cout << "Parking fee for vehicle with license plate " << ticket->vehicle->licensePlate << " is $" << parkingFee << endl;
        paymentStrategy->pay(parkingFee);
        
        //unpark vehicle
        ticket->parkingSpot->isOccupied = false;
        cout << "Vehicle with license plate " << ticket->vehicle->licensePlate << " unparked from spot " << ticket->parkingSpot->spotId << endl;
        
        delete feeStrategy;
        return true;
    }

    //get the singleton instance of the parking lot
    static ParkingLot* getInstance(){
        return instance;
    };
};



//initialize static member of ParkingLot class
ParkingLot* ParkingLot::instance = nullptr;

signed main() {
    srand(time(0)); // Seed for random ticket ID generation

    // Create parking lot with 2 car spots, 2 bike spots, and 1 truck spot
    ParkingLot::createParkingLot(2, 2, 1);
    ParkingLot* parkingLot = ParkingLot::getInstance();

    IVehicle* car1 = VehicleFactory::createVehicle("Car", "CAR123", ParkingFeeStrategyType::Standard);

    //park the vehicle
    Ticket* ticket1 = parkingLot->parkVehicle(car1);

    //unpark the vehicle after 3 hours with credit card payment
    IPaymentStrategy* paymentStrategy = new CreditCardPayment("1234-5678-9012-3456");
    parkingLot->unparkVehicle(ticket1, 3, paymentStrategy);
    
    delete paymentStrategy;
    delete ticket1;
    delete car1;
    delete parkingLot;
    return 0;
}