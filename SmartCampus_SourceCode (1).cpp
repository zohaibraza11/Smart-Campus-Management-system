#include <iostream>
#include <string>
#include <climits>
using namespace std;

// =============================================
//  SMART CAMPUS MANAGEMENT SYSTEM
//  UET Lahore (KSK Campus) - CSC-162 DSA CCP
//  Data Structures: Graph, BST, Queue, Stack,
//                   Hash Table, Sorting
// =============================================

// ---- UTILITY: Dynamic Array ----
template<typename T>
struct DynArray {
    T* data; int sz, cap;
    DynArray() : data(nullptr), sz(0), cap(0) {}
    void push(T v) {
        if (sz == cap) {
            cap = cap ? cap*2 : 4;
            T* n = new T[cap];
            for (int i=0;i<sz;i++) n[i]=data[i];
            delete[] data; data=n;
        }
        data[sz++]=v;
    }
    T& operator[](int i){ return data[i]; }
    int size(){ return sz; }
    ~DynArray(){ delete[] data; }
};

// =============================================
//  MODULE 1: NAVIGATION (GRAPH + DIJKSTRA)
// =============================================
const int MAXN = 15;
const int INF  = INT_MAX/2;

struct CampusGraph {
    int  n;
    string names[MAXN];
    int  adj[MAXN][MAXN];

    CampusGraph() : n(0) {
        for(int i=0;i<MAXN;i++)
            for(int j=0;j<MAXN;j++)
                adj[i][j] = (i==j)?0:INF;
    }

    int addLocation(string nm){
        if(n<MAXN){ names[n]=nm; return n++; }
        return -1;
    }

    void addRoad(int u,int v,int w){
        adj[u][v]=w; adj[v][u]=w;
    }

    void dijkstra(int src, int dist[], int prev[]){
        bool vis[MAXN]={};
        for(int i=0;i<n;i++){dist[i]=INF;prev[i]=-1;}
        dist[src]=0;
        for(int iter=0;iter<n;iter++){
            int u=-1;
            for(int i=0;i<n;i++)
                if(!vis[i]&&(u==-1||dist[i]<dist[u])) u=i;
            if(u==-1||dist[u]==INF) break;
            vis[u]=true;
            for(int v=0;v<n;v++)
                if(adj[u][v]!=INF&&!vis[v]&&dist[u]+adj[u][v]<dist[v]){
                    dist[v]=dist[u]+adj[u][v];
                    prev[v]=u;
                }
        }
    }

    void shortestPath(int src,int dst){
        int dist[MAXN],prev[MAXN];
        dijkstra(src,dist,prev);
        if(dist[dst]==INF){cout<<"  No path found.\n";return;}
        DynArray<int> path;
        for(int at=dst;at!=-1;at=prev[at]) path.push(at);
        cout<<"  Path: ";
        for(int i=path.sz-1;i>=0;i--){
            cout<<names[path[i]]; if(i>0)cout<<" -> ";
        }
        cout<<"\n  Distance: "<<dist[dst]<<" meters\n";
    }

    void bfs(int src){
        bool vis[MAXN]={};
        int queue[MAXN]; int front=0,rear=0;
        queue[rear++]=src; vis[src]=true;
        cout<<"  BFS from "<<names[src]<<": ";
        while(front<rear){
            int u=queue[front++];
            cout<<names[u]<<" ";
            for(int v=0;v<n;v++)
                if(adj[u][v]!=INF&&!vis[v]){vis[v]=true;queue[rear++]=v;}
        }
        cout<<"\n";
    }

    void dfs(int u, bool vis[]){
        vis[u]=true; cout<<names[u]<<" ";
        for(int v=0;v<n;v++)
            if(adj[u][v]!=INF&&!vis[v]) dfs(v,vis);
    }

    void showLocations(){
        cout<<"  Campus Locations:\n";
        for(int i=0;i<n;i++) cout<<"  ["<<i<<"] "<<names[i]<<"\n";
    }
};

// =============================================
//  MODULE 2: LOCATION MANAGEMENT (BST)
// =============================================
struct BSTNode {
    string name;
    int    graphId;
    BSTNode *left, *right;
    BSTNode(string nm, int id): name(nm), graphId(id), left(nullptr), right(nullptr){}
};

