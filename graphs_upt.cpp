#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <limits>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

// Utility functions
string toLower(const string& str) {
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Forward declarations
class Patient;
class Doctor;
class Room;
struct Relationship;

// Graph Node (Vertex)
template <typename T>
class Vertex {
public:
    string id;
    T data;
    
    // Add default constructor
    Vertex() : id(""), data() {}
    
    Vertex(string id, const T& data) : id(id), data(data) {}
};

// Patient class to store patient information
class Patient {
public:
    string id;
    string name;
    int age;
    string gender;
    string medical_history;
    string current_condition;
    time_t admission_date;

    Patient(string id = "", string name = "", int age = 0, string gender = "", 
            string medical_history = "", string current_condition = "")
        : id(id), name(name), age(age), gender(gender), 
          medical_history(medical_history), current_condition(current_condition) {
        admission_date = time(nullptr);
    }

    string toCSV() const {
        stringstream ss;
        ss << id << "," << name << "," << age << "," << gender << ","
           << medical_history << "," << current_condition << ","
           << admission_date;
        return ss.str();
    }

    static Patient fromCSV(const string& csvLine) {
        stringstream ss(csvLine);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 7) {
            throw runtime_error("Invalid CSV line for Patient");
        }

        Patient p(tokens[0], tokens[1], stoi(tokens[2]), tokens[3], tokens[4], tokens[5]);
        p.admission_date = stol(tokens[6]);
        return p;
    }

    void display() const {
        cout << "\nPatient ID: " << id << "\n";
        cout << "Name: " << name << "\n";
        cout << "Age: " << age << "\n";
        cout << "Gender: " << gender << "\n";
        cout << "Medical History: " << medical_history << "\n";
        cout << "Current Condition: " << current_condition << "\n";
        cout << "Admission Date: " << ctime(&admission_date);
    }
};

// Doctor class to store doctor information
class Doctor {
public:
    string id;
    string name;
    string specialization;
    vector<string> available_days;
    string contact_info;

    Doctor(string id = "", string name = "", string specialization = "", 
           vector<string> available_days = {}, string contact_info = "")
        : id(id), name(name), specialization(specialization), 
          available_days(available_days), contact_info(contact_info) {}

    string toCSV() const {
        stringstream ss;
        ss << id << "," << name << "," << specialization << ",";
        for (size_t i = 0; i < available_days.size(); ++i) {
            ss << available_days[i];
            if (i != available_days.size() - 1) ss << ";";
        }
        ss << "," << contact_info;
        return ss.str();
    }

    static Doctor fromCSV(const string& csvLine) {
        stringstream ss(csvLine);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 5) {
            throw runtime_error("Invalid CSV line for Doctor");
        }

        vector<string> days;
        stringstream days_ss(tokens[3]);
        string day;
        while (getline(days_ss, day, ';')) {
            days.push_back(day);
        }

        return Doctor(tokens[0], tokens[1], tokens[2], days, tokens[4]);
    }

    void display() const {
        cout << "\nDoctor ID: " << id << "\n";
        cout << "Name: " << name << "\n";
        cout << "Specialization: " << specialization << "\n";
        cout << "Available Days: ";
        for (const auto& day : available_days) {
            cout << day << " ";
        }
        cout << "\nContact Info: " << contact_info << "\n";
    }
};

// Room class to store room information
class Room {
public:
    string id;
    string type; // ICU, General, Emergency, etc.
    int capacity;
    bool occupied;

    Room(string id = "", string type = "", int capacity = 0, bool occupied = false)
        : id(id), type(type), capacity(capacity), occupied(occupied) {}

    string toCSV() const {
        return id + "," + type + "," + to_string(capacity) + "," + (occupied ? "1" : "0");
    }

    static Room fromCSV(const string& csvLine) {
        stringstream ss(csvLine);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 4) {
            throw runtime_error("Invalid CSV line for Room");
        }

        return Room(tokens[0], tokens[1], stoi(tokens[2]), tokens[3] == "1");
    }

    void display() const {
        cout << "\nRoom ID: " << id << "\n";
        cout << "Type: " << type << "\n";
        cout << "Capacity: " << capacity << "\n";
        cout << "Status: " << (occupied ? "Occupied" : "Available") << "\n";
    }
};

