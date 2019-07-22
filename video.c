#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "video.h"

#define FORMAT ".h264"
#define NAME "Video"

void record(int compteur)
{
    int i = 0;
    char instruct[100];

    if(compteur > 0)
    {
        for(i=0; i<compteur; i++)
        {
            sprintf(instruct, "raspivid -o "NAME"%d"FORMAT" -w 800 -h 600 -t 6000 -fps 25 -a 12 -a \"Aqua Project\" -b 3500000 -vs", i);
            printf("%s\n", instruct);
            system(instruct);
            sprintf(instruct, "mv "NAME"%i"FORMAT" /home/pi/Documents/recupvideo", i);
            system(instruct);
        }
    }
}
