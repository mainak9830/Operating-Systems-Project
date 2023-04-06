#include "filesys.h"
#include <signal.h>
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;

typedef struct inNode inNode;


#define BLOCK_SIZE 1024
// class FileSystem{
// private:
// 	fstream disk;
// public:
// 	FileSystem(char diskname[20]);

// 	int createFile(char name[8], int size);
// 	int deleteFile(char name[8]);

// 	int read(char name[8], int blockNum, char buff[1024]);
// 	int write(char name[8], int blockNum, char buff[1024]);
// 	int ls();
// 	int closeDisk();

// };

//Constructor open disk
FileSystem::FileSystem(char diskname[16]){
    this->disk.open(diskname, ios::out | ios::in);
    //cout << "file opened" << endl;
    //cout << sizeof(int) << " " << sizeof(char) << endl;
}

//Create File
int FileSystem::createFile(char name[8], int size){
    if(!this->disk.is_open())
        return -1;
    // Step 1: check to see if we have sufficient free space on disk by
    // reading in the free block list. To do this:
    // move the file pointer to the start of the disk file.
    // Read the first 128 bytes (the free/in-use block information)
    // Scan the list to make sure you have sufficient free blocks to
    // allocate a new file of this size
    this->disk.sync();
    this->disk.seekg(0, this->disk.beg);
   
    char *blockFilled = new char[128];
    this->disk.read(blockFilled, 128);
    vector<int> freeBlocks;

    for(int i = 1;i < 128;i++){
        if(blockFilled[i] == 0)
            freeBlocks.push_back(i);
        if(freeBlocks.size() >= (size_t)size)
            break;
    }
    if(freeBlocks.size() < (size_t)size){
        cout << "Error Not Sufficient Free Block for file creation!";
        return -1;
    }
    // Step 2: we look  for a free inode om disk
    // Read in a inode
    // check the "used" field to see if it is free
    // If not, repeat the above two steps until you find a free inode
    // Set the "used" field to 1
    // Copy the filename to the "name" field
    // Copy the file size (in units of blocks) to the "size" field

    inNode Node;
    int freeInNode = -1;
    for(int i = 0;i < 16;i++){
        char tbuffer[48];
        //cout << this->disk.tellg() << " ";
        this->disk.read(tbuffer, 48);
        memcpy(&Node, tbuffer, sizeof(Node));
        //cout << Node.name << endl;
        if(Node.used == 0){
            freeInNode = i;
            break;
        }
    }
    
    if(freeInNode == -1){
        //cout << "Free Node Not Available: " << endl;
        return -1;
    }
    //cout << "Free Node available: " << freeInNode << endl;

    Node.used = 1;
    strncpy(Node.name, name, 8);
    Node.size = size;
    //cout << Node.name << " "  << name << endl;
    // cout << sizeof(Node) << endl;
    // Step 3: Allocate data blocks to the file
    // for(i=0;i<size;i++)
    // Scan the block list that you read in Step 1 for a free block
    // Once you find a free block, mark it as in-use (Set it to 1)
    // Set the blockPointer[i] field in the inode to this block number.
    // 
    // end for

    for(int i = 0;i < size;i++){
        Node.blockPointers[i] = freeBlocks[i];
        blockFilled[freeBlocks[i]] = 1;
    }
    // Step 4: Write out the inode and free block list to disk
    //  Move the file pointer to the start of the file 
    // Write out the 128 byte free block list
    // Move the file pointer to the position on disk where this inode was stored
    // Write out the inode
    // this->disk.sync();
    this->disk.seekp(0, this->disk.beg);
    this->disk.write(blockFilled, 128);
    this->disk.seekp(48*freeInNode, this->disk.cur);

    char tmpbufferNode[48];
    //cout << "pointer " << disk.tellp() << " " << freeInNode << endl;
    memcpy(tmpbufferNode, &Node, sizeof(Node));
    this->disk.write(tmpbufferNode, 48);
    this->disk.flush();
    return 1;    
}


//read file
int FileSystem::read(char name[8], int blockNum, char buff[1024]){
    if(!this->disk.is_open())
        return -1;
   // read this block from this file

   // Step 1: locate the inode for this file
   // Move file pointer to the position of the 1st inode (129th byte)
   // Read in a inode
   // If the inode is in use, compare the "name" field with the above file
   // IF the file names don't match, repeat
    this->disk.sync();
    this->disk.seekg(128, this->disk.beg);
    inNode Node;
    int pos = -1;
    for(int i = 0;i < 16;i++){
        char tbuffer[48];
        this->disk.read(tbuffer, 48);
        memcpy(&Node, tbuffer, sizeof(Node));

        //cout << Node.name << " " << name << endl;
        if(Node.used == 1 && strncmp(Node.name, name, 8) == 0){
            pos = i;
            cout << "found position" << i << endl;
            break;
        }
    }
    if(blockNum >= Node.size){
        cout << "Error !!";
        return -1;
    }
    
    
   // Step 2: Read in the specified block
   // Check that blockNum < inode.size, else flag an error
   // Get the disk address of the specified block
   // That is, addr = inode.blockPointer[blockNum]
   // move the file pointer to the block location (i.e., to byte #
   // addr*1024 in the file)
    int offset = Node.blockPointers[blockNum] * BLOCK_SIZE;

    // Read in the block! => Read in 1024 bytes from this location
    // into the buffer "buf"
    this->disk.seekg(offset, this->disk.beg);
    this->disk.read(buff, BLOCK_SIZE);
}

