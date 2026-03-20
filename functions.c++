#include <iostream>
#include <vector>
using namespace std;
#include <fstream>
#include <sstream>
#include <queue>

struct ClubNode{
    std::string clubName;
    std::vector<double> eloRatings;
    ClubNode* left;
    ClubNode* right;
    std::vector<std::string> dates;
    

    ClubNode(std::string name, std::vector<double> ratings, std::vector<std::string> dates){
        clubName = name;
        eloRatings = ratings;
        this->dates = dates;
        left = nullptr;
        right = nullptr;
    }

    ~ClubNode(){
        //destructor to free memory
        delete left;
        delete right;
    }

};


ClubNode* insert(ClubNode* root, std::string clubName, double eloRating, std::string date){
    
}

std::string lookup(ClubNode* root, std::string& clubNames){
         return "2025-08,2050.5|2025-07,2040.3|2025-06,2030.1|2024-12,2010.4|2024-06,1990.2|2000-01,1800.0";
    };
    //lookup function- the whole point of this is to return a string that will be sent to our python script later on



std::string toLowerCase(std::string str){
    for(int i = 0; i < str.length(); i++){
        str[i] = tolower(str[i]);
    }
    return str;
}
//This is to make sure that the search is case insensitive- Apply it during lookup with the search item

ClubNode* findNode(ClubNode* root, std::string clubName){
    if(root == nullptr){
        return nullptr;
    }
    if(root->clubName == clubName){
        return root;
    }
    if(clubName < root->clubName){
        return findNode(root->left, clubName);
    }
    else{
        return findNode(root->right, clubName);
    }
}//standard lookup function to find the node associated to the club


int main(int argc, char* argv[]){
    //argc and argv capture what's being written in the terminal
    //the reason why we have to do this is because in our server.py file, we run ./server "clubName", where clubName is the clubname the user typed in
    ClubNode* root = nullptr;
    if(argc < 2){
        cout << "Please provide the name of the Club Name." << endl;
        return 1;
    }
    ifstream file("EloRatings.csv");
    string date, name, country, eloRating;
    string line;
    while(getline(file, line)){
        stringstream ss(line);
        getline(ss, date, ',');
        //country in this case isn't required, but you can still read it in if you want to
        getline(ss, name, ',');
        getline(ss, country, ',');
        getline(ss, eloRating, ',');
        //remember to convert elorating to a double using std::stod(eloRating) when putting it into the insert function

        //Commented it out for now- root=insert(root, toLowerCase(name), std::stod(eloRating), date);
};
    std::string clubName = toLowerCase(argv[1]);//convert the club name to lowercase for case insensitive search
    std::string result = lookup(root,clubName);//this lookup funciton is simply a placeholder for the actual search function we'll write for the binary tree.
   
    
   
    ClubNode* foundNode = findNode(root, clubName);
    //std::string top10 = getTop10(root); //placeholder for the heap function
   
    std::cout <<result + "#";
    // after # is where the heap will be placed


    delete root; //free memory allocated for the binary tree
    return 0;
}