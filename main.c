#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

struct mergeSortParameters
{
    int left,right;
};

int arraySize;
int *unsortedArray;
sem_t mutex;

void readInputFile()
{
    /** read the un sorted array from input file*/

    FILE *inputFile = fopen("input.txt","r"); // open input file to read the unsorted array
    fscanf(inputFile,"%d",&arraySize); // scan the array size
    unsortedArray = (int *) calloc(arraySize, sizeof(int)); // allocate memory space to the unsorted array and set its elements to zero

    // scan unsorted array elements from
    for(int i = 0; i < arraySize; i++)
        fscanf(inputFile,"%d",&unsortedArray[i]);
    fclose(inputFile);
}

void merge(int left, int right, int middle)
{
    sem_wait(&mutex); // the comming codes will be executed by only one thread

    /*create two arrays for the left and the right sub arrays
      that are in the rang from left to right then set thier elements*/

    int leftArraySize = middle -left +1, rightArraySize = right - middle ;
    int leftArray[leftArraySize], rightArray[rightArraySize];
    int index = 0;
    for(int i = left; i<= middle; i++)
        leftArray[index++] = unsortedArray[i];

    index = 0;
    for(int i = middle +1; i<= right; i++)
        rightArray[index++] = unsortedArray[i];

    /** the merge goes as follow : **/

    /** 1- set two pointers that point to the start of the left and the right sub arrays -> leftArrayIndex & rightArrayIndex
        2- set a pointer to be equale to the left argument -> unsortedArrayIndex**/

    int leftArrayIndex = 0, rightArrayIndex = 0, unsortedArrayIndex = left;

     /**3- compare leftArray[leftArrayIndex] with rightArray[rightArrayIndex] :**/

    while(leftArrayIndex < leftArraySize && rightArrayIndex < rightArraySize)
    {
        /** i- if leftArray[leftArrayIndex] < rightArray[rightArrayIndex] then
                unsortedArray[unsortedArrayIndex++] = leftArray[leftArrayIndex++]**/
        if(leftArray[leftArrayIndex] < rightArray[rightArrayIndex])
        {
            unsortedArray[unsortedArrayIndex++] = leftArray[leftArrayIndex++];
        }

         /** ii- if leftArray[leftArrayIndex] > rightArray[rightArrayIndex] then
                unsortedArray[unsortedArrayIndex++] = rightArray[rightArrayIndex++]**/
        else if(leftArray[leftArrayIndex] > rightArray[rightArrayIndex])
        {
            unsortedArray[unsortedArrayIndex++] = rightArray[rightArrayIndex++];
        }

         /** iii- if leftArray[leftArrayIndex] = rightArray[rightArrayIndex] then
                unsortedArray[unsortedArrayIndex++] = rightArray[rightArrayIndex++]
                unsortedArray[unsortedArrayIndex++] = leftArray[leftArrayIndex++]**/
        else
        {
            unsortedArray[unsortedArrayIndex++] = leftArray[leftArrayIndex++];
            rightArrayIndex++;
        }

    }

      /**4- if leftArrayIndex != leftArraySize then
            unsortedArray[unsortedArrayIndex++] = leftArray[leftArrayIndex++]**/
    while(leftArrayIndex < leftArraySize)
    {
        unsortedArray[unsortedArrayIndex++] = leftArray[leftArrayIndex++];
    }

     /**5- if rightArrayIndex != rightArraySize then
            unsortedArray[unsortedArrayIndex++] = rightArray[rightArrayIndex++]**/
    while(rightArrayIndex < rightArraySize)
    {
        unsortedArray[unsortedArrayIndex++] = rightArray[rightArrayIndex++];
    }
    sem_post(&mutex); // let any thread to execute the previous codes
}

void *mergeSort(void * threadArguments)
{
    // extract the rang of the array to be sorted (left and right) from the threadArguments
    struct mergeSortParameters *threadParameters = (struct mergeSortParameters *)threadArguments;
    int left = threadParameters->left, right = threadParameters->right;
    if(left < right)
    {
        int middle = (left + right) / 2; // get the middle of the range
        pthread_t leftSubArrayThread, rightSubArrayThread; // initialize two threads each one will sort sub array of the original one

        // define a struct to set the range that the thread will sort then create the thread and set its start routin
        // the start routin will be mergeSort function (recursive call)
        struct mergeSortParameters *leftThreadparameters = malloc(sizeof(struct mergeSortParameters));
        leftThreadparameters->left = left;
        leftThreadparameters->right = middle;
        pthread_create(&leftSubArrayThread, NULL, mergeSort, (void *) leftThreadparameters);

        struct mergeSortParameters *rightThreadparameters = malloc(sizeof(struct mergeSortParameters));
        rightThreadparameters->left = middle + 1;
        rightThreadparameters->right = right;
        pthread_create(&rightSubArrayThread, NULL, mergeSort, (void *) rightThreadparameters);

        //wait until leftSubArrayThread and rightSubArrayThread to finish there job then start merge them
        pthread_join(leftSubArrayThread, NULL);
        pthread_join(rightSubArrayThread, NULL);

        merge(left, right, middle);
    }
}

void initializeFirstThread()
{
    // initialize the main thread pf the merge sort algorithm
    pthread_t firstThread;
    //set the range to be sent which is from zero to unsortedArraysize
    struct mergeSortParameters *parameters = malloc(sizeof(struct mergeSortParameters));
    parameters->left = 0;
    parameters->right = arraySize - 1;

    //create the thread and set its start routin
    pthread_create(&firstThread, NULL, mergeSort, (void *) parameters);

    //wait until firstThread finish its job then make the main thread continue its job
    pthread_join(firstThread,NULL);
}

// write the sorted srray in an outputFile
void writeOutputFile()
{
    FILE *outputFile = fopen("output.txt","w");
    for(int i = 0; i < arraySize; i++)
        fprintf(outputFile,"%d  ", unsortedArray[i]);
    fclose(outputFile);
}

int main()
{
    sem_init(&mutex, 0, 1); // initialize semaphore (mutex)
    readInputFile(); // read the unsorted array from input file
    initializeFirstThread(); // begin merge sort
    writeOutputFile(); // write the sorted array to output file
    free(unsortedArray);
    /*for(int i = 0; i < arraySize; i++)
        printf("%d  ", unsortedArray[i]);*/
    return 0;
}