struct LocationBST {
    BSTNode* root;
    LocationBST(): root(nullptr){}

    BSTNode* insert(BSTNode* node, string nm, int id){
        if(!node) return new BSTNode(nm,id);
        if(nm < node->name) node->left  = insert(node->left,  nm, id);
        else if(nm > node->name) node->right = insert(node->right, nm, id);
        return node;
    }
    void insert(string nm, int id){ root=insert(root,nm,id); }

    BSTNode* search(BSTNode* node, string nm){
        if(!node||node->name==nm) return node;
        if(nm < node->name) return search(node->left, nm);
        return search(node->right, nm);
    }
    BSTNode* search(string nm){ return search(root,nm); }

    void inorder(BSTNode* node){
        if(!node) return;
        inorder(node->left);
        cout<<"  "<<node->name<<" (Graph ID: "<<node->graphId<<")\n";
        inorder(node->right);
    }
    void displayAll(){ inorder(root); }

    BSTNode* deleteNode(BSTNode* node, string nm){
        if(!node) return nullptr;
        if(nm < node->name) node->left=deleteNode(node->left,nm);
        else if(nm > node->name) node->right=deleteNode(node->right,nm);
        else {
            if(!node->left){BSTNode*t=node->right;delete node;return t;}
            if(!node->right){BSTNode*t=node->left;delete node;return t;}
            BSTNode* mn=node->right;
            while(mn->left) mn=mn->left;
            node->name=mn->name; node->graphId=mn->graphId;
            node->right=deleteNode(node->right,mn->name);
        }
        return node;
    }
    void remove(string nm){ root=deleteNode(root,nm); }
};

// =============================================
//  MODULE 3: REQUEST HANDLING (QUEUE)
// =============================================
struct QNode { string request; QNode* next; QNode(string r): request(r), next(nullptr){} };

struct RequestQueue {
    QNode *front, *rear; int sz;
    RequestQueue(): front(nullptr), rear(nullptr), sz(0){}

    void enqueue(string req){
        QNode* n=new QNode(req);
        if(!rear) front=rear=n;
        else { rear->next=n; rear=n; }
        sz++;
        cout<<"  Request queued: \""<<req<<"\"\n";
    }

    void processNext(){
        if(!front){cout<<"  Queue empty.\n";return;}
        cout<<"  Processing: \""<<front->request<<"\"\n";
        QNode* tmp=front; front=front->next;
        if(!front) rear=nullptr;
        delete tmp; sz--;
    }

    void display(){
        if(!front){cout<<"  No pending requests.\n";return;}
        QNode* cur=front; int i=1;
        while(cur){ cout<<"  "<<i++<<". "<<cur->request<<"\n"; cur=cur->next; }
    }
    ~RequestQueue(){ while(front) processNext(); }
};

// =============================================
//  MODULE 4: HISTORY TRACKING (STACK)
// =============================================
struct StackNode { string action; StackNode* next; StackNode(string a): action(a), next(nullptr){} };

struct HistoryStack {
    StackNode* top; int sz;
    HistoryStack(): top(nullptr), sz(0){}

    void push(string action){
        StackNode* n=new StackNode(action);
        n->next=top; top=n; sz++;
    }

    string pop(){
        if(!top) return "";
        string a=top->action;
        StackNode* tmp=top; top=top->next;
        delete tmp; sz--; return a;
    }

    void undo(){
        string a=pop();
        if(a.empty()) cout<<"  Nothing to undo.\n";
        else cout<<"  Undone: \""<<a<<"\"\n";
    }

    void display(){
        if(!top){cout<<"  History empty.\n";return;}
        StackNode* cur=top; int i=1;
        while(cur){ cout<<"  "<<i++<<". "<<cur->action<<"\n"; cur=cur->next; }
    }
};

// =============================================
//  MODULE 5: FAST LOOKUP (HASH TABLE)
// =============================================
const int HTSIZE = 20;

struct HTEntry { string key; int value; bool used; HTEntry(): used(false){} };

struct HashTable {
    HTEntry table[HTSIZE];