// Edge data for relationships between nodes
struct Relationship {
    string type; // "treats", "assigned_to", "shares_room", "refers_to", etc.
    string details;
    time_t timestamp;

    Relationship(string type = "", string details = "") 
        : type(type), details(details) {
        timestamp = time(nullptr);
    }

    string toCSV() const {
        return type + "," + details + "," + to_string(timestamp);
    }

    static Relationship fromCSV(const string& csvLine) {
        stringstream ss(csvLine);
        string token;
        vector<string> tokens;

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 3) {
            throw runtime_error("Invalid CSV line for Relationship");
        }

        Relationship r(tokens[0], tokens[1]);
        r.timestamp = stol(tokens[2]);
        return r;
    }

    void display() const {
        cout << "\nRelationship Type: " << type << "\n";
        if (!details.empty()) cout << "Details: " << details << "\n";
        cout << "Timestamp: " << ctime(&timestamp);
    }
};

// Graph implementation for Hospital Management System
class HospitalGraph {
private:
    // Adjacency list: maps vertex ID to list of pairs (neighbor ID, relationship)
    unordered_map<string, vector<pair<string, Relationship>>> adjList;
    
    // Maps to store different types of vertices
    unordered_map<string, Vertex<Patient>> patients;
    unordered_map<string, Vertex<Doctor>> doctors;
    unordered_map<string, Vertex<Room>> rooms;
    
    // Helper function to check if vertex exists
    bool vertexExists(const string& id) {
        return patients.find(id) != patients.end() || 
               doctors.find(id) != doctors.end() || 
               rooms.find(id) != rooms.end();
    }

public:
    // Vertex operations
    bool addPatient(const Patient& patient) {
        if (vertexExists(patient.id)) return false;
        Vertex<Patient> v(patient.id, patient);
        patients.emplace(patient.id, v);
        return true;
    }

    bool addDoctor(const Doctor& doctor) {
        if (vertexExists(doctor.id)) return false;
        Vertex<Doctor> v(doctor.id, doctor);
        doctors.emplace(doctor.id, v);
        return true;
    }

    bool addRoom(const Room& room) {
        if (vertexExists(room.id)) return false;
        Vertex<Room> v(room.id, room);
        rooms.emplace(room.id, v);
        return true;
    }

    bool removeVertex(const string& id) {
        // First check if the vertex exists and get its type
        bool isPatient = patients.find(id) != patients.end();
        bool isDoctor = doctors.find(id) != doctors.end();
        bool isRoom = rooms.find(id) != rooms.end();
        
        if (!isPatient && !isDoctor && !isRoom) return false;

        // Remove all edges pointing to this vertex
        for (auto& entry : adjList) {
            auto& neighbors = entry.second;
            neighbors.erase(remove_if(neighbors.begin(), neighbors.end(),
                [&id](const pair<string, Relationship>& p) { return p.first == id; }), 
                neighbors.end());
        }

        // Remove the vertex from its respective map
        if (isPatient) {
            patients.erase(id);
            // Update room occupancy if patient was in a room
            string roomId = getPatientRoom(id);
            if (!roomId.empty() && rooms.find(roomId) != rooms.end()) {
                rooms[roomId].data.occupied = false;
            }
        } else if (isDoctor) {
            doctors.erase(id);
        } else if (isRoom) {
            rooms.erase(id);
            // Remove all patient assignments to this room
            vector<string> patientsInRoom = getPatientsInRoom(id);
            for (const auto& patientId : patientsInRoom) {
                removeEdge(patientId, id, "assigned_to");
            }
        }

        // Remove the vertex's adjacency list entry
        adjList.erase(id);
        return true;
    }

    // Edge operations
    bool addEdge(const string& srcId, const string& destId, const Relationship& relationship) {
        if (!vertexExists(srcId) || !vertexExists(destId)) return false;
        
        // Check if edge already exists
        for (const auto& neighbor : adjList[srcId]) {
            if (neighbor.first == destId && neighbor.second.type == relationship.type) {
                return false;
            }
        }
        
        adjList[srcId].emplace_back(destId, relationship);
        return true;
    }

