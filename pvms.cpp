#include <iostream> // Include input/output stream library for console operations like cin/cout
#include <string> // Include string library for handling string data types
#include <unordered_map> // Include unordered_map for key-value storage (e.g., mapping lot IDs to ParkingLot objects)
#include <unordered_set> // Include unordered_set for tracking occupied parking spots efficiently
#include <vector> // Include vector for storing connections between parking lots
#include <fstream> // Include file stream library for reading/writing data to files
#include <sstream> // Include string stream library for parsing CSV file lines
#include <limits> // Include limits for INT_MIN/INT_MAX used in input validation
#include <algorithm> // Include algorithm for functions like max and remove_if
#include <climits> // Include climits for INT_MAX/INT_MIN constants
#include <ctime> // Include ctime for generating timestamps for parking sessions

using namespace std; // Use standard namespace to avoid prefixing std:: for standard library components

// ======== Utility: Safe Integer Input ========
// Function to safely read an integer input within a specified range
int readInt(const string &prompt, int minVal = INT_MIN, int maxVal = INT_MAX)
{
    while (true) // Infinite loop until valid input is received
    {
        cout << prompt; // Display the input prompt to the user
        string line; // Variable to store the input line as a string
        if (!getline(cin, line)) // Read a line of input; check if input stream is valid
        {
            cin.clear(); // Clear any error flags on the input stream
            continue; // Retry input
        }
        try // Try to convert string to integer
        {
            int val = stoi(line); // Convert input string to integer
            if (val < minVal || val > maxVal) // Check if value is within allowed range
            {
                cout << "Please enter a number"; // Inform user of invalid range
                if (minVal != INT_MIN && maxVal != INT_MAX) // If specific range is provided
                    cout << " between " << minVal << " and " << maxVal; // Include range in error message
                cout << ".\n"; // Complete error message
                continue; // Retry input
            }
            return val; // Return valid integer
        }
        catch (...) // Catch any conversion errors (e.g., non-numeric input)
        {
            cout << "Invalid input. Please enter a valid number.\n"; // Inform user of invalid input
            continue; // Retry input
        }
    }
}

// ======== Data Structures ========
// Structure to represent a vehicle
struct Vehicle
{
    string id; // License plate (e.g., "RAD 220 C")
    string type, owner; // Vehicle type (e.g., "Car") and owner name
    Vehicle *next; // Pointer to next vehicle in linked list
};

// Structure to represent a parking spot
struct ParkingSpot
{
    int id; // Unique ID for the parking spot
    string type; // Type of spot (e.g., "Compact", "Handicap")
    bool isOccupied; // Indicates if the spot is currently occupied
    ParkingSpot *next; // Pointer to next spot in linked list
};

// Structure to represent a parking session
struct ParkingSession
{
    int id; // Unique ID for the session
    string vehicleId; // License plate of the vehicle in the session
    int spotId; // ID of the parking spot used
    string entryTime, exitTime; // Timestamps for entry and exit
    ParkingSession *next; // Pointer to next session in linked list
};

// ======== ParkingLot Class ========
// Class to manage a single parking lot
class ParkingLot
{
public:
    string lotId, name, location; // Unique ID, name, and location of the parking lot
    Vehicle *vehicles = nullptr; // Linked list of vehicles registered in the lot
    ParkingSpot *spots = nullptr; // Linked list of parking spots in the lot
    ParkingSession *sessions = nullptr; // Linked list of parking sessions
    int nextSpotId = 1; // Counter for generating unique spot IDs
    int nextSessionId = 1; // Counter for generating unique session IDs

    // Default constructor
    ParkingLot() = default;

    // Parameterized constructor
    ParkingLot(const string &id, const string &nm, const string &loc)
        : lotId(id), name(nm), location(loc) // Initialize lot ID, name, and location
    {
        loadData(); // Load data from files (vehicles, spots, sessions)
        normalizeCounters(); // Update ID counters based on loaded data
        updateSpotStatuses(); // Update occupancy status of spots based on sessions
    }

