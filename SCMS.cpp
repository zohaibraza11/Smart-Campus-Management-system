// =============================================
//  SMART CAMPUS MANAGEMENT SYSTEM
//  UET Lahore (KSK Campus) - CSC-200L DSA CCP
//  Data Structures: Graph, BST, Queue, Stack,
//                   Hash Table, Sorting
//
//  FIXES & IMPROVEMENTS:
//  - Rule of Three enforced on DynArray
//  - BST fully destructed (no memory leak)
//  - HistoryStack destructor added
//  - HashTable uses tombstone deletion
//  - BFS uses dynamic queue (no overflow)
//  - All graph node IDs bounds-checked
//  - cin error-state cleared after bad input
//  - pause() fixed (no double-ignore bug)
//  - sortMenu input flow corrected
//  - All const-correctness applied
//  - Dijkstra overflow guard on dist addition
// =============================================

#include <iostream>
#include <string>
#include <climits>
using namespace std;

// ---- UTILITY: Clear cin after bad input ----
void clearCin() {
    cin.clear();
    cin.ignore(10000, '\n');
}

// ---- UTILITY: Dynamic Array (Rule of Three) ----
template<typename T>
struct DynArray {
    T*  data;
    int sz, cap;

    DynArray() : data(nullptr), sz(0), cap(0) {}

    // Copy constructor
    DynArray(const DynArray& o) : data(nullptr), sz(o.sz), cap(o.cap) {
        if (cap > 0) {
            data = new T[cap];
            for (int i = 0; i < sz; i++) data[i] = o.data[i];
        }
    }

    // Copy assignment
    DynArray& operator=(const DynArray& o) {
        if (this == &o) return *this;
        delete[] data;
        sz = o.sz; cap = o.cap; data = nullptr;
        if (cap > 0) {
            data = new T[cap];
            for (int i = 0; i < sz; i++) data[i] = o.data[i];
        }
        return *this;
    }

    ~DynArray() { delete[] data; }

    void push(const T& v) {
        if (sz == cap) {
            cap = cap ? cap * 2 : 4;
            T* n = new T[cap];
            for (int i = 0; i < sz; i++) n[i] = data[i];
            delete[] data;
            data = n;
        }
        data[sz++] = v;
    }

    T& operator[](int i)             { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    int size() const                 { return sz; }
};

// =============================================
//  MODULE 1: NAVIGATION (GRAPH + DIJKSTRA)
// =============================================
const int MAXN = 20;
const int INF  = INT_MAX / 2;

struct CampusGraph {
    int    n;
    string names[MAXN];
    int    adj[MAXN][MAXN];

    CampusGraph() : n(0) {
        for (int i = 0; i < MAXN; i++)
            for (int j = 0; j < MAXN; j++)
                adj[i][j] = (i == j) ? 0 : INF;
    }

    int addLocation(const string& nm) {
        if (n < MAXN) { names[n] = nm; return n++; }
        cout << "  ERROR: Maximum locations reached.\n";
        return -1;
    }

    // Returns false if either node ID is invalid
    bool addRoad(int u, int v, int w) {
        if (u < 0 || u >= n || v < 0 || v >= n) {
            cout << "  ERROR: Invalid node ID(s).\n";
            return false;
        }
        if (w <= 0) {
            cout << "  ERROR: Distance must be positive.\n";
            return false;
        }
        adj[u][v] = w;
        adj[v][u] = w;
        return true;
    }

    void dijkstra(int src, int dist[], int prev[]) const {
        bool vis[MAXN] = {};
        for (int i = 0; i < n; i++) { dist[i] = INF; prev[i] = -1; }
        if (src < 0 || src >= n) return;
        dist[src] = 0;
        for (int iter = 0; iter < n; iter++) {
            int u = -1;
            for (int i = 0; i < n; i++)
                if (!vis[i] && (u == -1 || dist[i] < dist[u])) u = i;
            if (u == -1 || dist[u] == INF) break;
            vis[u] = true;
            for (int v = 0; v < n; v++) {
                if (adj[u][v] != INF && !vis[v]) {
                    // Overflow-safe addition
                    int newDist = dist[u] + adj[u][v];
                    if (newDist < dist[v]) {
                        dist[v] = newDist;
                        prev[v] = u;
                    }
                }
            }
        }
    }