    int hash(string key){
        int h=0;
        for(char c:key) h=(h*31+c)%HTSIZE;
        return (h+HTSIZE)%HTSIZE;
    }

    void insert(string key, int val){
        int idx=hash(key);
        for(int i=0;i<HTSIZE;i++){
            int probe=(idx+i)%HTSIZE;
            if(!table[probe].used||table[probe].key==key){
                table[probe].key=key; table[probe].value=val; table[probe].used=true; return;
            }
        }
        cout<<"  Hash table full!\n";
    }

    int search(string key){
        int idx=hash(key);
        for(int i=0;i<HTSIZE;i++){
            int probe=(idx+i)%HTSIZE;
            if(!table[probe].used) return -1;
            if(table[probe].key==key) return table[probe].value;
        }
        return -1;
    }

    void display(){
        cout<<"  Hash Table Contents:\n";
        for(int i=0;i<HTSIZE;i++)
            if(table[i].used)
                cout<<"  ["<<i<<"] "<<table[i].key<<" -> ID:"<<table[i].value<<"\n";
    }
};

// =============================================
//  MODULE 6: SORTING & SEARCHING
// =============================================
struct Location { string name; int dist; };

void bubbleSort(Location arr[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)
            if(arr[j].name > arr[j+1].name)
                swap(arr[j],arr[j+1]);
}

void bubbleSortByDist(Location arr[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)
            if(arr[j].dist > arr[j+1].dist)
                swap(arr[j],arr[j+1]);
}

int binarySearch(Location arr[], int n, string target){
    int lo=0, hi=n-1;
    while(lo<=hi){
        int mid=(lo+hi)/2;
        if(arr[mid].name==target) return mid;
        if(arr[mid].name<target) lo=mid+1;
        else hi=mid-1;
    }
    return -1;
}

// =============================================
//  DEMO DATA SEEDER
// =============================================
void seedData(CampusGraph& g, LocationBST& bst, HashTable& ht){
    // Add campus locations
    int main_gate   = g.addLocation("Main Gate");
    int library     = g.addLocation("Library");
    int cs_dept     = g.addLocation("CS Department");
    int cafeteria   = g.addLocation("Cafeteria");
    int admin       = g.addLocation("Admin Block");
    int labs        = g.addLocation("Computer Labs");
    int sports      = g.addLocation("Sports Ground");
    int hostel      = g.addLocation("Hostel Block");

    // Roads (distances in meters)
    g.addRoad(main_gate, library,   150);
    g.addRoad(main_gate, admin,     100);
    g.addRoad(library,   cs_dept,   80);
    g.addRoad(cs_dept,   labs,      50);
    g.addRoad(cs_dept,   cafeteria, 120);
    g.addRoad(admin,     cafeteria, 90);
    g.addRoad(cafeteria, sports,    200);
    g.addRoad(sports,    hostel,    110);
    g.addRoad(labs,      hostel,    180);

    // BST
    bst.insert("Main Gate",     main_gate);
    bst.insert("Library",       library);
    bst.insert("CS Department", cs_dept);
    bst.insert("Cafeteria",     cafeteria);
    bst.insert("Admin Block",   admin);
    bst.insert("Computer Labs", labs);
    bst.insert("Sports Ground", sports);
    bst.insert("Hostel Block",  hostel);

    // Hash Table
    ht.insert("Main Gate",     main_gate);
    ht.insert("Library",       library);
    ht.insert("CS Department", cs_dept);
    ht.insert("Cafeteria",     cafeteria);
    ht.insert("Admin Block",   admin);
    ht.insert("Computer Labs", labs);
    ht.insert("Sports Ground", sports);
    ht.insert("Hostel Block",  hostel);
}

// =============================================
//  MENU HELPERS
// =============================================
void header(string t){
    cout<<"\n============================================\n";
    cout<<"  "<<t<<"\n";
    cout<<"============================================\n";
}
void pause(){ cout<<"\n  Press Enter..."; cin.ignore(); cin.get(); }