    // Register a new vehicle
    bool registerVehicle(const string &lp, const string &t, const string &own)
    {
        if (findVehicle(lp)) // Check if vehicle with license plate already exists
        {
            cout << "Vehicle already exists!\n"; // Inform user of duplicate
            return false; // Return failure
        }
        vehicles = new Vehicle{lp, t, own, vehicles}; // Add new vehicle to front of linked list
        saveData(); // Save updated vehicle list to file
        return true; // Return success
    }

    // Add a new parking spot
    int addParkingSpot(const string &t)
    {
        int id = nextSpotId++; // Generate new spot ID and increment counter
        spots = new ParkingSpot{id, t, false, spots}; // Add new spot to front of linked list
        saveData(); // Save updated spot list to file
        return id; // Return the new spot ID
    }

    // Start a new parking session
    int startParkingSession(const string &vId, int sid, const string &entry)
    {
        if (!findVehicle(vId) || !findSpot(sid)) // Verify vehicle and spot exist
            return -1; // Return -1 if either is invalid
        ParkingSpot *spot = findSpot(sid); // Find the parking spot
        if (spot->isOccupied) // Check if spot is already occupied
            return -2; // Return -2 if spot is occupied
        int id = nextSessionId++; // Generate new session ID and increment counter
        sessions = new ParkingSession{id, vId, sid, entry, "", sessions}; // Add new session to front of linked list
        spot->isOccupied = true; // Mark spot as occupied
        saveData(); // Save updated session and spot data to files
        return id; // Return the new session ID
    }

    // End a parking session
    bool endParkingSession(int sessionId, const string &exit)
    {
        ParkingSession *session = findSession(sessionId); // Find the session by ID
        if (!session || session->exitTime != "") // Check if session exists and is ongoing
            return false; // Return failure if session is invalid or already ended
        session->exitTime = exit; // Set exit timestamp
        ParkingSpot *spot = findSpot(session->spotId); // Find the associated spot
        if (spot) // If spot exists
            spot->isOccupied = false; // Mark spot as available
        saveData(); // Save updated session and spot data to files
        return true; // Return success
    }

    // Delete a vehicle
    bool deleteVehicle(const string &vId)
    {
        Vehicle **ptr = &vehicles; // Pointer to pointer for linked list traversal
        while (*ptr) // Traverse the vehicle list
        {
            if ((*ptr)->id == vId) // If vehicle is found
            {
                // Check for active sessions
                for (auto *s = sessions; s; s = s->next) // Traverse session list
                {
                    if (s->vehicleId == vId && s->exitTime == "") // If vehicle has active session
                    {
                        cout << "Cannot delete vehicle with active session\n"; // Inform user
                        return false; // Return failure
                    }
                }
                Vehicle *temp = *ptr; // Store pointer to vehicle to delete
                *ptr = temp->next; // Update list to skip the deleted vehicle
                delete temp; // Free memory
                saveData(); // Save updated vehicle list to file
                return true; // Return success
            }
            ptr = &(*ptr)->next; // Move to next vehicle
        }
        return false; // Return failure if vehicle not found
    }

    // Delete a parking spot
    bool deleteSpot(int sid)
    {
        ParkingSpot **ptr = &spots; // Pointer to pointer for linked list traversal
        while (*ptr) // Traverse the spot list
        {
            if ((*ptr)->id == sid) // If spot is found
            {
                if ((*ptr)->isOccupied) // Check if spot is occupied
                {
                    cout << "Cannot delete occupied spot\n"; // Inform user
                    return false; // Return failure
                }
                ParkingSpot *temp = *ptr; // Store pointer to spot to delete
                *ptr = temp->next; // Update list to skip the deleted spot
                delete temp; // Free memory
                saveData(); // Save updated spot list to file
                return true; // Return success
            }
            ptr = &(*ptr)->next; // Move to next spot
        }
        return false; // Return failure if spot not found
    }

