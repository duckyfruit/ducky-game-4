#include "DialogueTree.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void DialogueTree::Set_Name(std::string NPCname)
{
    name = NPCname;
}

void DialogueTree::Populate_NPC_Tree(std::string filename)
{
    std::ifstream readfile;
    readfile.open(filename);
    std::string line;
    std::string temp;
    int counter = -1;

    while (std::getline(readfile,line))
	{
        if(isdigit(line[0])) //check if the line is a digit
        {
            counter += 1; //convert char to int and put in counter 
           
            std::string newstr = temp;
            NPCTree.emplace_back(newstr);
            temp = " ";
           
        }
        else
        {
            temp.append(line);
            temp.push_back('\n');
        }
            
    }
    
    readfile.close();

}


void DialogueTree::Populate_Player_Tree(std::string filename)
{
    std::ifstream readfile;
    readfile.open(filename);
    std::string line;
    std::string temp;
    int counter = -1;

    while (std::getline(readfile,line))
	{
        if(isdigit(line[0])) //check if the line is a digit
        {
            counter += 1; //convert char to int and put in counter 
            
            std::string newstr = temp;
            PlayerTree.emplace_back(newstr);
            temp = " ";
       
        }
        else
        {
            temp.append(line);
            temp.push_back('\n');

        }
            
    }
    
    readfile.close();

}


void DialogueTree::Populate_Affection_Tree(std::string filename)
{
    std::ifstream readfile;
    readfile.open(filename);
    std::string line;

    while (std::getline(readfile,line)) //convert each line to an int and emplace back
	{
        int affecint = stoi(line, nullptr,10); 
        AffectionTree.emplace_back(affecint);
    }

}


std::string DialogueTree::get_NPC_Dialogue()
{
    if(index < NPCTree.size())
    return NPCTree[index];
    else
    return "";
}


std::string DialogueTree::get_Name()
{
    
    return name;
  
}

std::vector<std::string> DialogueTree::get_Player_Dialogue()
{
    std::vector<std::string> choices;
    if((index * 2 + 1) < PlayerTree.size() )
    {
        choices.emplace_back(PlayerTree[index * 2 + 1]);
        choices.emplace_back(PlayerTree[index * 2 + 2]);
    }
    
    return choices;
}

bool DialogueTree::isFriendly()
{
    return(affection > 0);
}

void DialogueTree::Update_Affection()
{
    affection += AffectionTree[index];
}


void DialogueTree::Update_Index(int decision)
{
    if(decision == 0)
    index = 2*index+1;
    else
    index = 2*index+2;
}


