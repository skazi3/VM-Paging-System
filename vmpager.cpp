/*
 Sarah Kazi
 CS 361
 HW 3
 Virtual Memory Paging System
 10/21/2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdbool.h>
int PAGETABLESIZE = 256;
int NPROCESSES = 256;

int page_misses = 0;
int page_hits = 0;

//memory access data struct
typedef struct{
    uint8_t pid;
    uint8_t page;
}MemoryAccess;

typedef struct {
    int frame; // -1 if not currently loaded
    int pageHits;
    int pageMisses;
    int infiniteFrame;
    int infiniteMisses;
    int infiniteHits;
}PageTableEntry;

// Define the frame table struct 
typedef struct {
    uint8_t pid;
    uint8_t page;
    bool vacant;
}FrameTableEntry;


//file isn't of right type
void usage( char * argv0 ) {
    printf("%s %s %s\n", "\nUsage: " , argv0 , " inputDataFile [ maxAccesses ]\n");
    printf("%s\n", "( File type must be bitmap, BMP )\n");
    return;
}

int main( int argc, char *argv[ ] )
{
    printf("Sarah Kazi\nCS 361\nnetid: skazi3\n10.21.2017\nVM Pager\n");
    int memoryAccesses = 0;
    int frameTableSize = 256; 
    int infiniteHits = 0, infiniteMisses = 0;
    
    //initialize data
    //actual pointer to array
    MemoryAccess *refs = NULL;

    //upper limit on array will be figured later
    int maxAccesses = -1, fd, filesize;
    struct stat statBuffer;
    //create 2d pageTable
    PageTableEntry pageTables[ PAGETABLESIZE ][ NPROCESSES ];
    //initialize to zero or -1
    int row, col;
    for(row = 0; row < PAGETABLESIZE; row++){
        for(col = 0; col < NPROCESSES; col++){
            pageTables[row][col].pageMisses = 0;
            pageTables[row][col].pageHits   = 0;
            pageTables[row][col].frame      = -1;
            pageTables[row][col].infiniteFrame = -1;
            pageTables[row][col].infiniteHits = 0;
            pageTables[row][col].infiniteMisses = 0;
        }
    }
    

    //open the file and get the file size
    if( ( fd = open(argv[1], O_RDWR) ) < 0 ) {
        perror( "opening file" );
        exit( -4 );
    }
    
    if( fstat(fd, &statBuffer) < 0 ) {
        perror("fstat");
        exit( -5 );
    }
    filesize = statBuffer.st_size;

    //upper limit is whole file for now until user command is taken
    memoryAccesses = (filesize) / sizeof( MemoryAccess );

    // process the command line arguments
    if( argc > 2 ) {
        
        if(atoi(argv[2]) != 0){
            //if memory accesses is not provided or specified as zero,
            //then program should read and process the entire data file
            memoryAccesses = atoi(argv[2]);
        }
        printf("# Memory Accesses entered by user: %d\n", memoryAccesses);
    }
    if(argc > 3){
        frameTableSize = atoi(argv[3]);
        printf("Frame table size entered by user: %d\n", frameTableSize);
    }
    //create the frame table
    FrameTableEntry frameTable[frameTableSize];
    int j;
    for(j = 0; j < frameTableSize; j++){
        frameTable[j].pid = 0;
        frameTable[j].page = 0;
        frameTable[j].vacant = true;
    }

    //memory map the file.
    if ( ( refs = ( MemoryAccess* ) mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0))
        == MAP_FAILED ) {
        perror("mmap");
        exit( -6 );
    }

    //how many memory accesses are we processing?
    printf("%s %s %s %i %s\n\n\n", "\nProcessing " , argv[ 1 ] , " with " , memoryAccesses
           , " memory accesses.\n");
    

    int i;
    int frameTableCounter = 0;
    //for loop to actually process now
    for(i = 0; i < memoryAccesses; i++ ) {

        if(pageTables[refs[i].page][refs[i].pid].frame >= 0){
            //PAGE HIT: just increment counters
            pageTables[refs[i].page][refs[i].pid].pageHits++;
            page_hits++;
            infiniteHits++;

        }
        else if(pageTables[refs[i].page][refs[i].pid].frame < 0){
            //PAGE MISS
            //increment counters
            page_misses++;
            pageTables[refs[i].page][refs[i].pid].pageMisses++;
            
            //FIFO ALGORITHM: circular queue (if we're reached end of frame table reset to zero)
            if(frameTableCounter == frameTableSize)
                frameTableCounter == 0;
            //grab current frame we will put our page,pid in
            FrameTableEntry curFrame = frameTable[frameTableCounter];

            //update frame in page table
            pageTables[refs[i].page][refs[i].pid].frame = frameTableCounter;


            if(curFrame.vacant == false){
                //change the frame field in the page table entry for the
                //victim page to -1
                pageTables[curFrame.page][curFrame.pid].frame = -1;

            }
            //store new pid, page in frame table
            frameTable[frameTableCounter].page = refs[i].page;
            frameTable[frameTableCounter].pid = refs[i].pid;
            frameTable[frameTableCounter].vacant = false;

            //INFINITE: now select a new frame by just changing frame to +1
            if(pageTables[refs[i].page][refs[i].pid].infiniteFrame < 0){
                //increment counters
                pageTables[refs[i].page][refs[i].pid].infiniteMisses++;
                infiniteMisses++;
                pageTables[refs[i].page][refs[i].pid].infiniteFrame = 1;
            }
            //check if we had a hit if the frame was never kicked out due infinite algorithm
            else if(pageTables[refs[i].page][refs[i].pid].infiniteFrame > 0){
                pageTables[refs[i].page][refs[i].pid].infiniteHits++;
                infiniteHits++;
            }
            //increment frame table counter
            frameTableCounter++;

        }
    }
    
    //all done! Print results
    printf("INFINITE\n");
    printf("%d total hits\n%d total misses\n", infiniteHits, infiniteMisses);

    printf("FIFO\n");
    printf("%d total hits\n%d total misses\n", page_hits, page_misses);

    //unmap before we close the file
    munmap(refs, filesize);
    close(fd);
    
    return EXIT_SUCCESS;
    
}
