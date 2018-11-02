
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <stdint.h>
#include <iomanip>  
#include "AtomicName/AtomicNumberToNameAndSymbol.hpp"
#include <string>

#if defined(_WIN32)
#include <experimental/filesystem> // C++-standard header file name  
#include <filesystem> // Microsoft-specific implementation header file name  
using namespace std::experimental::filesystem::v1;

#endif

#include "Isotope.hpp"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

struct Isotope;

bool ReadAtomicMass(std::string FilePath);

bool WriteCPPHeaderFile();

const int MAX_CHARS_PER_LINE = 200;
const int LINES_TILL_DATA = 39;
const int AtomZMax = 110;

std::vector<Isotope> IsotopeData;

int main(int argc, char** argv) {
    std::cout << "HELLO" << std::endl;

#if defined(_WIN32)
    std::cout << std::experimental::filesystem::current_path() << std::endl;
#endif

    IsotopeData.reserve(20000);
	//ReadHip("Hip_test.dat");
    ReadAtomicMass("./data/mass16.txt");
    WriteCPPHeaderFile();
    system("pause");
	return 0;
}







void PrintIsotopeInfo( const Isotope & isotope )
{
    std::cout << "Symbol:" <<isotope.Symbol << " A:" << isotope.A << " Mass:" << isotope.AtomicMassInteger << " " << isotope.AtomicMassMantissa << std::endl;
	/*std::cout 	<< "ID: " << star.ID
				<< " P: " << star.Parallax
				<< " RA: " << star.RA
				<< " DE: " << star.DE
				<< " Mag: " << star.ApparentMagnitude
				<< " BV: " << star.BV 
				<< std::endl; */
}

//https://stackoverflow.com/questions/5891610/how-to-remove-certain-characters-from-a-string-in-c
void removeCharsFromString(std::string &str, char* charsToRemove) {
    for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
        str.erase(std::remove(str.begin(), str.end(), charsToRemove[i]), str.end());
    }
}


bool ParseLineData(std::string &lineData, std::vector<std::string> &columns) {
    if (!lineData.empty()) {
        columns.emplace_back(lineData.substr(0, 1));
        columns.emplace_back(lineData.substr(1, 3));
        columns.emplace_back(lineData.substr(4, 5));
        columns.emplace_back(lineData.substr(9, 5));
        columns.emplace_back(lineData.substr(14, 5));
        columns.emplace_back(lineData.substr(19, 1));
        columns.emplace_back(lineData.substr(20, 3));
        columns.emplace_back(lineData.substr(23, 4));
        columns.emplace_back(lineData.substr(27, 1));
        columns.emplace_back(lineData.substr(28, 13));
        columns.emplace_back(lineData.substr(41, 11));
        columns.emplace_back(lineData.substr(52, 11));
        columns.emplace_back(lineData.substr(63, 9));
        columns.emplace_back(lineData.substr(72, 1));
        columns.emplace_back(lineData.substr(73, 2));
        columns.emplace_back(lineData.substr(75, 11));
        columns.emplace_back(lineData.substr(86, 9));
        columns.emplace_back(lineData.substr(95, 1));
        columns.emplace_back(lineData.substr(96, 3));
        columns.emplace_back(lineData.substr(99, 1));
        columns.emplace_back(lineData.substr(100, 12));
        columns.emplace_back(lineData.substr(112, 11));

        //Sanatize data:
        for (auto it = columns.begin(); it != columns.end(); it++) {
            char removeCharecters[] = "*# ";
            removeCharsFromString(*it, removeCharecters);
        }
        return true;
    }
    return false;
}

bool AddIsotope(std::vector<std::string> &columns) {
    //Generate Isotope from data
    IsotopeData.emplace_back(
        Isotope(
            columns[0],
            StringToInt(columns[1]),
            StringToInt(columns[2]),
            StringToInt(columns[3]),
            StringToInt(columns[4]),
            columns[5],
            columns[6],
            columns[7],
            columns[8],
            StringToDouble(columns[9]),
            StringToDouble(columns[10]),
            StringToDouble(columns[11]),
            StringToDouble(columns[12]),
            columns[13],
            columns[14],
            StringToDouble(columns[15]),
            StringToDouble(columns[16]),
            columns[17],
            StringToInt(columns[18]),
            columns[19],
            StringToDouble(columns[20]),
            StringToDouble(columns[21])
        ));
    //PrintIsotopeInfo(IsotopeData.back());
    return true;
}

bool ReadAtomicMass(std::string FilePath) {	
    std::cout << "In fucntion readatomic mass" << std::endl;
    //Buffers	
    char buffer[MAX_CHARS_PER_LINE];
    std::vector<std::string> columns; // Holds data in columns
    

  	std::ifstream file;
	file.open( FilePath.c_str() ); // open a file
    if (!file.good()) {
        std::cout << "FILE NOT FOUND" << std::endl;
        return 0; // exit if file not found
    }
    	
    
    
    //Advance pass header:
    for (int i = 1; i <= LINES_TILL_DATA; i++) {
        file.getline(buffer, MAX_CHARS_PER_LINE);
    }

    while (!file.eof())
  	{

        //Clears columns data
        columns.clear();
        columns.reserve(24);

  		file.getline( buffer, MAX_CHARS_PER_LINE );
  		
        //Turn into columns
        std::string lineData = std::string(buffer);
        
        if (ParseLineData(lineData, columns)) {
            AddIsotope(columns);
        }
      
	}
    PrintIsotopeInfo(IsotopeData[0] );
    std::cout << "Size of isotope database:" << IsotopeData.size() << std::endl;
  	file.close();
  	
  	return 1;
}