    // Delete a parking session
    bool deleteSession(int sid)
    {
        ParkingSession **ptr = &sessions; // Pointer to pointer for linked list traversal
        while (*ptr) // Traverse the session list
        {
            if ((*ptr)->id == sid) // If session is found
            {
                ParkingSession *temp = *ptr; // Store pointer to session to delete
                ParkingSpot *spot = findSpot(temp->spotId); // Find associated spot
                if (spot) // If spot exists
                    spot->isOccupied = false; // Mark spot as available
                *ptr = temp->next; // Update list to skip the deleted session
                delete temp; // Free memory
                saveData(); // Save updated session and spot data to files
                return true; // Return success
            }
            ptr = &(*ptr)->next; // Move to next session
        }
        return false; // Return failure if session not found
    }

    // Display all registered vehicles
    void displayVehicles()
    {
        cout << "-- Vehicles in " << name << " (" << lotId << ") --\n"; // Print header
        for (auto *v = vehicles; v; v = v->next) // Traverse vehicle list
            cout << "License: " << v->id << " | Type: " << v->type
                 << " | Owner: " << v->owner << "\n"; // Print vehicle details
    }

    // Display all parking spots
    void displaySpots()
    {
        cout << "-- Parking Spots in " << name << " (" << lotId << ") --\n"; // Print header
        for (auto *s = spots; s; s = s->next) // Traverse spot list
            cout << s->id << ": " << s->type << " | "
                 << (s->isOccupied ? "Occupied" : "Available") << "\n"; // Print spot details
    }

    // Display parking sessions (optionally only current ones)
    void displaySessions(bool currentOnly = false)
    {
        cout << "-- Parking Sessions in " << name << " (" << lotId << ") --\n"; // Print header
        for (auto *s = sessions; s; s = s->next) // Traverse session list
        {
            if (!currentOnly || s->exitTime == "") // Filter for current sessions if specified
                cout << s->id << ": " << s->vehicleId << " @ S" << s->spotId
                     << " | " << s->entryTime << " - "
                     << (s->exitTime == "" ? "Ongoing" : s->exitTime) << "\n"; // Print session details
        }
    }

    // Load data from CSV files
    void loadData()
    {
        loadList<Vehicle>(lotId + "_vehicles.csv", vehicles); // Load vehicles from file
        loadList<ParkingSpot>(lotId + "_spots.csv", spots); // Load spots from file
        loadList<ParkingSession>(lotId + "_sessions.csv", sessions); // Load sessions from file
    }

    // Save data to CSV files
    void saveData()
    {
        saveVehicles(lotId + "_vehicles.csv"); // Save vehicles to file
        saveSpots(lotId + "_spots.csv"); // Save spots to file
        saveSessions(lotId + "_sessions.csv"); // Save sessions to file
    }

private:
    // Find vehicle by license plate
    Vehicle *findVehicle(const string &vId)
    {
        for (auto *v = vehicles; v; v = v->next) // Traverse vehicle list
            if (v->id == vId) // If vehicle matches license plate
                return v; // Return pointer to vehicle
        return nullptr; // Return null if not found
    }

    // Find parking spot by ID
    ParkingSpot *findSpot(int id)
    {
        for (auto *s = spots; s; s = s->next) // Traverse spot list
            if (s->id == id) // If spot matches ID
                return s; // Return pointer to spot
        return nullptr; // Return null if not found
    }

    // Find parking session by ID
    ParkingSession *findSession(int id)
    {
        for (auto *s = sessions; s; s = s->next) // Traverse session list
            if (s->id == id) // If session matches ID
                return s; // Return pointer to session
        return nullptr; // Return null if not found
    }

    // Update occupancy status of spots based on active sessions
    void updateSpotStatuses()
    {
        unordered_set<int> occupiedSpots; // Set to track occupied spot IDs
        for (auto *s = sessions; s; s = s->next) // Traverse session list
        {
            if (s->exitTime == "") // If session is ongoing
            {
                occupiedSpots.insert(s->spotId); // Add spot ID to set
            }
        }
        for (auto *spot = spots; spot; spot = spot->next) // Traverse spot list
        {
            spot->isOccupied = occupiedSpots.count(spot->id); // Update occupancy status
        }
    }

