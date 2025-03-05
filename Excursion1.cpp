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
    float compVal = 0.0f;
};



int readNetlist(vector<branchElement> &netlist)
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

    int numRowsA = 0;


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
                numRowsA = max(numRowsA, stoi(sourceN));
            }
            line_element++;
            // STORES DEST NODE // 
            while (line[line_element] != ' ' )
            {
                destN.push_back(line[line_element]);
                line_element++;
                numRowsA = max(numRowsA, stoi(destN));
            }
            line_element++;
            // STORES COMP VAL // 
            while (line[line_element] != '\n' && line[line_element] != '\0')
            {
                compVal.push_back(line[line_element]);
                line_element++;
            }

            netlist[index].label = stoi(identifier);
            netlist[index].sourceNode = stoi(sourceN);
            netlist[index].destNode = stoi(destN);
            netlist[index].compVal = stof(compVal);

            index++;
            getline(file,line);
     
        }

    }
        

        file.close();
        return numRowsA;
    }
    
    void fillMatrix(vector<branchElement>& netlist, int matrixSize, int numNodes, vector<vector<float>>& matrix) {
        // Initialize matrix with zeros
        matrix = vector<vector<float>>(matrixSize, vector<float>(numNodes, 0.0f));
    
        int i = 0;
        for (const auto& elem : netlist) {
            int source = elem.sourceNode;
            int dest = elem.destNode;

            matrix[source][i] = 1;
            matrix[dest][i] = -1;

            i++;
        }
    }




int main()
{
    vector<branchElement> netlist;
    int matrixSize = 0;

    // Read netlist and fill a matrix
    matrixSize = readNetlist(netlist) + 1;

    int x = netlist.size();
    vector<vector<float>> aMatrix(matrixSize, vector<float>(x, 0.0f));

    fillMatrix(netlist, matrixSize, x, aMatrix);

    // Remove the first row of the matrix
    aMatrix.erase(aMatrix.begin());

    int aRows = aMatrix.size();
    int aCols = aMatrix[0].size();

    // Transpose the matrix
    vector<vector<float>> ATMatrixNeg(aCols, vector<float>(aRows, 0.0f));

    int aTRows = ATMatrixNeg.size();
    int aTCols = ATMatrixNeg[0].size();

    for (int i = 0; i < aTRows; ++i) {
        for (int j = 0; j < aTCols; ++j) {
            ATMatrixNeg[i][j] = -1 * aMatrix[j][i];
        }
    }

    vector<vector<float>> iMatrix(x, vector<float>(x, 0.0f));

    for (int i = 0; i < x; ++i) {
        iMatrix[i][i] = 1;
    }

    vector<vector<float>> fMatrix(x, vector<float>(x, 0.0f));

    for (int i = 0; i < x; ++i) {
        if (netlist[i].type == 'R') {
            fMatrix[i][i] = -netlist[i].compVal;
        }
    }

    vector<vector<float>> u(x, vector<float>(1, 0.0f));

    for (int i = 0; i < x; ++i) {
        if (netlist[i].type == 'V') {
            u[i][0] = netlist[i].compVal;
        }
    }

    int tSize = (matrixSize - 1) + 2 * x;
    vector<vector<float>> T(tSize, vector<float>(tSize, 0.0f));

    for (int i = 0; i < aRows; ++i) {
        for (int j = 0; j < aCols; ++j) {
            T[i][j + tSize - x] = aMatrix[i][j];
        }
    }

    for (int i = 0; i < aTRows; ++i) {
        for (int j = 0; j < aTCols; ++j) {
            T[i + aRows][j] = ATMatrixNeg[i][j];
            if (T[i + aRows][j] == -0.0f) {
                T[i + aRows][j] = 0.0f;
            }
        }
    }

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < x; ++j) {
            T[i + aRows][j + aTCols] = iMatrix[i][j];
        }
    }

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < x; ++j) {
            T[i + aRows + x][j + aTCols] = iMatrix[i][j];
        }
    }

    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < x; ++j) {
            T[i + tSize - x][j + tSize - x] = fMatrix[i][j];
        }
    }

    // Create a new matrix that is a copy of T but with one more column
    vector<vector<float>> eMatrix(tSize, vector<float>(tSize + 1, 0.0f));

    for (int i = 0; i < tSize; ++i) {
        for (int j = 0; j < tSize; ++j) {
            eMatrix[i][j] = T[i][j];
        }
    }

    for (int i = 0; i < x; ++i) {
        eMatrix[i + tSize - x][tSize] = u[i][0];
    }

    // Print the new matrix
    cout << "" << endl;
    for (int i = 0; i < tSize; ++i) {
        for (int j = 0; j < tSize + 1; ++j) {
            cout << eMatrix[i][j] << " ";
        }
        cout << endl;
    }




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