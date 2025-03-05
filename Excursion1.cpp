#include <iostream>
#include <vector>
#include <fstream> 
#include <string> 
#include <cmath>
#include <iomanip>

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
        
        while(getline(file,line))
        {
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
        }

    }
        

    file.close();
    return numRowsA;
}
    
// Function to row-reduce a matrix into reduced row echelon form
void RREFReduction(vector<vector<float>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    int pivRow = 0;
    int col = 0;
    while (col < cols && pivRow < rows)
    {
        int maxRow = pivRow;
        for (int i = pivRow + 1; i < rows; i++) {
            if (abs(matrix[i][col]) > abs(matrix[maxRow][col])) {
                maxRow = i;
            }
        }
        
        if (matrix[maxRow][col] != 0) {
            swap(matrix[pivRow], matrix[maxRow]);

            // Make the pivot 1
            float pivot = matrix[pivRow][col];
            for (int j = col; j < cols; j++) {
                matrix[pivRow][j] /= pivot;
            }
    
            // Turn values below pivot to 0
            for (int i = pivRow + 1; i < rows; i++) {
                if (matrix[i][col] != 0) {
                    float f = matrix[i][col];
                    for (int j = col; j < cols; j++) {
                        matrix[i][j] -= f * matrix[pivRow][j];
                    }
                }
            }
            pivRow++;
            col++;
        }
    }


    // Turn values above the pivot to 0
    for (int col = cols - 1; col >= 0; col--) 
    {
        for (int row = 0; row < rows; row++) 
        {
            if (matrix[row][col] == 1) 
            {
                for (int i = row - 1; i >= 0; i--) 
                {
                    if (matrix[i][col] != 0) 
                    {
                        float f = matrix[i][col];
                        for (int j = col; j < cols; j++) 
                        {
                            matrix[i][j] -= f * matrix[row][j];
                        }
                    }
                }
            }
        }
    }
}

// Function to fill A matrix with the netlist values
void fillAMatrix(vector<branchElement>& netlist, int matrixSize, int numNodes, vector<vector<float>>& matrix) {
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

    // Read netlist, fill a matrix, and store size
    matrixSize = readNetlist(netlist) + 1;

    int x = netlist.size();
    vector<vector<float>> aMatrix(matrixSize, vector<float>(x, 0.0f));
    fillAMatrix(netlist, matrixSize, x, aMatrix);

    // Remove the first row of the matrix
    aMatrix.erase(aMatrix.begin());

    int aRows = aMatrix.size();
    int aCols = aMatrix[0].size();

    // Transpose the matrix to AT
    vector<vector<float>> ATMatrixNeg(aCols, vector<float>(aRows, 0.0f));
    int aTRows = ATMatrixNeg.size();
    int aTCols = ATMatrixNeg[0].size();

    // Fill the AT matrix with the negative values of the A matrix in transposed positions
    for (int i = 0; i < aTRows; ++i) {
        for (int j = 0; j < aTCols; ++j) {
            ATMatrixNeg[i][j] = -1 * aMatrix[j][i];
        }
    }

    // Identity Matrix
    vector<vector<float>> iMatrix(x, vector<float>(x, 0.0f));
    for (int i = 0; i < x; ++i) {
        iMatrix[i][i] = 1;
    }

    // Fill n matrix with the negative values of the resistors
    vector<vector<float>> nMatrix(x, vector<float>(x, 0.0f));
    for (int i = 0; i < x; ++i) {
        if (netlist[i].type == 'R') {
            nMatrix[i][i] = -netlist[i].compVal;
        }
    }

    // Fill u matrix with the values of the voltage sources
    vector<vector<float>> u(x, vector<float>(1, 0.0f));
    for (int i = 0; i < x; ++i) {
        if (netlist[i].type == 'V') {
            u[i][0] = netlist[i].compVal;
        }
    }

    // Initialize the T matrix
    int tSize = (matrixSize - 1) + 2 * x;
    vector<vector<float>> T(tSize, vector<float>(tSize, 0.0f));

    // Fill the T matrix with the values of A Matrix
    for (int i = 0; i < aRows; ++i) {
        for (int j = 0; j < aCols; ++j) {
            T[i][j + tSize - x] = aMatrix[i][j];
        }
    }

    // Fill the T matrix with the values of ATMatrixNeg
    for (int i = 0; i < aTRows; ++i) {
        for (int j = 0; j < aTCols; ++j) {
            T[i + aRows][j] = ATMatrixNeg[i][j];
            if (T[i + aRows][j] == -0.0f) {
                T[i + aRows][j] = 0.0f;
            }
        }
    }

    // Fill the T matrix with the values of iMatrix
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < x; ++j) {
            T[i + aRows][j + aTCols] = iMatrix[i][j];
        }
    }

    // Fill the T matrix with the values of the iMatrix
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < x; ++j) {
            T[i + aRows + x][j + aTCols] = iMatrix[i][j];
        }
    }

    // Fill the T matrix with the values of the nMatrix
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < x; ++j) {
            T[i + tSize - x][j + tSize - x] = nMatrix[i][j];
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

    // Perform row reduction
    RREFReduction(eMatrix);

    // Fix the -0 values
    for (int i = 0; i < tSize; ++i) {
        for (int j = 0; j < tSize + 1; ++j) {
            if (eMatrix[i][j] == -0.0f) {
                eMatrix[i][j] = 0.0f;
            }
        }
    }

    // Print the last column values to output.txt with 3 decimal places
    ofstream outFile("output.txt");
    if (outFile.is_open()) {
        outFile << fixed << setprecision(3);
        for (int i = 0; i < tSize; ++i) {
            outFile << eMatrix[i][tSize] << " ";
        }
        outFile << endl;
        outFile.close();
    }

    return 0;
}