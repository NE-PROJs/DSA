#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <set>
#include <algorithm>
#include <cctype>
using namespace std;

const int MAX = 1000;
const float INF = 1e9;

// Structure for a health center (node)
struct HealthCenter {
    int id;
    string name;
    string district;
    float lat, lon;
    int capacity;
    string toCSV() const {
        return to_string(id) + "," + name + "," + district + "," + to_string(lat) + "," + to_string(lon) + "," + to_string(capacity);
    }
    string description() const {
        return "Name: " + name + ", District: " + district + ", Capacity: " + to_string(capacity);
    }
};

// Structure for a connection (edge)
struct Connection {
    int to;
    float distance;
    int time;
    string description; // Added for connection description
};

// Global data structures
vector<HealthCenter> centers;
vector<Connection> adjList[MAX];
vector<vector<float>> fwDistances; // For Floyd-Warshall

// Utility function for numeric validation
bool isNumber(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
}

// File I/O Functions
void createHealthCentersFile(const string& filename) {
    ofstream file(filename);
    file << "ID,Name,District,Latitude,Longitude,Capacity\n";
    file.close();
}

void createConnectionsFile(const string& filename) {
    ofstream file(filename);
    file << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
    file.close();
}

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

void readHealthCenters(const string& filename) {
    if (!fileExists(filename)) {
        cout << filename << " does not exist. Creating new file.\n";
        createHealthCentersFile(filename);
        return;
    }
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        HealthCenter hc;
        string temp;
        try {
            getline(ss, temp, ','); hc.id = stoi(temp);
            getline(ss, hc.name, ',');
            getline(ss, hc.district, ',');
            getline(ss, temp, ','); hc.lat = stof(temp);
            getline(ss, temp, ','); hc.lon = stof(temp);
            getline(ss, temp, ','); hc.capacity = stoi(temp);
            centers.push_back(hc);
        } catch (const invalid_argument& e) {
            cout << "Skipping invalid line in " << filename << ": " << line << endl;
            continue;
        } catch (const out_of_range& e) {
            cout << "Skipping out-of-range values in " << filename << ": " << line << endl;
            continue;
        }
    }
    file.close();
}

void readConnections(const string& filename) {
    if (!fileExists(filename)) {
        cout << filename << " does not exist. Creating new file.\n";
        createConnectionsFile(filename);
        return;
    }
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        int from, to, time;
        float distance;
        string desc, temp;
        try {
            getline(ss, temp, ','); from = stoi(temp);
            getline(ss, temp, ','); to = stoi(temp);
            getline(ss, temp, ','); distance = stof(temp);
            getline(ss, temp, ','); time = stoi(temp);
            getline(ss, desc); // Read description
            adjList[from].push_back({to, distance, time, desc});
            adjList[to].push_back({from, distance, time, desc}); // Undirected
        } catch (const invalid_argument& e) {
            cout << "Skipping invalid line in " << filename << ": " << line << endl;
            continue;
        } catch (const out_of_range& e) {
            cout << "Skipping out-of-range values in " << filename << ": " << line << endl;
            continue;
        }
    }
    file.close();
}

void saveHealthCenters(const string& filename) {
    ofstream file(filename);
    file << "ID,Name,District,Latitude,Longitude,Capacity\n";
    for (const auto& hc : centers) {
        file << hc.id << "," << hc.name << "," << hc.district << ","
             << fixed << setprecision(4) << hc.lat << "," << hc.lon << "," << hc.capacity << "\n";
    }
    file.close();
}

void saveConnections(const string& filename) {
    ofstream file(filename);
    file << "FromID,ToID,DistanceKM,TimeMinutes,Description\n";
    set<pair<int, int>> written; // To avoid duplicate edges
    for (int i = 0; i < MAX; ++i) {
        for (const auto& c : adjList[i]) {
            if (written.find({min(i, c.to), max(i, c.to)}) == written.end()) {
                file << i << "," << c.to << "," << fixed << setprecision(2) << c.distance << "," << c.time << "," << c.description << "\n";
                written.insert({min(i, c.to), max(i, c.to)});
            }
        }
    }
    file.close();
}

