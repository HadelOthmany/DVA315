#include <stdio.h>
#include <stdlib.h>

#define LRU 1
#define FIFO 2
#define LFU 3
#define OPT 4

int ALGORITHM =OPT;

typedef struct {
    int page;       // page stored in this memory frame
    int time;       // Time stamp of page stored in this memory frame
    int free;       // Indicates if frame is free or not

  // Add own data if needed for FIFO, OPT, LFU, Own algorithm
    int timeAccessed;
    int timeSinceSwitch; // time since frame switched
} frameType;

//---------------------- Initializes by reading stuff from file and inits all frames as free -----------------------------------------------------------

void initilize (int *no_of_frames, int *no_of_references, int refs[], frameType frames[]) {

    int i;
    FILE *fp;
    char fileName[50]="/home/student/Desktop/Labs_material/ref.txt";

    fp = fopen(fileName, "r");

    if(fp == NULL) {
        printf("Failed to open file %s", fileName);
        exit(-1);
    }

    fscanf(fp,"%d", no_of_frames);                  //Get the number of frames

    fscanf(fp,"%d", no_of_references);              //Get the number of references in the reference string

    for(i = 0; i < *no_of_references; ++i) {        // Get the reference string
        fscanf(fp,"%d", &refs[i]);
    }
    fclose(fp);

    for(i = 0; i < *no_of_frames; ++i) {
        frames[i].free = 1;                         // Indicates a free frame in memory
    }

    printf("\nPages in memory:\t");                 // Print header with frame numbers
    for(i = 0; i < *no_of_frames; ++i) {
        printf("\t%d", i);
    }
    printf("\n");
}

//-------------------- Prints the results of a reference,  all frames and their content and some info if page fault -----------------------------------

void printResultOfReference (int no_of_frames, frameType frames[], int pf_flag, int mem_flag, int pos, int mem_frame, int ref) {

    int j;

    printf("Acessing page %d:\t", ref);

    for(j = 0; j < no_of_frames; ++j) {             // Print out what pages are in memory, i.e. memory frames
        if (frames[j].free == 0) {                  // Page is in memory
            printf("\t%d", frames[j].page);
        }
        else {
            printf("\t ");
        }
    }

    if(pf_flag == 0) {                              // Page fault
        printf("\t\tPage fault");
    }
    if (mem_flag == 0) {                            // Did not find a free frame
        printf(", replaced frame: %d", pos);
    }
    else if (mem_frame != -1) {                     // A free frame was found
        printf(", used free frame %d", mem_frame);
    }
    printf("\n");
}

//----------- Finds the position in memory to evict in case of page fault and no free memory location ---------------------------------------------

