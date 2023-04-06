#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <string.h>
using namespace std;

struct inNode{
	char name[8]; //file name
	int size;     // file size (in number of blocks)
	int blockPointers[8]; // direct block pointers
	int used;
};

class FileSystem{
private:
	fstream disk;
public:
	FileSystem(char diskname[16]);

	int createFile(char name[8], int size);
	int deleteFile(char name[8]);

	int read(char name[8], int blockNum, char buff[1024]);
	int write(char name[8], int blockNum, char buff[1024]);
	int ls();
	int closeDisk();

};