// CRUD Operations
void addHealthCenter() {
    HealthCenter hc;
    string idStr, capacityStr;
    cout << "Enter ID: "; cin >> idStr;
    if (!isNumber(idStr)) {
        cout << "Error: ID must be a number.\n";
        return;
    }
    hc.id = stoi(idStr);
    for (const auto& center : centers) {
        if (center.id == hc.id) {
            cout << "ID already exists!\n";
            return;
        }
    }
    cout << "Enter Name: "; cin.ignore(); getline(cin, hc.name);
    cout << "Enter District: "; getline(cin, hc.district);
    cout << "Enter Latitude: "; cin >> hc.lat;
    cout << "Enter Longitude: "; cin >> hc.lon;
    cout << "Enter Capacity: "; cin >> capacityStr;
    if (!isNumber(capacityStr)) {
        cout << "Error: Capacity must be a number.\n";
        return;
    }
    hc.capacity = stoi(capacityStr);
    centers.push_back(hc);
    saveHealthCenters("health_centers.csv");
    cout << "Health center added.\n";
}

void editHealthCenter(int id) {
    for (auto& hc : centers) {
        if (hc.id == id) {
            cout << "Editing Health Center ID " << id << "\n";
            cout << "Enter new Name (current: " << hc.name << "): ";
            cin.ignore(); getline(cin, hc.name);
            cout << "Enter new District (current: " << hc.district << "): ";
            getline(cin, hc.district);
            cout << "Enter new Latitude (current: " << hc.lat << "): ";
            cin >> hc.lat;
            cout << "Enter new Longitude (current: " << hc.lon << "): ";
            cin >> hc.lon;
            string capacityStr;
            cout << "Enter new Capacity (current: " << hc.capacity << "): ";
            cin >> capacityStr;
            if (!isNumber(capacityStr)) {
                cout << "Error: Capacity must be a number.\n";
                return;
            }
            hc.capacity = stoi(capacityStr);
            saveHealthCenters("health_centers.csv");
            cout << "Health center updated.\n";
            return;
        }
    }
    cout << "Health center with ID " << id << " not found.\n";
}

void viewHealthCenters() {
    if (centers.empty()) {
        cout << "No health centers available.\n";
        return;
    }
    cout << "\nHealth Centers:\n";
    cout << "ID  | Name                  | District      | Latitude | Longitude | Capacity\n";
    cout << "----|-----------------------|---------------|----------|-----------|---------\n";
    for (const auto& hc : centers) {
        cout << left << setw(4) << hc.id << "| "
             << setw(22) << hc.name.substr(0, 21) << "| "
             << setw(14) << hc.district.substr(0, 13) << "| "
             << fixed << setprecision(4) << setw(8) << hc.lat << "| "
             << setw(9) << hc.lon << "| "
             << hc.capacity << "\n";
    }
}

void removeHealthCenter(int id) {
    auto it = remove_if(centers.begin(), centers.end(), [id](const HealthCenter& hc) {
        return hc.id == id;
    });
    if (it == centers.end()) {
        cout << "Health center not found.\n";
        return;
    }
    centers.erase(it, centers.end());
    adjList[id].clear();
    for (int i = 0; i < MAX; ++i) {
        adjList[i].erase(remove_if(adjList[i].begin(), adjList[i].end(), [id](const Connection& c) {
            return c.to == id;
        }), adjList[i].end());
    }
    saveHealthCenters("health_centers.csv");
    saveConnections("connections.csv");
    cout << "Health center removed.\n";
}

