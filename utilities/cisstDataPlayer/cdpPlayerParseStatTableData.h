#ifndef _cdpPlayerParseStatTableData_h
#define _cdpPlayerParseStatTableData_h

#include <cisstMultiTask.h>
// Always include last
#include "cdpExport.h"

class CISST_EXPORT cdpPlayerParseStatTableData{

public:
    cdpPlayerParseStatTableData();
    ~cdpPlayerParseStatTableData();
    // read header
    void ParseHeader(std::string);

    // We need to generate index for each input file
    // it generates a index per 1MB.
    // getnerateIndex from data file
    void GenerateIndex(void);
    // write Index to File
    void WriteIndexToFile(std::string);
    // Load Data into buffer by time index
    // Load a range of data, we may use a ping-pong buffer. Range should be time or data size. I am not sure yet
    // Compare TimeStampForSearch with TimeField which is set from user and load data & time to Data,Time respectively
    void LoadDataFromFile(double TimeStampForSearch, std::vector <double> &Data, std::vector <double> &Time ); 
    void SetDataFieldForSearch(std::string DataField);
    void SetTimeFieldForSearch(std::string TimeField);
    void GetBeginEndTime(double &Begin, double &End);

    // Test Function, remove later
    void TestIndex(void);
private:
    typedef vctDynamicVector<double> TimeField;
    typedef vctDynamicVector<double> DataField;
    
    std::string DataFieldForSearch;
    std::string TimeFieldForSearch;
    unsigned int IndexOfTimeField;
    unsigned int IndexOfDataField;

    void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");

    //structure for parsing header
    struct{
        std::string FilePath;
        std::string Date;
        std::string FileFormat;
        std::string Delimiter;
        unsigned int NumberOfTotalFields;
        unsigned int NumberOfTimeFields;
        unsigned int NumverOfDataFields;
        // retrieve type and dataName and write to Fields, which is a 2D vector
        // string is worked for CSV file, but for binary, we have to use other ways
        std::vector <std::string> Fields;
    }Header;

    // structure for generating index file
    struct IndexElement{
        // How far from beginning
        unsigned int ByteCount;
        // TimeStamp at the point
        std::string LineAtIndex;
    };

    std::vector <struct IndexElement> Index;

/*
    struct{
        struct{
            vctDynamicVector<TimeField> Time;    
            std::vector <int> pos;
        }Time;
        struct{
            vctDynamicVector<DataField> Data;
            std::vector <int> pos;
        }Data;
    }Elements;
*/

};



#endif