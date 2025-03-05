#include <iostream>
#include <vector>
#include <fstream> 
#include <string> 

using namespace std;

// Struct to Store readings from the netlist //
struct branchElement
{
    // V or R for voltage source or resistor (X means uninitialized)// 
    char type = 'X'; 
    int label = 0;

    int sourceNode = 0;
    int destNode = 0;
    int compVal = 0;

};



void readNetlist(vector<branchElement> &netlist)
{
    fstream file;
    string line = "V";
    string identifier;
    string sourceN;
    string destN;
    string compVal;
    int index = 0;
    int line_element = 0;
    branchElement sample;


    file.open("netlist.txt", ios::in);
    if(file.is_open())
    {
        getline(file,line);
        
        while(line[0] == 'V' || line[0] == 'R')
        {

            line.push_back('\n');
            identifier = {};
            sourceN = {};
            destN = {};
            compVal = {};
            line_element = 0;


            if (line [line_element] == 'V' || line[line_element] == 'R')
            {
                // STORES IDENTIFIER //
                netlist.push_back(sample);
                netlist[index].type = line[line_element];
                line_element++;
            }
                
            // STORES ELEMENT NUMBER // 
            while (line[line_element] != ' ')
            {
                identifier.push_back(line[line_element]);
                line_element++;
            }
            line_element++;
            // STORES SOURCE NODE // 
            while (line[line_element] != ' ')
            {
                sourceN.push_back(line[line_element]);
                line_element++;
            }
            line_element++;
            // STORES DEST NODE // 
            while (line[line_element] != ' ' )
            {
                destN.push_back(line[line_element]);
                line_element++;
            }
            line_element++;
            // STORES COMP VAL // 
            while (line[line_element] != '\n' )
            {
                compVal.push_back(line[line_element]);
                line_element++;
            }

            netlist[index].label = stoi(identifier);
            netlist[index].sourceNode = stoi(sourceN);
            netlist[index].destNode = stoi(destN);
            netlist[index].compVal = stoi(compVal);
            index++;
            getline(file,line);
     
        }

    }
        

        file.close();
   
    }
    





int main()
{
    vector<branchElement> netlist;
    readNetlist(netlist);
    string test = "1234";
    int bruh = stoi(test);




    // string line;
    // int index = 0;
    // branchElement sample;


    // do
    // {
    //     line  = readNetlist_line();

    //     if (line[0] == 'R')
    //     {
    //         netlist[index].type = 'R';
    //         netlist[index].label = line[1];

    //     }
    //     else if (line[0] == 'V')
    //     {
    //         netlist.push_back(sample);
    //         netlist[index].type = 'V';
    //         netlist[index].label = line[1] - '0';

    //     }

    //     index++;
    // } while (line[0] == 'V' || line[0] == 'R');
    






    return 0;
}