void addConnection(int from, int to, float distance, int time, const string& desc) {
    bool fromExists = false, toExists = false;
    for (const auto& hc : centers) {
        if (hc.id == from) fromExists = true;
        if (hc.id == to) toExists = true;
    }
    if (!fromExists || !toExists) {
        cout << "Invalid health center ID(s).\n";
        return;
    }
    for (const auto& c : adjList[from]) {
        if (c.to == to) {
            cout << "Connection already exists.\n";
            return;
        }
    }
    adjList[from].push_back({to, distance, time, desc});
    adjList[to].push_back({from, distance, time, desc}); // Undirected
    saveConnections("connections.csv");
    cout << "Connection added.\n";
}

void editConnection(int from, int to) {
    bool found = false;
    for (auto& c : adjList[from]) {
        if (c.to == to) {
            cout << "Editing Connection from " << from << " to " << to << "\n";
            cout << "Enter new DistanceKM (current: " << c.distance << "): ";
            cin >> c.distance;
            cout << "Enter new TimeMinutes (current: " << c.time << "): ";
            cin >> c.time;
            cout << "Enter new Description (current: " << c.description << "): ";
            cin.ignore(); getline(cin, c.description);
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "Connection from " << from << " to " << to << " not found.\n";
        return;
    }
    for (auto& c : adjList[to]) {
        if (c.to == from) {
            cout << "Enter new DistanceKM (current: " << c.distance << "): ";
            cin >> c.distance;
            cout << "Enter new TimeMinutes (current: " << c.time << "): ";
            cin >> c.time;
            cout << "Enter new Description (current: " << c.description << "): ";
            cin.ignore(); getline(cin, c.description);
            break;
        }
    }
    saveConnections("connections.csv");
    cout << "Connection updated.\n";
}

void viewConnections() {
    set<pair<int, int>> displayed;
    bool hasConnections = false;
    cout << "\nConnections:\n";
    cout << "FromID | ToID | DistanceKM | TimeMinutes | Description\n";
    cout << "-------|------|------------|-------------|------------\n";
    for (int i = 0; i < MAX; ++i) {
        for (const auto& c : adjList[i]) {
            if (displayed.find({min(i, c.to), max(i, c.to)}) == displayed.end()) {
                cout << left << setw(7) << i << "| "
                     << setw(5) << c.to << "| "
                     << fixed << setprecision(2) << setw(10) << c.distance << "| "
                     << setw(11) << c.time << "| "
                     << c.description << "\n";
                displayed.insert({min(i, c.to), max(i, c.to)});
                hasConnections = true;
            }
        }
    }
    if (!hasConnections) {
        cout << "No connections available.\n";
    }
}

void viewRelationships() {
    ofstream out("relationship_table.csv");
    out << "HealthCenter,ConnectedCenters,Description\n";
    cout << "\nHealth Center | Connected Centers        | Description\n";
    cout << "--------------|-------------------------|-------------------\n";
    for (const auto& hc : centers) {
        string links;
        for (const auto& c : adjList[hc.id]) {
            links += to_string(c.to) + "(" + c.description + ") ";
        }
        if (links.empty()) links = "None";
        string desc = hc.description();
        cout << left << setw(14) << hc.id << "| " << setw(24) << links.substr(0, 23) << "| " << desc << "\n";
        out << hc.id << "," << links << "," << desc << "\n";
    }
    out.close();
    cout << "\nRelationship table saved to 'relationship_table.csv'.\n";
}

void removeConnection(int from, int to) {
    auto it = remove_if(adjList[from].begin(), adjList[from].end(), [to](const Connection& c) {
        return c.to == to;
    });
    if (it == adjList[from].end()) {
        cout << "Connection not found.\n";
        return;
    }
    adjList[from].erase(it, adjList[from].end());
    adjList[to].erase(remove_if(adjList[to].begin(), adjList[to].end(), [from](const Connection& c) {
        return c.to == from;
    }), adjList[to].end());
    saveConnections("connections.csv");
    cout << "Connection removed.\n";
}

// Graph Algorithms
void dijkstra(int start, int end) {
    vector<float> dist(MAX, INF);
    vector<int> prev(MAX, -1);
    dist[start] = 0;
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        float d = pq.top().first;
        pq.pop();
        if (d > dist[u]) continue;
        for (const auto& c : adjList[u]) {
            if (dist[c.to] > dist[u] + c.distance) {
                dist[c.to] = dist[u] + c.distance;
                prev[c.to] = u;
                pq.push({dist[c.to], c.to});
            }
        }
    }

    if (dist[end] == INF) {
        cout << "No path from " << start << " to " << end << ".\n";
        return;
    }

    cout << "Shortest Distance from " << start << " to " << end << ": " << dist[end] << " km\n";
    cout << "Path: ";
    vector<int> path;
    for (int at = end; at != -1; at = prev[at]) path.push_back(at);
    for (int i = path.size() - 1; i >= 0; --i) cout << path[i] << (i > 0 ? " -> " : "\n");
}

