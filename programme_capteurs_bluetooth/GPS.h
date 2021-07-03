#include "mbed.h"

typedef struct GGA_DATA_T{
	char UTC[11];				//UTC Time
	char LAT[14];				//Latitude
	char NS;					//North or south indicator
	char LONG[15];			    //Longitude
	char EW;					//East or west indicator
	char PFI;					//Position fixed indicator
	char NUMSAT[3];		        //Number of satellites used
	char HDOP[5];			    //HDOP
	char ALT[10];				//MSL Altitude
	char AUNIT;				    //Units
	char GSEP[5];			    //Geoidal Separation
	char GUNIT;				    //Units
	char AODC[11];			    //Age of Diff. Corr.
	char CHECKSUM[3];	        //Checksum
} GGA_DATA;

class donneGPS
{
	public:

	GGA_DATA getGGA();
    void formatGGA(char* data_array);

	private:	
	void formatCOORDS(char* coords);
	GGA_DATA GGAdata;
	
};