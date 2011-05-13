
#include <cisstOSAbstraction/osaGetTime.h>
#include "cdpPlayerParseStatTableData.h"

#include <cstdlib>

CMN_IMPLEMENT_SERVICES(cdpPlayerParseStatTableData);

cdpPlayerParseStatTableData::cdpPlayerParseStatTableData(const std::string & name)
: mtsTaskContinuous(name)
{
    StartRun = false;
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
    //while(!std::getline(inf, Temp).eof() )
    for(int i = 0; i < Header.NumberOfTotalFields; i++){
        std::getline(inf, Temp);
        Header.Fields.push_back(Temp);
    }    
#if 0
    std::cerr<<"/***************************************************************************/"<<std::endl;
    std::cerr<<"FilePath: "<<Header.FilePath<<  std::endl;
    std::cerr<<"Date: "<<Header.Date<<  std::endl;
    std::cerr<<"FileFormat: "<<Header.FileFormat<<  std::endl;
    std::cerr<<"Delimiter: "<<Header.Delimiter<<  std::endl;
    std::cerr<<"NumberOfTotalFields: "<<Header.NumberOfTotalFields<<  std::endl;
    std::cerr<<"NumberOfTimeFields: "<<Header.NumberOfTimeFields<<  std::endl;
    std::cerr<<"NumverOfDataFields: "<<Header.NumverOfDataFields<<  std::endl;
    for(int i = 0; i < Header.NumberOfTotalFields; i++){
        std::cerr<<"Fields: "<<Header.Fields.at(i)<<  std::endl;      
    }  
#endif
    inf.close();
}