void bfs(int start) {
    vector<bool> visited(MAX, false);
    queue<int> q;
    visited[start] = true;
    q.push(start);
    cout << "BFS Traversal: ";
    while (!q.empty()) {
        int u = q.front(); q.pop();
        cout << u << " ";
        for (const auto& c : adjList[u]) {
            if (!visited[c.to]) {
                visited[c.to] = true;
                q.push(c.to);
            }
        }
    }
    cout << endl;
}

bool dfsCycle(int u, int parent, vector<bool>& visited) {
    visited[u] = true;
    for (const auto& c : adjList[u]) {
        if (!visited[c.to]) {
            if (dfsCycle(c.to, u, visited)) return true;
        } else if (c.to != parent) return true;
    }
    return false;
}

void detectCycle() {
    vector<bool> visited(MAX, false);
    bool hasCycle = false;
    for (int i = 0; i < MAX; ++i) {
        if (!adjList[i].empty() && !visited[i]) {
            if (dfsCycle(i, -1, visited)) {
                hasCycle = true;
                break;
            }
        }
    }
    cout << (hasCycle ? "Cycle detected in the network.\n" : "No cycle found in the network.\n");
}

void floydWarshall(int numCenters) {
    fwDistances.assign(MAX, vector<float>(MAX, INF));
    for (int i = 0; i < MAX; ++i) fwDistances[i][i] = 0;

    for (int i = 0; i < MAX; ++i) {
        for (const auto& c : adjList[i]) {
            fwDistances[i][c.to] = c.distance;
            fwDistances[c.to][i] = c.distance;
        }
    }

    for (int k = 0; k < numCenters; ++k) {
        for (int i = 0; i < numCenters; ++i) {
            for (int j = 0; j < numCenters; ++j) {
                if (fwDistances[i][k] != INF && fwDistances[k][j] != INF) {
                    fwDistances[i][j] = min(fwDistances[i][j], fwDistances[i][k] + fwDistances[k][j]);
                }
            }
        }
    }

    cout << "Floyd-Warshall All-Pairs Shortest Paths:\n";
    for (const auto& hc1 : centers) {
        for (const auto& hc2 : centers) {
            if (hc1.id != hc2.id && fwDistances[hc1.id][hc2.id] != INF) {
                cout << "From " << hc1.id << " to " << hc2.id << ": " << fwDistances[hc1.id][hc2.id] << " km\n";
            }
        }
    }
}

void primMST(int numCenters) {
    vector<bool> inMST(MAX, false);
    vector<float> key(MAX, INF);
    vector<int> parent(MAX, -1);
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;

    if (centers.empty()) {
        cout << "No health centers available for MST.\n";
        return;
    }

    int start = centers[0].id;
    key[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (inMST[u]) continue;
        inMST[u] = true;

        for (const auto& c : adjList[u]) {
            if (!inMST[c.to] && c.distance < key[c.to]) {
                key[c.to] = c.distance;
                parent[c.to] = u;
                pq.push({key[c.to], c.to});
            }
        }
    }

    cout << "Minimum Spanning Tree Edges:\n";
    float totalCost = 0;
    for (int i = 0; i < numCenters; ++i) {
        if (parent[centers[i].id] != -1) {
            cout << parent[centers[i].id] << " - " << centers[i].id << ": " << key[centers[i].id] << " km\n";
            totalCost += key[centers[i].id];
        }
    }
    cout << "Total MST Cost: " << totalCost << " km\n";
}