    bool removeEdge(const string& srcId, const string& destId, const string& relationshipType = "") {
        if (adjList.find(srcId) == adjList.end()) return false;
        
        auto& neighbors = adjList[srcId];
        if (relationshipType.empty()) {
            auto it = remove_if(neighbors.begin(), neighbors.end(),
                [&destId](const pair<string, Relationship>& p) { return p.first == destId; });
            if (it != neighbors.end()) {
                neighbors.erase(it, neighbors.end());
                return true;
            }
        } else {
            auto it = remove_if(neighbors.begin(), neighbors.end(),
                [&destId, &relationshipType](const pair<string, Relationship>& p) { 
                    return p.first == destId && p.second.type == relationshipType; 
                });
            if (it != neighbors.end()) {
                neighbors.erase(it, neighbors.end());
                return true;
            }
        }
        return false;
    }

    // Getter methods
    const unordered_map<string, Vertex<Patient>>& getPatients() const { return patients; }
    const unordered_map<string, Vertex<Doctor>>& getDoctors() const { return doctors; }
    const unordered_map<string, Vertex<Room>>& getRooms() const { return rooms; }
    const unordered_map<string, vector<pair<string, Relationship>>>& getAdjList() const { return adjList; }

    // Query operations
    vector<string> getPatientsTreatedByDoctor(const string& doctorId) {
        vector<string> patientIds;
        if (doctors.find(doctorId) == doctors.end()) return patientIds;

        for (const auto& neighbor : adjList[doctorId]) {
            if (neighbor.second.type == "treats" && patients.find(neighbor.first) != patients.end()) {
                patientIds.push_back(neighbor.first);
            }
        }
        return patientIds;
    }

    vector<string> getDoctorsTreatingPatient(const string& patientId) {
        vector<string> doctorIds;
        if (patients.find(patientId) == patients.end()) return doctorIds;

        // Create a set to store unique doctor IDs
        unordered_set<string> uniqueDoctors;
        
        // Check all edges where the patient is the destination
        for (const auto& entry : adjList) {
            const string& sourceId = entry.first;
            // Only check if the source is a doctor
            if (doctors.find(sourceId) != doctors.end()) {
                // Check if there's a "treats" relationship to this patient
                for (const auto& neighbor : entry.second) {
                    if (neighbor.first == patientId && neighbor.second.type == "treats") {
                        uniqueDoctors.insert(sourceId);
                        break;  // No need to check other relationships from this doctor
                    }
                }
            }
        }

        // Convert set to vector
        doctorIds.assign(uniqueDoctors.begin(), uniqueDoctors.end());
        return doctorIds;
    }

    string getPatientRoom(const string& patientId) {
        if (patients.find(patientId) == patients.end()) return "";

        for (const auto& entry : adjList) {
            if (entry.first == patientId) {
                for (const auto& neighbor : entry.second) {
                    if (neighbor.second.type == "assigned_to" && rooms.find(neighbor.first) != rooms.end()) {
                        return neighbor.first;
                    }
                }
            }
        }
        return "";
    }

    vector<string> getPatientsInRoom(const string& roomId) {
        vector<string> patientIds;
        if (rooms.find(roomId) == rooms.end()) return patientIds;

        for (const auto& entry : adjList) {
            if (patients.find(entry.first) != patients.end()) {
                for (const auto& neighbor : entry.second) {
                    if (neighbor.first == roomId && neighbor.second.type == "assigned_to") {
                        patientIds.push_back(entry.first);
                        break;
                    }
                }
            }
        }
        return patientIds;
    }

    // CSV operations
    void saveToCSV(const string& baseFilename) {
        // Save patients
        ofstream patientFile(baseFilename + "_patients.csv");
        patientFile << "ID,Name,Age,Gender,MedicalHistory,CurrentCondition,AdmissionDate\n";
        for (const auto& entry : patients) {
            patientFile << entry.second.data.toCSV() << "\n";
        }
        patientFile.close();

        // Save doctors
        ofstream doctorFile(baseFilename + "_doctors.csv");
        doctorFile << "ID,Name,Specialization,AvailableDays,ContactInfo\n";
        for (const auto& entry : doctors) {
            doctorFile << entry.second.data.toCSV() << "\n";
        }
        doctorFile.close();

        // Save rooms
        ofstream roomFile(baseFilename + "_rooms.csv");
        roomFile << "ID,Type,Capacity,Occupied\n";
        for (const auto& entry : rooms) {
            roomFile << entry.second.data.toCSV() << "\n";
        }
        roomFile.close();

        // Save relationships
        ofstream edgeFile(baseFilename + "_relationships.csv");
        edgeFile << "Source,Destination,Type,Details,Timestamp\n";
        for (const auto& entry : adjList) {
            for (const auto& neighbor : entry.second) {
                edgeFile << entry.first << "," << neighbor.first << "," << neighbor.second.toCSV() << "\n";
            }
        }
        edgeFile.close();

        cout << "Data saved successfully to " << baseFilename << "_*.csv files\n";
    }

