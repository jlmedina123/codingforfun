#include <iostream>
#include <cstring>
using std::cout;
using std::endl;
using std::strncpy;
using std::strlen;
 
#define FIRSTNAME_LEN 10
 
/* Concepts to remember in this program:
    - Class, class' member functions (or methods),
      class' data members (or attributes)
    - const object
    - const member function
    - static data member and static member function
    - inheritance, base class, derived class
    - composition
    - friend class and friend function
    - polymorphism
    - virtual function
    - abstract class
    - dynamic memory allocation
    - class' constructor and destructor
    - default constractor
    - dangling pointer
 
*/
 
/* const == read-only -> must be initialized when created, and cant be modified after
   const variable: can be initialized, but not modified after that: const int a = 5;
   const obj: object cannot be modified: const Date d(1, 1, 2000)
              If it is member of class, must be initialized via member initializer
              It can only call methods defined as const
   const member funcs: the only methods that can be called by const object
   const data member: must be initialized via member initializer (':' in the constructor')
                      cannot be modified after */
 
/**** CLASSES INTERFACES ****/
 
class Date {
 
/* public data can be accessed by methods of class, and by objects of the class */
public:
    Date(int = 1, int = 1, int = 2014);  // constructor. Defaults to 1, 1, 2014
    void setDate(int, int, int);         // non const method: cannot be called by const class object
    void printDate() const;              // const method: can be called by const class object
 
/* private data can only be accessed by the methods of the class
   They cannot be accessed by objects of the class (data encapsulation or data hiding)
   Objects of the class usually access them via public setters and getters
   They cannot be accessed by methods of a derived class */
private:
    int day;
    int month;
    int year;
 
/* protected data, like private data, cannot be accessed by objects
   of the class. Only by methods of the class.
   But protected data can be accessed by methods of a derived class */
//protected:
 
/* by default (if not public/private keyword is used), the data members
   will be private. This is a difference with struct in C, where all
   data is by default public. */
};
 
/* an abstract class is a class with at least one pure virtual
   function. Since they have one or more pure virtual functions,
   which do not have the function implementation, they canNOT
   instantiate objects */
class AbstractEmployee {
public:
     /* pure virtual function is a virtual function with the '=0'
        It means that implementation in the base class is not provided
        Therefore all derived class must implement this function,
        which will overried the function of the abstract class */
     virtual void printInfovirtual() const = 0;
};
 
class Employee : public AbstractEmployee{
 
public: // public members can be accessed only by objects of the class
    Employee(const char firstname[],        // array of const data (firstname is already a const ptr)
             const char * const middlename, // const pointer to const data
             int salary,
/* Using composition. constant object of Date
   Passed as reference for better performance
   If this function (constructor) changed datehire, it would
   also change for the function calling, because it is passed
   by reference. To protect it, it is declared const, so constructor
   cannot change it */
             const Date &datehire);
 
/* this class requires explicit constructor, destructor, copy constructor
   and assignment operator because some data members (middlename)
   use dynamic memory */
    ~Employee();                   // Destructor
    Employee(const Employee &src); // copy constructor
    Employee& operator=(const Employee &rhs); // assignment operator
    void setnames(const char [], const char * const);
    Employee& setsalary(int);   // cascading method. Returns pointer to object (this)
    static int getnumbemployees();  // static member function. Used to access static data member
    void printname() const;    // const method, which means it can be used by const objects
 
    void printInfo() const;
/* next function is pure virtual function in AbstractEmployee, so it
   needs to be implemented in this class.
   declared as virtual to enable polymorphism.
   Pointer to base class Employee will still call method of derived class
   Manager if object is from derived class */
    virtual void printInfovirtual() const;
 
private:
/* first and middle name could be defined const, but then it wouldnt be possible
   to modify them after employee object was created. */
    char firstname_array[FIRSTNAME_LEN]; // using array.
    char *middlename_dynamic;  // using dynamic memory allocation
 
    int salary;
 
/* employee HAS a datehire of type Date (composition)
   Defined as const, so it cannot be modified after object initialized
   It CANNOT be changed after object employee is created
   It must be initialized in the constructor via member initializer (:)
   This object data member can only call Date methods defined with const */
    const Date dateHire;
/* static data member means one copy of the variable is shared by
   all objects of the class(as opposed to non static, for which
   each object has its own copy of the data members)
   So in this case all objects will see the same number
   of employees in the company
   If public, it could be accessed as object.totalnumbemployees or
   Employee::totalnumbemployees
   Private static data member needs a public static member
   function to be accessed */
    static int totalnumbemployees;
 
/* static, so one copy for all objects.
   const, so it cannot be modified after it is defined
   private, so it will need a static method to access it*/
    const static char companyname[];
 
};
 
