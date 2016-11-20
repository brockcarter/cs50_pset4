/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(void) {
    FILE* card = fopen("card.raw", "r");
    
    if (card == NULL) {
        fclose(card);
        printf("unable to find card. get your shit together.\n");
        return 1;
    }
    
    int counter = 0;
    
    BYTE buffer[512];
    
    char filename[10];
    
    FILE* photo = NULL;
    
    while ((fread(buffer, sizeof(buffer), 1, card)) == 1) {
        
        if(feof(card)) {
            if(photo != NULL) {
                fclose(photo);
            }
            return 0;
        }
        
        //fread(buffer, sizeof(buffer), 1, card);
        
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] >= 0xe0 && buffer[3] <= 0xef)) {
            
            if(photo != NULL) {
                fclose(photo);
            }
            
            sprintf(filename, "%03d.jpg", counter);
            
            photo = fopen(filename, "w");
            
            counter++;
            
            fwrite(buffer, sizeof(buffer), 1, photo);
        }
        
        else {
            if(photo != NULL) {
                fwrite(buffer, sizeof(buffer), 1, photo);
            }
        }

    }
    
    fclose(photo);
    fclose(card);
    
    return 0;
}
