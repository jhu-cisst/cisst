
#include <cisstOSAbstraction/osaGetTime.h>
#include "cdpPlayerParseStatTableData.h"
#include <cstdlib>
//CMN_IMPLEMENT_SERVICES(cdpPlayerParseStatTableData);

cdpPlayerParseStatTableData::cdpPlayerParseStatTableData(){

}

cdpPlayerParseStatTableData::~cdpPlayerParseStatTableData(){
    // release all elements
    Index.clear();
}

void cdpPlayerParseStatTableData::ParseHeader(std::string Path){
    std::string Temp;
    std::ifstream inf(Path.c_str());
    std::cerr<<Path<<"\n";
    // get FilePath
    std::getline(inf, Header.FilePath);
    std::getline(inf, Header.Date);   
    std::getline(inf, Header.FileFormat);
    std::getline(inf, Header.Delimiter);
    std::getline(inf, Temp);
    Header.NumberOfTotalFields =atoi(Temp.c_str());
    std::getline(inf, Temp);
    Header.NumberOfTimeFields = atoi(Temp.c_str());    
    std::getline(inf, Temp);
    Header.NumverOfDataFields = atoi(Temp.c_str());

    while(!std::getline(inf, Temp).eof() )
        Header.Fields.push_back(Temp);            
    inf.close();
}

// read data File and make index for every 1MB data
void cdpPlayerParseStatTableData::GenerateIndex(void){
    const unsigned long interval = 1<<18; //512 K
    size_t totalBytes = 0;    
    size_t Boundary = interval;
    size_t FileSize = 0;
    IndexElement TempElement = {0," "};
    IndexElement LastElement ={0," "};

    // Read from file
    // If it is CSV format, just read a line and write into index
    // if it is binary format, we have to re-arrange them into a string line, then save it
    if(Header.FileFormat.compare("CSV") == 0){
        std::ifstream inf(Header.FilePath.c_str());

        inf.seekg(0,std::ios::end);
        FileSize = inf.tellg();
        inf.seekg(0,std::ios::beg);

        while(Boundary < FileSize ){
            inf.seekg(Boundary,std::ios::beg);
            // we don't know where is here in file, but next line should be good
            std::getline(inf, TempElement.LineAtIndex); 
            // get good one
            TempElement.ByteCount = inf.tellg();
            std::getline(inf, TempElement.LineAtIndex);             
            TempElement.LineAtIndex = TempElement.LineAtIndex;
             Index.push_back(TempElement);
             Boundary+= interval;             
        }

        // get last line and position
        totalBytes = inf.tellg();
        while(!std::getline(inf, TempElement.LineAtIndex).eof()){
            LastElement.ByteCount = totalBytes;
            LastElement.LineAtIndex = TempElement.LineAtIndex;
            totalBytes = inf.tellg();
        }
        Index.push_back(LastElement);
    }
}


void cdpPlayerParseStatTableData::WriteIndexToFile(std::string Path){
    std::ofstream outf(Path.c_str());
    unsigned int i;

    for( i  = 0 ; i < Index.size(); i++){
        outf<<Index.at(i).ByteCount<<Header.Delimiter<<Index.at(i).LineAtIndex<<std::endl;
    }
    outf.close();
}


void cdpPlayerParseStatTableData::TestIndex(void){
// print out everything on index point
    std::ifstream inf(Header.FilePath.c_str()); 
    std::string TempString;
    unsigned int i  = 0;
    for( i  = 0 ; i < Index.size(); i++){
        inf.seekg(Index.at(i).ByteCount, std::ios::beg);
        std::getline(inf, TempString);
        std::cerr<<TempString<<"\n";
    }
    inf.close();
}

void cdpPlayerParseStatTableData::LoadDataFromFile(double TimeStampForSearch, std::vector <double> &Data, std::vector <double> &Time){

    // Load Relative Field 
    std::vector <double> TimeIndex;    
    unsigned int i  = 0;
    std::string ExtractLine;    
  
    // Claer Data and Time
    Data.clear();
    Time.clear();

    // where is TimeStampForSearch postition in index file? 
    for( i  = 0 ; i < Index.size(); i++){        
        std::vector <std::string> Token;   
        Tokenize(Index.at(i).LineAtIndex, Token, Header.Delimiter);// get rid of first string
        if(TimeStampForSearch < strtod(Token.at(IndexOfTimeField).c_str(), NULL ))
            break;
        Token.clear();        
    }    

    i == Index.size() ? (i = Index.size()-1) : i;

    // Load Data&Time from the interval which TimeStampForSearch lays
    // Data range is Index(i-1) to Index(i)
    size_t BeginPos = 0, EndPos = 0, CurrentPos = 0 ;
    if(i == 0){ // range from 0  to Index(0);
        BeginPos = 0; 
        EndPos = Index.at(0).ByteCount;
    }else{
        BeginPos = Index.at(i-1).ByteCount;
        EndPos = Index.at(i).ByteCount;
    }
    std::ifstream inf(Header.FilePath.c_str());
    inf.seekg(BeginPos, std::ios::beg);

    do{
        std::string TempLine;
        std::vector <std::string> Token;   
        std::getline(inf, TempLine);
        Tokenize(TempLine, Token, Header.Delimiter);
        Data.push_back( strtod(Token.at(IndexOfDataField).c_str() , NULL) ); 
        Time.push_back( strtod(Token.at(IndexOfTimeField).c_str() , NULL) ); 
        CurrentPos = inf.tellg();
    }while(CurrentPos != EndPos);

    inf.close();
    return;

}

void cdpPlayerParseStatTableData::GetBeginEndTime(double &Begin, double &End){
    
    std::ifstream inf(Header.FilePath.c_str());
    std::string TempLine;
    std::vector <std::string> Token;      
    std::getline(inf, TempLine);
    Tokenize(TempLine, Token, Header.Delimiter);

    Begin = strtod(Token.at(IndexOfTimeField).c_str() , NULL);
    inf.close();

    Token.clear();
    Tokenize( Index.at(Index.size()-1).LineAtIndex, Token, Header.Delimiter);
    End = strtod(Token.at(IndexOfTimeField).c_str() , NULL);
}


void cdpPlayerParseStatTableData::SetDataFieldForSearch(std::string DataField){
    DataFieldForSearch = DataField;
    // find IndexOfDataField
    for(int i = 0; i < Header.Fields.size(); i ++){
        if(Header.Fields.at(i).find(DataFieldForSearch) != std::string::npos){
            IndexOfDataField = i;
            break;
        }
    }
}

void cdpPlayerParseStatTableData::SetTimeFieldForSearch(std::string TimeField){
    TimeFieldForSearch = TimeField;
    // find IndexOfTimeField
    for(int i = 0; i < Header.Fields.size(); i ++){
        if(Header.Fields.at(i).find(TimeFieldForSearch) != std::string::npos){
            IndexOfTimeField = i;
            break;
        }
    }
}


void cdpPlayerParseStatTableData::Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