int findPageToEvict(frameType frames[], int n, int refs[], int refpos, int no_of_references) {   // LRU eviction strategy -- This is what you are supposed to change in the lab for LFU and OPT
	// LRU eviction strategy -- This is what you are supposed to change in the lab for LFU and OPT

	    if(ALGORITHM == LRU){
	        int i, minimum = frames[0].time, pos = 0;           //value of the timestamp (time) of the first frame, frame 0.

	        for(i = 0; i < n; ++i) {                            //loop through memory frames
	            if(frames[i].time < minimum){                   // Find the page position with minimum time stamp among all frames to evict it, the page that was last accessed
	                minimum = frames[i].time;
	                pos = i;
	            }
	        }
	        return pos;
	    }

	    else if(ALGORITHM == FIFO){
	        int i, maximum = frames[0].timeSinceSwitch, pos = 0;    // Initialize maximum with the time since switch of the first frame

	        for(i = 0; i < n; ++i){                             //loop through memory frames to find the frame with the max time since switch
	            if(frames[i].timeSinceSwitch >= maximum){        //determine which page is oldest (loaded first) to eviction
	                maximum = frames[i].timeSinceSwitch;
	                pos = i;
	            }
	        }
	        return pos;      // Return the position of the frame with the oldest time since switch
	    }
	    else if(ALGORITHM == LFU){
	   // Initialize variables to track minimum access count and its corresponding position
	   int i, minimum = frames[0].timeAccessed , pos = 0;
	   for (i = 1; i < n; i++)               // Loop through memory frames

	   {
	   if (frames[i].timeAccessed < minimum)
	   {
	   // Update minimum access count and its corresponding position
	   minimum = frames[i].timeAccessed;
	   pos = i;
	   }
	   }
	   // Return the position of the frame with the least access count
	   return pos;
	   }
	    else if(ALGORITHM == OPT){    //the page that will not be used for the longest time
	          int i, j, page, count = 0, pos = 0, max = 0;
	          for(i = 0; i < n; ++i){                             // Loop over each frame in memory
	              page = frames[i].page;                          // Get the page stored in the current frame
	              for(j = refpos+1; j < no_of_references; ++j){   // Loop over future references after the current reference refpos+1
	                  if(refs[j] == page){                        // Check if the page occurs in the remaining references
	                      count = j - refpos;                     // Calculate the time to the next occurrence of the page by subtracting the current ref pos from the pos of its next occurence
	                      break;                                  // Break out of the loop since the page has been found
	                  }
	                  if(j == no_of_references - 1){              // If no occurrence was found in the remaining references
	                      count = no_of_references;               // Set count to a large value to signify that the page will not be used again
	                  }
	              }
	              if(count > max){                                // Compare the time to next occurrence with the maximum count so far
	                  max = count;                                // Update the maximum count
	                  pos = i;                                    // Store the position of the frame with the largest count
	              }
	          }
	          return pos;                                         // Return the position of the frame to evict
	      }
}

//---- Main loops ref string, for each ref 1) check if ref is in memory, 2) if not, check if there is free frame, 3) if not, find a page to evict --
int main()
{

    int no_of_frames, no_of_references, refs[100], counter = 0, page_fault_flag, no_free_mem_flag, i, j, pos = 0, faults = 0, free = 0;
    frameType frames[20];

    initilize (&no_of_frames, &no_of_references, refs, frames);

    for(i = 0; i < no_of_references; ++i) {
        page_fault_flag = no_free_mem_flag = 0;

        for(j = 0; j < no_of_frames; ++j) {
            if(frames[j].page == refs[i]) {
//no page fault occurs
                counter++;
                frames[j].time = counter;
                frames[j].timeAccessed++;
                page_fault_flag = no_free_mem_flag = 1;
                free = -1;
                break;
            }
            frames[j].timeSinceSwitch++;

        }

        if(page_fault_flag == 0) {                  // We have a page fault
            for(j = 0; j < no_of_frames; ++j) {     // Loop over memory
                if(frames[j].free == 1) {           // Do we have a free frame, if free found update
                    counter++;
                    faults++;
                    frames[j].page= refs[i];        // Update memory frame with referenced page
                    frames[j].time = counter;       // Update the time stamp for this frame
                    frames[j].free = 0;             // This frame is no longer free
                    frames[j].timeSinceSwitch = 0;
                     frames[j].timeAccessed=0;
                    no_free_mem_flag = 1;
                    free = j;
                    break;
                }
            }
        }

        if(no_free_mem_flag == 0) {                 // Page fault and memory is full, we need to know what page to evict
            pos = findPageToEvict(frames, no_of_frames, refs, i, no_of_references); // Get memory position to evict among all frames
            counter++;
            faults++;
            frames[pos].page = refs[i];
            frames[pos].time = counter;
            frames[pos].timeSinceSwitch = 0;
            frames[j].timeAccessed = 0;

        }
        printResultOfReference (no_of_frames, frames, page_fault_flag, no_free_mem_flag, pos, free, refs[i]); // Print result of referencing ref[i]
    }
    printf("\n\nTotal Page Faults = %d\n\n", faults);

    return 0;
}