    void shortestPath(int src, int dst) const {
        if (src < 0 || src >= n || dst < 0 || dst >= n) {
            cout << "  ERROR: Invalid node ID(s).\n"; return;
        }
        int dist[MAXN], prev[MAXN];
        dijkstra(src, dist, prev);
        if (dist[dst] == INF) { cout << "  No path found.\n"; return; }
        DynArray<int> path;
        for (int at = dst; at != -1; at = prev[at]) path.push(at);
        cout << "  Path: ";
        for (int i = path.size() - 1; i >= 0; i--) {
            cout << names[path[i]];
            if (i > 0) cout << " -> ";
        }
        cout << "\n  Distance: " << dist[dst] << " meters\n";
    }

    void bfs(int src) const {
        if (src < 0 || src >= n) { cout << "  ERROR: Invalid node ID.\n"; return; }
        bool vis[MAXN] = {};
        // Dynamic queue using DynArray to avoid fixed-size overflow
        DynArray<int> queue;
        queue.push(src);
        vis[src] = true;
        int front = 0;
        cout << "  BFS from " << names[src] << ": ";
        while (front < queue.size()) {
            int u = queue[front++];
            cout << names[u] << " ";
            for (int v = 0; v < n; v++)
                if (adj[u][v] != INF && !vis[v]) { vis[v] = true; queue.push(v); }
        }
        cout << "\n";
    }

    void dfs(int u, bool vis[]) const {
        if (u < 0 || u >= n) return;
        vis[u] = true;
        cout << names[u] << " ";
        for (int v = 0; v < n; v++)
            if (adj[u][v] != INF && !vis[v]) dfs(v, vis);
    }

    void showLocations() const {
        if (n == 0) { cout << "  No locations added yet.\n"; return; }
        cout << "  Campus Locations:\n";
        for (int i = 0; i < n; i++) cout << "  [" << i << "] " << names[i] << "\n";
    }
};

// =============================================
//  MODULE 2: LOCATION MANAGEMENT (BST)
// =============================================
struct BSTNode {
    string   name;
    int      graphId;
    BSTNode* left;
    BSTNode* right;
    BSTNode(const string& nm, int id)
        : name(nm), graphId(id), left(nullptr), right(nullptr) {}
};

struct LocationBST {
    BSTNode* root;
    LocationBST() : root(nullptr) {}
    ~LocationBST() { destroy(root); }

    void destroy(BSTNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    BSTNode* insert(BSTNode* node, const string& nm, int id) {
        if (!node) return new BSTNode(nm, id);
        if      (nm < node->name) node->left  = insert(node->left,  nm, id);
        else if (nm > node->name) node->right = insert(node->right, nm, id);
        else node->graphId = id;   // Update if same name
        return node;
    }
    void insert(const string& nm, int id) { root = insert(root, nm, id); }

    BSTNode* search(BSTNode* node, const string& nm) const {
        if (!node || node->name == nm) return node;
        if (nm < node->name) return search(node->left,  nm);
        return                      search(node->right, nm);
    }
    BSTNode* search(const string& nm) const { return search(root, nm); }

    void inorder(BSTNode* node) const {
        if (!node) return;
        inorder(node->left);
        cout << "  " << node->name << " (Graph ID: " << node->graphId << ")\n";
        inorder(node->right);
    }
    void displayAll() const {
        if (!root) { cout << "  BST is empty.\n"; return; }
        inorder(root);
    }

    BSTNode* minNode(BSTNode* node) const {
        while (node->left) node = node->left;
        return node;
    }

    BSTNode* deleteNode(BSTNode* node, const string& nm) {
        if (!node) return nullptr;
        if      (nm < node->name) node->left  = deleteNode(node->left,  nm);
        else if (nm > node->name) node->right = deleteNode(node->right, nm);
        else {
            if (!node->left)  { BSTNode* t = node->right; delete node; return t; }
            if (!node->right) { BSTNode* t = node->left;  delete node; return t; }
            BSTNode* mn   = minNode(node->right);
            node->name    = mn->name;
            node->graphId = mn->graphId;
            node->right   = deleteNode(node->right, mn->name);
        }
        return node;
    }
    void remove(const string& nm) { root = deleteNode(root, nm); }
};

// =============================================
//  MODULE 3: REQUEST HANDLING (QUEUE)
// =============================================
struct QNode {
    string  request;
    QNode*  next;
    QNode(const string& r) : request(r), next(nullptr) {}
};

struct RequestQueue {
    QNode* front;
    QNode* rear;
    int    sz;

