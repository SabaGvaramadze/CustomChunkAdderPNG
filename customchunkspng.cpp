#include <iostream>
#include <fstream>

using namespace std;

/* Table of CRCs of all 8-bit messages. */
unsigned long crc_table[256];
      
/* Flag: has the table been computed? Initially false. */
int crc_table_computed = 0;
        

void reverseBytes(char *a,const size_t size);

union char_int{
	char a[4];
	int b;
};



void make_crc_table(void);
unsigned long update_crc(unsigned long crc, unsigned char *buf,int len);
unsigned long get_crc(unsigned char *buf, int len);

int main(int argc,char ** argv){
	string filePath;
	if(argc>=2)filePath = argv[1];
	else{
		cout << "please input ffile path: ";
		cin >> filePath;

	}
	ifstream file(filePath,ios_base::binary | ios::ate);
	cout << file.is_open();
	int fileSize = file.tellg();
	file.seekg(0);
	unsigned char pngSignature[8] = {137,80,78,71,13,10,26,10};
	char fileSignature[8];
	file.read(fileSignature,8);
	for(int i=0;i<8;i++){//check for invalid files
		if((unsigned char)fileSignature[i] != pngSignature[i]){
			cout << (int)(unsigned char)fileSignature[i];
			cout << "invalid file";
			return 1;
		}
	}
	string CChunkName;//get data for the custom chunk
	do{
		cout << "Enter a custom chunk name: ";
		cin >> CChunkName;
	}while(CChunkName.length()!= 4);
	string CData;
	cout << "Enter data to be saved in the chunk: ";
	cin >> CData;
	int numberOfChunks=0;
	while(file.tellg()!=fileSize){//count the number of chunks
		char_int chunkSize;
		file.read(chunkSize.a,4);
		reverseBytes(chunkSize.a,4);
		cout << chunkSize.b << endl;
		file.seekg((int)file.tellg()+chunkSize.b+8);
		numberOfChunks++;
	}
	int CChunkPos;
	do{
		cout << "Enter the position of your custom chunk from 1 to " << numberOfChunks+1 << endl;
		cin >> CChunkPos;
	}while(CChunkPos < 0 || CChunkPos > numberOfChunks+1);
	ofstream ofile(filePath.substr(0,filePath.length()-4) + "E" + ".png",ios_base::binary);
	ofile.write((char*)pngSignature,8);
	int i=1;
	file.seekg(8);
	while(file.tellg() != fileSize){//create a new file with the custom chunk and put the custom chunk in it
		char_int chunkSize;
		file.read(chunkSize.a,4);
		reverseBytes(chunkSize.a,4);
		
		int IChunkSize = chunkSize.b;
		
		reverseBytes(chunkSize.a,4);
		
		char chunkName[4];
		file.read(chunkName,4);
		
		char *chunkData = (char*)malloc(IChunkSize);
		file.read(chunkData,IChunkSize);
		
		char crc[4];
		file.read(crc,4);
		
		if(i == CChunkPos){
			char_int CChunkSize;
			CChunkSize.b = CData.length();
			reverseBytes(CChunkSize.a,4);
			
			ofile.write(CChunkSize.a,4);
			ofile.write(CChunkName.c_str(),4);
			ofile.write(CData.c_str(),CData.length());
			unsigned char* crc_buf = (unsigned char*)(malloc(CData.length()+4));
			memcpy(crc_buf,CChunkName.c_str(),4);
			memcpy(crc_buf+4,CData.c_str(),CData.length());
			char new_crc[4] = {0};
			*((unsigned int*)new_crc) = get_crc(crc_buf,CData.length()+4);
			reverseBytes((char*)new_crc,4);
			ofile.write(new_crc,4);
		}
		i++;
		
		ofile.write(chunkSize.a,4);
		ofile.write(chunkName,4);
		ofile.write(chunkData,IChunkSize);
		ofile.write(crc,4);
		
		free(chunkData);
	}
	file.close();
	ofile.close();
	
}

#ifdef __linux__
void getCorrectByteOrder(char*,const size_t size){
}
#else
void getCorrectByteOrder(char *a,const size_t size){
	for(int i=0;i<size/2;i++){
		char tmp;
		tmp = a[i];
		a[i] = a[size-i-1];
		a[size-i-1] = tmp;
	}
}
#endif

void reverseBytes(char *a,const size_t size){
	for(int i=0;i<size/2;i++){
		char tmp;
		tmp = a[i];
		a[i] = a[size-i-1];
		a[size-i-1] = tmp;
	}
	

}



/* Make the table for a fast CRC. */
void make_crc_table(void){
	unsigned long c;
	int n, k;

	for (n = 0; n < 256; n++) {
		c = (unsigned long) n;
		for (k = 0; k < 8; k++) {
			if (c & 1)c = 0xedb88320L ^ (c >> 1);
			else c = c >> 1;
		}
		crc_table[n] = c;
	}
	crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
*       should be initialized to all 1's, and the transmitted value
*             is the 1's complement of the final running CRC (see the
*                   crc() routine below)). */


unsigned long update_crc(unsigned long crc, unsigned char *buf,int len){
	unsigned long c = crc;
	int n;
	
	if (!crc_table_computed)
	make_crc_table();
	for (n = 0; n < len; n++) {
		c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	}
	return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long get_crc(unsigned char *buf, int len){
	return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