    // Template function to load linked list from CSV file
    template <typename T>
    void loadList(const string &fn, T *&head)
    {
        ifstream f(fn); // Open input file stream
        if (!f) // If file cannot be opened
            return; // Exit function
        string line; // Buffer for reading lines
        getline(f, line); // Skip header line
        while (getline(f, line)) // Read each data line
        {
            stringstream ss(line); // Create string stream for parsing
            vector<string> cols; // Vector to store CSV columns
            string tok; // Buffer for each token
            while (getline(ss, tok, ',')) // Parse columns delimited by commas
                cols.push_back(tok); // Add column to vector

            // Handle different types based on template parameter
            if constexpr (is_same<T, Vehicle>::value) // If loading Vehicle
            {
                head = new Vehicle{cols[0], cols[1], cols[2], head}; // Create new Vehicle node
            }
            else if constexpr (is_same<T, ParkingSpot>::value) // If loading ParkingSpot
            {
                head = new ParkingSpot{stoi(cols[0]), cols[1], cols[2] == "1", head}; // Create new ParkingSpot node
            }
            else // If loading ParkingSession
            {
                head = new ParkingSession{stoi(cols[0]), cols[1],
                                          stoi(cols[2]), cols[3], cols[4], head}; // Create new ParkingSession node
            }
        }
    }

    // Save vehicles to CSV file
    void saveVehicles(const string &fn)
    {
        ofstream f(fn); // Open output file stream
        f << "license_plate,type,owner\n"; // Write header
        for (auto *v = vehicles; v; v = v->next) // Traverse vehicle list
            f << v->id << ',' << v->type << ',' << v->owner << "\n"; // Write vehicle data
    }

    // Save parking spots to CSV file
    void saveSpots(const string &fn)
    {
        ofstream f(fn); // Open output file stream
        f << "id,type,is_occupied\n"; // Write header
        for (auto *s = spots; s; s = s->next) // Traverse spot list
            f << s->id << ',' << s->type << ',' << (s->isOccupied ? 1 : 0) << "\n"; // Write spot data
    }

    // Save parking sessions to CSV file
    void saveSessions(const string &fn)
    {
        ofstream f(fn); // Open output file stream
        f << "id,vehicle_id,spot_id,entry_time,exit_time\n"; // Write header
        for (auto *s = sessions; s; s = s->next) // Traverse session list
            f << s->id << ',' << s->vehicleId << ',' << s->spotId << ','
              << s->entryTime << ',' << s->exitTime << "\n"; // Write session data
    }

    // Update ID counters based on loaded data
    void normalizeCounters()
    {
        for (auto *s = spots; s; s = s->next) // Traverse spot list
            nextSpotId = max(nextSpotId, s->id + 1); // Update nextSpotId to max ID + 1
        for (auto *s = sessions; s; s = s->next) // Traverse session list
            nextSessionId = max(nextSessionId, s->id + 1); // Update nextSessionId to max ID + 1
    }
};

// ======== ParkingNetwork Class ========
// Class to manage a network of parking lots
class ParkingNetwork
{
private:
    // Check if two parking lots are connected
    bool areConnected(const string &a, const string &b)
    {
        if (adj.find(a) == adj.end()) // If lot 'a' has no connections
            return false; // Not connected
        for (const auto &edge : adj[a]) // Check connections of lot 'a'
            if (edge.first == b) // If connected to lot 'b'
                return true; // Return true
        return false; // Not connected
    }

public:
    unordered_map<string, ParkingLot *> nodes; // Map of lot IDs to ParkingLot objects
    unordered_map<string, vector<pair<string, int>>> adj; // Adjacency list for lot connections (lot ID to list of {connected lot ID, distance})
    int nextLotIndex = 1; // Counter for generating unique lot IDs

    // Constructor
    ParkingNetwork()
    {
        loadLots(); // Load parking lots from file
        loadConnections(); // Load connections between lots from file
    }