// =============================================
//  MODULE MENUS
// =============================================
void navigationMenu(CampusGraph& g){
    int ch;
    do {
        header("MODULE 1: Navigation (Graph + Dijkstra)");
        cout<<"  1. Show All Locations\n";
        cout<<"  2. Find Shortest Path\n";
        cout<<"  3. BFS Traversal\n";
        cout<<"  4. DFS Traversal\n";
        cout<<"  5. Add New Location\n";
        cout<<"  6. Add Road Between Locations\n";
        cout<<"  0. Back\n  Choice: "; cin>>ch;

        if(ch==1){ header("Campus Locations"); g.showLocations(); }
        else if(ch==2){
            g.showLocations();
            int s,d; cout<<"  Source ID: ";cin>>s; cout<<"  Destination ID: ";cin>>d;
            g.shortestPath(s,d);
        }
        else if(ch==3){
            g.showLocations(); int s; cout<<"  Start node: ";cin>>s; g.bfs(s);
        }
        else if(ch==4){
            g.showLocations(); int s; cout<<"  Start node: ";cin>>s;
            bool vis[MAXN]={};
            cout<<"  DFS: "; g.dfs(s,vis); cout<<"\n";
        }
        else if(ch==5){
            string nm; cin.ignore(); cout<<"  Location name: ";getline(cin,nm);
            int id=g.addLocation(nm);
            cout<<"  Added as node "<<id<<"\n";
        }
        else if(ch==6){
            int u,v,w;
            cout<<"  Node 1: ";cin>>u; cout<<"  Node 2: ";cin>>v; cout<<"  Distance(m): ";cin>>w;
            g.addRoad(u,v,w); cout<<"  Road added.\n";
        }
        if(ch) pause();
    } while(ch);
}

void bstMenu(LocationBST& bst, CampusGraph& g){
    int ch;
    do {
        header("MODULE 2: Location Management (BST)");
        cout<<"  1. Display All Locations (In-Order)\n";
        cout<<"  2. Search Location\n";
        cout<<"  3. Add Location\n";
        cout<<"  4. Delete Location\n";
        cout<<"  0. Back\n  Choice: "; cin>>ch;

        if(ch==1){ header("Locations (Sorted A-Z)"); bst.displayAll(); }
        else if(ch==2){
            string nm; cin.ignore();
            cout<<"  Location name: "; getline(cin,nm);
            BSTNode* res=bst.search(nm);
            if(res) cout<<"  Found: "<<res->name<<" (Graph ID: "<<res->graphId<<")\n";
            else cout<<"  Not found.\n";
        }
        else if(ch==3){
            string nm; int id; cin.ignore();
            cout<<"  Name: ";getline(cin,nm);
            cout<<"  Graph ID: ";cin>>id;
            bst.insert(nm,id);
            cout<<"  Added.\n";
        }
        else if(ch==4){
            string nm; cin.ignore();
            cout<<"  Name to delete: ";getline(cin,nm);
            bst.remove(nm); cout<<"  Deleted (if existed).\n";
        }
        if(ch) pause();
    } while(ch);
}

void requestMenu(RequestQueue& q){
    int ch;
    do {
        header("MODULE 3: Request Handling (Queue)");
        cout<<"  1. View Pending Requests\n";
        cout<<"  2. Add New Request\n";
        cout<<"  3. Process Next Request\n";
        cout<<"  0. Back\n  Choice: "; cin>>ch;

        if(ch==1){ header("Pending Requests"); q.display(); }
        else if(ch==2){
            string r; cin.ignore();
            cout<<"  Request: ";getline(cin,r); q.enqueue(r);
        }
        else if(ch==3) q.processNext();
        if(ch) pause();
    } while(ch);
}

void historyMenu(HistoryStack& st){
    int ch;
    do {
        header("MODULE 4: History Tracking (Stack)");
        cout<<"  1. View History\n";
        cout<<"  2. Add to History\n";
        cout<<"  3. Undo Last Action\n";
        cout<<"  0. Back\n  Choice: "; cin>>ch;

        if(ch==1){ header("Search History"); st.display(); }
        else if(ch==2){
            string a; cin.ignore();
            cout<<"  Action: ";getline(cin,a); st.push(a);
            cout<<"  Recorded.\n";
        }
        else if(ch==3) st.undo();
        if(ch) pause();
    } while(ch);
}