    void loadFromCSV(const string& baseFilename) {
        // Clear existing data
        patients.clear();
        doctors.clear();
        rooms.clear();
        adjList.clear();

        // Helper function to check if file exists and is readable
        auto checkFile = [](const string& filename) -> bool {
            ifstream file(filename);
            if (!file.is_open()) {
                cerr << "Error: Could not open file " << filename << endl;
                return false;
            }
            return true;
        };

        // Load patients
        string patientFilename = baseFilename + "_patients.csv";
        if (checkFile(patientFilename)) {
            ifstream patientFile(patientFilename);
            string line;
            getline(patientFile, line); // Skip header
            int lineNum = 2;
            while (getline(patientFile, line)) {
                try {
                    if (line.empty()) continue;
                    Patient p = Patient::fromCSV(line);
                    if (!addPatient(p)) {
                        cerr << "Warning: Failed to add patient at line " << lineNum << endl;
                    }
                } catch (const exception& e) {
                    cerr << "Error loading patient at line " << lineNum << ": " << e.what() << endl;
                }
                lineNum++;
            }
            patientFile.close();
        }

        // Load doctors
        string doctorFilename = baseFilename + "_doctors.csv";
        if (checkFile(doctorFilename)) {
            ifstream doctorFile(doctorFilename);
            string line;
            getline(doctorFile, line); // Skip header
            int lineNum = 2;
            while (getline(doctorFile, line)) {
                try {
                    if (line.empty()) continue;
                    Doctor d = Doctor::fromCSV(line);
                    if (!addDoctor(d)) {
                        cerr << "Warning: Failed to add doctor at line " << lineNum << endl;
                    }
                } catch (const exception& e) {
                    cerr << "Error loading doctor at line " << lineNum << ": " << e.what() << endl;
                }
                lineNum++;
            }
            doctorFile.close();
        }

        // Load rooms
        string roomFilename = baseFilename + "_rooms.csv";
        if (checkFile(roomFilename)) {
            ifstream roomFile(roomFilename);
            string line;
            getline(roomFile, line); // Skip header
            int lineNum = 2;
            while (getline(roomFile, line)) {
                try {
                    if (line.empty()) continue;
                    Room r = Room::fromCSV(line);
                    if (!addRoom(r)) {
                        cerr << "Warning: Failed to add room at line " << lineNum << endl;
                    }
                } catch (const exception& e) {
                    cerr << "Error loading room at line " << lineNum << ": " << e.what() << endl;
                }
                lineNum++;
            }
            roomFile.close();
        }

        // Load relationships
        string relationshipFilename = baseFilename + "_relationships.csv";
        if (checkFile(relationshipFilename)) {
            ifstream edgeFile(relationshipFilename);
            string line;
            getline(edgeFile, line); // Skip header
            int lineNum = 2;
            while (getline(edgeFile, line)) {
                try {
                    if (line.empty()) continue;
                    stringstream ss(line);
                    string srcId, destId, type, details, timestamp;
                    
                    if (!getline(ss, srcId, ',') || !getline(ss, destId, ',') ||
                        !getline(ss, type, ',') || !getline(ss, details, ',') ||
                        !getline(ss, timestamp, ',')) {
                        throw runtime_error("Invalid relationship format");
                    }

                    Relationship r(type, details);
                    r.timestamp = stol(timestamp);
                    
                    if (!addEdge(srcId, destId, r)) {
                        cerr << "Warning: Failed to add relationship at line " << lineNum << endl;
                    }
                } catch (const exception& e) {
                    cerr << "Error loading relationship at line " << lineNum << ": " << e.what() << endl;
                }
                lineNum++;
            }
            edgeFile.close();
        }

        cout << "Data loading completed. Check error messages above for any issues.\n";
    }

