// Parking Management System in C++ (Console-Based)
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
using namespace std;

struct User {
    int id;
    string firstName;
    string lastName;
    string email;
    string password;
    string role; // "admin" or "attendant"
};

struct Parking {
    string code;
    string name;
    int availableSpaces;
    string location;
    float feePerHour;
};

struct CarEntry {
    int id;
    string plateNumber;
    string parkingCode;
    time_t entryTime;
    time_t exitTime;
    float chargedAmount;
    bool exited;
};

vector<User> users;
vector<Parking> parkings;
vector<CarEntry> carEntries;
int currentUserId = -1;
string currentUserRole = "";

time_t getCurrentTime() {
    return time(0);
}

string formatTime(time_t rawTime) {
    char buffer[80];
    struct tm * timeinfo = localtime(&rawTime);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

void registerUser() {
    User u;
    cout << "Enter ID: "; cin >> u.id;
    cout << "Enter First Name: "; cin >> u.firstName;
    cout << "Enter Last Name: "; cin >> u.lastName;
    cout << "Enter Email: "; cin >> u.email;
    cout << "Enter Password: "; cin >> u.password;
    cout << "Enter Role (admin/attendant): "; cin >> u.role;

    for (User user : users) {
        if (user.id == u.id) {
            cout << "User ID already exists.\n";
            return;
        }
    }
    users.push_back(u);
    cout << "User registered successfully.\n";
}

bool login() {
    string email, password;
    cout << "Email: "; cin >> email;
    cout << "Password: "; cin >> password;
    for (User u : users) {
        if (u.email == email && u.password == password) {
            currentUserId = u.id;
            currentUserRole = u.role;
            cout << "Login successful as " << currentUserRole << "\n";
            return true;
        }
    }
    cout << "Invalid credentials.\n";
    return false;
}

void addParking() {
    if (currentUserRole != "admin") {
        cout << "Only admin can add parking locations.\n";
        return;
    }
    Parking p;
    cout << "Enter Parking Code: "; cin >> p.code;
    cout << "Enter Name: "; cin >> p.name;
    cout << "Available Spaces: "; cin >> p.availableSpaces;
    cout << "Location: "; cin >> p.location;
    cout << "Fee per Hour: "; cin >> p.feePerHour;
    parkings.push_back(p);
    cout << "Parking added successfully.\n";
}

void viewParkings() {
    for (Parking p : parkings) {
        cout << "Code: " << p.code << ", Name: " << p.name << ", Spaces: " << p.availableSpaces
             << ", Fee/hr: " << p.feePerHour << ", Location: " << p.location << "\n";
    }
}

void carEntry() {
    CarEntry c;
    cout << "Enter Entry ID: "; cin >> c.id;
    cout << "Plate Number: "; cin >> c.plateNumber;
    cout << "Parking Code: "; cin >> c.parkingCode;

    bool found = false;
    for (auto &p : parkings) {
        if (p.code == c.parkingCode) {
            if (p.availableSpaces <= 0) {
                cout << "No space available.\n";
                return;
            }
            p.availableSpaces--;
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "Invalid parking code.\n";
        return;
    }
    c.entryTime = getCurrentTime();
    c.exitTime = 0;
    c.chargedAmount = 0.0f;
    c.exited = false;
    carEntries.push_back(c);
    cout << "Car entry registered.\n";
}

void carExit() {
    int entryId;
    cout << "Enter Entry ID: "; cin >> entryId;
    for (auto &entry : carEntries) {
        if (entry.id == entryId && !entry.exited) {
            entry.exitTime = getCurrentTime();
            double hours = difftime(entry.exitTime, entry.entryTime) / 3600.0;
            for (auto &p : parkings) {
                if (p.code == entry.parkingCode) {
                    entry.chargedAmount = p.feePerHour * hours;
                    p.availableSpaces++;
                    break;
                }
            }
            entry.exited = true;
            cout << "Car exited. Duration: " << fixed << setprecision(2) << hours << " hours, Charged: " << entry.chargedAmount << " RWF\n";
            return;
        }
    }
    cout << "Car entry not found or already exited.\n";
}

void viewReports() {
    cout << "Car Exits Report:\n";
    for (CarEntry e : carEntries) {
        if (e.exited) {
            cout << "Plate: " << e.plateNumber << ", From: " << formatTime(e.entryTime) << ", To: " << formatTime(e.exitTime)
                 << ", Amount: " << e.chargedAmount << "\n";
        }
    }
}

int main() {
    int choice;
    while (true) {
        cout << "\n--- Parking Management System ---\n";
        cout << "1. Register User\n2. Login\n3. Add Parking (Admin)\n4. View Parkings\n5. Car Entry\n6. Car Exit\n7. View Reports\n8. Exit\nChoice: ";
        cin >> choice;
        switch (choice) {
            case 1: registerUser(); break;
            case 2: login(); break;
            case 3: addParking(); break;
            case 4: viewParkings(); break;
            case 5: carEntry(); break;
            case 6: carExit(); break;
            case 7: viewReports(); break;
            case 8: return 0;
            default: cout << "Invalid choice.\n";
        }
    }
}