class Manager : public Employee {
/* manager IS an employee (inheritance)
   public inh: public members of base class become public members of derived class.
               protected members of base class become protected members of derived class.
               derived class canNOT access private members of base class.
               Need to use public getters/setters to access private members of base class
   private inh: public and protected members of base clase become
                private member of derived class. This means that all data of the
                derived class object will be inaccessible (private data is only accessible
                by methods of class, not by object, and not by methods of derived class).
                It is not a 'is-a' relationship, but more like a 'has-a' relationship
   protected inh: public and protected members of base clase become
                protected members of derived class (data accessible by methods of class
                and methods of derived class, not by object)
                Usually used for last level of inheritance in a hierarchy */
public:
    Manager(const char firstname[],
            const char * const middlename,
            int salary, int bonus,
            const Date &datehire,
            int teamSize);
    ~Manager();                            // destructor
    int getteamsize();
    void setteamsize(int );
 
    void printInfo() const;
    virtual void printInfovirtual() const;
 
private:
    int bonus;
    int teamSize;
/* class inherites all the data members from base class (firstname, etc)
   but it wont be able to access private data member from base class */
};
 
/***** CLASSES IMPLEMENTATION *****/
 
/**** Date member functions *****/
Date::Date(int d, int m, int y) :
    day (d), month (m), year (y) { // member initializer
    cout << "Date constructor for: " << m << "/" << d << "/" << y << endl;
}
void Date::setDate(int d, int m, int y) // cannot be called by const object
{
    this->day = d;
    month = m;
    year = y;
}
void Date::printDate() const {  // defined as const, so it can be called by const objects
    cout << month << "/" << day << "/" << year << endl;
}
 
/***** Employee member functions ******/
 
// static data member needs to be
// initialized ONCE
int Employee::totalnumbemployees = 0;
const char Employee::companyname[] = "Medina Corp";
 
Employee::Employee(const char first[],
                   const char * const middle,
                   int s, const Date &date) :
                   dateHire (date) {  // dateHire is const member,
                                      // so it must be initialized here
    cout << "Employee constructor for " << first << endl;
    setnames(first, middle);
    salary = s;
    totalnumbemployees++;
    // dateHire = date; // this would fail because dateHire
                        // is const member
}
Employee::~Employee() {
    cout << "Employee destructor for " << firstname_array << endl;
    delete [] middlename_dynamic;
    totalnumbemployees--;
}
 
/* copy constructor is required when a class uses dynamically allocated data
   The default copy constructor makes shallow copies of data. This means
   that the copied object will get a copy of the data member pointer, but the
   underlying data on the heap will be the same. So if the copied object changes
   the data or exists and frees the data, the original object will have its
   data member pointers pointing to non-valid memory (dangling pointers).
   The explicit copy constructor makes deep copies, so that copied object has its
  own allocated memory */
Employee::Employee(const Employee &src) :
    dateHire (src.dateHire) {
    cout << "Employee copy constructor for " << src.firstname_array << " " << endl;
    setnames(src.firstname_array, src.middlename_dynamic);
 
    // setnames makes deep copy of dynamic memory
    salary = src.salary;
}
 
/* assignment operator. The difference with copy constructor is
   that the assignment operator is used on a already initialized object.
   When two objects are instantiated for a class, each
   will have its own memory.
   Manager manager1(), manager2();
   manager2 = manager1;
   When assignment is used, the class default assignment
   operator will direct the data pointers of the left
   object to the memory of right object. But the dynamically
   allocated memory of the left object will not be
   freed but will be unaccessible (orphaned memory).
   This causes leakage memory.
   To avoid this, dynamic memory of left object
   should be freed. And then deep copy of
   dynamically allocated data should be performed.
*/
Employee& Employee::operator=(const Employee &rhs) {
    cout << "Employee assignment operator for " << rhs.firstname_array << endl;
    // check self assignment
    if (this == &rhs) { return *this; }
    // free lhs dynamic memory
    delete [] middlename_dynamic;
    // dynamic allocation and deep copy data
    setnames(rhs.firstname_array, rhs.middlename_dynamic);
}
 
