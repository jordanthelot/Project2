#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <queue>
using namespace std;


//Minimum degree for the b-tree (t=3 means each node can hold 2t-1 = 5 keys max)
const int T = 3;

//This struct holds all the data for one club
struct ClubNode {
    string clubName;
    vector<double> eloRatings;
    vector<string> dates;
};

//B-tree node - Stores multiple keys and child pointers
struct BTreeNode {
    vector<ClubNode> keys;       //The actual club data stored in this node
    vector<BTreeNode*> children; //Child pointers
    bool leaf;                   //True if this is a leaf node
    int n;                       //Current number of keys in this node

    BTreeNode(bool isLeaf) {
        leaf = isLeaf;
        n = 0;
        //Pre-allocate space for max keys and children
        keys.resize(2 * T - 1);
        children.resize(2 * T, nullptr);
    }

    ~BTreeNode() {
        //Destructor to free memory
        for (int i = 0; i <= n; i++) {
            delete children[i];
        }
    }
};

//Forward declarations
void splitChild(BTreeNode* parent, int i, BTreeNode* fullChild);
void insertNonFull(BTreeNode* node, string clubName, double eloRating, string date);
BTreeNode* insert(BTreeNode* root, string clubName, double eloRating, string date);
string lookup(BTreeNode* root, string& clubName);
ClubNode* findNode(BTreeNode* root, string clubName);

