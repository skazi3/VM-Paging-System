/* imageEditorTemplate.c

    Written October 2017 by John Bell and Hanye for CS 361
    
    This program uses memory mapping to modify the data ( colors ) in a 
    BMP format file.
    
    TODO:  Find the ???? and correct the system calls to make the program run.
    
    Modified by . . . 
    
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


// Define the Pixel struct data type, as 3 unsigned 8-bit ints ( chars )

typedef struct {
    uint8_t red, green, blue;
} Pixel; 

void usage( char * argv0 ) {
      printf("%s %s %s\n", "\nUsage: " , argv0 , " inputDataFile [ maxPixels ]\n");
      printf("%s\n", "( File type must be bitmap, BMP )\n");
      return;
}

int main( int argc, char *argv[ ] )
{
    char *fileStart = NULL;  // char * can access any address, byte by byte
    int nPixels = 0, maxPixels = -1, fd, filesize;
    struct stat statBuffer;
    
    Pixel * pixels = NULL;  // This will be treated as an array of Pixels
    
    // First to process the command line arguments
    
    if( argc < 2 || argc > 3 ) {
        usage( argv[ 0 ] );
        exit( -1 );
    }
    
    if( argc > 2 ) {
        maxPixels = atoi( argv[ 2 ] );
        if( maxPixels < 0 ) {
            usage( argv[ 0 ] );
            exit( -2 );
        }
    }
    
    // Next to open the file and get the file size
    // Set the maximum number of pixels based on file size, minus header
    
    if( ( fd = open( ???/* TODO: Open for reading and writing */ ) ) < 0 ) {
        perror( "opening file" );
        exit( -4 );
    } 
    
    if( fstat( ???? ) < 0 ) {
        perror("fstat");
        exit( -5 );
    }
    
    filesize = ????; // TODO:  Copy the size field from statBuffer
    
    // The first 56 bytes of a BMP file are header, to be skipped
    maxPixels = ( filesize - 56 ) / sizeof( Pixel );
    
    if( nPixels > maxPixels || nPixels <= 0 )
        nPixels = maxPixels;       
    
    // Now to memory map the file.
    // For this program use a char * to the beginning of the file, so that
    // we can skip over the headers later.
    
    if ( ( fileStart = ( char * ) mmap( ??? Reading and Writing, Shared ) ) 
        == MAP_FAILED ) {
            perror("%s\n", "mmap");
            exit( -6 );
    }
    
    // Set the pixel pointer to the start of the pixel data
    // Skip the first 56 bytes, which are header in a BMP file
    
    pixels = ( Pixel * ) ( fileStart + 56 );
    
    // Okay, all set up.  Report to the user.
    
    printf("%s %s %s %i %s\n\n\n", "\nProcessing " , argv[ 1 ] , " with " , nPixels 
        , " pixels.\n");
                   
    // Now to process Pixels
    int i = 0;
    for(i = 0; i < nPixels; i++ ) {
        
        ????
        /*  TODO:  Modify the pixels, in some way that will be visible
            Suggestions:
                1. set pixels[ i ].red to 0 for all pixels to remove the 
                    red channel.
                2. Set each red, green, and blue value to either 0 or 255,
                    depending on whether they are less or more than 127,
                    to posterize the image.
                3. For each pixel, set the red, greeen, and blue values all to 
                    the average of the three, to get a grayscale image.
                    ( True grayscale uses a weighted average. )
                4. Convert the image to black and white, with no gray
                5. Can you figure out how to darken, lighten, or change contrast
                6. Ask the user before the loop what they want, and then use
                    a switch inside the loop to perform different tasks.
                7. Green screening.  This one requires two files.  For any pixel
                    that is ( mostly ) green in one file, replace it with the
                    corresponding pixel from the other file.
        */

    } // for loop through pixels
    
    // Okay, we are done processing.  Now to close everything up.
    munmap( ??? );
    close( ??? );
    
    return EXIT_SUCCESS;

} 