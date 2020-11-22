#include "magic.hpp"
#include "io.hpp"
#include "main.h"

/*
 * This is a Transcompiler intended for small ~500 max python files.
 * I will probably work on larger projects, but now I'm just writing
 * this to transform my small time python scripts that I hevely use
 * into faster compiled file.
 */

int main(int argc, char *argv[]){

    //start(argc, argv);

    DocumentBucket Document;
    //set explicit scope to deallocate file data and save memory
    {
        std::string file;
        getfile("test.py", file);
        getPhysicalLineTokens(file, Document);
    }

    //convert to logical lines
    //aka look for ';'
    convertToLogicalLines(Document);
    //Keep python logic and CPP separate in order to modulerize code to differing langs

    //pythonToJson(str){

    //  lines = logicalLines(physicalLines)
    //  lines = foreach(line){ tokenizeLine(line) }
    //  nodes = SyntaxGroups(lines)
    //  python_Patterns = semanticAnalysis(groups)
    //
    //  jsonRepresentation = toJson(python_Patterns)
    //  ~Transform to Json~
    //  saveAndWriteToFile(jsonRepresentation)
    //
    //}

    //jsonToCPP(str){
    //  str = getfile()
    //  group
    //  str = stringifyCPPLogic(map<> patterns)
    //  writeCPPfile(str)
    //}
    return 0;
}