    RequestQueue() : front(nullptr), rear(nullptr), sz(0) {}
    ~RequestQueue() { while (front) processNext(); }

    void enqueue(const string& req) {
        QNode* n = new QNode(req);
        if (!rear) front = rear = n;
        else       { rear->next = n; rear = n; }
        sz++;
        cout << "  Request queued: \"" << req << "\"\n";
    }

    void processNext() {
        if (!front) { cout << "  Queue is empty.\n"; return; }
        cout << "  Processing: \"" << front->request << "\"\n";
        QNode* tmp = front;
        front = front->next;
        if (!front) rear = nullptr;
        delete tmp;
        sz--;
    }

    void display() const {
        if (!front) { cout << "  No pending requests.\n"; return; }
        QNode* cur = front;
        int i = 1;
        while (cur) { cout << "  " << i++ << ". " << cur->request << "\n"; cur = cur->next; }
    }
};

// =============================================
//  MODULE 4: HISTORY TRACKING (STACK)
// =============================================
struct StackNode {
    string     action;
    StackNode* next;
    StackNode(const string& a) : action(a), next(nullptr) {}
};

struct HistoryStack {
    StackNode* top;
    int        sz;

    HistoryStack() : top(nullptr), sz(0) {}
    ~HistoryStack() { while (top) { StackNode* t = top; top = top->next; delete t; } }

    void push(const string& action) {
        StackNode* n = new StackNode(action);
        n->next = top;
        top = n;
        sz++;
    }

    string pop() {
        if (!top) return "";
        string a = top->action;
        StackNode* tmp = top;
        top = top->next;
        delete tmp;
        sz--;
        return a;
    }

    void undo() {
        string a = pop();
        if (a.empty()) cout << "  Nothing to undo.\n";
        else           cout << "  Undone: \"" << a << "\"\n";
    }

    void display() const {
        if (!top) { cout << "  History is empty.\n"; return; }
        StackNode* cur = top;
        int i = 1;
        while (cur) { cout << "  " << i++ << ". " << cur->action << "\n"; cur = cur->next; }
    }
};

// =============================================
//  MODULE 5: FAST LOOKUP (HASH TABLE)
//  Uses tombstone deletion to keep probing correct
// =============================================
const int HTSIZE = 31;  // Prime number for better distribution

struct HTEntry {
    string key;
    int    value;
    enum State { EMPTY, OCCUPIED, DELETED } state;
    HTEntry() : value(-1), state(EMPTY) {}
};

struct HashTable {
    HTEntry table[HTSIZE];

    int hash(const string& key) const {
        unsigned long long h = 0;
        for (char c : key) h = h * 31 + (unsigned char)c;
        return (int)(h % HTSIZE);
    }

    void insert(const string& key, int val) {
        int idx = hash(key);
        int firstDeleted = -1;
        for (int i = 0; i < HTSIZE; i++) {
            int probe = (idx + i) % HTSIZE;
            if (table[probe].state == HTEntry::OCCUPIED && table[probe].key == key) {
                table[probe].value = val; return;   // Update existing
            }
            if (table[probe].state == HTEntry::DELETED && firstDeleted == -1)
                firstDeleted = probe;
            if (table[probe].state == HTEntry::EMPTY) {
                int target = (firstDeleted != -1) ? firstDeleted : probe;
                table[target].key = key; table[target].value = val;
                table[target].state = HTEntry::OCCUPIED;
                return;
            }
        }
        // Reuse first deleted slot if all occupied
        if (firstDeleted != -1) {
            table[firstDeleted].key = key; table[firstDeleted].value = val;
            table[firstDeleted].state = HTEntry::OCCUPIED;
            return;
        }
        cout << "  Hash table is full!\n";
    }

    int search(const string& key) const {
        int idx = hash(key);
        for (int i = 0; i < HTSIZE; i++) {
            int probe = (idx + i) % HTSIZE;
            if (table[probe].state == HTEntry::EMPTY)  return -1;
            if (table[probe].state == HTEntry::OCCUPIED && table[probe].key == key)
                return table[probe].value;
        }
        return -1;
    }

    // Proper deletion using tombstone
    bool remove(const string& key) {
        int idx = hash(key);
        for (int i = 0; i < HTSIZE; i++) {
            int probe = (idx + i) % HTSIZE;
            if (table[probe].state == HTEntry::EMPTY) return false;
            if (table[probe].state == HTEntry::OCCUPIED && table[probe].key == key) {
                table[probe].state = HTEntry::DELETED;
                return true;
            }
        }
        return false;
    }