    // Add a new parking lot
    void addParkingLot()
    {
        cout << "Name: "; // Prompt for lot name
        string nm; // Variable for name
        getline(cin, nm); // Read name
        cout << "Location: "; // Prompt for location
        string loc; // Variable for location
        getline(cin, loc); // Read location
        string id = genId(); // Generate unique lot ID
        nodes[id] = new ParkingLot(id, nm, loc); // Create new ParkingLot object
        saveLots(); // Save updated lot list to file
        saveConnections(); // Save connections (empty for new lot)
        cout << "Added: " << id << "\n"; // Confirm addition
    }

    // Update an existing parking lot's details
    void updateParkingLot()
    {
        cout << "Parking Lot ID to update: "; // Prompt for lot ID
        string id; // Variable for ID
        getline(cin, id); // Read ID

        if (!nodes.count(id)) // Check if lot exists
        {
            cout << "Parking lot not found.\n"; // Inform user if not found
            return; // Exit function
        }

        ParkingLot *lot = nodes[id]; // Get pointer to the lot
        cout << "\nCurrent Information:\n"; // Display header
        cout << "ID: " << lot->lotId << "\n"; // Show current ID
        cout << "Name: " << lot->name << "\n"; // Show current name
        cout << "Location: " << lot->location << "\n\n"; // Show current location

        cout << "What would you like to update?\n"; // Prompt for update choice
        cout << "1. Name\n"; // Option to update name
        cout << "2. Location\n"; // Option to update location
        cout << "3. Both Name and Location\n"; // Option to update both

        int choice = readInt("Choose: ", 1, 3); // Read user's choice (1-3)

        string newName = lot->name; // Initialize new name with current name
        string newLocation = lot->location; // Initialize new location with current location

        if (choice == 1 || choice == 3) // If updating name or both
        {
            cout << "Enter new name (current: " << lot->name << "): "; // Prompt for new name
            getline(cin, newName); // Read new name
            if (newName.empty()) // Check if name is empty
            {
                cout << "Name cannot be empty. Keeping current name.\n"; // Inform user
                newName = lot->name; // Revert to current name
            }
        }

        if (choice == 2 || choice == 3) // If updating location or both
        {
            cout << "Enter new location (current: " << lot->location << "): "; // Prompt for new location
            getline(cin, newLocation); // Read new location
            if (newLocation.empty()) // Check if location is empty
            {
                cout << "Location cannot be empty. Keeping current location.\n"; // Inform user
                newLocation = lot->location; // Revert to current location
            }
        }

        lot->name = newName; // Update lot name
        lot->location = newLocation; // Update lot location

        saveLots(); // Save updated lot list to file

        cout << "\nParking lot updated successfully!\n"; // Confirm update
        cout << "Updated Information:\n"; // Display updated info header
        cout << "ID: " << lot->lotId << "\n"; // Show updated ID
        cout << "Name: " << lot->name << "\n"; // Show updated name
        cout << "Location: " << lot->location << "\n"; // Show updated location
    }

    // Delete a parking lot
    void deleteParkingLot()
    {
        cout << "Lot ID to delete: "; // Prompt for lot ID
        string id; // Variable for ID
        getline(cin, id); // Read ID
        if (!nodes.count(id)) // Check if lot exists
        {
            cout << "Not found.\n"; // Inform user if not found
            return; // Exit function
        }

        // Delete associated files
        remove((id + "_vehicles.csv").c_str()); // Delete vehicles file
        remove((id + "_spots.csv").c_str()); // Delete spots file
        remove((id + "_sessions.csv").c_str()); // Delete sessions file

        delete nodes[id]; // Free memory for ParkingLot object
        nodes.erase(id); // Remove lot from nodes map
        adj.erase(id); // Remove lot from adjacency list

        // Remove connections to this lot from other lots
        for (auto &kv : adj) // Traverse all lots in adjacency list
        {
            auto &v = kv.second; // Get connection vector
            v.erase(remove_if(v.begin(), v.end(),
                              [&](auto &p) { return p.first == id; }), // Remove connections to deleted lot
                    v.end());
        }

        saveLots(); // Save updated lot list
        saveConnections(); // Save updated connections
        cout << "Deleted " << id << "\n"; // Confirm deletion
    }

