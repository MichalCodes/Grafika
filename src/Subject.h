
class Observer;
class Subject{
    protected:
        virtual ~Subject() = default;
        virtual void addObserver(Observer* obs) = 0;
        virtual void removeObserver(Observer* obs) = 0;
        virtual void notify() = 0;
};