void hashMenu(HashTable& ht){
    int ch;
    do {
        header("MODULE 5: Fast Lookup (Hash Table)");
        cout<<"  1. Display Table\n";
        cout<<"  2. Search Location\n";
        cout<<"  3. Insert Entry\n";
        cout<<"  0. Back\n  Choice: "; cin>>ch;

        if(ch==1){ header("Hash Table"); ht.display(); }
        else if(ch==2){
            string key; cin.ignore();
            cout<<"  Location name: ";getline(cin,key);
            int id=ht.search(key);
            if(id>=0) cout<<"  Found! Graph ID = "<<id<<"\n";
            else cout<<"  Not found.\n";
        }
        else if(ch==3){
            string key; int val; cin.ignore();
            cout<<"  Name: ";getline(cin,key);
            cout<<"  ID: ";cin>>val;
            ht.insert(key,val); cout<<"  Inserted.\n";
        }
        if(ch) pause();
    } while(ch);
}

void sortMenu(CampusGraph& g){
    int ch;
    do {
        header("MODULE 6: Sorting & Searching");
        cout<<"  1. Sort Locations by Name (A-Z)\n";
        cout<<"  2. Sort by Distance from Main Gate\n";
        cout<<"  3. Binary Search by Name\n";
        cout<<"  0. Back\n  Choice: "; cin>>ch;

        if(ch==1||ch==2||ch==3){
            int dist[MAXN], prev[MAXN];
            g.dijkstra(0, dist, prev);
            Location locs[MAXN];
            int cnt=g.n;
            for(int i=0;i<cnt;i++){
                locs[i].name=g.names[i];
                locs[i].dist=(dist[i]==INF)?-1:dist[i];
            }
            if(ch==1){
                bubbleSort(locs,cnt);
                cout<<"  Locations (A-Z):\n";
                for(int i=0;i<cnt;i++) cout<<"  "<<locs[i].name<<"\n";
            } else if(ch==2){
                bubbleSortByDist(locs,cnt);
                cout<<"  Locations by distance from Main Gate:\n";
                for(int i=0;i<cnt;i++){
                    cout<<"  "<<locs[i].name;
                    if(locs[i].dist>=0) cout<<" - "<<locs[i].dist<<"m";
                    else cout<<" - unreachable";
                    cout<<"\n";
                }
            } else {
                bubbleSort(locs,cnt);
                string target; cin.ignore();
                cout<<"  Search name: ";getline(cin,target);
                int res=binarySearch(locs,cnt,target);
                if(res>=0) cout<<"  Found at sorted position "<<res<<"\n";
                else cout<<"  Not found.\n";
            }
        }
        if(ch) pause();
    } while(ch);
}

// =============================================
//  MAIN
// =============================================
int main(){
    CampusGraph   graph;
    LocationBST   bst;
    RequestQueue  reqQ;
    HistoryStack  history;
    HashTable     ht;

    seedData(graph, bst, ht);

    reqQ.enqueue("Find path to Library");
    reqQ.enqueue("Add new classroom");
    history.push("Searched: Main Gate to CS Department");
    history.push("Searched: Library to Cafeteria");

    int ch;
    do {
        header("Smart Campus Management System - UET KSK");
        cout<<"  1. Navigation Module       (Graph + Dijkstra)\n";
        cout<<"  2. Location Management     (BST)\n";
        cout<<"  3. Request Handling        (Queue)\n";
        cout<<"  4. History Tracking        (Stack)\n";
        cout<<"  5. Fast Lookup             (Hash Table)\n";
        cout<<"  6. Sorting & Searching\n";
        cout<<"  0. Exit\n  Choice: "; cin>>ch;

        switch(ch){
            case 1: navigationMenu(graph); break;
            case 2: bstMenu(bst,graph);    break;
            case 3: requestMenu(reqQ);     break;
            case 4: historyMenu(history);  break;
            case 5: hashMenu(ht);          break;
            case 6: sortMenu(graph);       break;
            case 0: cout<<"  Goodbye!\n";  break;
            default: cout<<"  Invalid.\n"; break;
        }
    } while(ch);
    return 0;
}