bool CreateIsotopeTable(std::vector<std::vector<Isotope*>> &isotopeTable) {
    isotopeTable.clear();
    isotopeTable.resize(200);

    for (auto it = IsotopeData.begin(); it != IsotopeData.end(); it++) {
        isotopeTable[it->Z].emplace_back(&(*it));
    }
    return true;
}







void CreateDoxygenGroups(std::ofstream &file, std::vector<std::vector<Isotope*>> &isotopeTable) {

    for (int Z = 0; Z < AtomZMax; Z++) {

        auto name = atomicNameSymbols[Z];
        
        file << std::endl;
        file << "/// @defgroup AtomMassConst-" << name.first << " " << name.first << std::endl;
        file << "/// @ingroup AtomMassConst" << std::endl;
        file << std::endl;

        //Isotopes
        for (std::vector<Isotope*>::iterator it = (isotopeTable[Z]).begin(); it != (isotopeTable[Z]).end(); it++) {

            file << "///     @defgroup AtomMassConst-" << name.first << "-" << name.second << (**it).A <<  " " << name.first << " " << (**it).A << std::endl;
            file << "///     @ingroup AtomMassConst-" << name.first << std::endl;
            file << std::endl;
        }

    }
}

void CreateConstant(std::ofstream &file, Isotope &isotope, std::string &constName) {
    int constPad = 30;
    int namePad = 35;
    int spacePad = 20;
    int perc = 15; // double percision

    double AtomicMass = isotope.AtomicMassInteger + (isotope.AtomicMassMantissa * 1e-6);
    double AtomicMassUnc = isotope.AtomicMassUnc * 1e-6;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_INT    " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "Neutrons = " )    << std::right << std::setw(spacePad) << isotope.N <<                                             ";           " << "/**< \f$ \f$ Neutrons in " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << ". */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_INT    " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "Protons = ")      << std::right << std::setw(spacePad) << isotope.Z <<                                             ";           " << "/**< \f$ \f$ Protons in " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << ". */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_INT    " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "AtomicWeight = ") << std::right << std::setw(spacePad) << isotope.A <<                                             ";           " << "/**< \f$ \f$ Atomic weight of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << ". */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_DOUBLE " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "MassExc = ")      << std::right << std::setw(spacePad) << std::setprecision(perc) << isotope.MassExc <<              ";           " << "/**< \f$ \f$ Mass excess of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << " in keV. */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_DOUBLE " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "MassExcUnc = ")   << std::right << std::setw(spacePad) << std::setprecision(perc) << isotope.MassExcUnc <<           ";           " << "/**< \f$ \f$ Uncertainty in mass excess of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << " in keV. */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_DOUBLE " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "BindEPerA = ")    << std::right << std::setw(spacePad) << std::setprecision(perc) << isotope.BindEPerA <<            ";           " << "/**< \f$ \f$ Binding energy per A of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << " in keV. */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_DOUBLE " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "BindEPerAUnc = ") << std::right << std::setw(spacePad) << std::setprecision(perc) << isotope.BindEPerAUnc <<         ";           " << "/**< \f$ \f$ Uncertainty in binding energy per A of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << " in keV. */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_DOUBLE " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "AtomicMass = ")   << std::right << std::setw(spacePad) << std::setprecision(perc) << AtomicMass <<                   ";           " << "/**< \f$ \f$ Atomic mass of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << " in amu. */" << std::endl;
    file << std::left << std::setw(constPad) << "   ATOMMASSCONST_DOUBLE " << std::setw(namePad) << (constName + std::to_string(isotope.A) + "AtomicMassUnc = ")<< std::right << std::setw(spacePad) << std::setprecision(perc) << AtomicMassUnc <<                ";           " << "/**< \f$ \f$ Uncertainty in atomic mass of " << atomicNameSymbols[isotope.Z].first << "-" << isotope.A << " in amu. */" << std::endl;

}

void CreateConstantGroup(std::ofstream &file, std::vector<std::vector<Isotope*>> &isotopeTable) {
    //Iterate over all proton numbers
    for (int Z = 0; Z < AtomZMax; Z++) {
        auto name = atomicNameSymbols[Z];


        //Isotopes
        for (auto it = isotopeTable[Z].begin(); it != isotopeTable[Z].end(); it++) {
            file << "   /// @addtogroup AtomMassConst-" << name.first << "-" << name.second << (**it).A << std::endl;
            file << "   /// @{" << std::endl;
            CreateConstant(file, **it, name.first);
            file << "   /// @}" << std::endl;
            file << std::endl;
            
        }

        
    }
}


bool WriteCPPHeaderFile() {
    std::vector<std::vector<Isotope*>> isotopeTable;
    std::ofstream file;
    
    file.open("Atom.hpp", std::ios::out);
    if (!file.good())
    {
        std::cout << "File Write Error";
    }

    file << "#ifdef __cplusplus  //Check if C++" << std::endl;
    file << "   #ifdef ATOMMASSCONST_PRECXX11" << std::endl;
    file << "       #define ATOMMASSCONST_INT static const int" << std::endl;
    file << "       #define ATOMMASSCONST_DOUBLE static const double" << std::endl;
    file << "   #else" << std::endl;
    file << "       #define ATOMMASSCONST_INT static constexpr int" << std::endl;
    file << "       #define ATOMMASSCONST_DOUBLE static constexpr double" << std::endl;
    file << "   #endif" << std::endl;
    file << "#else   //For C" << std::endl;
    file << "   #define ATOMMASSCONST_INT static const int" << std::endl;
    file << "   #define ATOMMASSCONST_DOUBLE static const double" << std::endl;
    file << "#endif" << std::endl;



    CreateIsotopeTable(isotopeTable);
    CreateDoxygenGroups(file, isotopeTable);
    CreateConstantGroup(file, isotopeTable);

        
    return true;
}