void emergencyRouting(int start, int minCapacity) {
    vector<float> dist(MAX, INF);
    vector<int> prev(MAX, -1);
    dist[start] = 0;
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        float d = pq.top().first;
        pq.pop();
        if (d > dist[u]) continue;
        for (const auto& c : adjList[u]) {
            if (dist[c.to] > dist[u] + c.distance) {
                dist[c.to] = dist[u] + c.distance;
                prev[c.to] = u;
                pq.push({dist[c.to], c.to});
            }
        }
    }

    int bestCenter = -1;
    float minDist = INF;
    for (const auto& hc : centers) {
        if (hc.capacity >= minCapacity && dist[hc.id] < minDist) {
            minDist = dist[hc.id];
            bestCenter = hc.id;
        }
    }

    if (bestCenter == -1) {
        cout << "No health center with capacity >= " << minCapacity << " found.\n";
        return;
    }

    cout << "Nearest health center with capacity >= " << minCapacity << ": ID " << bestCenter << ", Distance: " << minDist << " km\n";
    cout << "Path: ";
    vector<int> path;
    for (int at = bestCenter; at != -1; at = prev[at]) path.push_back(at);
    for (int i = path.size() - 1; i >= 0; --i) cout << path[i] << (i > 0 ? " -> " : "\n");
}

// Main Menu
int main() {
    readHealthCenters("health_centers.csv");
    readConnections("connections.csv");

    int choice;
    while (true) {
        cout << "\n==== Health Center Network System ====\n";
        cout << "1. Add Health Center\n";
        cout << "2. Edit Health Center\n";
        cout << "3. View Health Centers\n";
        cout << "4. Remove Health Center\n";
        cout << "5. Add Connection\n";
        cout << "6. Edit Connection\n";
        cout << "7. View Connections\n";
        cout << "8. Remove Connection\n";
        cout << "9. View Relationships\n";
        cout << "10. Dijkstra's Shortest Path\n";
        cout << "11. BFS Traversal\n1";
        cout << "12. Detect Cycle\n";
        cout << "13. Floyd-Warshall All-Pairs\n";
        cout << "14. Prim's MST\n";
        cout << "15. Emergency Routing\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 0) break;

        int from, to, time, minCapacity;
        float dist;
        string desc;
        switch (choice) {
            case 1:
                addHealthCenter();
                break;
            case 2:
                cout << "Enter ID to edit: "; cin >> from;
                editHealthCenter(from);
                break;
            case 3:
                viewHealthCenters();
                break;
            case 4:
                cout << "Enter ID to remove: "; cin >> from;
                removeHealthCenter(from);
                break;
            case 5:
                cout << "Enter FromID ToID DistanceKM TimeMinutes Description: ";
                cin >> from >> to >> dist >> time;
                cin.ignore(); getline(cin, desc);
                addConnection(from, to, dist, time, desc);
                break;
            case 6:
                cout << "Enter FromID ToID: "; cin >> from >> to;
                editConnection(from, to);
                break;
            case 7:
                viewConnections();
                break;
            case 8:
                cout << "Enter FromID ToID: "; cin >> from >> to;
                removeConnection(from, to);
                break;
            case 9:
                viewRelationships();
                break;
            case 10:
                cout << "Enter StartID EndID: "; cin >> from >> to;
                dijkstra(from, to);
                break;
            case 11:
                cout << "Enter StartID: "; cin >> from;
                bfs(from);
                break;
            case 12:
                detectCycle();
                break;
            case 13:
                floydWarshall(centers.size());
                break;
            case 14:
                primMST(centers.size());
                break;
            case 15:
                cout << "Enter StartID MinCapacity: "; cin >> from >> minCapacity;
                emergencyRouting(from, minCapacity);
                break;
            default:
                cout << "Invalid choice.\n";
        }
    }
    return 0;
}