// read data File and make index for every 1MB data
void cdpPlayerParseStatTableData::GenerateIndex(void){
    const unsigned long interval = 1<<12; //512 K
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

// return next boundary of time
void cdpPlayerParseStatTableData::GetBoundary(vctPlot2DBase::Trace *  TraceHandle, double &TopBoundary, double &LowBoundary){

    TraceHandle->GetLeftRightDataX(LowBoundary,TopBoundary);
    return;
}

void cdpPlayerParseStatTableData::GetBoundary(double TimeStampForSearch, double &TopBoundary, double &LowBoundary){
    // Load Relative Field 
    std::vector <double> TimeIndex;    
    unsigned int i  = 0;
    std::string ExtractLine;        
    TopBoundary = LowBoundary = 0.0;
        // where is TimeStampForSearch postition in index file? 
    for( i  = 0 ; i < Index.size(); i++){        
        std::vector <std::string> Token;   
        Tokenize(Index.at(i).LineAtIndex, Token, Header.Delimiter);// get rid of first string
        TopBoundary =  strtod(Token.at(IndexOfTimeField).c_str(), NULL );
        if(TimeStampForSearch <= TopBoundary)           
            break;        
        LowBoundary = TopBoundary;
        Token.clear();
    } 

    return;
}

size_t cdpPlayerParseStatTableData::GetDataPoisitionFromFile(double Data, int FieldNumber){
    
    int i ;
    size_t TryToCount = 0;

    for( i  = 0 ; i < Index.size(); i++){        
        std::vector <std::string> Token;   
        Tokenize(Index.at(i).LineAtIndex, Token, Header.Delimiter);// get rid of first string
        if(Data <= strtod(Token.at(FieldNumber).c_str(), NULL ))
            break;
        Token.clear();
    }

     i == Index.size() ? (i = Index.size()-1) : i;
    
    size_t CurrentPos, EndPos;
    if(i == 0)
        CurrentPos = 0;
    else
        CurrentPos = Index.at(i-1).ByteCount;
    EndPos = Index.at(i).ByteCount;

    std::ifstream inf(Header.FilePath.c_str());
    double DataElement;
    TryToCount = CurrentPos;
    std::string TempLine;
    // binary search in file
    do{
        size_t Mid = (CurrentPos+EndPos)/2;
        inf.seekg(Mid, std::ios::beg);
        std::vector <std::string> Token;   
        // discard the unalignment line
        std::getline(inf, TempLine); 
        Mid = inf.tellg();
        if(CurrentPos >= EndPos || CurrentPos == Mid || EndPos == Mid)
            break;
        // read a whole line
        std::getline(inf, TempLine); 
        TryToCount += TempLine.size()+1;
        Tokenize(TempLine, Token, Header.Delimiter);
        DataElement = strtod(Token.at(FieldNumber).c_str() , NULL); 
        if(DataElement < Data)
            CurrentPos = Mid;
        else
            EndPos = Mid;
    }while(1);

    inf.close();

    return CurrentPos;
}

void cdpPlayerParseStatTableData::TriggerLoadDataFromFile(vctPlot2DBase::Trace *  TraceHandle ,double TimeStampForSearch, double VisualRange,  bool ResetTraceBuffer){
    CS.Enter();
    this->TracePointer = TraceHandle;
    this->MiddleTime = TimeStampForSearch;
    this->TimeRange = VisualRange;
    this->ResetBuffer = ResetTraceBuffer;
    this->StartRun = true;
    CS.Leave();
}


void cdpPlayerParseStatTableData::LoadDataFromFile(vctPlot2DBase::Trace *  TraceHandle ,double TimeStampForSearch, double VisualRange,  bool ResetTraceBuffer){
    double MinimumTime, MaxmumTime; 
    std::string TempLine;
    std::vector <std::string> Token;  
    std::vector <double> TimeIndex;
    size_t LeftBoundaryPosition = 0, RightBoundaryPosition = 0;
    double LeftBoundaryTime = 0.0, RightBoundaryTime = 0.0;
    size_t ElementsNumber =0, TraceBufferSize = 0;

    if(ResetTraceBuffer){
        TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);     
        TraceHandle->SetSize(TraceBufferSize);
    }
    // Get Max & Minimum Time
    std::ifstream inf(Header.FilePath.c_str());
    std::getline(inf, TempLine);
    Tokenize(TempLine, Token, Header.Delimiter);    
    MinimumTime = strtod(Token.at(IndexOfTimeField).c_str() , NULL);
    inf.close();
    Token.clear();
    Tokenize(Index.at(Index.size()-1).LineAtIndex, Token, Header.Delimiter);        
    MaxmumTime =strtod(Token.at(IndexOfTimeField).c_str() , NULL);  

    LeftBoundaryTime = (TimeStampForSearch-(1.5*VisualRange)) ;
    LeftBoundaryTime = (LeftBoundaryTime<MinimumTime)?MinimumTime:LeftBoundaryTime;
    RightBoundaryTime = (TimeStampForSearch+(1.5*VisualRange));
    RightBoundaryTime = (RightBoundaryTime>MaxmumTime)?MaxmumTime:RightBoundaryTime;

    if( RightBoundaryTime < MinimumTime || LeftBoundaryTime > MaxmumTime ||  LeftBoundaryTime >= RightBoundaryTime) 
        return;
   
    // find where is the LeftBoundaryPosition & RightBoundaryPosition
    // where is TimeStampForSearch postition in index file?     
    double  min, max;
    TraceHandle->GetLeftRightDataX(min,max);   
    if(min<= LeftBoundaryTime && (LeftBoundaryTime >=  MinimumTime && !ResetTraceBuffer)){
        LeftBoundaryPosition = GetDataPoisitionFromFile(max,  IndexOfTimeField);             
        RightBoundaryPosition = GetDataPoisitionFromFile(RightBoundaryTime, IndexOfTimeField);       
        if(LeftBoundaryPosition >= RightBoundaryPosition)
            return;
       
        // load Data from File: [LeftBoundaryPosition, RightBoundaryPosition]
        inf.open(Header.FilePath.c_str(), std::ios_base::binary | std::ios_base::in);
        inf.seekg(LeftBoundaryPosition, std::ios::beg);
        double TimeElement = 0, DataElement;        
        char *StringBuffer = new char [RightBoundaryPosition - LeftBoundaryPosition];        
        inf.read(StringBuffer, RightBoundaryPosition - LeftBoundaryPosition);
        std::string stringvalues(StringBuffer, RightBoundaryPosition - LeftBoundaryPosition);
        std::istringstream iss (stringvalues);
        inf.close();                
        while(1){         
            std::string TempLine = "aaaa";
            std::vector <std::string> Token;         
            std::getline(iss, TempLine);           
            
            Tokenize(TempLine, Token, Header.Delimiter);
            if(Token.size() < IndexOfDataField || Token.size() <  IndexOfTimeField || iss.eof())
                break;
            DataElement = strtod(Token.at(IndexOfDataField).c_str() , NULL);
            TimeElement = strtod(Token.at(IndexOfTimeField).c_str() , NULL);
            TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);       
            if(ElementsNumber == TraceBufferSize && (LeftBoundaryTime <= min)){     
                // Buffer overflow, we need to add size
                TraceBufferSize *= 1.5;
                TraceHandle->ReSize(TraceBufferSize);
            }
            TraceHandle->AddPoint(vctDouble2(TimeElement,DataElement));
            TraceHandle->GetLeftRightDataX(min,max);       
        }       
        // should we resize to a smaller one?
        TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);  
        if(TraceBufferSize > ElementsNumber*1.5){
            TraceHandle->ReSize(ElementsNumber*1.5);
        }
        delete StringBuffer;
    }
    else if(min > RightBoundaryTime || max < LeftBoundaryTime){        
        // if every thing is not in our range, reset buffer and reload everything
        TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);     
        TraceHandle->SetSize(TraceBufferSize);
        LeftBoundaryPosition = GetDataPoisitionFromFile(LeftBoundaryTime  ,IndexOfTimeField);
        RightBoundaryPosition = GetDataPoisitionFromFile(RightBoundaryTime, IndexOfTimeField);

        if(LeftBoundaryPosition >= RightBoundaryPosition)
            return;        
        // load Data from File: [LeftBoundaryPosition, RightBoundaryPosition]
        inf.open(Header.FilePath.c_str(), std::ios_base::binary | std::ios_base::in);
        inf.seekg(LeftBoundaryPosition, std::ios::beg);
        double TimeElement = 0, DataElement;
        double CurrentPos;
        char *StringBuffer = new char [RightBoundaryPosition - LeftBoundaryPosition];
        inf.read(StringBuffer, RightBoundaryPosition - LeftBoundaryPosition);
        std::string stringvalues(StringBuffer,  RightBoundaryPosition - LeftBoundaryPosition);
        std::istringstream iss (stringvalues);
        inf.close();        
        while(1){
            std::string TempLine;
            std::vector <std::string> Token;   
            std::getline(iss, TempLine);
            Tokenize(TempLine, Token, Header.Delimiter);
            if(Token.size() < IndexOfDataField || Token.size() <  IndexOfTimeField || iss.eof())
                break;
            DataElement = strtod(Token.at(IndexOfDataField).c_str() , NULL);
            TimeElement = strtod(Token.at(IndexOfTimeField).c_str() , NULL);

            TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);       
            if(ElementsNumber == TraceBufferSize && (LeftBoundaryTime <= min)){
                // Buffer overflow, we need to add size
                TraceBufferSize *= 1.5;
                TraceHandle->ReSize(TraceBufferSize);
            }
        
            TraceHandle->AddPoint(vctDouble2(TimeElement,DataElement));
            TraceHandle->GetLeftRightDataX(min,max);       
        }        
        // should we resize to a smaller one?
        TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);  
        if(TraceBufferSize > ElementsNumber*1.5){
            TraceHandle->ReSize(ElementsNumber*1.5);
        }
        delete StringBuffer;
    }
    else{               
        // reload only what we need 
        TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);     
        TraceHandle->ReSize(TraceBufferSize);
        LeftBoundaryPosition = GetDataPoisitionFromFile(LeftBoundaryTime  ,IndexOfTimeField);        
        //*********************************************************************************************
        // SECTION ONE, prepend data
        //*********************************************************************************************
        RightBoundaryPosition = GetDataPoisitionFromFile(min  ,IndexOfTimeField);        
        {
            inf.open(Header.FilePath.c_str(), std::ios_base::binary | std::ios_base::in);
            inf.seekg(LeftBoundaryPosition, std::ios::beg);
            double TimeElement = 0, DataElement;
            char *StringBuffer = new char [RightBoundaryPosition - LeftBoundaryPosition];
            inf.read(StringBuffer, RightBoundaryPosition - LeftBoundaryPosition);
            std::string stringvalues(StringBuffer,  RightBoundaryPosition - LeftBoundaryPosition);
            std::istringstream iss (stringvalues);
            inf.close();
            std::vector <double> TimeDataBuffer;
            TimeDataBuffer.clear();
            while(1){
                std::string TempLine;
                std::vector <std::string> Token;   
                std::getline(iss, TempLine);
                Tokenize(TempLine, Token, Header.Delimiter);
                if(Token.size() < IndexOfDataField || Token.size() <  IndexOfTimeField || iss.eof())
                    break;
                DataElement = strtod(Token.at(IndexOfDataField).c_str() , NULL);
                TimeElement = strtod(Token.at(IndexOfTimeField).c_str() , NULL);
                //TraceHandle->AddPoint(vctDouble2(TimeElement,DataElement));
                TimeDataBuffer.push_back(TimeElement);
                TimeDataBuffer.push_back(DataElement);
            }                            
            // reset buffer size for prepend data
            TraceHandle->GetNumberOfPoints(ElementsNumber, TraceBufferSize);            
            if((TraceBufferSize - ElementsNumber) < TimeDataBuffer.size()){               
                TraceHandle->ReSize(TraceBufferSize+ TimeDataBuffer.size());       
            }            
            if(!TimeDataBuffer.empty())
                TraceHandle->PrependArray((double*) &TimeDataBuffer[0], TimeDataBuffer.size());            
            delete StringBuffer;
        }
        //*********************************************************************************************
        // SECTION 2, append data, we let PLAY to load data ahead
        //*********************************************************************************************
    }

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
    for(size_t i = 0; i < Header.Fields.size(); i ++){
        if(Header.Fields.at(i).find(DataFieldForSearch) != std::string::npos){
            IndexOfDataField = i;
            break;
        }
    }
}

void cdpPlayerParseStatTableData::SetTimeFieldForSearch(std::string TimeField){
    TimeFieldForSearch = TimeField;
    // find IndexOfTimeField
    for(size_t i = 0; i < Header.Fields.size(); i ++){
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

void cdpPlayerParseStatTableData::Run(void){

    CS.Enter();
    if(!StartRun){
        CS.Leave();
        return;
    }
    StartRun = false;
    CS.Leave();
    // start to read file
    this->LoadDataFromFile(TracePointer, MiddleTime, TimeRange, ResetBuffer);
    return;
}
