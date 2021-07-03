#include "GPS.h"

GGA_DATA donneGPS::getGGA()
{
	return GGAdata;
}

void donneGPS::formatGGA(char* data_array)
{
	enum cases {UTC, LAT, NS, LONG, EW, PFI, NUMSAT, HDOP, ALT, AUNIT, GSEP, GUNIT, AODC};
	cases datamember= UTC;
	char* start_ptr;
	char* end_ptr = data_array+7;//Set start pointer after the message ID ("$GPGGA,")
	bool flag=1;
	char COORDbuf[14]={0};
	char checksum[3]={0};

	while (flag)
	{
		start_ptr = end_ptr;
		
		while (*end_ptr!=',' && (*(end_ptr+1)!=10)&& *end_ptr!='*')end_ptr++;//Increment ptr until a comma is found
		
		if (*end_ptr==10||*(end_ptr+1)==10||(*end_ptr=='*'&&*(end_ptr-1)==',')){//End reached
			flag=0;
			break;
		}
		
		switch(datamember){
				case UTC:
					memcpy(GGAdata.UTC, start_ptr, (end_ptr - start_ptr));
					GGAdata.UTC[end_ptr - start_ptr] = '\0';//End null char
					datamember = LAT;
					break;
				case LAT:
                    memcpy(GGAdata.LAT, start_ptr, (end_ptr - start_ptr));
					GGAdata.LAT[end_ptr - start_ptr] = '\0';
					datamember=NS;
					break;
				case NS:
					if (*start_ptr!=','){
						GGAdata.NS = *start_ptr;
						strncat(GGAdata.LAT, start_ptr, 1);
					}
					datamember=LONG;
					break;
				case LONG:
					memcpy(GGAdata.LONG, start_ptr, (end_ptr - start_ptr ));
					GGAdata.LONG[end_ptr - start_ptr] = '\0';
					datamember=EW;
					break;
				case EW:
					if (*start_ptr!=','){
						GGAdata.EW = *start_ptr;
						strncat(GGAdata.LONG, start_ptr, 1);
					}
					datamember=PFI;
					break;
				case PFI:
					if (*start_ptr!=',')GGAdata.PFI = *start_ptr;
					datamember=NUMSAT;
					break;
				case NUMSAT:
					memcpy(GGAdata.NUMSAT, start_ptr, (end_ptr - start_ptr));
					GGAdata.NUMSAT[end_ptr - start_ptr] = '\0';
					datamember=HDOP;
					break;
				case HDOP:
					memcpy(GGAdata.HDOP, start_ptr, (end_ptr - start_ptr));
					GGAdata.HDOP[end_ptr - start_ptr] = '\0';
					datamember=ALT;
					break;
				case ALT:
					memcpy(GGAdata.ALT, start_ptr, (end_ptr - start_ptr));
					GGAdata.ALT[end_ptr - start_ptr] = '\0';
					datamember=AUNIT;
					break;
				case AUNIT:
					if (*start_ptr!=',')GGAdata.AUNIT= *start_ptr;
					datamember=GSEP;
					break;
				case GSEP:
					memcpy(GGAdata.GSEP, start_ptr, (end_ptr - start_ptr));
					GGAdata.GSEP[end_ptr - start_ptr] = '\0';
					datamember=GUNIT;
					break;
				case GUNIT:
					if (*start_ptr!=',')GGAdata.GUNIT=*start_ptr;
					datamember=AODC;
					break;
				case AODC:
					memcpy(GGAdata.AODC, start_ptr, (end_ptr - start_ptr));
					GGAdata.AODC[end_ptr - start_ptr] = '\0';
					flag=0;
					break;

					}
			end_ptr++;//Increment past the last comma
	}
	//Get checksum
	while(*(end_ptr)!=10)end_ptr++;
		checksum[0] = *(end_ptr - 3);
		checksum[1] = *(end_ptr - 2);
		checksum[2] = NULL;
	memcpy(GGAdata.CHECKSUM, checksum, 3);
	return;
}

void donneGPS::formatCOORDS(char* coords)
{
	char formatted[14]={0};
	int i=0;
	char* coordsstart= coords;
	
	while (*(coords)){
		
		formatted[i]=*coords;
		formatted[++i]; coords++;
		if (*(coords+2)=='.')
		{
			formatted[i]='d';//degrees symbol
			i++;
		}
		else if (*coords=='.')
		{
			formatted[i] = 39;// ' symbol for minutes
			i++;
			coords++;
		}
		else if (*(coords-3)=='.')
		{
			formatted[i]='.';//Decimal for seconds
			i++;
		}
		else if (*(coords-5)=='.')
		{
			formatted[i]='"';// " for seconds
			i++;
			formatted[i]=0;//Null char
		}
	}

	strcpy(coordsstart, formatted);

	//coords=formatted;
	return;
	
}