//Convert to lowercase for case insensitive search
string toLowerCase(string str) {
    for (int i = 0; i < str.length(); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

//Splits a full child node during insertion
//Parent is the non-full parent, i is the index of the full child, fullChild is the child to split
void splitChild(BTreeNode* parent, int i, BTreeNode* fullChild) {
    //Create new node that will hold the right half of fullChild's keys
    BTreeNode* newNode = new BTreeNode(fullChild->leaf);
    newNode->n = T - 1;

    //Copy the last T-1 keys from fullChild to newNode
    for (int j = 0; j < T - 1; j++) {
        newNode->keys[j] = fullChild->keys[j + T];
    }

    //If not a leaf, copy the last T children too
    if (!fullChild->leaf) {
        for (int j = 0; j < T; j++) {
            newNode->children[j] = fullChild->children[j + T];
            fullChild->children[j + T] = nullptr;
        }
    }

    fullChild->n = T - 1;

    //Shift parent's children to make room for newNode
    for (int j = parent->n; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = newNode;

    //Shift parent's keys to make room for the median key
    for (int j = parent->n - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }

    //Move the median key up to the parent
    parent->keys[i] = fullChild->keys[T - 1];
    parent->n++;
}

//Inserts into a node that is guaranteed to not be full
void insertNonFull(BTreeNode* node, string clubName, double eloRating, string date) {
    int i = node->n - 1;

    if (node->leaf) {
        //Check if club already exists in this node
        for (int j = 0; j < node->n; j++) {
            if (node->keys[j].clubName == clubName) {
                //Club exists, just push the new rating and date
                node->keys[j].eloRatings.push_back(eloRating);
                node->keys[j].dates.push_back(date);
                return;
            }
        }

        //Club not found in this leaf, insert it in sorted position
        //Shift keys to make room
        while (i >= 0 && node->keys[i].clubName > clubName) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }

        //Create new club data and insert
        ClubNode newClub;
        newClub.clubName = clubName;
        newClub.eloRatings.push_back(eloRating);
        newClub.dates.push_back(date);
        node->keys[i + 1] = newClub;
        node->n++;
    }
    else {
        //Not a leaf, first check if club exists in this node's keys
        for (int j = 0; j < node->n; j++) {
            if (node->keys[j].clubName == clubName) {
                node->keys[j].eloRatings.push_back(eloRating);
                node->keys[j].dates.push_back(date);
                return;
            }
        }

        //Find the child to recurse into
        while (i >= 0 && node->keys[i].clubName > clubName) {
            i--;
        }
        i++;

        //If the child is full, split it first
        if (node->children[i]->n == 2 * T - 1) {
            splitChild(node, i, node->children[i]);

            //After split, check if the key that moved up is our club
            if (node->keys[i].clubName == clubName) {
                node->keys[i].eloRatings.push_back(eloRating);
                node->keys[i].dates.push_back(date);
                return;
            }

            //Decide which child to go into after split
            if (node->keys[i].clubName < clubName) {
                i++;
            }
        }
        insertNonFull(node->children[i], clubName, eloRating, date);
    }
}

//Main insert function - Handles the case where root itself is full
BTreeNode* insert(BTreeNode* root, string clubName, double eloRating, string date) {
    if (root == nullptr) {
        //Tree is empty, create first node
        root = new BTreeNode(true);
        ClubNode newClub;
        newClub.clubName = clubName;
        newClub.eloRatings.push_back(eloRating);
        newClub.dates.push_back(date);
        root->keys[0] = newClub;
        root->n = 1;
        return root;
    }

    //If root is full, tree grows taller
    if (root->n == 2 * T - 1) {
        BTreeNode* newRoot = new BTreeNode(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);

        //Figure out which child gets the new key
        int i = 0;
        if (newRoot->keys[0].clubName < clubName) {
            i++;
        }
        //Check if the promoted key is our club
        if (newRoot->keys[0].clubName == clubName) {
            newRoot->keys[0].eloRatings.push_back(eloRating);
            newRoot->keys[0].dates.push_back(date);
        } else {
            insertNonFull(newRoot->children[i], clubName, eloRating, date);
        }
        return newRoot;
    }
    else {
        insertNonFull(root, clubName, eloRating, date);
        return root;
    }
}

//Lookup function - Searches the b-tree for a club and returns the formatted string
//Output format: "date,elo|date,elo|date,elo" (newest first)
string lookup(BTreeNode* root, string& clubName) {
    if (root == nullptr) {
        return "Club not found";
    }

    //Search through keys in this node
    int i = 0;
    while (i < root->n && root->keys[i].clubName < clubName) {
        i++;
    }

    //Found the club in this node
    if (i < root->n && root->keys[i].clubName == clubName) {
        string result = "";
        //Loop backwards through dates vector (newest to oldest)
        for (int j = root->keys[i].dates.size() - 1; j >= 0; j--) {
            result += root->keys[i].dates[j] + "," + to_string(root->keys[i].eloRatings[j]);
            if (j > 0) {
                result += "|";
            }
        }
        return result;
    }

    //If this is a leaf and we didnt find it, club doesnt exist
    if (root->leaf) {
        return "Club not found";
    }

    //Recurse into the appropriate child
    return lookup(root->children[i], clubName);
}

//Finds the club in the b-tree and returns a pointer to its ClubNode data
//This gives Michele a clean ClubNode* to build the heap from
ClubNode* findNode(BTreeNode* root, string clubName) {
    if (root == nullptr) {
        return nullptr;
    }

    int i = 0;
    while (i < root->n && root->keys[i].clubName < clubName) {
        i++;
    }

    //Found the club in this node, return pointer to its data
    if (i < root->n && root->keys[i].clubName == clubName) {
        return &root->keys[i];
    }

    //If leaf and not found, club doesnt exist
    if (root->leaf) {
        return nullptr;
    }

    //Recurse into the appropriate child
    return findNode(root->children[i], clubName);
}

string getTop10ForClub(ClubNode* club){
    // creates the priority heap
    std::priority_queue <std::pair<double, std::string>> leaderboard;

    // initializes a result string to store the result
    string result; 

    // loops through the ratings pushing the elo, date pair onto the heap
    for(int i = 0; i < club -> eloRatings.size(); i++){
        // pushes the eloRating, date pair onto the heap
        leaderboard.push({club -> eloRatings[i], club -> dates[i]});
        }
    // extracts only the top 10
    for(int i = 0; i < 10 && !leaderboard.empty(); i++){
        // stores the entry to a variable before popping it off the top 
        auto entry = leaderboard.top();
        leaderboard.pop();

        // builds the string
        result += entry.second + "," + std::to_string(entry.first);
        
        // makes sure the | is not added to the last entry
        if (i < 9 && !leaderboard.empty()) {
            result += "|";
        }   
    }
    // returns the result string
    return result;
}


int main(int argc, char* argv[]) {
    //Argc and argv capture what's being written in the terminal
    //We run ./server "clubName", where clubName is what the user typed in
    BTreeNode* root = nullptr;

    if (argc < 2) {
        cout << "Please provide the name of the Club Name." << endl;
        return 1;
    }

    ifstream file("EloRatings.csv");
    string date, name, country, eloRating;
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, date, ',');
        //Country in this case isn't required, but you can still read it in
        getline(ss, name, ',');
        getline(ss, country, ',');
        getline(ss, eloRating, ',');
        //Remember to convert elorating to a double using stod
        try {
        root = insert(root, toLowerCase(name), stod(eloRating), date);
    } catch (...) {
        continue; // skip any malformed lines
    }
    };

    string clubName = toLowerCase(argv[1]); //Convert to lowercase for case insensitive search
    string result = lookup(root, clubName);  //Search the b-tree for the club

    ClubNode* foundNode = findNode(root, clubName);
    if(foundNode == nullptr) {
        std::cout << result; //Club not found, just print that
        delete root;
        return 0;
    }
    std::string top10 = getTop10ForClub(foundNode); //Placeholder for the heap function

    std::cout << result + "#" + top10;

    //After # is where the heap will be placed

    delete root; //Free memory allocated for the b-tree
    return 0;
}