    // Connect two parking lots
    void connectParkingLots()
    {
        cout << "From Lot ID: "; // Prompt for source lot ID
        string a; // Variable for source ID
        getline(cin, a); // Read source ID
        cout << "To Lot ID:   "; // Prompt for destination lot ID
        string b; // Variable for destination ID
        getline(cin, b); // Read destination ID
        if (!nodes.count(a) || !nodes.count(b)) // Check if both lots exist
        {
            cout << "Invalid IDs.\n"; // Inform user
            return; // Exit function
        }
        if (areConnected(a, b)) // Check if lots are already connected
        {
            cout << "Lots are already connected.\n"; // Inform user
            return; // Exit function
        }
        int dist = readInt("Distance (meters): ", 0); // Read distance (non-negative)
        adj[a].push_back({b, dist}); // Add connection from a to b
        adj[b].push_back({a, dist}); // Add connection from b to a (undirected)
        saveConnections(); // Save updated connections
        cout << "Connected " << a << " <-> " << b << "\n"; // Confirm connection
    }

    // List all parking lots
    void listParkingLots()
    {
        cout << "-- Parking Lots --\n"; // Print header
        for (auto &kv : nodes) // Traverse all lots
            cout << kv.first << " | " << kv.second->name
                 << " | " << kv.second->location << "\n"; // Print lot details
    }

    // Display network connections
    void displayNetwork()
    {
        cout << "-- Parking Network --\n"; // Print header
        for (auto &kv : adj) // Traverse adjacency list
        {
            cout << kv.first << " -> "; // Print source lot ID
            for (auto &e : kv.second) // Traverse connections
                cout << e.first << "(" << e.second << "m) "; // Print connected lot and distance
            cout << "\n"; // Newline after connections
        }
    }

