#include "filesys.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "usage: %s <diskFileName> \n", argv[0]);
        exit(0);
    }

    // filesystem initialisation
    
    // reading from input file
    ifstream inputfile(argv[2]);
    
    FileSystem fs ((char *)"disk0");
    char *line = new char[100];
    char *split;
    char *command[3];
    char buff[1024];

    for(int i = 0;i < 1024;i++){
        buff[i] = '1';
    }
    
    if (inputfile.is_open()) {
      
        while(inputfile.getline(line, 100)){
            
            
            split = strtok(line, " ");
            int i = 0;
            // inputfile >> line;
            while(split != NULL){
                command[i] = split;
                split = strtok(NULL, " ");
                i++;
            }
           
            switch (command[0][0]){
                case 'C':
                    /* code */
                    //cout << "Create COmmmand   " << command[1] << endl;
                    fs.createFile((char *)command[1], atoi(command[2]));
                    break;
                
                case 'L':
                    /* code */
                    fs.ls();
                    break;
                case 'W':
                    fs.write((char *)command[1], atoi(command[2]), buff);
                    /* code */
                    break;
                case 'R':
                    fs.read((char *)command[1], atoi(command[2]), buff);
                    /* code */
                    break;
                case 'D':
                    /* code */
                    fs.deleteFile((char *)command[1]);
                    break;
            
                default:
                    //cout << line << endl;
                    break;
            }
           
        }
        // cout << line << endl;
       
    }

    fs.closeDisk();
    inputfile.close();

    return 0;
}