    // Display functions
    void displayAllPatients() const {
        if (patients.empty()) {
            cout << "No patients in the system.\n";
            return;
        }
        cout << "\n=== All Patients ===\n";
        for (const auto& entry : patients) {
            entry.second.data.display();
            cout << "-------------------\n";
        }
    }

    void displayAllDoctors() const {
        if (doctors.empty()) {
            cout << "No doctors in the system.\n";
            return;
        }
        cout << "\n=== All Doctors ===\n";
        for (const auto& entry : doctors) {
            entry.second.data.display();
            cout << "-------------------\n";
        }
    }

    void displayAllRooms() const {
        if (rooms.empty()) {
            cout << "No rooms in the system.\n";
            return;
        }
        cout << "\n=== All Rooms ===\n";
        for (const auto& entry : rooms) {
            entry.second.data.display();
            cout << "-------------------\n";
        }
    }

    void displayGraphStats() const {
        cout << "\n=== Hospital Management System Statistics ===\n";
        cout << "Total Patients: " << patients.size() << "\n";
        cout << "Total Doctors: " << doctors.size() << "\n";
        cout << "Total Rooms: " << rooms.size() << "\n";
        
        size_t totalRelationships = 0;
        for (const auto& entry : adjList) {
            totalRelationships += entry.second.size();
        }
        cout << "Total Relationships: " << totalRelationships << "\n";
    }
};

// [Rest of the HospitalUI class implementation remains the same...]

// User Interface Class
class HospitalUI {
private:
    HospitalGraph hospital;
    string dataFilename = "hospital_data";

    void displayMainMenu() {
        clearScreen();
        cout << "=== HOSPITAL MANAGEMENT SYSTEM ===\n";
        cout << "1. Patient Management\n";
        cout << "2. Doctor Management\n";
        cout << "3. Room Management\n";
        cout << "4. Relationship Management\n";
        cout << "5. Reports and Queries\n";
        cout << "6. Data Operations\n";
        cout << "0. Exit\n";
        cout << "=================================\n";
        cout << "Enter your choice: ";
    }