//write file
int FileSystem::write(char name[8], int blockNum, char buff[1024]){
    // write this block to this file
    if(!this->disk.is_open())
        return -1;
   // Step 1: locate the inode for this file
   // Move file pointer to the position of the 1st inode (129th byte)
   // Read in a inode
   // If the inode is in use, compare the "name" field with the above file
   // IF the file names don't match, repeat
    this->disk.sync();
    this->disk.seekg(128, this->disk.beg);
    inNode Node;
    int pos = -1;
    for(int i = 0;i < 16;i++){
        char tbuffer[48];
        this->disk.read(tbuffer, 48);
        memcpy(&Node, tbuffer, sizeof(Node));

        //cout << Node.name << " " << name << endl;
        if(Node.used == 1 && strncmp(Node.name, name, 8) == 0){
            pos = i;
            cout << "found position" << i << endl;
            break;
        }
    }
    if(blockNum >= Node.size){
        cout << "Error !!";
        return -1;
    }
   // Step 2: Write to the specified block
   // Check that blockNum < inode.size, else flag an error
   // Get the disk address of the specified block
   // That is, addr = inode.blockPointer[blockNum]
   // move the file pointer to the block location (i.e., byte # addr*1024)
   int offset = Node.blockPointers[blockNum] * BLOCK_SIZE;
    // Write the block! => Write 1024 bytes from the buffer "buff" to 
    // this location
    this->disk.seekp(offset, disk.beg);
    this->disk.write(buff, BLOCK_SIZE);

    this->disk.flush();
    return 1;
}

//Delete File
int FileSystem::deleteFile(char name[8]){
    if(!this->disk.is_open())
        return -1;
    // Delete the file with this name

    // Step 1: Locate the inode for this file
    // Move the file pointer to the 1st inode (129th byte)
    // Read in a inode
    // If the iinode is free, repeat above step.
    // If the iinode is in use, check if the "name" field in the
    // inode matches the file we want to delete. IF not, read the next
    //  inode and repeat
    this->disk.sync();
    this->disk.seekg(128, this->disk.beg);

    inNode Node;
    int pos = -1;
    for(int i = 0;i < 16;i++){
        char tbuffer[48];
        this->disk.read(tbuffer, 48);
        memcpy(&Node, tbuffer, sizeof(Node));

        //cout << Node.name << " " << name << endl;
        if(Node.used == 1 && strncmp(Node.name, name, 8) == 0){
            pos = i;
            cout << "found position" << i << endl;
            break;
        }
    }
    if(pos == -1){
        cout << "Free Node Not Available: " << endl;
        return -1;
    }
    //
    // Step 2: free blocks of the file being deleted
    // Read in the 128 byte free block list (move file pointer to start
    // of the disk and read in 128 bytes)
    // Free each block listed in the blockPointer fields as follows:
    // for(i=0;i< inode.size; i++) 
    // freeblockList[ inode.blockPointer[i] ] = 0;
    this->disk.seekp(0, this->disk.beg);
    char filledBlocks[128];
    this->disk.read(filledBlocks, 128);
   

    for(int i = 0;i < Node.size;i++){
        filledBlocks[Node.blockPointers[i]] = 0;
    }
    // Step 3: mark inode as free
    // Set the "used" field to 0.
    Node.used = 0;
    // Step 4: Write out the inode and free block list to disk
    //  Move the file pointer to the start of the file 
    // Write out the 128 byte free block list
    // Move the file pointer to the position on disk where this inode was stored
    // Write out the inode
    this->disk.seekp(0, this->disk.beg);
    this->disk.write(filledBlocks, 128);
    this->disk.seekp(48*pos, this->disk.cur);

    char tmpbufferNode[48];
    //cout << "pointer " << disk.tellp() << " " << freeInNode << endl;
    memcpy(tmpbufferNode, &Node, sizeof(Node));
    this->disk.write(tmpbufferNode, 48);
    this->disk.flush();
    return 1; 
 
}
//ls 
int FileSystem::ls(){
    if(!this->disk.is_open())
        return -1;
    this->disk.sync();
    this->disk.seekg(128, this->disk.beg);

    cout << "Listing Nodes " << endl;

    for(int i = 0;i < 16;i++){
        inNode Node;
        char tbuffer[48];
        this->disk.read(tbuffer, 48);
        memcpy(&Node, tbuffer, sizeof(Node));
        cout << Node.name << " " << endl;
        if(Node.used == 1){
            cout << "InNode: " << Node.name << "Size: " << Node.size << endl;
        }
    }

    return 1;
}
//close disk
int FileSystem::closeDisk(){
    this->disk.close();
    return 1;
}

