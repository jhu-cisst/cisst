#ifndef _ccalFileIO_h
#define _ccalFileIO_h
#include <cv.h>

using namespace std;
using namespace cv;

class ccalFileIO
{
public:
	enum formatEnum {ORIGINAL, IMPROVED}; // For subclass of ccalFileIO for the DLR .pts type
// Constructor takes filename string of file to read in
	ccalFileIO::ccalFileIO(const char* filename): END(-1)
	{
		myFileName = filename;

	}
// Parses file into lines, and passes each line to ParseLine()
	void ccalFileIO::parseFile()
	{
		char buffer[200];
		int lineNum = 0;
		int sectionNum = 0;

		ifstream& fileStream = openFile(myFileName);

		try{
			while (fileStream.good() && !fileStream.eof() && !fileStream.fail())
			{
				if(sections[sectionNum]->lineNum < lineNum)
				{
					if(sections[sectionNum]->lineNum != END)
						sectionNum++;
				}


					fileStream.getline(buffer,200,'\n');
					ccalFileIO::parseLine(buffer,sections[sectionNum]->fmt,lineNum);
					lineNum++;

					if (fileStream.fail() && !fileStream.eof())
					{
						cout << "fileStream FAILED" << endl;
						break;
					}
			}
		}catch(ios_base::failure)
		{
			cout << myFileName << " failed." << endl;
			return;
		}
	}
	// Virtual method to repack the file data into file-specific data structures
	// implemented in detail in each subclass
	virtual void repackData(){};

protected:
// Object to keep track of file formats for different sections of files
	class SectionFormat
	{
	public:
		SectionFormat::SectionFormat(int lnNum, char* fmtarray)
		{
			lineNum = lnNum;
			fmt = fmtarray;
		}
		int lineNum;
		char* fmt;
	};
// Creates an ifstream object for the specified file
	ifstream& ccalFileIO::openFile(const char* filename)
	{
		// from C++ for dummies
		ifstream* pFileStream = new ifstream(filename);

		if (pFileStream->good())
		{
			cerr << "Sucessfully opened " << filename << endl;
		}
		else
		{
			cerr << "Could not open " << filename << endl;
			//delete pFileStream;
		}
		return *pFileStream;
	}

	//Parses a line from a file according to a format string
	void ccalFileIO::parseLine(char line[], char format[], int lineNum)
	{
		char buffer[200];
		int i = 0;
		int numsRead = 0;
		istringstream inp(line);
		double d;

		while(format[i] != '\n')
		{
			if(format[i] == '#')
			{
				i++;
				char delim = format[i];
				inp.getline(buffer,200,delim);	
				istringstream inp2(buffer);
				while(inp2 >> d) {
					data[lineNum][numsRead] = d;
					numsRead++;
				}
			}
			else
			{
				inp.getline(buffer,200,format[i]);
			}
			i++;
		}
	}


	// parameters
	const static bool debug = false;
	const char* myFileName;
	int fileFormat;
	SectionFormat* sections[12];
	float data [500][12];
	const int END;
		
};

//================== Subclass of ccalFileIO for the DLR .pts type ==================//
class ccalPointsFileIO : public ccalFileIO
{
public:
	ccalPointsFileIO(const char* filename, int fileFormat);
	void repackData();
	void repackData(IplImage* iplImage);
	float errorCheck(vctDynamicVector<vctDynamicVector<vct3>> Dc, vctDynamicVector<vctDynamicVector<vct3>> Ac, vctDynamicVector<vctDynamicVector<vct3>> Cc);
	void showData();
	
	std::vector<cv::Point2f> imagePoints;
	std::vector<cv::Point3f> calibrationGridPoints;
	CvMat* worldToTCP;

protected:
	int pointsCount;
	cv::Size imageSize;

};

//================== Subclass of ccalFileIO for the DLR cameraCalibration.m type ==================//
class ccalDLRCalibrationFileIO : public ccalFileIO
{
public:
	ccalDLRCalibrationFileIO(const char* filename);
	void repackData(int numImages);
	
	cv::Mat tcpToCamera;
	std::vector<CvMat*> cameraMatrix;
	cv::Point2f focalLength;
	cv::Point2f principalPoint;
	float alpha;
	cv::Mat distCoeffs;
	int numImages;
	

protected:
	const static bool debug = false;
	void ccalDLRCalibrationFileIO::printCameraMatrix();

};

//================== Subclass of ccalFileIO for the tracking .coords type ==================//
class ccalTrackerCoordsFileIO : public ccalFileIO
{
public:
	ccalTrackerCoordsFileIO(const char* filename);
	void repackData();
	
	CvMat* worldToTCP;
	

protected:
	const static bool debug = false;

};

#endif