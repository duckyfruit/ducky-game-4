
#include <vector>
#include <string>

struct DialogueTree{ //each NPC has a dialogue Tree

    DialogueTree::DialogueTree()
    {
        name = "";
        index = 0;
        affection = 0;

    }

    DialogueTree::DialogueTree(std::string NPCName)
    {
        name = NPCName;
        index = 0;
        affection = 0;

    }
    
    DialogueTree::DialogueTree(std::string NPCName, int startAffec)
    {
        name = NPCName;
        affection = startAffec;
    }
    

    uint8_t index = 0; //index pointing to the dialogue option
    int affection =0; //affection counter, could be anything but for this game to tell
                       //whether the fish will go with you
    std::string name = ""; //character name
    
    std::vector<std::string> NPCTree; //the tree structure for the decisions
    std::vector<std::string> PlayerTree; //the tree structure for the decisions
    std::vector<int> AffectionTree; //the tree structure for the affection counter

    void Set_Name(std::string NPCname); //fill the tree with the text file choices
    void Populate_NPC_Tree(std::string filename); //fill the tree with the text file choices
    void Populate_Player_Tree(std::string filename); //fill the tree with the text file choices
    void Populate_Affection_Tree(std::string filename); //fill the tree with the text file choices
    void Update_Index(int decision); //update the index pointer with the player decision
    void Update_Affection(); //update the affection counter

    std::string get_NPC_Dialogue(); //return a string of the NPC Dialogue
    std::vector<std::string> get_Player_Dialogue(); //return a vector of strings of the Player Dialogue
    std::string get_Name(); //return a string of the NPC Dialogue

    bool isFriendly(); //return whether the NPC likes you

};