    // Manage a specific parking lot
    void manageParkingLot()
    {
        cout << "Parking Lot ID: "; // Prompt for lot ID
        string lid; // Variable for lot ID
        getline(cin, lid); // Read lot ID
        if (!nodes.count(lid)) // Check if lot exists
        {
            cout << "Not found.\n"; // Inform user
            return; // Exit function
        }
        ParkingLot *lot = nodes[lid]; // Get pointer to the lot
        while (true) // Loop for lot management menu
        {
            cout << "\n-- Managing " << lot->name << " (" << lid << ") --\n" // Display menu header
                 << "1. Register Vehicle\n" // Option to register vehicle
                 << "2. Add Parking Spot\n" // Option to add spot
                 << "3. Start Parking Session\n" // Option to start session
                 << "4. End Parking Session\n" // Option to end session
                 << "5. Display Vehicles\n" // Option to display vehicles
                 << "6. Display Spots\n" // Option to display spots
                 << "7. Display Current Sessions\n" // Option to display current sessions
                 << "8. Display All Sessions\n" // Option to display all sessions
                 << "9. View Connections\n" // Option to view lot connections
                 << "10. Delete Vehicle\n" // Option to delete vehicle
                 << "11. Delete Spot\n" // Option to delete spot
                 << "12. Delete Session\n" // Option to delete session
                 << "13. Go Back\n"; // Option to exit menu
            int c = readInt("Choose: ", 1, 13); // Read user's choice (1-13)
            if (c == 13) // If user chooses to go back
                break; // Exit loop
            switch (c) // Handle menu choice
            {
            case 1: // Register Vehicle
            {
                cout << "License Plate: "; // Prompt for license plate
                string lp; // Variable for license plate
                getline(cin, lp); // Read license plate
                cout << "Vehicle Type: "; // Prompt for vehicle type
                string t; // Variable for type
                getline(cin, t); // Read type
                cout << "Owner Name: "; // Prompt for owner name
                string own; // Variable for owner
                getline(cin, own); // Read owner
                if (lot->registerVehicle(lp, t, own)) // Attempt to register vehicle
                    cout << "Vehicle registered\n"; // Confirm success
                else
                    cout << "Registration failed\n"; // Inform failure
                break;
            }
            case 2: // Add Parking Spot
            {
                cout << "Spot Type: "; // Prompt for spot type
                string t; // Variable for type
                getline(cin, t); // Read type
                cout << "Added Spot " << lot->addParkingSpot(t) << "\n"; // Add spot and display ID
                break;
            }
            case 3: // Start Parking Session
            {
                cout << "Vehicle License: "; // Prompt for vehicle license
                string vId; // Variable for license
                getline(cin, vId); // Read license
                int sid = readInt("Spot ID: ", 1); // Read spot ID (positive)
                time_t now = time(0); // Get current time
                string entry = ctime(&now); // Convert to string
                entry.erase(entry.find('\n')); // Remove newline
                int id = lot->startParkingSession(vId, sid, entry); // Start session
                if (id == -1) // If vehicle or spot invalid
                    cout << "Invalid IDs\n"; // Inform user
                else if (id == -2) // If spot occupied
                    cout << "Spot occupied\n"; // Inform user
                else
                    cout << "Session started: " << id << "\n"; // Confirm session start
                break;
            }
            case 4: // End Parking Session
            {
                int sid = readInt("Session ID: ", 1); // Read session ID (positive)
                time_t now = time(0); // Get current time
                string exit = ctime(&now); // Convert to string
                exit.erase(exit.find('\n')); // Remove newline
                if (lot->endParkingSession(sid, exit)) // Attempt to end session
                    cout << "Session ended\n"; // Confirm success
                else
                    cout << "Invalid session or already ended\n"; // Inform failure
                break;
            }
            case 5: // Display Vehicles
                lot->displayVehicles(); // Call display function
                break;
            case 6: // Display Spots
                lot->displaySpots(); // Call display function
                break;
            case 7: // Display Current Sessions
                lot->displaySessions(true); // Call display function for current sessions
                break;
            case 8: // Display All Sessions
                lot->displaySessions(); // Call display function for all sessions
                break;
            case 9: // View Connections
            {
                cout << "-- Connections from " << lid << " --\n"; // Print header
                for (auto &e : adj[lid]) // Traverse connections for this lot
                    cout << e.first << "(" << e.second << "m)\n"; // Print connected lot and distance
                break;
            }
            case 10: // Delete Vehicle
            {
                cout << "Vehicle License: "; // Prompt for license
                string vId; // Variable for license
                getline(cin, vId); // Read license
                if (lot->deleteVehicle(vId)) // Attempt to delete vehicle
                    cout << "Vehicle deleted\n"; // Confirm success
                else
                    cout << "Delete failed\n"; // Inform failure
                break;
            }
            case 11: // Delete Spot
            {
                int sid = readInt("Spot ID: ", 1); // Read spot ID (positive)
                if (lot->deleteSpot(sid)) // Attempt to delete spot
                    cout << "Spot deleted\n"; // Confirm success
                else
                    cout << "Delete failed\n"; // Inform failure
                break;
            }
            case 12: // Delete Session
            {
                int sid = readInt("Session ID: ", 1); // Read session ID (positive)
                if (lot->deleteSession(sid)) // Attempt to delete session
                    cout << "Session deleted\n"; // Confirm success
                else
                    cout << "Delete failed\n"; // Inform failure
                break;
            }
            }
        }
    }

private:
    // Generate unique lot ID
    string genId() { return "L" + to_string(nextLotIndex++); } // Return "L" + incremented index