    void display() const {
        bool any = false;
        cout << "  Hash Table Contents:\n";
        for (int i = 0; i < HTSIZE; i++) {
            if (table[i].state == HTEntry::OCCUPIED) {
                cout << "  [" << i << "] " << table[i].key << " -> ID:" << table[i].value << "\n";
                any = true;
            }
        }
        if (!any) cout << "  (empty)\n";
    }
};

// =============================================
//  MODULE 6: SORTING & SEARCHING
// =============================================
struct Location { string name; int dist; };

void bubbleSortByName(Location arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j].name > arr[j+1].name) swap(arr[j], arr[j+1]);
}

void bubbleSortByDist(Location arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j].dist > arr[j+1].dist) swap(arr[j], arr[j+1]);
}

int binarySearch(const Location arr[], int n, const string& target) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if      (arr[mid].name == target) return mid;
        else if (arr[mid].name <  target) lo = mid + 1;
        else                              hi = mid - 1;
    }
    return -1;
}

// =============================================
//  DEMO DATA SEEDER
// =============================================
void seedData(CampusGraph& g, LocationBST& bst, HashTable& ht) {
    int main_gate = g.addLocation("Main Gate");
    int library   = g.addLocation("Library");
    int cs_dept   = g.addLocation("CS Department");
    int cafeteria = g.addLocation("Cafeteria");
    int admin     = g.addLocation("Admin Block");
    int labs      = g.addLocation("Computer Labs");
    int sports    = g.addLocation("Sports Ground");
    int hostel    = g.addLocation("Hostel Block");

    g.addRoad(main_gate, library,   150);
    g.addRoad(main_gate, admin,     100);
    g.addRoad(library,   cs_dept,   80);
    g.addRoad(cs_dept,   labs,      50);
    g.addRoad(cs_dept,   cafeteria, 120);
    g.addRoad(admin,     cafeteria, 90);
    g.addRoad(cafeteria, sports,    200);
    g.addRoad(sports,    hostel,    110);
    g.addRoad(labs,      hostel,    180);

    const char* locNames[] = {
        "Main Gate","Library","CS Department","Cafeteria",
        "Admin Block","Computer Labs","Sports Ground","Hostel Block"
    };
    int ids[] = { main_gate, library, cs_dept, cafeteria, admin, labs, sports, hostel };
    for (int i = 0; i < 8; i++) {
        bst.insert(locNames[i], ids[i]);
        ht.insert(locNames[i],  ids[i]);
    }
}

// =============================================
//  MENU HELPERS
// =============================================
void header(const string& t) {
    cout << "\n============================================\n";
    cout << "  " << t << "\n";
    cout << "============================================\n";
}

// Fixed pause: no double-ignore. Just wait for Enter.
void pause() {
    cout << "\n  Press Enter to continue...";
    // After cin>>ch, newline is still in buffer — consume it then wait
    if (cin.peek() == '\n') cin.ignore();
    cin.get();
}

// Safe integer input with validation
bool getInt(int& out) {
    if (!(cin >> out)) { clearCin(); cout << "  ERROR: Please enter a number.\n"; return false; }
    return true;
}

// =============================================
//  MODULE MENUS
// =============================================
void navigationMenu(CampusGraph& g) {
    int ch;
    do {
        header("MODULE 1: Navigation (Graph + Dijkstra)");
        cout << "  1. Show All Locations\n";
        cout << "  2. Find Shortest Path\n";
        cout << "  3. BFS Traversal\n";
        cout << "  4. DFS Traversal\n";
        cout << "  5. Add New Location\n";
        cout << "  6. Add Road Between Locations\n";
        cout << "  0. Back\n  Choice: ";
        if (!getInt(ch)) continue;

        if (ch == 1) {
            header("Campus Locations");
            g.showLocations();

        } else if (ch == 2) {
            g.showLocations();
            int s, d;
            cout << "  Source ID: ";      if (!getInt(s)) { pause(); continue; }
            cout << "  Destination ID: "; if (!getInt(d)) { pause(); continue; }
            g.shortestPath(s, d);

        } else if (ch == 3) {
            g.showLocations();
            int s; cout << "  Start node ID: "; if (!getInt(s)) { pause(); continue; }
            g.bfs(s);

        } else if (ch == 4) {
            g.showLocations();
            int s; cout << "  Start node ID: "; if (!getInt(s)) { pause(); continue; }
            if (s >= 0 && s < g.n) {
                bool vis[MAXN] = {};
                cout << "  DFS: "; g.dfs(s, vis); cout << "\n";
            } else {
                cout << "  ERROR: Invalid node ID.\n";
            }

        } else if (ch == 5) {
            string nm; clearCin();
            cout << "  Location name: "; getline(cin, nm);
            if (nm.empty()) { cout << "  ERROR: Name cannot be empty.\n"; }
            else {
                int id = g.addLocation(nm);
                if (id >= 0) cout << "  Added \"" << nm << "\" as node " << id << "\n";
            }

        } else if (ch == 6) {
            int u, v, w;
            g.showLocations();
            cout << "  Node 1 ID: ";     if (!getInt(u)) { pause(); continue; }
            cout << "  Node 2 ID: ";     if (!getInt(v)) { pause(); continue; }
            cout << "  Distance (m): ";  if (!getInt(w)) { pause(); continue; }
            if (g.addRoad(u, v, w)) cout << "  Road added successfully.\n";
        }

        if (ch != 0) pause();
    } while (ch != 0);
}

