#include <bits/stdc++.h>
using namespace std;
#define endl '\n'
#define int long long

//Notifications and Decorators

class INotification {
public:
    virtual string getContent() const = 0;

    virtual ~INotification() {};
};

// Concrete Notification:simple text notification
class SimpleNotification : public INotification {
private:
    string txt;
public:
    SimpleNotification(const string& msg) : txt(msg) {}

    string getContent() const override {
        return txt;
    }
};

//abstract Decorator:wraps an INotification object
class INotificationDecorator : public INotification {
protected:
    INotification* notification;
public:
    INotificationDecorator(INotification* msg) : notification(msg) {}

    virtual ~INotificationDecorator() {
        delete notification;
    }
};

// Concrete Decorator: adds a timestamp to the notification content
class TimestampDecorator : public INotificationDecorator {
public:
    TimestampDecorator(INotification* msg) : INotificationDecorator(msg) {}

    string getContent() const override {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        string timeStr = std::ctime(&time);
        timeStr.pop_back(); 
        return timeStr + ": " + notification->getContent();
    }
};

// Concrete Decorator: appends a signature to the notification content
class SignatureDecorator : public INotificationDecorator {
private:
    string signature;
public:
    SignatureDecorator(INotification* msg, const string& sig) : INotificationDecorator(msg), signature(sig) {}

    string getContent() const override {
        return notification->getContent() + "\n-- " + signature;
    }
};

//Observer Pattern Component

//observer interface
class IObserver {
public:
    virtual void update() = 0;

    virtual ~IObserver() {};
};

class IObservable {
public:
    virtual void addObserver(IObserver* observer) = 0;
    virtual void removeObserver(IObserver* observer) = 0;
    virtual void notifyObservers() = 0;
    virtual ~IObservable() {};
};

//Concrete Observable: manages a list of observers and notifies them of changes
class NotificationObservable : public IObservable {
private:
    vector<IObserver*> observers;
    INotification* currentNotification;
public:
    NotificationObservable() : currentNotification(nullptr) {}

    void addObserver(IObserver* observer) override {
        observers.push_back(observer);
    }

    void removeObserver(IObserver* observer) override {
        observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notifyObservers() override {
        for (auto observer : observers) {
            observer->update();
        }
    }

    void setNotification(INotification* notification) {
        currentNotification = notification;
        notifyObservers();
    }

    INotification* getNotification() const {
        return currentNotification;
    }

    string getCurrentNotificationContent() const {
        return currentNotification ? currentNotification->getContent() : "No notification";
    }

    ~NotificationObservable() {
    }
};

//Concrete Observer 1
class Logger: public IObserver {
private:
    NotificationObservable* notificationObservable;
public:
    Logger();

    Logger(NotificationObservable* observable) {
        this->notificationObservable = observable;
    }
    
    void update(){
        cout << "Logger: New notification received: " << notificationObservable->getCurrentNotificationContent() << endl;
    }
};

//Abstract class for different notification strategies
class NotificationStrategy{
public:
    virtual void sendNotification(const string& message) = 0;
    virtual ~NotificationStrategy() = default;

};

class EmailStrategy : public NotificationStrategy {
private: 
    string emailId;
public:
    EmailStrategy(const string& email) : emailId(email) {}

    void sendNotification(const string& message) override {
        cout << "Sending email to " << emailId << ": " << message << endl;
    }
};

class SMSStrategy : public NotificationStrategy {
private: 
    string phoneNumber;
public: 
    SMSStrategy(const string& phone) : phoneNumber(phone) {}

    void sendNotification(const string& message) override {
        cout << "Sending SMS to " << phoneNumber << ": " << message << endl;
    }
};

class PopUpStrategy : public NotificationStrategy {
public:
    void sendNotification(const string& message) override {
        cout << "Displaying pop-up notification: " << message << endl;
    }
};

class NotificationEngine: public IObserver {
private:
    NotificationObservable* notificationObservable;
    vector<NotificationStrategy*> strategies;
public:
    NotificationEngine();

    NotificationEngine(NotificationObservable* observable) {
        this->notificationObservable = observable;
    }
    void addStrategy(NotificationStrategy* strategy) {
        this->strategies.push_back(strategy);
    }

    void update(){
        string notificationContent = notificationObservable->getCurrentNotificationContent();
        for (auto strategy : strategies) {
            strategy->sendNotification(notificationContent);
        }
    }
    ~NotificationEngine() {
        for (auto strategy : strategies) {
            delete strategy;
        }
    }
};

//Notification Service: manages the creation and sending of notifications
//Singleton class
class NotificationService {
private:
    NotificationObservable* observable;
    static NotificationService* instance;
    vector<INotification*> notifications;

    NotificationService() {
        observable = new NotificationObservable();
    }
public:
    static NotificationService* getInstance() {
        if (instance == nullptr) {
            instance = new NotificationService();
        }
        return instance;
    }

    NotificationObservable* getObservable() const {
        return observable;
    }

    void sendNotification(INotification* notification) {
        notifications.push_back(notification);
        observable->setNotification(notification);
    }

    ~NotificationService() {
        delete observable;
        for (auto notification : notifications) {
            delete notification;
        }
    }
};

NotificationService* NotificationService::instance = nullptr;

Logger::Logger() {
    this->notificationObservable = NotificationService::getInstance()->getObservable();
    notificationObservable->addObserver(this);
}

NotificationEngine::NotificationEngine() {
    this->notificationObservable = NotificationService::getInstance()->getObservable();
    notificationObservable->addObserver(this);
}

signed main() {
    //Create notification service and observers
    NotificationService* service = NotificationService::getInstance();

    Logger* logger = new Logger();
    NotificationEngine* engine = new NotificationEngine();

    //Add strategies to the notification engine
    engine->addStrategy(new EmailStrategy("user@example.com"));
    engine->addStrategy(new SMSStrategy("123-456-7890"));
    engine->addStrategy(new PopUpStrategy());

    //Send a notification with decorators
    INotification* notification = new SimpleNotification("Hello, this is a test notification!");
    notification = new TimestampDecorator(notification);
    notification = new SignatureDecorator(notification, "Best regards, Notification Service");
    service->sendNotification(notification);

    // Clean up
    delete logger;
    delete engine;
    delete service;

    return 0;
}