    // Load parking lots from CSV file
    void loadLots()
    {
        ifstream f("parking_lots.csv"); // Open lots file
        if (!f) // If file cannot be opened
            return; // Exit function
        string line; // Buffer for reading lines
        getline(f, line); // Skip header line
        int maxIdx = 0; // Track max lot index
        while (getline(f, line)) // Read each data line
        {
            stringstream ss(line); // Create string stream for parsing
            string id, nm, loc; // Variables for ID, name, location
            getline(ss, id, ','); // Read ID
            getline(ss, nm, ','); // Read name
            getline(ss, loc, ','); // Read location
            maxIdx = max(maxIdx, stoi(id.substr(1))); // Update max index from ID
            nodes[id] = new ParkingLot(id, nm, loc); // Create new ParkingLot
        }
        nextLotIndex = maxIdx + 1; // Set next index for new lots
    }

    // Save parking lots to CSV file
    void saveLots()
    {
        ofstream f("parking_lots.csv"); // Open output file
        f << "id,name,location\n"; // Write header
        for (auto &kv : nodes) // Traverse all lots
            f << kv.first << ',' << kv.second->name
              << ',' << kv.second->location << "\n"; // Write lot data
    }

    // Load connections between lots from CSV file
    void loadConnections()
    {
        ifstream f("connections.csv"); // Open connections file
        if (!f) // If file cannot be opened
            return; // Exit function
        string line; // Buffer for reading lines
        getline(f, line); // Skip header line
        while (getline(f, line)) // Read each data line
        {
            stringstream ss(line); // Create string stream for parsing
            string a, b; // Variables for lot IDs
            int d; // Variable for distance
            getline(ss, a, ','); // Read source lot ID
            getline(ss, b, ','); // Read destination lot ID
            ss >> d; // Read distance
            if (nodes.count(a) && nodes.count(b)) // If both lots exist
            {
                adj[a].push_back({b, d}); // Add connection a->b
                adj[b].push_back({a, d}); // Add connection b->a
            }
        }
    }

    // Save connections to CSV file
    void saveConnections()
    {
        ofstream f("connections.csv"); // Open output file
        f << "from,to,distance\n"; // Write header
        for (auto &kv : adj) // Traverse adjacency list
        {
            const string &a = kv.first; // Get source lot ID
            for (auto &e : kv.second) // Traverse connections
            {
                const string &b = e.first; // Get destination lot ID
                int d = e.second; // Get distance
                if (a < b) // Write only one direction to avoid duplicates
                    f << a << ',' << b << ',' << d << "\n"; // Write connection data
            }
        }
    }
};

// ======== Main Function ========
// Program entry point
int main()
{
    ParkingNetwork pn; // Create ParkingNetwork object
    while (true) // Main menu loop
    {
        cout << "\n=== Parking Management System ===\n" // Display menu header
             << "1. Add Parking Lot\n" // Option to add lot
             << "2. Update Parking Lot\n" // Option to update lot
             << "3. Manage Parking Lot\n" // Option to manage a specific lot
             << "4. Connect Parking Lots\n" // Option to connect lots
             << "5. List Parking Lots\n" // Option to list all lots
             << "6. Display Network\n" // Option to display connections
             << "7. Delete Parking Lot\n" // Option to delete lot
             << "8. Exit\n"; // Option to exit program
        int choice = readInt("Choose: ", 1, 8); // Read user's choice (1-8)
        if (choice == 8) // If user chooses to exit
            break; // Exit loop
        switch (choice) // Handle menu choice
        {
        case 1: // Add Parking Lot
            pn.addParkingLot(); // Call add function
            break;
        case 2: // Update Parking Lot
            pn.updateParkingLot(); // Call update function
            break;
        case 3: // Manage Parking Lot
            pn.manageParkingLot(); // Call manage function
            break;
        case 4: // Connect Parking Lots
            pn.connectParkingLots(); // Call connect function
            break;
        case 5: // List Parking Lots
            pn.listParkingLots(); // Call list function
            break;
        case 6: // Display Network
            pn.displayNetwork(); // Call display function
            break;
        case 7: // Delete Parking Lot
            pn.deleteParkingLot(); // Call delete function
            break;
        }
    }
    cout << "Goodbye!\n"; // Print exit message
    return 0; // Exit program
}