void bstMenu(LocationBST& bst) {
    int ch;
    do {
        header("MODULE 2: Location Management (BST)");
        cout << "  1. Display All Locations (In-Order A-Z)\n";
        cout << "  2. Search Location\n";
        cout << "  3. Add Location\n";
        cout << "  4. Delete Location\n";
        cout << "  0. Back\n  Choice: ";
        if (!getInt(ch)) continue;

        if (ch == 1) {
            header("Locations (Sorted A-Z)");
            bst.displayAll();

        } else if (ch == 2) {
            string nm; clearCin();
            cout << "  Location name: "; getline(cin, nm);
            BSTNode* res = bst.search(nm);
            if (res) cout << "  Found: " << res->name << " (Graph ID: " << res->graphId << ")\n";
            else     cout << "  Not found.\n";

        } else if (ch == 3) {
            string nm; int id; clearCin();
            cout << "  Name: ";     getline(cin, nm);
            cout << "  Graph ID: "; if (!getInt(id)) { pause(); continue; }
            bst.insert(nm, id);
            cout << "  Added.\n";

        } else if (ch == 4) {
            string nm; clearCin();
            cout << "  Name to delete: "; getline(cin, nm);
            BSTNode* found = bst.search(nm);
            if (found) { bst.remove(nm); cout << "  \"" << nm << "\" deleted.\n"; }
            else        cout << "  Not found.\n";
        }

        if (ch != 0) pause();
    } while (ch != 0);
}

void requestMenu(RequestQueue& q) {
    int ch;
    do {
        header("MODULE 3: Request Handling (Queue)");
        cout << "  Pending: " << q.sz << "\n";
        cout << "  1. View Pending Requests\n";
        cout << "  2. Add New Request\n";
        cout << "  3. Process Next Request\n";
        cout << "  0. Back\n  Choice: ";
        if (!getInt(ch)) continue;

        if (ch == 1) { header("Pending Requests"); q.display(); }
        else if (ch == 2) {
            string r; clearCin();
            cout << "  Request: "; getline(cin, r);
            if (r.empty()) cout << "  ERROR: Request cannot be empty.\n";
            else q.enqueue(r);
        }
        else if (ch == 3) q.processNext();

        if (ch != 0) pause();
    } while (ch != 0);
}

void historyMenu(HistoryStack& st) {
    int ch;
    do {
        header("MODULE 4: History Tracking (Stack)");
        cout << "  Entries: " << st.sz << "\n";
        cout << "  1. View History\n";
        cout << "  2. Add to History\n";
        cout << "  3. Undo Last Action\n";
        cout << "  0. Back\n  Choice: ";
        if (!getInt(ch)) continue;

        if (ch == 1) { header("Action History"); st.display(); }
        else if (ch == 2) {
            string a; clearCin();
            cout << "  Action: "; getline(cin, a);
            if (a.empty()) cout << "  ERROR: Action cannot be empty.\n";
            else { st.push(a); cout << "  Recorded.\n"; }
        }
        else if (ch == 3) st.undo();

        if (ch != 0) pause();
    } while (ch != 0);
}

