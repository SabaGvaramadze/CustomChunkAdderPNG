#include <iostream>
#include <fstream>

using namespace std;

void reverseBytes(char *a,const size_t size);

union char_int{
	char a[4];
	int b;
};

int main(){
	string filePath = "test.png";
	ifstream file(filePath,ios_base::binary | ios::ate);
	cout << file.is_open();
	int fileSize = file.tellg();
	file.seekg(0);
	unsigned char pngSignature[8] = {137,80,78,71,13,10,26,10};
	char fileSignature[8];
	file.read(fileSignature,8);
	for(int i=0;i<8;i++){//check for invalid fies
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
	cout << "burh";
	cout <<  "what";
	int numberOfChunks=0;
	cout << "hello";
	while(file.tellg()!=fileSize){//count the number of chunks
		char_int chunkSize;
		file.read(chunkSize.a,4);
		reverseBytes(chunkSize.a,4);
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
	while(file.tellg() != fileSize){
		char_int chunkSize;
		file.read(chunkSize.a,4);
		char chunkName[4];
		file.read(chunkName,4);
		char *chunkData = (char*)malloc(chunkSize.b);
		file.read(chunkData,chunkSize.b);
		char crc[4];
		file.read(crc,4);
		if(i == CChunkPos){
			char_int CChunkSize;
			CChunkSize.b = CData.length();
			ofile.write(CChunkSize.a,4);
			ofile.write(chunkName,4);
			ofile.write(CData.c_str(),CData.length());
			ofile.write(crc,4);
		}
		i++;
		ofile.write(chunkSize.a,4);
		ofile.write(chunkName,4);
		ofile.write(chunkData,chunkSize.b);
		ofile.write(crc,4);
	}
	file.close();
	ofile.close();
	
}


void reverseBytes(char *a,const size_t size){
	for(int i=0;i<size;i++){
		char tmp;
		tmp = a[i];
		a[i] = a[size-i-1];
		a[size-i-1] = tmp;
	}
	

}
