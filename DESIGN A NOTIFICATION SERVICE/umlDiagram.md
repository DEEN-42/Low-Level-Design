classDiagram
    direction TB

    %% ── TOP: Singleton ──
    class NotificationService {
        <<Singleton>>
        -NotificationObservable* observable
        -static NotificationService* instance
        -vector~INotification*~ notifications
        -NotificationService()
        +getInstance() NotificationService*
        +getObservable() NotificationObservable*
        +sendNotification(notification: INotification*) void
    }

    %% ── LEFT: INotification tree ──
    class INotification {
        <<abstract>>
        +getContent() string
    }

    class SimpleNotification {
        -string txt
        +getContent() string
    }

    class INotificationDecorator {
        <<abstract>>
        -INotification* notif
    }

    class TimestampDecorator {
        -INotification* notif
        +getContent() string
    }

    class SignatureDecorator {
        -INotification* notif
        +getContent() string
    }

    %% ── CENTER: Observable ──
    class IObservable {
        <<abstract>>
        +add(observer: IObserver*) void
        +remove(observer: IObserver*) void
        +notify() void
    }

    class NotificationObservable {
        -INotification* notif
        +add(observer: IObserver*) void
        +remove(observer: IObserver*) void
        +notify() void
        +setNotification(n: INotification*) void
        +getNotification() INotification*
    }

    %% ── RIGHT: Observer tree ──
    class IObserver {
        <<abstract>>
        +update() void
    }

    class Logger {
        -NotificationObservable* no
        +update() void
    }

    class NotificationEngine {
        -NotificationObservable* no
        -vector~NotificationStrategy*~ nls
        +addStrategy(s: NotificationStrategy*) void
        +update() void
    }

    %% ── BOTTOM: Strategy tree ──
    class NotificationStrategy {
        <<abstract>>
        +sendNotification(content: string) void
    }

    class EmailStrategy {
        +sendNotification(content: string) void
    }

    class SMSStrategy {
        +sendNotification(content: string) void
    }

    class PopUpStrategy {
        +sendNotification(content: string) void
    }

    %% ── RELATIONSHIPS ──

    %% Singleton owns Observable (top → center)
    NotificationService "1" --> "1" NotificationObservable : owns
    NotificationService "1" o--> "1..*" INotification : stores

    %% INotification left hierarchy
    INotification <|.. SimpleNotification
    INotification <|.. INotificationDecorator
    INotificationDecorator <|-- TimestampDecorator
    INotificationDecorator <|-- SignatureDecorator
    INotificationDecorator o--> INotification : wraps

    %% Observable center hierarchy
    IObservable <|.. NotificationObservable

    %% Observer right hierarchy
    IObserver <|.. Logger
    IObserver <|.. NotificationEngine

    %% Observable ↔ Observer cross links
    IObservable "1" --> "1..*" IObserver : notifies
    Logger --> NotificationObservable : observes
    NotificationEngine "1" --> "1" NotificationObservable : observes

    %% Strategy bottom hierarchy
    NotificationStrategy <|-- EmailStrategy
    NotificationStrategy <|-- SMSStrategy
    NotificationStrategy <|-- PopUpStrategy

    %% NotificationEngine → Strategy (1..*)
    NotificationEngine "1" o--> "1..*" NotificationStrategy : uses