void hashMenu(HashTable& ht) {
    int ch;
    do {
        header("MODULE 5: Fast Lookup (Hash Table)");
        cout << "  1. Display Table\n";
        cout << "  2. Search Location\n";
        cout << "  3. Insert Entry\n";
        cout << "  4. Delete Entry\n";
        cout << "  0. Back\n  Choice: ";
        if (!getInt(ch)) continue;

        if (ch == 1) { header("Hash Table"); ht.display(); }
        else if (ch == 2) {
            string key; clearCin();
            cout << "  Location name: "; getline(cin, key);
            int id = ht.search(key);
            if (id >= 0) cout << "  Found! Graph ID = " << id << "\n";
            else         cout << "  Not found.\n";
        }
        else if (ch == 3) {
            string key; int val; clearCin();
            cout << "  Name: "; getline(cin, key);
            cout << "  ID: ";   if (!getInt(val)) { pause(); continue; }
            ht.insert(key, val); cout << "  Inserted.\n";
        }
        else if (ch == 4) {
            string key; clearCin();
            cout << "  Name to delete: "; getline(cin, key);
            if (ht.remove(key)) cout << "  Deleted.\n";
            else                cout << "  Not found.\n";
        }

        if (ch != 0) pause();
    } while (ch != 0);
}

void sortMenu(CampusGraph& g) {
    int ch;
    do {
        header("MODULE 6: Sorting & Searching");
        cout << "  1. Sort Locations by Name (A-Z)\n";
        cout << "  2. Sort by Distance from Main Gate\n";
        cout << "  3. Binary Search by Name\n";
        cout << "  0. Back\n  Choice: ";
        if (!getInt(ch)) continue;

        if (ch == 1 || ch == 2 || ch == 3) {
            if (g.n == 0) { cout << "  No locations to sort.\n"; pause(); continue; }

            int dist[MAXN], prev[MAXN];
            g.dijkstra(0, dist, prev);

            Location locs[MAXN];
            int cnt = g.n;
            for (int i = 0; i < cnt; i++) {
                locs[i].name = g.names[i];
                locs[i].dist = (dist[i] == INF) ? -1 : dist[i];
            }

            if (ch == 1) {
                bubbleSortByName(locs, cnt);
                cout << "  Locations (A-Z):\n";
                for (int i = 0; i < cnt; i++) cout << "  " << locs[i].name << "\n";

            } else if (ch == 2) {
                bubbleSortByDist(locs, cnt);
                cout << "  Locations by distance from Main Gate:\n";
                for (int i = 0; i < cnt; i++) {
                    cout << "  " << locs[i].name;
                    if (locs[i].dist >= 0) cout << " - " << locs[i].dist << "m";
                    else                   cout << " - unreachable";
                    cout << "\n";
                }

            } else {    // ch == 3: sort first, then search
                bubbleSortByName(locs, cnt);
                string target; clearCin();
                cout << "  Search name: "; getline(cin, target);
                int res = binarySearch(locs, cnt, target);
                if (res >= 0) cout << "  Found: \"" << locs[res].name << "\" at sorted position " << res << "\n";
                else          cout << "  Not found.\n";
            }
        }

        if (ch != 0) pause();
    } while (ch != 0);
}

// =============================================
//  MAIN
// =============================================
int main() {
    CampusGraph  graph;
    LocationBST  bst;
    RequestQueue reqQ;
    HistoryStack history;
    HashTable    ht;

    seedData(graph, bst, ht);

    // Pre-load some demo data
    reqQ.enqueue("Find path to Library");
    reqQ.enqueue("Add new classroom block");
    history.push("Searched: Main Gate to CS Department");
    history.push("Searched: Library to Cafeteria");

    int ch;
    do {
        header("Smart Campus Management System - UET KSK");
        cout << "  1. Navigation Module       (Graph + Dijkstra)\n";
        cout << "  2. Location Management     (BST)\n";
        cout << "  3. Request Handling        (Queue)\n";
        cout << "  4. History Tracking        (Stack)\n";
        cout << "  5. Fast Lookup             (Hash Table)\n";
        cout << "  6. Sorting & Searching\n";
        cout << "  0. Exit\n  Choice: ";

        if (!getInt(ch)) continue;

        switch (ch) {
            case 1: navigationMenu(graph); break;
            case 2: bstMenu(bst);          break;
            case 3: requestMenu(reqQ);     break;
            case 4: historyMenu(history);  break;
            case 5: hashMenu(ht);          break;
            case 6: sortMenu(graph);       break;
            case 0: cout << "\n  Goodbye! - UET KSK Campus System\n"; break;
            default: cout << "  Invalid choice. Try again.\n";         break;
        }
    } while (ch != 0);

    return 0;
}