void Employee::setnames(const char first[], const char * const middle) {
    // array
    strcpy(firstname_array, first);
    // truncating firstname if array entered greater than firstname_len
    if (strlen(first) > FIRSTNAME_LEN) {
        memset(firstname_array + FIRSTNAME_LEN - 1, '\0', 1);
    }
    // dynamic memory
    middlename_dynamic = new char[strlen(middle) + 1];
    strcpy(middlename_dynamic, middle);  //strcpy performs deep copy of pointer
    // string
}
 
Employee& Employee::setsalary(int s) {
    if (s > 0 && s < 200000) { // setter functions should check data
        salary = s;
    }
    return *this; // enables cascading calls such as empl.setsalar(1000).printsalary()
}
void Employee::printname() const {
    cout << firstname_array << " " << middlename_dynamic;
}
void Employee::printInfo() const {
    cout << "Employee class (base) printInfo" << endl;
    cout << "Name: " << firstname_array << endl;
    cout << "Salary: " << salary << endl;
}
 
void Employee::printInfovirtual() const {
    cout << "Employee class (base) printInfo" << endl;
    cout << "Name: " << firstname_array << endl;
    cout << "Salary: " << salary << endl;
}
 
/*********** MANAGER CLASS IMPLEMENTATION ************/
 
Manager::Manager(const char first[], const char * const middle,
                 int sal, int b, const Date &hiredate,
                 int numbteam)
    : Employee(first, middle, sal, hiredate) { // calls base class
    cout << "Constructor for Manager called" << endl;
    teamSize = numbteam;
    bonus = b;
}
 
Manager::~Manager() {
    cout << "Manager Destructor called for "; // << firstname_array << endl;
    printname();
    cout << endl;
    // base class destructor is called implicitaly
}
 
void Manager::printInfo() const {
    cout << "Manager class (derived) printInfo" << endl;
    Employee::printInfo();
    cout << "Bonus: " << bonus << endl;
}
 
void Manager::printInfovirtual() const {
    cout << "Manager class (derived) printInfo" << endl;
    Employee::printInfo();
    cout << "Bonus: " << bonus << endl;
}
 
int main() {
    cout << "Instatiatiating date: " << endl;
    Date date1(10, 10, 2000);
    cout << endl << "Instantiating employee 1: " << endl;
    Employee empl1("name1", "last1", 1000, date1);
    cout << endl << "Instantiating employee 2: " << endl;
    Employee empl2("name2", "last2", 200, Date(2, 2, 2000));
    cout << endl << "Instantiating employee 3: " << endl;
    Employee empl3("name3", "last3", 300, Date(3, 3, 3000));
    cout << endl << "Copying employee 1 to employee 3:" << endl;
    empl3 = empl1;           // using assignment operator
    cout << endl << "Instantiating employee 4 as copy of employee 1:" << endl;
    Employee empl4 = empl1;  // using copy constructor
 
    /* Polymorphism causes the same function (printInfo) to cause different actions
       depending on the type of object that invokes the function
       Without virtual functions, the type of pointer to the object determines the
       function called (pointer to base class will call method of base class, pointer
       to derived class will call method of derived class, regardless of object)
       With virtual functions, the the type of object determines the function called  */
 
    /* Using non virtual functions: pointer determines behavior */
    // base pointer to derived class
    cout << endl << "Instatiating manager:" << endl;
    Manager derivedobject("managername", "managerlast", 1000, 1000, Date(7, 7, 2000), 10);
    Employee * baseptr = &derivedobject;
    cout << endl << "Calling printInfo with Employee (base) pointer to Manager (derived) object" << endl;
    baseptr->printInfo(); // invokes printInfo of base class (Employee)
 
    // derived pointer to base class is illegal
    //Manager * derivedptr = &empl1; //  error: invalid conversion from ‘Employee*’ to ‘Manager*’
    //derivedptr->printInfo(); // Manager printInfo would try to print bonus, which Employee doesnt have
 
    /* Using virtual functions: object determines behavior */
    cout << endl << "Calling printInfovirtual with Employee (base) pointer to Manager (derived) object" << endl;
    baseptr->printInfovirtual();
 
    /* thanks to polymorphims and virtual functions, we can have an array of base class type with objects
       of the base or derived classes. And just use the base ptr to call functions on the objects */
 
    cout << endl << "Done:" << endl;
    return 0;
}