    void patientManagementMenu() {
        int choice;
        do {
            clearScreen();
            cout << "=== PATIENT MANAGEMENT ===\n";
            cout << "1. Add New Patient\n";
            cout << "2. View All Patients\n";
            cout << "3. Search Patient\n";
            cout << "4. Update Patient\n";
            cout << "5. Remove Patient\n";
            cout << "0. Back to Main Menu\n";
            cout << "==========================\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: addPatient(); break;
                case 2: hospital.displayAllPatients(); pressEnterToContinue(); break;
                case 3: searchPatient(); break;
                case 4: updatePatient(); break;
                case 5: removePatient(); break;
                case 0: break;
                default: cout << "Invalid choice. Please try again.\n"; pressEnterToContinue();
            }
        } while (choice != 0);
    }

    void addPatient() {
        clearScreen();
        cout << "=== ADD NEW PATIENT ===\n";
        
        string id, name, gender, medicalHistory, currentCondition;
        int age;
        
        cout << "Enter Patient ID: ";
        getline(cin, id);
        if (hospital.getPatients().find(id) != hospital.getPatients().end()) {
            cout << "Patient with this ID already exists.\n";
            pressEnterToContinue();
            return;
        }
        
        cout << "Enter Patient Name: ";
        getline(cin, name);
        
        cout << "Enter Age: ";
        cin >> age;
        cin.ignore();
        
        cout << "Enter Gender: ";
        getline(cin, gender);
        
        cout << "Enter Medical History: ";
        getline(cin, medicalHistory);
        
        cout << "Enter Current Condition: ";
        getline(cin, currentCondition);
        
        Patient newPatient(id, name, age, gender, medicalHistory, currentCondition);
        if (hospital.addPatient(newPatient)) {
            cout << "Patient added successfully!\n";
        } else {
            cout << "Failed to add patient.\n";
        }
        pressEnterToContinue();
    }

    void searchPatient() {
        clearScreen();
        cout << "=== SEARCH PATIENT ===\n";
        cout << "Enter Patient ID or Name: ";
        string query;
        getline(cin, query);
        query = toLower(trim(query));

        bool found = false;
        for (const auto& entry : hospital.getPatients()) {
            if (toLower(entry.first) == query || toLower(entry.second.data.name).find(query) != string::npos) {
                entry.second.data.display();
                found = true;

                // Show doctors treating this patient
                auto doctors = hospital.getDoctorsTreatingPatient(entry.first);
                if (!doctors.empty()) {
                    cout << "\nTreated by Doctors:\n";
                    for (const auto& docId : doctors) {
                        cout << "- " << docId << ": ";
                        auto it = hospital.getDoctors().find(docId);
                        if (it != hospital.getDoctors().end()) {
                            cout << it->second.data.name << " (" << it->second.data.specialization << ")\n";
                        } else {
                            cout << "Unknown doctor\n";
                        }
                    }
                }

                // Show room assignment
                string roomId = hospital.getPatientRoom(entry.first);
                if (!roomId.empty()) {
                    cout << "\nAssigned to Room: " << roomId;
                    auto it = hospital.getRooms().find(roomId);
                    if (it != hospital.getRooms().end()) {
                        cout << " (" << it->second.data.type << ")\n";
                    } else {
                        cout << " (Unknown room)\n";
                    }
                }

                cout << "-------------------\n";
            }
        }

        if (!found) {
            cout << "No matching patients found.\n";
        }
        pressEnterToContinue();
    }

    void updatePatient() {
        clearScreen();
        cout << "=== UPDATE PATIENT ===\n";
        cout << "Enter Patient ID to update: ";
        string id;
        getline(cin, id);

        auto it = hospital.getPatients().find(id);
        if (it == hospital.getPatients().end()) {
            cout << "Patient not found.\n";
            pressEnterToContinue();
            return;
        }

        Patient current = it->second.data;
        current.display();

        string name, gender, medicalHistory, currentCondition;
        int age;
        
        cout << "\nEnter new Name (leave blank to keep current): ";
        getline(cin, name);
        if (!name.empty()) current.name = name;
        
        cout << "Enter new Age (0 to keep current): ";
        cin >> age;
        cin.ignore();
        if (age > 0) current.age = age;
        
        cout << "Enter new Gender (leave blank to keep current): ";
        getline(cin, gender);
        if (!gender.empty()) current.gender = gender;
        
        cout << "Enter new Medical History (leave blank to keep current): ";
        getline(cin, medicalHistory);
        if (!medicalHistory.empty()) current.medical_history = medicalHistory;
        
        cout << "Enter new Current Condition (leave blank to keep current): ";
        getline(cin, currentCondition);
        if (!currentCondition.empty()) current.current_condition = currentCondition;

        // Remove and re-add to update
        hospital.removeVertex(id);
        hospital.addPatient(current);

        cout << "Patient updated successfully!\n";
        pressEnterToContinue();
    }

    void removePatient() {
        clearScreen();
        cout << "=== REMOVE PATIENT ===\n";
        cout << "Enter Patient ID to remove: ";
        string id;
        getline(cin, id);

        if (hospital.removeVertex(id)) {
            cout << "Patient removed successfully!\n";
        } else {
            cout << "Patient not found or could not be removed.\n";
        }
        pressEnterToContinue();
    }

    // Similar menu and functions for Doctor and Room management
    // [Implementation omitted for brevity, but would follow similar patterns]

    // Relationship management functions
    void manageRelationships() {
        int choice;
        do {
            clearScreen();
            cout << "=== RELATIONSHIP MANAGEMENT ===\n";
            cout << "1. Assign Doctor to Patient\n";
            cout << "2. Assign Patient to Room\n";
            cout << "3. View All Relationships\n";
            cout << "4. Remove Relationship\n";
            cout << "0. Back to Main Menu\n";
            cout << "===============================\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: assignDoctorToPatient(); break;
                case 2: assignPatientToRoom(); break;
                case 3: viewAllRelationships(); break;
                case 4: removeRelationship(); break;
                case 0: break;
                default: cout << "Invalid choice. Please try again.\n"; pressEnterToContinue();
            }
        } while (choice != 0);
    }

    void assignDoctorToPatient() {
        clearScreen();
        cout << "=== ASSIGN DOCTOR TO PATIENT ===\n";
        
        string doctorId, patientId, details;
        cout << "Enter Doctor ID: ";
        getline(cin, doctorId);
        
        cout << "Enter Patient ID: ";
        getline(cin, patientId);
        
        cout << "Enter Treatment Details: ";
        getline(cin, details);
        
        if (hospital.addEdge(doctorId, patientId, Relationship("treats", details))) {
            cout << "Assignment successful!\n";
        } else {
            cout << "Assignment failed. Check if IDs are correct.\n";
        }
        pressEnterToContinue();
    }

    void assignPatientToRoom() {
        clearScreen();
        cout << "=== ASSIGN PATIENT TO ROOM ===\n";
        
        string patientId, roomId;
        cout << "Enter Patient ID: ";
        getline(cin, patientId);
        
        cout << "Enter Room ID: ";
        getline(cin, roomId);
        
        // Check if room is already occupied
        auto roomIt = hospital.getRooms().find(roomId);
        if (roomIt != hospital.getRooms().end() && roomIt->second.data.occupied) {
            cout << "Room is already occupied. Assignment failed.\n";
            pressEnterToContinue();
            return;
        }
        
        if (hospital.addEdge(patientId, roomId, Relationship("assigned_to"))) {
            // Update room status
            Room updatedRoom = roomIt->second.data;
            updatedRoom.occupied = true;
            hospital.removeVertex(roomId);
            hospital.addRoom(updatedRoom);
            
            cout << "Assignment successful!\n";
        } else {
            cout << "Assignment failed. Check if IDs are correct.\n";
        }
        pressEnterToContinue();
    }

    void viewAllRelationships() {
        clearScreen();
        cout << "=== ALL RELATIONSHIPS ===\n";
        
        const auto& adjList = hospital.getAdjList();
        if (adjList.empty()) {
            cout << "No relationships in the system.\n";
            pressEnterToContinue();
            return;
        }
        
        for (const auto& entry : adjList) {
            cout << "\nSource: " << entry.first << "\n";
            for (const auto& neighbor : entry.second) {
                cout << "  -> " << neighbor.first << " : ";
                neighbor.second.display();
            }
        }
        pressEnterToContinue();
    }

    void removeRelationship() {
        clearScreen();
        cout << "=== REMOVE RELATIONSHIP ===\n";
        
        string srcId, destId, relType;
        cout << "Enter Source ID: ";
        getline(cin, srcId);
        
        cout << "Enter Destination ID: ";
        getline(cin, destId);
        
        cout << "Enter Relationship Type (leave blank for any): ";
        getline(cin, relType);
        
        if (hospital.removeEdge(srcId, destId, relType)) {
            cout << "Relationship removed successfully!\n";
            
            // If this was a room assignment, mark room as available
            if (relType == "assigned_to" || relType.empty()) {
                auto roomIt = hospital.getRooms().find(destId);
                if (roomIt != hospital.getRooms().end()) {
                    Room updatedRoom = roomIt->second.data;
                    updatedRoom.occupied = false;
                    hospital.removeVertex(destId);
                    hospital.addRoom(updatedRoom);
                }
            }
        } else {
            cout << "Relationship not found or could not be removed.\n";
        }
        pressEnterToContinue();
    }

    // Reports and queries
    void reportsMenu() {
        int choice;
        do {
            clearScreen();
            cout << "=== REPORTS AND QUERIES ===\n";
            cout << "1. System Statistics\n";
            cout << "2. Patients by Doctor\n";
            cout << "3. Doctors by Patient\n";
            cout << "4. Patients in Room\n";
            cout << "5. Available Rooms\n";
            cout << "0. Back to Main Menu\n";
            cout << "===========================\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: hospital.displayGraphStats(); pressEnterToContinue(); break;
                case 2: patientsByDoctor(); break;
                case 3: doctorsByPatient(); break;
                case 4: patientsInRoom(); break;
                case 5: availableRooms(); break;
                case 0: break;
                default: cout << "Invalid choice. Please try again.\n"; pressEnterToContinue();
            }
        } while (choice != 0);
    }

    void patientsByDoctor() {
        clearScreen();
        cout << "=== PATIENTS BY DOCTOR ===\n";
        cout << "Enter Doctor ID: ";
        string doctorId;
        getline(cin, doctorId);

        auto patients = hospital.getPatientsTreatedByDoctor(doctorId);
        if (patients.empty()) {
            cout << "No patients found for this doctor or doctor doesn't exist.\n";
            pressEnterToContinue();
            return;
        }

        cout << "\nPatients treated by " << doctorId << ":\n";
        for (const auto& pid : patients) {
            auto it = hospital.getPatients().find(pid);
            if (it != hospital.getPatients().end()) {
                cout << "- " << pid << ": " << it->second.data.name << "\n";
            }
        }
        pressEnterToContinue();
    }

    void doctorsByPatient() {
        clearScreen();
        cout << "=== DOCTORS BY PATIENT ===\n";
        cout << "Enter Patient ID: ";
        string patientId;
        getline(cin, patientId);

        auto doctors = hospital.getDoctorsTreatingPatient(patientId);
        if (doctors.empty()) {
            cout << "No doctors found for this patient or patient doesn't exist.\n";
            pressEnterToContinue();
            return;
        }

        cout << "\nDoctors treating " << patientId << ":\n";
        for (const auto& did : doctors) {
            auto it = hospital.getDoctors().find(did);
            if (it != hospital.getDoctors().end()) {
                cout << "- " << did << ": " << it->second.data.name 
                     << " (" << it->second.data.specialization << ")\n";
            }
        }
        pressEnterToContinue();
    }

    void patientsInRoom() {
        clearScreen();
        cout << "=== PATIENTS IN ROOM ===\n";
        cout << "Enter Room ID: ";
        string roomId;
        getline(cin, roomId);

        auto patients = hospital.getPatientsInRoom(roomId);
        if (patients.empty()) {
            cout << "No patients found in this room or room doesn't exist.\n";
            pressEnterToContinue();
            return;
        }

        cout << "\nPatients in room " << roomId << ":\n";
        for (const auto& pid : patients) {
            auto it = hospital.getPatients().find(pid);
            if (it != hospital.getPatients().end()) {
                cout << "- " << pid << ": " << it->second.data.name << "\n";
            }
        }
        pressEnterToContinue();
    }

    void availableRooms() {
        clearScreen();
        cout << "=== AVAILABLE ROOMS ===\n";
        
        bool found = false;
        for (const auto& entry : hospital.getRooms()) {
            if (!entry.second.data.occupied) {
                entry.second.data.display();
                cout << "-------------------\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No available rooms.\n";
        }
        pressEnterToContinue();
    }

    // Data operations
    void dataOperationsMenu() {
        int choice;
        do {
            clearScreen();
            cout << "=== DATA OPERATIONS ===\n";
            cout << "1. Save Data to File\n";
            cout << "2. Load Data from File\n";
            cout << "0. Back to Main Menu\n";
            cout << "=======================\n";
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: 
                    cout << "Enter filename (without extension): ";
                    getline(cin, dataFilename);
                    hospital.saveToCSV(dataFilename); 
                    pressEnterToContinue();
                    break;
                case 2: 
                    cout << "Enter filename (without extension): ";
                    getline(cin, dataFilename);
                    hospital.loadFromCSV(dataFilename); 
                    pressEnterToContinue();
                    break;
                case 0: break;
                default: cout << "Invalid choice. Please try again.\n"; pressEnterToContinue();
            }
        } while (choice != 0);
    }

public:
    void run() {
        // Try to load data automatically at startup
        try {
            hospital.loadFromCSV(dataFilename);
        } catch (const exception& e) {
            cout << "Could not load initial data: " << e.what() << endl;
        }

        int choice;
        do {
            displayMainMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1: patientManagementMenu(); break;
                case 2: /* doctorManagementMenu(); */ break;
                case 3: /* roomManagementMenu(); */ break;
                case 4: manageRelationships(); break;
                case 5: reportsMenu(); break;
                case 6: dataOperationsMenu(); break;
                case 0: 
                    // Save data automatically on exit
                    hospital.saveToCSV(dataFilename);
                    cout << "Data saved. Exiting...\n";
                    break;
                default: cout << "Invalid choice. Please try again.\n"; pressEnterToContinue();
            }
        } while (choice != 0);
    }
};

int main() {
    HospitalUI ui;
    ui.run();
    return 0;
}