// Grain identification and grain size determination code
// Derrick T. Carpenter, Jeffrey M. Rickman, Katy Barmak
// Lehigh University
// 6/9/99


/* When compiling...use the following command line:
   cc source.c -o outfile -std1 -lm
   writes outfile to be the executable.
   -std1 specifies strict ANSI rules (on Digital Alpha only)
   -lm is necessary to properly link the math library

   NOTE: the // is used to comment out the rest of a line,
   a C++ notation which may not be recognized by older C
   compilers unless they are told to...check the man file
   to find the right switch. 
   */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>


#define maxDim          2400 /* limits size of image arrays */
#define MaxOffset	50 /* maximum offset allowed between images in any direction*/
#define ChiSqrWindow	15 /* max dist from a pixel we will look when calculating chiSqr*/
#define ErodeLength	7 /* length removed from a line when calling PruneImage*/
#define Refresh         1
#define NoRefresh       0
#define maxKernel      50 /* largest convolution kernel allowed */

// Below #defines apply to specialised edge finding routine and a
// few other functions which are not currently used but have still
// been included in the code.

#define waveSmoothWindow 7 /* used when smoothing waves before peak finding */
#define smoothWindow	 9  /* kernel size for 2D image smoothing */
#define smallWindow   3	/* used to smooth derivatives during peak finding */
#define ampCutoff    0.1  /* min amplitude req'd for a peak to be found */
#define Dmult	      100 /* multiply by this to save decimal info as int*/
#define Cluster1	25
#define Cluster2	55
#define ConnectRad1   	  3  /* first connection radius */
#define ConnectRad2   	  6  /* second connection radius */
#define CloseDiam1		8  /* first closing width */
#define CloseDiam2		10  /* second closing width */


/* QuickSort parameters*/

#define M 7
#define NSTACK 50
#define FM 7875
#define FA 211
#define FC 1663

// The following data structure is used to keep track of information about
// black pixels in the image, used in the FilterImage function among
// others. These structs are used in a linked list (usually)

typedef struct _pixelRec{
  int startX;
  int startY;
  int currentX;
  int currentY;
  float displacement;
  struct _pixelRec *link;
} pixelRec;
 
// Global variables
// These include the image arrays image, tempImage, temp, buffer, neighbors,
// and convolution. The first four are used to manipulate and hold image
// data during various steps, while neighbors is used to speed up the
// dilation and erosion functions and convolution is used to hold the
// results after the application of a convolution kernel.

int image[maxDim][maxDim];/* global data structure */
int tempImage[maxDim][maxDim]; /* temp storage for image processing*/
int temp[maxDim][maxDim]; /* another temp storage*/
int buffer[maxDim][maxDim]; /* another temp storage*/
int neighbors[maxDim][maxDim]; /*holds number of black neighboring pixels*/
double convolution[maxDim][maxDim];/*holds results of convolution*/
int grains[2*maxDim]; /* Used to store grain sizes */
int stack[10*maxDim][2];
int kernel[maxKernel][maxKernel];/* stores the kernel to be convolved*/
int minDisplacement; /*user-defined length, used to eliminate short segments*/

// Names of files to be written when using the enhanced edge-finding
// functions. Not currently used, but necessary if future use is desired.

char rows[] = "rows";
char cols[] = "cols";
char dnRt[] = "dnRt";
char upRt[] = "upRt";
char none[] = "none";

/* Function Declarations*/

void WriteProcessorTime(void);
void InvertAndFilter(int width, int height, char *filename); 
void FilterImage(int width, int height, int segLength);
int RecursiveWalker(pixelRec **pixelList, pixelRec *aPixel, int width, int height, int lastDir, int segLength);
int GetCurrentTime(void);
void GetImageSize(int *width, int *height, char *filename);
void ReadImageToArray(int width, int height, char *filename);
void ReadBinaryFile(int width, int height, char *filename);
pixelRec *MakeNewPixel(void);
pixelRec *AppendPixel(pixelRec *lastPixel);
int CreatePixelList(int width, int height, pixelRec **firstPixel);
int GenerateRandOnetoEight(void);
void UpdatePixelDisplacement(pixelRec *aPixel);
int TryStep(int direction, pixelRec *aPixel, int width, int height);
void TakeOneStep(int direction, pixelRec *aPixel, int width, int height);
void RemoveLongPixelsFromList(pixelRec **pixelList, int segLength);
void RemoveShortPixelsFromImage(pixelRec *pixelList);
void WriteFile(int width, int height, char filename[]);
void FreePixelList(pixelRec *pixelList); 
int *FirstDerivative(int *wave, int wavelength);
int *SecondDerivative(int *wave, int wavelength);
int FindPeak(int *wave, int wavelength, int leftLimit, int rightLimit, float minAmp);
void SmoothImage(int width, int height, int kernelSize);
float SquareAverage(int edge, int startX, int startY);
float MoveSquare(int edge, int startX, int startY, float prevAvg);
void SmoothWave(int *wave, int wavelength, int smoothWidth);
void AbsWave(int *wave, int wavelength);
int *FindEdges(int *wave, int wavelength, int smoothWidth, float minAmp);
void ProcessRow(int yPosition, int width, int smoothWidth, float minAmp);
void ProcessColumn(int xPosition, int height, int smoothWidth, float minAmp);
void ProcessDiagUpRight(int startX, int startY, int width, int height,int smoothWidth, float minAmp);
void ProcessDiagDownRight(int startX, int startY, int width, int height,int smoothWidth, float minAmp);
void Overlay(int width, int height);
int CountNeighbors(int xCoord, int yCoord);
void DilateImage(int width, int height, int minNeighbors, int numIterations, int refresh);
int LesserOf(int x, int y);
void ProcessImage(int width, int height, int kernelSize, float minAmp);
int SkelFilter(int width, int height, int pass, int *table);
void Skeletonize(int width, int height);
void DrawCircle(int x, int y, int radius, int width, int height);
void ConnectEndPoints(int width, int height, int radius);
void FillWhitePerimeter(int x, int y, int halfWidth);
void TempGetsImage(int width, int height);
void ImageGetsTemp(int width, int height);
void BufferGetsImage(int width, int height);
void ImageGetsBuffer(int width, int height);
void TempGetsBuffer(int width, int height);
void BufferGetsTemp(int width, int height);
void EraseClusters(int width, int height, int edgeLength);
int CountBlackPixelsInBox(int x, int y, int halfWidth);
void EraseBoxFromTemp(int x, int y, int halfWidth);
int CountBlackPixelsInPerimeter(int x, int y, int halfWidth);
void CloseSmallGrains(int width, int height, int size);
void EraseBorder(int width, int height, int borderSize);
float SmallestDistance(int x, int y, int halfWidth);
float FindNearestNeighbor(int x, int y, int limit);
float CalculateChiSquared(int width, int height, int halfWidth, int xOffset, int yOffset);
void MinimizeChiSquared(int width, int height, int halfWidth, int offset);
void ShiftImage(int width, int height, int xOffset, int yOffset);
void ShiftTempImage(int width, int height, int xOffset, int yOffset);
void ErodeImage(int width, int height, int maxNeighbors);
float FindSmallerChiSqr(int width, int height, int halfWidth, int *x, int* y, float chiSqr);
void SmartMinChiSqr(int width, int height, int halfWidth, int offset);
void PruneImage(int width, int height, int maxNeighbors, int iterations, int refresh);
void GetCleanSkelEdges(char *infile, char *outfile, int width, int height);
void StackImages(char *image1, char *image2, char *outfile, int width, int height);
void InvertImage(int width, int height);
void AddBorder(int width, int height, int borderSize);
void FillRegion(int width, int height, int x, int y, int value);
int FloodFill(int width, int height, int x, int y, int value, int stack[10*maxDim][2]);
void MaximizeContrast(int width, int height, char *infile);
void GetGrainSizes(int width, int height, char *infile, char *outfile);
void FindWhiteNeighbor(int *x, int *y);
void FillBadGrains(int width, int height);
void WriteGrainSizes(int numGrains, char filename[]);
void NormalizeImage(int width, int height);
void ThresholdImage(int width, int height, int threshold);
int GetBoxMedian(int x, int y, int edgeLength);
int Median(int *list, int length);
void CreateMedianImage(int width, int height, int edgeLength);
void QuickSort(float arr[], int n);
void SortList(int *list, int length);
void InsertElement(int *list, int length, int position, int element);
void RemoveVertices(int width, int height, int minNeighbors);
void CreateRangeImage(int width, int height, int edgeLength);
void Convolve(int width, int height, int edgeLength);
void ReadKernel(char *filename, int edgeLength);
void NegateKernel(int edgeLength);
void KirschFilter(int width, int height);
void AbsoluteImage(int width, int height);
void AutomaticThreshold(int width, int height);
void AddRangeImage(int width, int height, char *outfile);
void AddMexHatImage(int width, int height, char *outfile);
void AddKirschImage(int width, int height, char *outfile);
void ZeroImage(int array[maxDim][maxDim]);
void ZeroConvolution(double array[maxDim][maxDim]);

void main(void)
{
  int W, H, // user-defined width and height...fixed for duration
      width, height, // current width and height...changes after AddBorder
      numfiles, // user-entered number of files to be analyzed, max 4
      i, j, k;  // dummy variables for iteration
  char line[100]; // user input is stored here
  char IN[100];   // keeps track of the current input filename
  char OUT[100];  // keeps track of the current output filename

// User-entered input filenames for a set of 4 images
  char in1[100];
  char in2[100];
  char in3[100];
  char in4[100];
// Output filenames to store "traced" images
  char out1[100];
  char out2[100];
  char out3[100];
  char out4[100];
// Stacked images created by combining "traced" images
  char stackA[] = "stackA";
  char stackB[] = "stackB";
  char stackC[] = "stackC";
// User-specified final output image...fully "traced" and combined
  char final[100];

  clock(); //Initializes the system clock for processor time calculations

  printf("How many files would you like to analyze? ");
  gets(line);
  numfiles = atoi(line);
// NOTE: numfiles expected to be 1, 2, 3, or 4...no error checking is done.


// Get filenames of input files from user
// Output filenames are created automatically by appending ".out"
  for (i=0;i<numfiles;i++) {
    printf("Please enter a filename: ");
    gets(line);
    switch (i) {
      case (0) : strcpy(in1, line);
                 strcpy(out1, strcat(line, ".out"));
	         break;
      case (1) : strcpy(in2, line);
                 strcpy(out2, strcat(line, ".out"));
	         break;
      case (2) : strcpy(in3, line);
                 strcpy(out3, strcat(line, ".out"));
	         break;
      case (3) : strcpy(in4, line);
                 strcpy(out4, strcat(line, ".out"));
	         break;
    }
  }
  
  //Get final output filename, width, height, length scale
  printf("Please enter a filename for the final image: ");
  gets(line);
  strcpy(final, line);
  printf("Enter the width of each image in pixels: ");
  gets(line);
  W = atoi(line);
  printf("Enter the height of each image in pixels: ");
  gets(line);
  H = atoi(line);
  printf("Enter the length of the shortest likely GB segment in pixels (default=15): ");
  gets(line);
  if (line == NULL) {
    minDisplacement = 7;
  } else {
    minDisplacement = atoi(line)/2;
  }
  printf("\n");

// No more user input required from here on.

  for (i=0;i<numfiles;i++) {
  //for each file, update the current filenames for input and output
    switch (i) {
    case (0) : strcpy(IN, in1);
               strcpy(OUT, out1);
               break;
    case (1) : strcpy(IN, in2);
               strcpy(OUT, out2);
               break;
    case (2) : strcpy(IN, in3);
               strcpy(OUT, out3);
               break;
    case (3) : strcpy(IN, in4);
               strcpy(OUT, out4);
               break;
    }

// Initialize image storage arrays
    ZeroImage(image);
    ZeroImage(tempImage);
    ZeroImage(temp);
    ZeroImage(buffer);
    ZeroImage(neighbors);
    ZeroConvolution(convolution);
    printf("Initialization complete.\n");

// update current width and height, read input file, add a border, and update width and height
    width = W; height = H;
    ReadBinaryFile(width, height, IN);
    AddBorder(width, height, MaxOffset);
    width+=2*MaxOffset; height+=2*MaxOffset;

// smooth the image and store to file "median"
    CreateMedianImage(width, height, 3);
    WriteFile(width, height, "median");

// Get edges with three different edge filters, each starting with the stored "median"
// image and combine all the edges
    AddKirschImage(width, height, none);
    AddRangeImage(width, height, none);
    AddMexHatImage(width, height, none);

// Consolidation, short segment removal, consolidation again, cluster removal,
// and border erase
    InvertAndFilter(width, height, none);
    RemoveVertices(width, height, 3);
    FilterImage(width, height, minDisplacement/2);
    FilterImage(width, height, minDisplacement/2);
    InvertAndFilter(width, height, none);
    EraseClusters(width, height, 15);
    EraseBorder(width, height, MaxOffset+3);
    
// Write output file and report processor time, then move on to the next image    
    WriteFile(width, height, OUT);
    printf("finished  %s in ", IN);
    WriteProcessorTime();
    printf("\n");

  }

// By this point, there exists an output file for each input file which contains the
// "traced" image. Here we assume that there are 4 files and therefore 3 stacking
// steps are necessary...we would need to add more control logic to handle the general
// case. The stacked images are written to disk

  StackImages(out1, out2, stackA, width, height);
  StackImages(out3, out4, stackB, width, height);
  StackImages(stackA, stackB, stackC, width, height);

// read the last stack (4 images combined) and consolidate, then skeletonize and prune
  ReadBinaryFile(width, height, stackC);
  DilateImage(width, height, 4, 10, Refresh);
  ErodeImage(width, height, 2);
  DilateImage(width, height, 1, 1, Refresh);
  InvertAndFilter(width, height, none);
  Skeletonize(width, height);
  PruneImage(width, height, 1, ErodeLength, Refresh);

// Write the final output file
  WriteFile(width, height, final);
  printf("Execution complete.\n");
  WriteProcessorTime();

// Erase extra files created during execution.
  remove(stackA);
  remove(stackB);
  remove(stackC);
  remove("median");
  remove(in1); remove(out1);
  remove(in2); remove(out2);
  remove(in3); remove(out3);
  remove(in4); remove(out4);
  
// NOTE: I originally had a separate executable to calculate the grain sizes
// from the final image since this is relatively easy...the code is in this
// file but it'd probably be easiest to just duplicate the file and replace the
// main function with something that reads in a file and calculates its grain
// size using the non-recursive FloodFill routine. 

}


// Function: WriteProcessorTime
// Prints to screen the elapsed time of execution from the start of execution.

void WriteProcessorTime(void)
{
  clock_t process;
  ldiv_t result;

  process = clock()/CLOCKS_PER_SEC;
  result = ldiv(process, (long)60);
  printf("%ld minutes and %ld seconds\n", result.quot, result.rem);
}

// Function: InvertAndFilter
// Applies a series of dilation operations to an image followed by a
// skeletonization, used during image processing to consolidate nearby
// features. The details of the dilation may be changed to affect the result.
// Note that the name no longer properly describes the functionality.

void InvertAndFilter(int width, int height, char *filename)
{
  int useFile;

  if (useFile = strcmp(filename, none) != 0) {
    ReadBinaryFile(width, height, filename);
  }
  DilateImage(width, height, 1, 1, Refresh);
  DilateImage(width, height, 2, 1, NoRefresh);
  DilateImage(width, height, 3, 1, NoRefresh);
//  DilateImage(width, height, 4, 1, NoRefresh);
  Skeletonize(width, height);
  if (useFile != 0) {
    WriteFile(width, height, filename);
  }
}

// Function: FilterImage
// Given a binary image and a minimum segment length (in pixels) this function
// removes any line segment from the image which is shorter than specified. Used to
// eliminate spurious features before image combination. Make sure images are
// skeletonized first (won't crash on un-skeletonized images, but won't work very
// well. In general, FilterImage is called twice in succession and will remove
// segments of length 1 to 2*segLength pixels. If only called once, the centers
// of some segments will be removed, namely those with lengths between segLength+1
// and 2*segLength.

void FilterImage(int width, int height, int segLength)
{
	int numBlackPixels=0;
	pixelRec *pixelList, *aPixel;

  numBlackPixels = CreatePixelList(width, height, &pixelList);
  printf("There are %d black pixels in the image\n", numBlackPixels);
  if (numBlackPixels == 0) {
    printf("Image is empty. Exiting program...\n");
    exit(0);
  }
  // After creating the linked list of pixelRecs corresponding to all the black
  // pixels in the image, a recursive function call is used to check if each pixel
  // is well-connected, that is if it belongs to a segment of at least segLength pixels.
  // If not, pixels are removed from the image.
  aPixel = pixelList;
  while (aPixel != NULL) {
    if (RecursiveWalker(&pixelList, aPixel, width, height, 0, segLength) == 0) {
      RecursiveWalker(&pixelList, aPixel, width, height, 4, segLength);
    }
    aPixel = aPixel->link;
  }
  RemoveLongPixelsFromList(&pixelList, segLength);
  RemoveShortPixelsFromImage(pixelList);
  FreePixelList(pixelList);
}


/*Function RecursiveWalker:
  A walker starts at a black pixel and walks recursively, along neigboring black
  pixels only, until it has travelled segLength pixels; if successful, the pixel
  is removed from the list. Any pixels remaining in the linked list (i.e., those
  that failed to travel the required distance) will be removed from the image (i.e.,
  colored white instead of black) */

int RecursiveWalker(pixelRec **pixelList, pixelRec *aPixel, int width, int height, int lastDir, int segLength)
{
  int i, dir=lastDir, amount;
  pixelRec *currPixel, *prevPixel=NULL, *nextPixel;

    currPixel = aPixel;
    nextPixel = currPixel->link;
    for (i=0;i<7;i++) {
      amount = (i%2 == 0) ? i : -i;
      dir = (dir-amount+8)%8;
      if (TryStep(dir, aPixel, width, height) == 1) {
        if (aPixel->displacement > segLength) {
          return(1);
        }
        if (RecursiveWalker(pixelList, aPixel, width, height, dir, segLength) == 1) {
          return(1);
        }
      }
    }
    TryStep((lastDir+4)%8, aPixel, width, height);
    aPixel->displacement -= 2;
    return(0);
}



/* Function GetTime: gets the current local time from the operating system
   and outputs it as an integer (used as a seed for the random numbers) */
int GetCurrentTime(void)
{
  struct tm *t;
  int test;
  time_t ltime;
  time(&ltime);

  t = localtime(&ltime);
  test = (t->tm_sec)+(t->tm_min)*60.0+(t->tm_hour)*3600.0+
    (t->tm_mday)*(100000.0);
  printf("Random seed is %d\n",test);
  return(test);
}
 
/* Function GetImageSize:
	 Reads in the image from filename and counts delimited integers from the first line
	 to determine the width. Counts newlines to determine height. Make sure the input
	 file contains a trailing newline. 
	 
	 NOTE: Obselete. This works for text files, but binary files are now being used
	 as they are more compact and quicker to deal with. */

void GetImageSize(int *width, int *height, char *filename)
{
  int count=0, c;
  FILE *inputFile;
  
  inputFile = fopen(filename, "r");
  if (inputFile == NULL) {
    printf("Could not open file!");
    exit(0);
  }
  while ((c = getc(inputFile)) != '\n') {
    ungetc(c,inputFile);
    fscanf(inputFile, "%d", &c);
    count++;
  }
  *width = count;
  count = 1;
  while ((c = getc(inputFile)) != EOF) {
    if (c == '\n') {
      count++;
    }
  }
  *height = count;
  fclose(inputFile);
  printf("The image is %d pixels across and %d pixels down\n",*width,*height);
}
 
/* Function ReadImageToArray: opens "in.txt" and reads values into the 
   array image[width][height] which is declared globally (outside of main).
   Does no error checking since it takes width and height as parameters. 
   
   NOTE: again, OBSELETE since binary files are now used. */

void ReadImageToArray(int width, int height, char *filename)
{
  int i=0, j=0, c;
  FILE *inputFile;
 
  inputFile = fopen(filename, "r");
  if (inputFile == NULL) {
    printf("Could not open file!");
    exit(0);
  }
  for (j=0;j<(height);j++) {
    for (i=0;i<(width);i++) {
      fscanf(inputFile, "%d", &(image[i][j]));
    }
    while ((c = getc(inputFile)) != '\n') {
    }
  }
  fclose(inputFile);
  printf("Image has been successfully read into memory\n");
}


/* Function ReadBinaryFile: reads in an image file (requires width and height as
   parameters) assuming one byte per pixel */
   
void ReadBinaryFile(int width, int height, char *filename)
{
  int i=0, j=0, c;
  FILE *inputFile;
 
  inputFile = fopen(filename, "rb");
  if (inputFile == NULL) {
    printf("Could not open file!");
    exit(0);
  }
  for (j=0;j<(height);j++) {
    for (i=0;i<(width);i++) {
       image[i][j] = (int)(getc(inputFile));
    }
  }
  fclose(inputFile);
  printf("%s has been successfully read into memory\n", filename);
}


/* Function MakeNewPixel: allocates memory for a new pixelRec and returns the
   pointer to that pixelRec. Used in generating the linked list of black
   pixels used in FilterImage. */
 
pixelRec *MakeNewPixel(void)
{
  pixelRec *newPixel;
  
  newPixel = (pixelRec *)malloc(sizeof(pixelRec));
  if (newPixel == NULL) {
    printf("Error creating a new pixel: out of memory!");
    exit(0);
  }
  return(newPixel);
}


/* Function AppendPixel: takes a pointer to the lastPixel in the list and
   adds a new pixel to the list after lastPixel. Sets the link on the new
   pixel to NULL and returns the pointer to the new pixel. Used in generating
   the linked list of black pixels used in FilterImage.*/
 
pixelRec *AppendPixel(pixelRec *lastPixel)
{
  pixelRec *newPixel;
  
  newPixel = MakeNewPixel();
  if (lastPixel != NULL) {
    lastPixel->link = newPixel;
  }
  newPixel->link = NULL;
  return (newPixel);
}
 


/* Function CreatePixelList: Takes the width and height of the image array and
   iterates through all the pixels in it. Each time a black pixel is found, a
   new pixel is added to the pixel list (beginning with firstPixel). Returns 
   the number of pixels which have been stored in the list. Used to create the
   linked list of black pixels used in FilterImage. */
 
int CreatePixelList(int width, int height, pixelRec **firstPixel)
{
  pixelRec *currentPixel=NULL, *lastPixel=NULL;
  int i=0, j=0, numPixels=0;
  
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      if (image[i][j] > 0) {
		currentPixel = AppendPixel(lastPixel);
		currentPixel->startX = i;
		currentPixel->startY = j;
		currentPixel->currentX = i;
		currentPixel->currentY = j;
		currentPixel->displacement = 0;
		lastPixel = currentPixel;
		if (numPixels++ == 0) {
		  *firstPixel = currentPixel;
		}
      }
    }
  }
  return(numPixels);
}

//Function: GenerateRandOnetoEight:
// The name says it all (almost) ! Generates a random integer between 0 and 7.
int GenerateRandOnetoEight(void)
{
  return (rand() % 8);
}
 

/* Function UpdatePixelDisplacement: calculates and returns the square
   of the distance between a pixel and the "walker" associated with it.
   Used to check whether the pixel has moved far enough along a segment. */
 
void UpdatePixelDisplacement(pixelRec *aPixel)
{
  double deltaX, deltaY, dist;
  
  deltaX = aPixel->currentX - aPixel->startX;
  deltaY = aPixel->currentY - aPixel->startY;
  dist = deltaX*deltaX + deltaY*deltaY;
  aPixel->displacement = (float)dist;
}
 

/* Function TryStep: Checks a random neighbor of aPixel's "walker" to see
   if it's black. If so, the "walker" moves there and the displacement is
   recalculated. Returns (1) if the step was successful, (0) if not. Called
   by RecursiveWalker, occurs during the FilterImage function. */
 
int TryStep(int direction, pixelRec *aPixel, int width, int height)
{
  switch (direction) {
  case 0: if ((aPixel->currentY + 1 < height) &&
	      (image[aPixel->currentX][aPixel->currentY + 1] == 255)) {
    aPixel->currentY++;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 1: if ((aPixel->currentY + 1 < height) &&
              (aPixel->currentX + 1 < width) &&
	      (image[aPixel->currentX + 1][aPixel->currentY + 1] == 255)) {
    aPixel->currentX++;
    aPixel->currentY++;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 2: if ((aPixel->currentX + 1 < width) &&
	      (image[aPixel->currentX + 1][aPixel->currentY] == 255)) {
    aPixel->currentX++;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 3: if ((aPixel->currentY - 1 >= 0) &&
              (aPixel->currentX + 1 < width) &&
	      (image[aPixel->currentX + 1][aPixel->currentY - 1] == 255)) {
    aPixel->currentX++;
    aPixel->currentY--;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 4: if ((aPixel->currentY - 1 >= 0) &&
	      (image[aPixel->currentX][aPixel->currentY - 1] == 255)) {
    aPixel->currentY--;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 5: if ((aPixel->currentY - 1 >= 0) &&
              (aPixel->currentX - 1 >= 0) &&
	      (image[aPixel->currentX - 1][aPixel->currentY - 1] == 255)) {
    aPixel->currentX--;
    aPixel->currentY--;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 6: if ((aPixel->currentX - 1 >= 0) &&
	      (image[aPixel->currentX - 1][aPixel->currentY] == 255)) {
    aPixel->currentX--;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  case 7: if ((aPixel->currentY + 1 < height) &&
              (aPixel->currentX - 1 >= 0) &&
	      (image[aPixel->currentX - 1][aPixel->currentY + 1] == 255)) {
    aPixel->currentX--;
    aPixel->currentY++;
    aPixel->displacement++;
    return(1);
  }
  return(0);
  default: printf("Illegal number passed to TryStep\n");
  return(0);
  }
}
 


/* Function TakeOneStep:
   aPixel's walker tries to step in direction. If unsuccessful, direction is
   incremented and the walker tries again. If no step can be made in any direction
   the function breaks out. If a successful step has been made, the walker
   takes another step in the same direction until it hits a white pixel. 
   
   NOTE: this function is not currently used
*/

void TakeOneStep(int direction, pixelRec *aPixel, int width, int height)
{
  int currDir, startDir;
 
  startDir = direction;
  currDir = direction;
  while (TryStep(currDir, aPixel, width, height) != 1) {
    currDir = (currDir+1) % 8;
    if (currDir == startDir) {
      return;
    }
  }
  while (TryStep(currDir, aPixel, width, height) ==1) {
  }
}
 

/* Function RemoveLongPixelsFromList: Given the pixelList, iterates through
   the list and removes pixels which have displacements greater than the
   global constant minDisplacement. Memory associated with removed pixels is
   freed. The linked list is now shorter and includes only those pixels whose
   walkers have not yet travelled segLength pixels. */

void RemoveLongPixelsFromList(pixelRec **pixelList, int segLength)
{
  pixelRec *currPixel, *prevPixel, *nextPixel;
  
  currPixel = *pixelList;
  prevPixel = NULL;
  
  while (currPixel != NULL) {
    nextPixel = currPixel->link;
    if (currPixel->displacement > segLength) {
      if (prevPixel == NULL) {
	*pixelList = currPixel->link;
      } else {
	prevPixel->link = currPixel->link;
      }
      free(currPixel);
      currPixel = nextPixel;
    } else {
      prevPixel = currPixel;
      currPixel = nextPixel;
    }
  }
}
 

/* Function RemoveShortPixelsFromImage:
   Iterates through the pixelList (which contains only pixels belonging to
   short segments) and erases them from the image (pixel value set to 0). */

void RemoveShortPixelsFromImage(pixelRec *pixelList)
{
  pixelRec *currPixel;
  
  printf("removing short pixels\n");
  currPixel = pixelList;
  while (currPixel != NULL) {
    image[currPixel->startX][currPixel->startY] = 0;
    currPixel = currPixel->link;
  }
  printf("finished removing short pixels\n");
}



/* Function WriteFile:
   Writes the contents of the image array to a binary file named filename.
   No header is used, it is assumed that the user already knows the width
   and height of the image independently. File size is therefore exactly
   equal to width*height bytes. */

void WriteFile(int width, int height, char filename[])
{
  int i=0, j=0;
  FILE *outFile;

  outFile = fopen(filename, "wb");
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
    	fprintf(outFile, "%c", image[i][j]);
      /*if (i == width-1) {
            fprintf(outFile, "%d", image[i][j]);
      } else {
            fprintf(outFile, "%d\t", image[i][j]);
      }*/
    }
    /*fprintf(outFile, "\n");*/
  }
  fclose(outFile);
}


/* Function FreePixelList:
   Cycles through the pixelList and frees the memory associated with each pixel.
   Done after the FilterImage function is finished executing. */

void FreePixelList(pixelRec *pixelList)
{
  pixelRec *currPixel, *lastPixel;

  currPixel = pixelList;
  while (currPixel != NULL) {
    lastPixel = currPixel;
    currPixel = currPixel->link;
    free(lastPixel);
  }
}


/* Function FirstDerivative: takes the first derivative of an array of
   numbers and returns the pointer to the array. Derivative is a centered
   difference correct to fourth order in dx, taken from Hornbeck p. 22,
   table 3.4b. First two and last two pixels are second order forward
   and backward differences as appropriate from table 3.3a and b
   
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason.  */

int *FirstDerivative(int *wave, int wavelength)
{
	int i=0, *firstD;
	float *temp;

	temp = (float *)malloc(wavelength*sizeof(float));
	if (temp == NULL) {
		printf("Error taking first derivative: out of memory!\n");
		exit(0);
	}
	firstD = (int *)malloc(wavelength*sizeof(int));
	if (firstD == NULL) {
		printf("Error taking first derivative: out of memory!\n");
		exit(0);
	}
	temp[0] = 0; temp[wavelength-1] =0;
	/* centered difference for middle pixels*/

	for (i=1;i<wavelength-1;i++) {
	  temp[i] = ((float)wave[i+1] - wave[i-1])/2;
	}
	for (i=0;i<wavelength;i++) {
		firstD[i] = (int)(Dmult*temp[i]);
	}
	free(temp);
	return (firstD);
}



/* Function SecondDerivative: takes the second derivative of an array of
   numbers and returns the pointer to the array. Derivative is a centered
   difference correct to fourth order in dx, taken from Hornbeck p. 22,
   table 3.4b. First two and last two pixels are second order forward
   and backward differences as appropriate from table 3.3a and b 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

int *SecondDerivative(int *wave, int wavelength)
{
	int i=0, *secondD;
	float *temp;

	temp = (float *)malloc(wavelength*sizeof(float));
	if (temp == NULL) {
		printf("Error taking second derivative: out of memory!\n");
		exit(0);
	}
	secondD = (int *)malloc(wavelength*sizeof(int));
	if (secondD == NULL) {
		printf("Error taking second derivative: out of memory!\n");
		exit(0);
	}

	temp[0] = 0; temp[wavelength-1] = 0;
	/* centered difference for middle pixels*/

	for (i=1;i<wavelength-1;i++) {
	  temp[i] = ((float)wave[i-1] -2*wave[i]+wave[i+1]); 
	}

	for (i=0;i<wavelength;i++) {
		secondD[i] = (int)(Dmult*temp[i]);
	}
	free(temp);
	return (secondD);
}


/* Function FindPeak: looks for a peak in wave between leftLimit and rightLimit,
   with the requirement that the amplitude of the peak must be greater than
   minAmp. Takes first and second derivatives of the wave (best if the wave
   has already been smoothed) and requires firstD = 0, secondD < 0, and wave >
   minAmp before a peak is identified. Returns the position at which these
   conditions are met, or -1 if no peak is found. Changes leftLimit to the
   position of the right side inflection point (zero value of secondD) 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

int FindPeak(int *wave, int wavelength, int leftLimit, int rightLimit, float minAmp)
{
	int i=0, j=0, *firstD, *secondD;

	firstD = FirstDerivative(wave, wavelength);
	SmoothWave(firstD, wavelength, smallWindow);
	secondD = SecondDerivative(wave, wavelength);
	SmoothWave(secondD, wavelength, smallWindow);
	i = leftLimit;
	while (i < rightLimit-1) {
		while (((float)firstD[i] * firstD[i+1])>0) {
		  i++;
		  if (i == rightLimit-1) {
		       	free(firstD);
		       	free(secondD);
		       	return(-1);
		  }
		}
		if ((wave[i] > Dmult*minAmp) && (firstD[i] > firstD[i+1]) && (secondD[i] < 0)) {
			 free(firstD);
			 free(secondD);
			 return(i);
		}
		i++;
	}
	free(firstD);
	free(secondD);
	return(-1);
}


/* Function SmoothImage: smooths the image by averaging neighboring pixels
   with each pixel in the image. The edge length of the box around the pixel
   is determined by kernelSize. Make sure kernelSize is odd. Ignores those
   pixels which are too near the edge of the image, but this should not
   have a big effect.    
   
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason.
   
   Added functionality is now available using the Convolve function which
   allows the use of arbitrary kernels defined in text files. */

void SmoothImage(int width, int height, int kernelSize)
{
	int i=0, j=0, halfWidth;
	float prevAvg;

	printf("Smoothing image...\n");
	halfWidth = (kernelSize-1)/2;
	for (j=halfWidth;j<(height-halfWidth);j++) {
		for (i=halfWidth;i<(width-halfWidth);i++) {
			if (i == halfWidth) {
				prevAvg = SquareAverage(kernelSize, i, j);
				tempImage[i][j] = (int)prevAvg;
			} else {
				prevAvg = MoveSquare(kernelSize, i, j, prevAvg);
				tempImage[i][j] = (int)prevAvg;
			}
		}
	}
	for (j=halfWidth;j<height-halfWidth;j++) {
		for (i=halfWidth;i<width-halfWidth;i++) {
			image[i][j] = tempImage[i][j];
		}
	}
}

/* Function SquareAverage: returns the average value of the elements
   of a square subset of image. Does no error checking on the values it
   is passed, so care must be taken. 2*halfWidth+1 corresponds to the edge
   length of the square. Image array indexing is assumed to start from 0,0.
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

float SquareAverage(int edge, int startX, int startY)
{
	int i=0, j=0, halfWidth;
	float avg=0;
	
	halfWidth = (edge-1)/2;
	for (j=(startY-halfWidth);j<=(startY+halfWidth);j++) {
		for (i=(startX-halfWidth);i<=(startX+halfWidth);i++) {
			avg += ((float)(image[i][j])/(edge*edge));
		}
	}
	return(avg);
}

/* Function MoveSquare: similar to SquareAverage except that it uses the average of
   the previous square as a starting point to minimize additional calculation. While
   SquareAverage is O(n^2) on the edge length, MoveSquare is only O(n), making it much
   faster. This function will be used on all pixels except the leftmost pixel of each
   row. 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

float MoveSquare(int edge, int startX, int startY, float prevAvg)
{
	int i=0, j=0, halfWidth;
	float avg=0;
	
	avg = prevAvg;
	halfWidth = (edge-1)/2;
	i = startX-halfWidth-1;
	for (j=(startY-halfWidth);j<=(startY+halfWidth);j++) {
		avg -= (float)(image[i][j])/(edge*edge);
	}
	i = startX+halfWidth;
	for (j=(startY-halfWidth);j<=(startY+halfWidth);j++) {
		avg += (float)(image[i][j])/(edge*edge);
	}
	return(avg);
}

/* Function SmoothWave: takes an array of ints, its length, and the width of
the smoothing window (must be odd) to be used. For an n-point smoothing
window, the average of n values is calculated and stored at the center
position of the window, then the window position is incremented. This
calculates a smoothed list except for (n-1)/2 points in the list at each end.
Values are calculated for these positions by reflecting real values across the
end of the list to give a window of the proper width. 
   
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

void SmoothWave(int *list, int length, int smoothWidth)
{
  int *templist, i=0, j=0, halfWidth=(smoothWidth-1)/2, avg=0, *subList;

  templist = (int *)malloc(length*sizeof(int));
  if (templist == NULL) {
    printf("Error smoothing wave: out of memory!\n");
    exit(0);
  }
  subList = (int *)malloc(3*halfWidth*sizeof(int));
  if (subList == NULL) {
    printf("Error smoothing wave: out of memory!\n");
    exit(0);
  }

  /* create a sublist for the left side which reflects list values across
     the 0 position so that the low-index values can be averaged as well
  */

  for(j=0;j<=halfWidth;j++) {
      subList[halfWidth+j]=list[j];
      subList[halfWidth-j]=list[j];
  }
  for(j=halfWidth+1;j<(3*halfWidth);j++) {
      subList[j]=list[j];
  }

  /* Calculate average values and place them in templist for the low-index
     points (indices less than halfWidth)
  */

  for(i=halfWidth;i<2*halfWidth;i++) {
    avg=0;
    for(j=i-halfWidth;j<=i+halfWidth;j++) {
      avg+=subList[j];
    }
    avg=(float)(avg)/smoothWidth;
    templist[i-halfWidth]=avg;
  }  

  /* Calculate average values and place them in templist for all internal
     points
  */

  for(i=halfWidth;i<length-halfWidth;i++) {
    avg=0;
    for(j=i-halfWidth;j<=i+halfWidth;j++) {
      avg+=list[j];
    }
    avg=(float)(avg)/smoothWidth;
    templist[i]=avg;
  }

  /* create a sublist for the right side which reflects list values across
     the 'length' position so that high-index values can be averaged as well
  */

  for(j=0;j<2*halfWidth;j++) {
    subList[j]=list[length-2*halfWidth+j];
  }
  for(j=2*halfWidth;j<(3*halfWidth);j++) {
      subList[j]=subList[4*halfWidth-j];
  }

  /* Calculate average values for high index points (from length-halfWidth up)*/

  for (i=halfWidth;i<2*halfWidth;i++) {
    avg = 0;
    for (j=i-halfWidth;j<=i+halfWidth;j++) {
      avg+=subList[j];
    }
    avg=(float)(avg)/smoothWidth;
    templist[length-2*halfWidth+i]=avg;
  }

  /* Write smoothed wave over original*/
  for (i=0;i<length;i++) {
    list[i] = templist[i];
  }
  free(templist);
  free(subList);
}

/* Function: AbsWave
   Replaces each point in the wave with its absolute value.
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */
   
void AbsWave(int *wave, int wavelength)
{
	int i=0;

	for (i=0;i<wavelength;i++) {
	     if (wave[i] <0) {
	        wave[i] = -wave[i];
	      }
	}
}


/* Function FindEdges: uses FindPeak to locate peaks in the wave. Identifies
   all peaks which have at least minAmp amplitude and overwrites the wave with
   a binary image (white with black edges at peaks) 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

int *FindEdges(int *wave, int wavelength, int smoothWidth, float minAmp)
{
	int *edgeWave, *firstD, *secondD, i=0, j=0, peakLoc, *temp, halfWidth=(smoothWidth-1)/2, width=1;

	edgeWave = (int *)malloc(wavelength*sizeof(int));
	if (edgeWave == NULL) {
		printf("Error finding edges: out of memory!\n");
		exit(0);
	}
	for (i=0;i<wavelength;i++) {
		edgeWave[i] = 0;
	}
	temp = wave;
	wave = FirstDerivative(wave, wavelength);
	free(temp);
	AbsWave(wave, wavelength);
	SmoothWave(wave, wavelength, smoothWidth);
        firstD = FirstDerivative(wave, wavelength);
        SmoothWave(firstD, wavelength, smallWindow);
        secondD = SecondDerivative(wave, wavelength);
        SmoothWave(secondD, wavelength, smallWindow);
	
	for (i=halfWidth;i<wavelength-halfWidth;i++) {
	  if ((firstD[i]*firstD[i+1] <= 0) && 
	      (secondD[i] < 0) /*&&
	      (wave[i] > Dmult*minAmp)*/) {
	    width = 1;
	    j=i-1;
	    while (secondD[j] <= 0) {
	      width++;
	      j--;
	    }
	    j=i+1;
	    while (secondD[j] <=0) {
	      width++;
	      j++;
	    }
	    edgeWave[i] = (int)(Dmult*(float)wave[i]/width);
	  } else {
	    edgeWave[i] = 0;
	  }
	}
	free(wave);
	free(firstD);
	free(secondD);
	return(edgeWave);
}


/* Function ProcessRow: takes one row from the image, binarizes it using
   FindEdges, and writes it to its original position in tempImage 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

void ProcessRow(int yPosition, int width, int smoothWidth, float minAmp)
{
	int *wave, *temp, i=0;

	wave = (int *)malloc(width*sizeof(int));
	if (wave == NULL) {
		printf("Error processing row: out of memory!\n");
		exit(0);
	}
	
	for (i=0;i<width;i++) {
		wave[i] = image[i][yPosition];
	}
	temp = wave;
	wave = FindEdges(wave, width, smoothWidth, minAmp);
	for (i=0;i<width;i++) {
		tempImage[i][yPosition] = wave[i];
	}
	free(temp);
	free(wave);
}


/* Function ProcessColumn: takes one column from the image array, binarizes
   it using FindEdges, and writes it to its position in tempImage 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

void ProcessColumn(int xPosition, int height, int smoothWidth, float minAmp)
{
	int *wave, *temp, i=0;

	wave = (int *)malloc(height*sizeof(int));
	if (wave == NULL) {
		printf("Error processing column: out of memory!\n");
		exit(0);
	}
	
	for (i=0;i<height;i++) {
	  wave[i] = image[xPosition][i];
	}
	temp = wave;
	wave = FindEdges(wave, height, smoothWidth, minAmp);
	for (i=0;i<height;i++) {
	  tempImage[xPosition][i] = wave[i];
	}
        free(temp);
	free(wave);
}


/* Function ProcessDiagUpRight: Creates a wave going from lower left to
   upper right, binarizes it using FindEdges, and writes it to its position
   in tempImage. 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

void ProcessDiagUpRight(int startX, int startY, int width, int height, int smoothWidth, float minAmp)
{
  int count=0, i=startX, j=startY, k=0, *wave, *ptr, list[maxDim], shortSmoothWidth = smoothWidth/1.414;

  if (shortSmoothWidth % 2 == 0) {
  	shortSmoothWidth++;
  }
  while ((i < width) && (j > 0)) {
    list[count] = image[i][j];
    i++;
    j--;
    count++;
  }
  wave = (int *)malloc(count*sizeof(int));
  if (wave == NULL) {
    printf("Error processing diagonal: out of memory!\n");
    exit(0);
  }
  for (i=0;i<count;i++) {
    wave[i] = list[i];
  }
  ptr = wave;
  wave = FindEdges(wave, count, shortSmoothWidth, minAmp);
  i=startX;
  j=startY;
  for (k=0;k<count;k++) {
    tempImage[i][j] = wave[k];
    i++;
    j--;
  }
  free(ptr);
  free(wave);
}


/* Function ProcessDiagDownRight: Creates a wave going from upper left to
   lower right, binarizes it using FindEdges, and writes it to its position
   in tempImage. 
      
   NOTE: currently not used. This is a part of the edge-detection algorithm
   developed early on...the algorithm, though useful, is very slow and is
   not used for this reason. */

void ProcessDiagDownRight(int startX, int startY, int width, int height, int smoothWidth, float minAmp)
{
  int count=0, i=startX, j=startY, k=0, *wave, *ptr, list[maxDim], shortSmoothWidth = smoothWidth/1.414;

  if (shortSmoothWidth % 2 == 0) {
  	shortSmoothWidth++;
  }
  while ((i < width) && (j < height)) {
    list[count] = image[i][j];
    i++;
    j++;
    count++;
  }
  wave = (int *)malloc(count*sizeof(int));
  if (wave == NULL) {
    printf("Error processing diagonal: out of memory!\n");
    exit(0);
  }
  for (i=0;i<=count;i++) {
    wave[i] = list[i];
  }
  ptr = wave;
  wave = FindEdges(wave, count, shortSmoothWidth, minAmp);
  i=startX;
  j=startY;
  for (k=0;k<count;k++) {
    tempImage[i][j] = wave[k];
    i++;
    j++;
  }
  free(ptr);
  free(wave);
}

/*Function Overlay: Logical OR of image and tempImage. Saved to the image array. */

void Overlay(int width, int height)
{
  int i=0, j=0;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      if (image[i][j] <  tempImage[i][j]) {
	image[i][j] = tempImage[i][j];
      }
    }
  }
}

/*Function CountNeighbors: Returns the number of black pixels surrounding a given black pixel
  at location (x,y) in the image array */

int CountNeighbors(int xCoord, int yCoord)
{
  int i, j, sum=0;

  for (i=xCoord-1;i<=xCoord+1;i++) {
    for (j=yCoord-1;j<=yCoord+1;j++) {
      if (image[i][j] != 0) {
	sum++;
      }
    }
  }
  if (image[xCoord][yCoord] != 0) {
    sum--;
  }
  return(sum);
}

/*Function DilateImage: if the number of black neighbors that a pixel has is at least
  minNeighbors, that pixel is colored black. Otherwise it is left alone. Dilation is
  repeated numIterations times and the neighbor array is used to store the number of
  black neighbors that each pixel has. This speeds up operation tremendously when
  many iterations are required. If the refresh parameter is 0, the neighbors array
  will be determined during the first pass through the image, otherwise it is
  assumed that the number of neighbors listed in the array is correct. Generally,
  the refresh parameter is passed as 1 to ensure that the neighbor array is kept
  current. */

void DilateImage(int width, int height, int minNeighbors, int numIterations, int refresh)
{
        int i, j, k, m, n, value;
	if (refresh == 1) {
	  for (j=MaxOffset;j<height-MaxOffset;j++) {
	    for (i=MaxOffset;i<width-MaxOffset;i++) {
	      tempImage[i][j] = CountNeighbors(i, j);
	    }
	  }
	}
        for (k=0;k<numIterations;k++) {
	  for (j=MaxOffset;j<height-MaxOffset;j++) {
            for (i=MaxOffset;i<width-MaxOffset;i++) {
	      neighbors[i][j] = tempImage[i][j];
	    }
	  }
	  for (j=MaxOffset;j<height-MaxOffset;j++) {
            for (i=MaxOffset;i<width-MaxOffset;i++) {
              if ((image[i][j] != 255) && (neighbors[i][j] >= minNeighbors)) {
                image[i][j] = 255;
                for (n=j-1;n<=j+1;n++) {
                  for (m=i-1;m<=i+1;m++) {
                    value = tempImage[m][n];
                    if ((m!=i) || (n!=j)) {
                      tempImage[m][n]++;
                    }
                  }
                }
              }
            }
          }
        }
}


 // Function: LesserOf
 // Returns the lesser of two integers x and y.
    
int LesserOf(int x, int y)
{
  if (x < y) {
    return(x);
  } else {
    return(y);
  }
}


// Function: ProcessImage
// Obselete. This is the edge-finding code which was originally developed but
// dropped due to speed and complexity considerations. It called the ProcessRow,
// ProcessColumn, etc. functions to find edges in various directions and map
// them onto the image. kernelSize is the size of the smoothing region, and
// minAmp is the minimum amplitude of a peak found along a given direction for
// it to be considered a real edge. 

void ProcessImage(int width, int height, int kernelSize, float minAmp)
{
  int i=0, j=0;

  printf("Begin image processing...\n");
/*  SmoothImage(width, height, kernelSize);*/

  CreateMedianImage(width, height, 3);
  CreateMedianImage(width, height, 7);
  CreateMedianImage(width, height, 11);
/*  CreateMedianImage(width, height, 15);*/
  printf("Finished smoothing.\n");
/*  WriteFile(width, height, "smoothed");*/
  for (j=0;j<height;j++) {
        for (i=0;i<width;i++) {
                temp[i][j] = image[i][j];
		tempImage[i][j] = 0;
	      }
      }

  for (j=0;j<height;j++) {
    ProcessRow(j, width, waveSmoothWindow, minAmp);
  }
  ImageGetsTemp(width, height);
  PruneImage(width, height, 1, 3, Refresh);
  BufferGetsImage(width, height);

/*  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);
  BufferGetsImage(width, height);*/
  printf("Finished processing rows\n");

  for (j=0;j<height;j++) {
        for (i=0;i<width;i++) {
                image[i][j] = temp[i][j];
                tempImage[i][j] = 0;
        }
  }
  for (i=0;i<width;i++) {
    ProcessColumn(i, height, waveSmoothWindow, minAmp);
  }
  ImageGetsTemp(width, height);
  PruneImage(width, height, 1, 3, Refresh);

/*  DilateImage(width, height, 1, 1, Refresh);
  DilateImage(width, height, 2, 1, NoRefresh);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);*/
  TempGetsBuffer(width, height);
  Overlay(width, height);
  BufferGetsImage(width, height);
  printf("Finished processing columns\n");

  for (j=0;j<height;j++) {
  	for (i=0;i<width;i++) {
  		image[i][j] = temp[i][j];
  		tempImage[i][j] = 0;
  	}
  }
  for (j=0;j<height;j++) {
    if (LesserOf(j, width) > 2*waveSmoothWindow+1) {
      ProcessDiagUpRight(0, j, width, height, waveSmoothWindow, minAmp);
    }
  }
  for (i=0;i<width;i++) {
    if (LesserOf(width-i, height) > 2*waveSmoothWindow+1) {
      ProcessDiagUpRight(i, height-1, width, height, waveSmoothWindow, minAmp);
    }
  }
  ImageGetsTemp(width, height);
  PruneImage(width, height, 1, 3, Refresh);

/*  DilateImage(width, height, 1, 1, NoRefresh);
  DilateImage(width, height, 2, 1, NoRefresh);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);*/
  TempGetsBuffer(width, height);
  Overlay(width, height);
  BufferGetsImage(width, height);
  for (j=0;j<height;j++) {
  	for (i=0;i<width;i++) {
  		image[i][j] = temp[i][j];
  		tempImage[i][j] = 0;
  	}
  }
  for (j=0;j<height;j++) {
    if (LesserOf(height-j, width) > 2*waveSmoothWindow+1) {
      ProcessDiagDownRight(0, j, width, height, waveSmoothWindow, minAmp);
    }
  }
  for (i=0;i<width;i++) {
    if (LesserOf(width-i, height) > 2*waveSmoothWindow+1) {
      ProcessDiagDownRight(i, 0, width, height, waveSmoothWindow, minAmp);
    }
  }
  ImageGetsTemp(width, height);
  PruneImage(width, height, 1, 3, Refresh);


/*  DilateImage(width, height, 1, 1, NoRefresh);
  DilateImage(width, height, 2, 1, NoRefresh);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);*/
  TempGetsBuffer(width, height);
  Overlay(width, height);
  printf("Finished processing diagonals\n");
  WriteProcessorTime();
  EraseBorder(width, height, MaxOffset+5);
  NormalizeImage(width, height);
//  WriteFile(width, height, "edgeImage");
  ThresholdImage(width, height, 64);
//  PruneImage(width, height, 2, 2, Refresh);
//  DilateImage(width, height, 1, 1, NoRefresh);
  DilateImage(width, height, 2, 1, NoRefresh);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 5, Refresh);
//  WriteFile(width, height, "processed");
}

/* Function SkelFilter:
	 Translated from NIH Image source code (Pascal) written by Wayne Rasband. 
	 Used in the Skeletonize function below. */

int SkelFilter(int width, int height, int pass, int *table) 
{
	int i, j, code, index=0, pixelsRemoved=0, value=0;
	
	for (j=1;j<height-1;j++) {
		for (i=1;i<width-1;i++) {
			value = image[i][j];
			if (value == 255) {
				index = 0;
				if (image[i-1][j-1] == 255) {
					index += 1;
				}
				if (image[i][j-1] == 255) {
					index += 2;
				}
				if (image[i+1][j-1] == 255) {
					index += 4;
				}
				if (image[i+1][j] == 255) {
					index += 8;
				}
				if (image[i+1][j+1] == 255) {
					index += 16;
				}
				if (image[i][j+1] == 255) {
					index += 32;
				}
				if (image[i-1][j+1] == 255) {
					index += 64;
				}
				if (image[i-1][j] == 255) {
					index += 128;
				}
				code = table[index];
				if ((pass % 2) != 0) {
					if ((code == 2) || (code == 3)) {
							value = 0;
							pixelsRemoved++;
					}
				} else {
					if ((code == 1) || (code == 3)) {
						value = 0;
						pixelsRemoved++;
					}
				}
			}
			tempImage[i][j] = value;
		}
	}
	ImageGetsTemp(width, height);
	return (pixelsRemoved);
}

/*Function: Skeletonize
  Taken from NIH Image source code by Wayne Rasband, Nat'l Institutes of Health.
  Requires a binary image as input, reduces black features to lines or points of
  single pixel width. */

void Skeletonize(int width, int height)
{
	int fateTable[256], i, pass=0, pixelsRemoved=1;
	const char s000[65] = "0001001300311013000000002020303300000000300000002000000020003022";
	const char s064[65] = "0000000000000000000000000000000020000000200020003000000030003020";
	const char s128[65] = "0131001300000001000000000000000131000000000000002000000000000000";
	const char s192[65] = "2313001300000001000000000000000023010001000000003301000022002000";
	
	for (i=0;i<64;i++) {
		fateTable[i] = s000[i] - '0';
		fateTable[i+64] = s064[i] - '0';
		fateTable[i+128] = s128[i] - '0';
		fateTable[i+192] = s192[i] - '0';
	}

	while (pixelsRemoved != 0) {
		pixelsRemoved = 0;
		pixelsRemoved += SkelFilter(width, height, pass, fateTable);
		pass++;
		pixelsRemoved += SkelFilter(width, height, pass, fateTable);
		pass++;
	}
}

/*Function: DrawCircle
  Draws a circle of radius at position x, y in the tempImage array with proper
  error checking for the edges of the image. 
  
  NOTE: called in ConnectEndPoints, a simple reconnection scheme */

void DrawCircle(int x, int y, int radius, int width, int height)
{
	int i=0, j=0;
	
	for (j=y-radius;j<=y+radius;j++) {
      	  for (i=x-radius;i<=x+radius;i++) {
	    if ((i-x)*(i-x) + (j-y)*(j-y) <= radius*radius) {
	      if ((i<0) || (i>=width) || (j<0) || (j>=height)) {
		break;
	      } else {
	       	tempImage[i][j] = 255;
	      }
	    }
	  }
	}
}

/*Function: ConnectEndPoints
  Simple connection scheme using circles at endpoints to induce connectivity.
  Currently unused, but could be reintroduced. Note that this approach is not
  without its problems, such as the loss of short branches from a triple
  junction if the length of the branch is less than the radius of the circle.*/

void ConnectEndPoints(int width, int height, int radius)
{
	int i=0, j=0;
	
	printf("Connecting fragments using circles of radius %d\n", radius);
	TempGetsImage(width, height);
	for (j=MaxOffset;j<height-MaxOffset;j++) {
		for (i=MaxOffset;i<width-MaxOffset;i++) {
			if (CountNeighbors(i, j) == 1) {
				DrawCircle(i, j, radius, width, height);
			}
		}
	}
	ImageGetsTemp(width, height);
	Skeletonize(width, height);
}

/*Function: FillWhitePerimeter
  Checks the perimeter of a square centered at (x, y) and if there are no black pixels
  along that perimeter, the square is filled white in the tempImage array. Used to
  remove clusters from the image, which the segment filter might miss.  */

void FillWhitePerimeter(int x, int y, int halfWidth)
{
	int i, j;
	
	i = x - halfWidth;
	j = y - halfWidth;
	while (i <= x + halfWidth) {
		if (image[i][j] == 255) {
			return;
		}
		i++;
	}
	while (j <= y + halfWidth) {
		if (image[i][j] == 255) {
			return;
		}
 		j++;
	}
	while (i > x - halfWidth) {
		if (image[i][j] == 255) {
			return;
		}
 		i--;
	}
	while (j > y - halfWidth) {
		if (image[i][j] == 255) {
			return;
		}
 		j--;
	}
	for (j=y-halfWidth;j<=y+halfWidth;j++) {
		for (i=x-halfWidth;i<=x+halfWidth;i++) {
			tempImage[i][j] = 0;
		}
	}
}


// Function: ImageGetsTemp
// Writes the contents of the tempImage array into the image array. Used during
// many processing steps to store or manipulate copies of the image.

void ImageGetsTemp(int width, int height)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      image[i][j] = tempImage[i][j];
    }
  }
}



// Function: TempGetsImage
// Writes the contents of the image array into the tempImage array. Used during
// many processing steps to store or manipulate copies of the image.

void TempGetsImage(int width, int height)
{
  int i, j;
	
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {   
      tempImage[i][j] = image[i][j];
    }
  }
}



// Function: BufferGetsTemp
// Writes the contents of the tempImage array into the buffer array. Used during
// many processing steps to store or manipulate copies of the image.

void BufferGetsTemp(int width, int height)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      buffer[i][j] = tempImage[i][j];
    }
  }
}



// Function: TempGetsBuffer
// Writes the contents of the buffer array into the tempImage array. Used during
// many processing steps to store or manipulate copies of the image.

void TempGetsBuffer(int width, int height)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      tempImage[i][j] = buffer[i][j];
    }
  }
}



// Function: BufferGetsImage
// Writes the contents of the image array into the buffer array. Used during
// many processing steps to store or manipulate copies of the image.

void BufferGetsImage(int width, int height)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      buffer[i][j] = image[i][j];
    }
  }
}




// Function: ImageGetsBuffer
// Writes the contents of the buffer array into the image array. Used during
// many processing steps to store or manipulate copies of the image.

void ImageGetsBuffer(int width, int height)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      image[i][j] = buffer[i][j];
    }
  }
}





/*Function: EraseClusters
  Searches the image and erases any feature which fits entirely inside
  a box whose side measures edgeLength pixels. Calls FillWhitePerimeter
  defined above at each point in the image. */

void EraseClusters(int width, int height, int edgeLength)
{
	int i, j, halfWidth=(edgeLength-1)/2;
	
	printf("Erasing clusters of width %d\n", edgeLength);
	TempGetsImage(width, height);
	for (j=MaxOffset+halfWidth;j<height-MaxOffset-halfWidth;j++) {
		for (i=MaxOffset+halfWidth;i<width-MaxOffset-halfWidth;i++) {
		  FillWhitePerimeter(i, j, halfWidth);
		}
	}
	ImageGetsTemp(width, height);
}


// Function: CountBlackPixelsInBox
// Returns the number of black pixels inside a box of edge length 2*halfWidth+1.
// I don't think this is currently used (can't remember why I wrote it!)

int CountBlackPixelsInBox(int x, int y, int halfWidth)
{
	int i, j, count=0;
	
	for (j=y-halfWidth;j<=y+halfWidth;j++) {
		for (i=x-halfWidth;i<=x+halfWidth;i++) {
			if (image[i][j] == 255) {
				count++;
			}
		}
	}
	return(count);
}


// Function: EraseBoxFromTemp
// Erases the box with edge length 2*halfWidth+1 centered at (x, y) from the tempImage
// array. Again, not currently used...I think I was trying some sort of feature
// removal algorithm, but discarded it.

void EraseBoxFromTemp(int x, int y, int halfWidth)
{
	int i, j;
	
	for (j=y-halfWidth;j<=y+halfWidth;j++) {
		for (i=x-halfWidth;i<=x+halfWidth;i++) {
			tempImage[i][j] = 0;
		}
	}
}


// Function: CountBlackPixelsInPerimeter
// Returns the number of black pixels found in the perimeter of a box centered at
// (x, y) with edge length 2*halfWidth+1. Again, not used, but I think it was
// intended for some type of feature removal algorithm.

int CountBlackPixelsInPerimeter(int x, int y, int halfWidth)
{	
	int i, j, count=0;
	
	i = x - halfWidth;
	j = y - halfWidth;
	while (i <= x + halfWidth) {
		if (image[i][j] == 255) {
			count++;
		}
		i++;
	}
	while (j <= y + halfWidth) {
		if (image[i][j] == 255) {
			count++;
		}
		j++;
	}
	while (i > x - halfWidth) {
		if (image[i][j] == 255) {
			count++;
		}
		i--;
	}
	while (j > y - halfWidth) {
		if (image[i][j] == 255) {
			count++;
		}
		j--;
	}
return(count);
}
	

// Function: CloseSmallGrains
// Simple-minded function that closes small grains by multiple dilations followed
// by skeletonization. The size parameter is an estimate of the diameter of the
// grains to be closed. It doesn't really close grains any better than a simple
// series of dilations, so it was not used. 

void CloseSmallGrains(int width, int height, int size)
{
   int i;

   DilateImage(width, height, 5, 2*size, Refresh);
   for (i=0;i<size/2;i++) {
     DilateImage(width, height, 4, 1, NoRefresh);
     DilateImage(width, height, 3, 1, NoRefresh);
   }
   Skeletonize(width, height);
}


// Function: EraseBorder
// Erases the border of the image, whiting out a region determined by borderSize.

void EraseBorder(int width, int height, int borderSize)
{
	int i, j;
	
	for (i=0;i<width;i++) {	
		for (j=0;j<borderSize;j++) {
			image[i][j] = 0;
			image[i][height-1-j] = 0;
		}
	}
	for (j=0;j<height;j++) {
		for (i=0;i<borderSize;i++) {
			image[i][j] = 0;
			image[width-1-i][j] = 0;
		}
	}
}


/*Function: SmallestDistance
  Returns the smallest distance between a point (x,y) and any black pixel along the
  perimeter of a square centered at (x,y) with edge length 2*halfWidth+1. Will be
  used in a situation where there is known to be at least one black pixel in the
  perimeter, so it will always give a non-zero value, and will find all black pixels
  before returning the shortest distance.*/

float SmallestDistance(int x, int y, int halfWidth)
{
	int i, j;
	double distance=sqrt((double)(2*halfWidth*halfWidth)), temp=0;
	
	i = x - halfWidth;
	j = y - halfWidth;
	while (i <= x + halfWidth) {
		if (image[i][j] == 255) {
			temp = sqrt(pow(i-x, 2) + pow(j-y, 2));
			distance = (distance < temp) ? distance: temp;
		}
		i++;
	}
	while (j <= y + halfWidth) {
		if (image[i][j] == 255) {
			temp = sqrt(pow(i-x, 2) + pow(j-y, 2));
			distance = (distance < temp) ? distance: temp;
		}
		j++;
	}
	while (i >= x - halfWidth) {
		if (image[i][j] == 255) {
			temp = sqrt(pow(i-x, 2) + pow(j-y, 2));
			distance = (distance < temp) ? distance: temp;
		}
		i--;
	}
	while (j > y - halfWidth) {
		if (image[i][j] == 255) {
			temp = sqrt(pow(i-x, 2) + pow(j-y, 2));
			distance = (distance < temp) ? distance: temp;
		}
		j--;
	}
	return((float)distance);
}
	

/*Function: FindNearestNeighbor
  Finds the distance from a point (x,y) in the tempImage array to the nearest
  black point in the image array, within a box of edge length 2*limit+1. */

float FindNearestNeighbor(int x, int y, int limit)
{
	int i=0, count=0;
	
	if (image[x][y] == 255) {
		return(0);
	}
	while ((count == 0) && (i <= limit)) {
		i++;
		count = CountBlackPixelsInPerimeter(x, y, i);
	}
	if (i > limit) {
		return(-1);
	} else {
		return(SmallestDistance(x, y, i));
	}
}





/*Function: CalculateChiSquared
  Calculates a figure of merit for the goodness of fit between image and tempImage,
  examining a square neighborhood with edge length 2*halfWidth+1 around each
  pixel. Chi squared is the sum of the squares of the distances between a black pixel in
  tempImage and the nearest black pixel in image. The values of halfWidth, xOffset and
  yOffset must all be less than MaxOffset which is #defined previously.
  */

float CalculateChiSquared(int width, int height, int halfWidth, int xOffset, int yOffset)
{
	int i, j, numBlackPixels=0, numNonNeg=0;
	float chiSqr=0, dist;
	
	for (j=MaxOffset;j<height-MaxOffset;j++) {
		for (i=MaxOffset;i<width-MaxOffset;i++) {
			if (tempImage[i+xOffset][j+yOffset] == 255) {
				numBlackPixels++;
				dist = FindNearestNeighbor(i, j, halfWidth);
				if (dist > -1) {
					chiSqr += (dist*dist);
      				numNonNeg++;
				}
			}
		}
	}
	if (numNonNeg == 0) {
		return(1000);
	} else {
		return((float)(sqrt((double)(chiSqr/numNonNeg))));
	}
}

/*Function MinimizeChiSquared: calculates chi squared over a matrix of image
  offsets and finds the minimum value, then shifts the two images into
  coincidence and overlays them. Each image is shifted by half the total
  magnitude necessary to keep the data centered as much as possible. Make
  sure that halfWidth is greater than or equal to offset...is the maximum
  translation of one image with respect to the other along the x or y axis 
  (in pixels), while halfWidth describes the size of the neighborhood over
  which ChiSquared is calculated. 
  
  NOTE: this is clearly a brute-force solution and is very time-intensive.
  A better method was developed (SmartMinChiSqr, below) which first uses
  a coarse matrix then refines the size of it steps to find the best fit.
  This version was kept as a backup, but is not used.
  
  NOTE2: the chi-squared minimization is most sensitive when using
  skeletonized images, though it will still work for images with thick
  features. */

void MinimizeChiSquared(int width, int height, int halfWidth, int offset)
{
	int i, j, x, y, count=0;
	float chiSquared=0, min;
	
	for (j=-offset;j<=offset;j++) {
		for (i=-offset;i<=offset;i++) {
			if (count == 0) {
				chiSquared = CalculateChiSquared(width, height, halfWidth, i, j);
				printf("%d, %d, %f\n", i, j, chiSquared);
				min = chiSquared;
				x=i;
				y=j;
				count++;
			} else {
				chiSquared = CalculateChiSquared(width, height, halfWidth, i, j);
				printf("%d, %d, %f\n", i, j, chiSquared);
				if (chiSquared < min) {
					min = chiSquared;
					x=i;
					y=j;
				}
				count++;
			}
		}
	}
	printf("Minimum chi squared is %f at x offset = %d and y offset = %d\n", min, x, y);
	ShiftTempImage(width, height, x/2 + x%2, y/2 + y%2);
	ShiftImage(width, height, -x/2, -y/2);
	Overlay(width, height);
}

// Function: ShiftTempImage
// Shifts the data in tempImage by xOffset in the x direction, yOffset in the
// y direction. The offsets may be positive or negative. 

void ShiftTempImage(int width, int height, int xOffset, int yOffset)
{
  int i, j;

  for (j=(int)fabs((double)yOffset);j<height-(int)fabs((double)yOffset);j++) {
    for (i=(int)fabs((double)xOffset);i<width-(int)fabs((double)xOffset);i++) {
      temp[i][j] = tempImage[i+xOffset][j+yOffset];
    }
  }
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      tempImage[i][j] = 0;
    }
  }
  for (j=(int)fabs((double)yOffset);j<height-(int)fabs((double)yOffset);j++) {
    for (i=(int)fabs((double)xOffset);i<width-(int)fabs((double)xOffset);i++) {
      tempImage[i][j] = temp[i][j];
    }
  }
}


// Function: ShiftImage
// Shifts the data in the image array by xOffset in the x direction, yOffset 
// in the y direction. The offsets may be positive or negative. 

void ShiftImage(int width, int height, int xOffset, int yOffset)
{
  int i, j;

  for (j=(int)fabs((double)yOffset);j<height-(int)fabs((double)yOffset);j++) {
    for (i=(int)fabs((double)xOffset);i<width-(int)fabs((double)xOffset);i++) {
      temp[i][j] = image[i+xOffset][j+yOffset];
    }
  }
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      image[i][j] = 0;
    }
  }
  for (j=(int)fabs((double)yOffset);j<height-(int)fabs((double)yOffset);j++) {
    for (i=(int)fabs((double)xOffset);i<width-(int)fabs((double)xOffset);i++) {
      image[i][j] = temp[i][j];
    }
  }
}


// Function: ErodeImage
// Inverse of the dilation function; if a pixel has too few black neighbors
// (determined by maxNeighbors) then the pixel is erased.
//
// NOTE: this is a slow method if multiple passes are required. A faster
// implementation is PruneImage, below, which is very similar to DilateImage
// and uses the neighbors array to keep track of the numbers of neighbors
// rather than counting them each time. 

void ErodeImage(int width, int height, int maxNeighbors)
{
  int i, j;

  TempGetsImage(width, height);
  for (j=1;j<height-1;j++) {
    for (i=1;i<width-1;i++) {
      if (CountNeighbors(i,j) <= maxNeighbors) {
                tempImage[i][j] = 0;
      }
    }
  }
  ImageGetsTemp(width, height);
}


// Function: FindSmallerChiSqr
// Looks at eight different offsets and returns the minimum chi squared value.
// Also updates the values of x and y (outside the function) to reflect the
// offsets associated with that value of chi squared.

float FindSmallerChiSqr(int width, int height, int halfWidth, int *x, int *y, float chiSqr)
{
	int i=*x, j=*y, which=0;
	float up, down, left, right, upright, upleft, downright, downleft, min;
	
	up = CalculateChiSquared(width, height, halfWidth, i, j+1);
	down = CalculateChiSquared(width, height, halfWidth, i, j-1);
	left = CalculateChiSquared(width, height, halfWidth, i-1, j);
	right = CalculateChiSquared(width, height, halfWidth, i+1, j);
	upright = CalculateChiSquared(width, height, halfWidth, i+1, j+1);
	upleft = CalculateChiSquared(width, height, halfWidth, i-1, j+1);
	downright = CalculateChiSquared(width, height, halfWidth, i+1, j-1);
	downleft = CalculateChiSquared(width, height, halfWidth, i-1, j-1);
	if (up < down) {
		min = up;
		which = 1;
	} else {
		min = down;
		which = 2;
	}
	if (left < min) { 
		min = left;
		which = 3;
	}
	if (right < min) {
		min = right;
		which = 4;
	}
	if (upright < min) {
		min = upright;
		which = 5;
	}
	if (upleft < min) {
		min = upleft;
		which = 6;
	}
	if (downleft < min) {
		min = downleft;
		which = 7;
	}
	if (downright < min) {
		min = downright;
		which = 8;
	}
	if (chiSqr < min) {
		return(-1);
	}
	switch (which) {
		case 1: (*y)++;
				break;
		case 2: (*y)--;
				break;
		case 3: (*x)--;
				break;
		case 4: (*x)++;
				break;
		case 5: (*x)++;
				(*y)++;
				break;
		case 6: (*x)--;
				(*y)++;
				break;
		case 7: (*x)--;
				(*y)--;
				break;
		case 8: (*x)++;
				(*y)--;
				break;
	}
	return(min);
}


/*Function: SmartMinChiSqr
  Finds the minimum chi squared in a slightly more intelligent fashion than
  MinimizeChiSquared. First calculates chi squared at about 50 points on a grid
  over the total offset area, finding the position with the smallest value.
  Chi squared is calculated for each of the neighboring positions and the smallest
  one is chosen to be the next step. Continues until a position is found where all
  8 neighbors have greater chi squared. The images are shifted by the proper amount
  and overlaid. */

void SmartMinChiSqr(int width, int height, int halfWidth, int offset)
{
	int i, j, x=0, y=0, count=0;
	float chiSquared, min=1000, temp;
	
	printf("Minimizing chi squared...\n");
	for (j=-7*offset/10;j<=7*offset/10;j+=offset/10) {
		for (i=-7*offset/10;i<=7*offset/10;i+=offset/10) {
			chiSquared = CalculateChiSquared(width, height, halfWidth, i, j);
			if (chiSquared < min) {
				x=i;
				y=j;
				min = chiSquared;
			}
		}
	}
	chiSquared = min;
	/*	chiSquared = CalculateChiSquared(width, height, halfWidth, x, y);*/
	printf("Start: %d, %d:\t%f\n", x, y, chiSquared);
	min = FindSmallerChiSqr(width, height, halfWidth, &x, &y, chiSquared);
	while ((min != -1) && (fabs((double)x) < offset) && (fabs((double)y) < offset)) {
		chiSquared = min;
		count++;
		printf("%d, %d:\t%f (iteration %d)\n", x, y, chiSquared, count);
		min = FindSmallerChiSqr(width, height, halfWidth, &x, &y, chiSquared);
		if (min == -1) {
			min = chiSquared;
			for (j=y-2;j<=y+2;j++) {
				for (i=x-2;i<=x+2;i++) {
					if ((fabs((double)i) <= offset) && (fabs((double)j) <= offset)) {
						temp = CalculateChiSquared(width, height, halfWidth, i, j);
						if (temp < min) {
							x=i;
							y=j;
							min = temp;
						}
					}
				}
			}
			if (min == chiSquared) {
				min = -1;
			}
		}			
	}
	printf("Minimum chi squared is %f at x offset = %d and y offset = %d\n", chiSquared, x, y);
	printf("%d iterations were required\n", count);
	ShiftTempImage(width, height, x/2 + x%2, y/2 + y%2);
	ShiftImage(width, height, -x/2, -y/2);
	Overlay(width, height);
}


// Function: PruneImage
// The inverse of DilateImage. Erases pixels if they have too few black neighbors
// (as specified by maxNeighbors). Again, uses the neighbors array to keep track
// of how many black neighbors each pixel has to speed things up when many iterations
// are required. Repeats the erosion as required by iterations, if refresh==0 it
// assumes the values in the neighbors array are correct, otherwise it calculates
// the proper values during the first iteration.

void PruneImage(int width, int height, int maxNeighbors, int iterations, int refresh)
{
	int i, j, k, m, n, value;

	if (refresh == 1) {
	  for (j=MaxOffset;j<height-MaxOffset;j++) {
	    for (i=MaxOffset;i<width-MaxOffset;i++) {
	      tempImage[i][j] = CountNeighbors(i, j);
	    }
	  }
	}
	for (k=0;k<iterations;k++) {
	  for (j=MaxOffset;j<height-MaxOffset;j++) {
      for (i=MaxOffset;i<width-MaxOffset;i++) {
	      neighbors[i][j] = tempImage[i][j];
	    }
	  }
    for (j=MaxOffset;j<height-MaxOffset;j++) {
	    for (i=MaxOffset;i<width-MaxOffset;i++) {
	      if ((image[i][j] != 0) && (neighbors[i][j] <= maxNeighbors)) {
					image[i][j] = 0;
						for (n=j-1;n<=j+1;n++) {
		  				for (m=i-1;m<=i+1;m++) {
		    				value = tempImage[m][n];
		    				if ((m!=i) || (n!=j)) {
		      				tempImage[m][n]--;
		   					}
		  				}
						}
	     		}
	    	}
	  	}
		}
  }

// Function: GetCleanSkelEdges
// Workhorse function in the original version of this code, though it is
// no longer used. Originally this was the function that called all the others
// in order to clean up the image, find the edges, consolidate, etc. It has
// been replaced by other functions.

void GetCleanSkelEdges(char *infile, char *outfile, int width, int height)
{
  MaximizeContrast(width, height, infile);
  AddBorder(width, height, MaxOffset);
  width = width+2*MaxOffset;
  height = height+2*MaxOffset;
  printf("Read file, enhance contrast, and add border: ");
  WriteProcessorTime();
  ProcessImage(width, height, smoothWindow, ampCutoff);
  printf("Smooth and find edges: ");
  WriteProcessorTime();
  FilterImage(width, height, (int)(minDisplacement/3));
  FilterImage(width, height, (int)(minDisplacement/3));
  InvertAndFilter(width, height, none);
  PruneImage(width, height, 1, ErodeLength, Refresh);
  RemoveVertices(width, height, 3);
  FilterImage(width, height, (int)(minDisplacement/3));
  FilterImage(width, height, (int)(minDisplacement/3));
  DilateImage(width, height, 3, 2, Refresh);
  Skeletonize(width, height);
  FilterImage(width, height, (int)(minDisplacement*2));
  FilterImage(width, height, (int)(minDisplacement*2));
  printf("Filter short segments: ");
  WriteProcessorTime();
  DilateImage(width, height, 1, 1, Refresh);
  EraseClusters(width, height, Cluster1);
  EraseClusters(width, height, Cluster2);
  Skeletonize(width, height);
  printf("Erase clusters: ");
  WriteProcessorTime();
  WriteFile(width, height, outfile);
 }



// Function: StackImages
// Reads image1 and image2 from files, stacks them with minimum chi-squared
// and writes the result into a new file named outfile. Note that input files
// should be binary images consisting of skeletonized features for the best
// results.

void StackImages(char *image1, char *image2, char *outfile, int width, int height)
{
  int i;

  ReadBinaryFile(width, height, image1);
  EraseBorder(width, height, MaxOffset+5);
  TempGetsImage(width, height);
  ReadBinaryFile(width, height, image2);
  EraseBorder(width, height, MaxOffset+5);
  SmartMinChiSqr(width, height, ChiSqrWindow, MaxOffset);
//  InvertAndFilter(width, height, none);
//  PruneImage(width, height, 1, ErodeLength, Refresh);
  WriteFile(width, height, outfile);
}



// Function: InvertImage
// Returns the inverse grayscale image, i.e., each pixel is transformed from
// initial value p to final value 255-p.

void InvertImage(int width, int height)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      image[i][j] = 255 - image[i][j];
    }
  }
}



// Function: AddBorder
// Adds a white border of width borderSize into the image array. Used to add some
// white space around the "traced" images so that data won't be lost when two
// images are translated and overlaid. Note that when the border is added, the
// values of width and height need to be updated external to this function.

void AddBorder(int width, int height, int borderSize)
{
  int i, j;

  for (j=0;j<height+2*borderSize;j++) {
    for (i=0;i<width+2*borderSize;i++) {
      tempImage[i][j] = 0;
    }
  }
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      tempImage[i+borderSize][j+borderSize] = image[i][j];
    }
  }
  ImageGetsTemp(width=width+2*borderSize, height=height+2*borderSize);
}



// Function: FillRegion
// Recursive flood-fill which fills any closed region (bounded by non-zero values)
// with the specified value. This is used to fill grains after the tracing is
// complete so that they can be identified.
//
// NOTE: this algorithm is a brute-force approach and is problematic when filling
// large regions. It will take up _HUGE_ amounts of space since a new copy of the
// function executes for each pixel found in the region...not recommended for
// general use. There is a non-recursive function included below which has
// replaced this one and has no such constraints on region size.

void FillRegion(int width, int height, int x, int y, int value)
{
  int i=x, j=y;

  if (image[x][y] != 0) {
    return;
  }
  image[x][y] = value;
  if ((x+1 < width) && (image[x+1][y] == 0)) {
    FillRegion(width, height, x+1, y, value);
  }
    if ((y+1 < height) && (image[x][y+1] == 0)) {
    FillRegion(width, height, x, y+1, value);
  }
  if ((x-1 >= 0) && (image[x-1][y] == 0)) {
    FillRegion(width, height, x-1, y, value);
  }
  if ((y-1 >= 0) && (image[x][y-1] == 0)) {
    FillRegion(width, height, x, y-1, value);
  }
/*  if ((x+1 < width) && (y+1 < height) && (image[x+1][y+1] == 0)) {
    FillRegion(width, height, x+1, y+1, value);
  }
  if ((x+1 < width) && (y-1 >= 0) && (image[x+1][y-1] == 0)) {
    FillRegion(width, height, x+1, y-1, value);
  }
  if ((x-1 >= 0) && (y+1 < height) && (image[x-1][y+1] == 0)) {
    FillRegion(width, height, x-1, y+1, value);
  }
  if ((x-1 >= 0) && (y-1 >= 0) && (image[x-1][y-1] == 0)) {
    FillRegion(width, height, x-1, y-1, value);
  }*/
}    
    

// Function: FloodFill
// A non-recursive area filling function based on pseudo-code posted on the
// comp.graphics.algorithms newsgroup. Only limitation is the size of the
// array called stack, here defined to be 10*maxDim by 2. This should be
// more than enough, but could always be increased if needed. This algorithm
// is much more efficient than the recursive method, both in terms of memory
// demands and speed. Functionality is identical.

// NOTE: The function in the grain sizing algorithms is still the recursive
// one, but this one can easily be substituted if desired.


int FloodFill(int width, int height, int x, int y, int value, int stack[10*maxDim][2])
{
	int i, j, m, n, left, right, pos=0, count=1, isNewSegment=1;
	
	if (image[x][y] != 0) return(-1);
	stack[pos][0] = x;
	stack[pos][1] = y;
	
	while (pos>=0 && pos<10*maxDim) {
		m = i = stack[pos][0];
		n = j = stack[pos][1];
		count--;
		pos--;
		while (i>=0 && i<width && image[i][j]==0) image[i++][j] = value;
		right = i;
		i = m-1;
		while (i>=0 && i<width && image[i][j]==0) image[i--][j] = value;
		left = i;
		
		j++;
		if (j<height) {
			isNewSegment = 1;
			for (i=left;i<=right;i++) {
				if (i>=0 && i<width && isNewSegment && 
				    image[i][j]==0 && image[i][j-1]==value) {
					isNewSegment = 0;
		   		   	stack[++pos][0] = i;
		 	  	   	stack[pos][1] = j;
		   		   	count++;
				}
       				if (i>=0 && i<width && image[i][j] == 255) isNewSegment=1;
			}
		}
		
		j-=2;
		if (j>=0) {
			isNewSegment = 1;
			for (i=left;i<=right;i++) {
				if (i>=0 && i<width && isNewSegment &&
				    image[i][j]==0 && image[i][j+1]==value) {
					isNewSegment = 0;
		   	   		stack[++pos][0] = i;
		 	  	   	stack[pos][1] = j;
		   		   	count++;
			   	 }
			   	 if (i>=0 && i<width && image[i][j] == 255) isNewSegment=1;
			}
		}
	}
	if (pos >= 10*maxDim) {
		printf("Stack overflow in FillRegion. Exiting...\n");
		exit(0);
	}
	return(1);
}




// Function: MaximizeContrast
// Reads data from infile and does a black-level expansion to maximize the
// contrast in the image. That is, the smallest value is subtracted from all
// pixels (bringing the value of the smallest pixel down to zero), then the
// image is scaled to bring the maximum value up to 255. This spreads the
// pixel values apart as much as possible without saturating at either
// end. However, this can introduce problems if there are only a few different
// pixel values.

void MaximizeContrast(int width, int height, char *infile)
{
  int i, j, min=255, max=0, value;

  ReadBinaryFile(width, height, infile);
  for (j=100;j<height-100;j++) {
    for (i=100;i<width-100;i++) {
      value = image[i][j];
      min = (value < min) ? value : min;
      max = (value > max) ? value : max;
    }
  }
  if ((min > 0) || (max < 255)) {
    for (j=0;j<height;j++) {
      for (i=0;i<width;i++) {
        tempImage[i][j] = (int)((image[i][j] - min)*255/(max - min));
	if (value=tempImage[i][j]<0) tempImage[i][j] =0;
	if (value>255) tempImage[i][j] =255;
      }
    }
  }
  ImageGetsTemp(width, height);
}



// Function: GetGrainSizes
// Reads the data from infile and blacks out any "bad" grains (regions touching
// the edge of the image or containing dangling endpoints). Writes a file named
// "grains" containing this image, then fills each remaining grain with a unique
// value. Grain areas are determined in terms of the number of pixels for each
// of the grains identified, and are stored in the array called grains. The
// results are written as a list of integers to outfile.

void GetGrainSizes(int width, int height, char *infile, char *outfile)
{
  int i, j, currentColor=256, numGrains=0;

  printf("Getting grain size from %s\n", infile);
  ReadBinaryFile(width, height, infile);
  FillBadGrains(width, height);
  WriteFile(width, height, "grains");
  printf("Finished filling bad grains\n");
  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      if (image[i][j] == 0) {
	FillRegion(width, height, i, j, currentColor);
	currentColor++;
	numGrains++;
      }
    }
  }
  printf("Measured %d grains\n", numGrains);
  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      if (image[i][j] != 255) {
	currentColor = image[i][j];
	grains[currentColor-256]++;
      }
    }
  }
  WriteGrainSizes(numGrains, outfile);
}



// Function: FindWhiteNeighbor
// Integers x and y are updated to the coordinates of a white pixel which is
// a neighbor to the original (x,y) position.

void FindWhiteNeighbor(int *x, int *y)
{
  int i, j;

  for (j=*y-1;j<=*y+1;j++) {
    for (i=*x-1;i<=*x+1;i++) {
      if ((i == *x) && (j == *y)) {
	break;
      }
      if (image[i][j] == 0) {
	*x = i;
	*y = j;
	return;
      }
    }
  }
}


// Function: FillBadGrains
// Colors the outer perimeter of the image black, then finds all the dangling endpoints
// in the image. Any region containing an endpoint (defined as a black pixel with only
// one black neighbor) is also colored black. 

void FillBadGrains(int width, int height)
{
  int i, j, m, n;

  FillRegion(width, height, 1, 1, 255);
  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      if ((image[i][j] == 255) && (CountNeighbors(i, j) == 1)) {
	m=i;
	n=j;
	FindWhiteNeighbor(&m, &n);
	FillRegion(width, height, m, n, 255);
      }
    }
  }
}


// Function: WriteGrainSizes
// Writes the data file based on the contents of the array called grains

void WriteGrainSizes(int numGrains, char filename[])
{
  int i=0;
  FILE *outFile;

  outFile = fopen(filename, "w");
  for (i=0;i<numGrains;i++) {
    fprintf(outFile, "%d\n", grains[i]);
  }
  fclose(outFile);
}


// Function: NormalizeImage
// Normalizes the values in the image array so that the range from 0-90% of the
// intensity scale cover the full range, and the top 10% of pixel intensities
// are saturated (set to 255). This reduces the total spread of the pixel
// values and may be helpful in certain circumstances.

// NOTE: This function is only called by functions which are not in use, so
// it is not actually used in the processing. Might turn out to be useful,
// might not.

void NormalizeImage(int width, int height)
{
  int i, j, max=0, min=10000, max90=1, value, count=1;
  float fraction=0, *array;

  array = (float *)(malloc(width*height*sizeof(float)));
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      value = image[i][j];
      max = (value > max) ? value : max;
      min = ((value != 0) && (value < min)) ? value : min;
      if (value != 0) {
	array[count++] = (float)value;
      }
    }
  }
  QuickSort(array, count);
  max90 = (int)(array[(int)(count*0.9)]);
  printf("max= %d, min= %d, 90 percent max= %d\n", max, min, (int)(max90));
  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      if (image[i][j] > 0) {
	value = (int)((image[i][j]-min)*((float)255/max90));
        if (value > 255) {
          image[i][j] = 255;
        } else {
          image[i][j] = value;
	}
      }
    }
  }
  free(array);
}



// Function: ThresholdImage
// Changes all pixels whose values exceed the threshold value to 255,
// all others to 0. Used to transform a grayscale image to a binary image.
// In our case, thresholding is always done after some sort of edge-finding.

void ThresholdImage(int width, int height, int threshold)
{
  int i, j;

  for (j=0;j<height;j++) {
    for (i=0;i<width;i++) {
      if (image[i][j] >= threshold) {
	image[i][j] = 255;
      } else {
	image[i][j] = 0;
      }
    }
  }
}



// Function: GetBoxMedian
// Calculates the median values of four different lines of pixels through the
// center of a box which is centered at (x,y) and is edgeLength wide. A median
// value is then calculated from these four medians and the value of the center
// pixel, and this median value is returned. This function is used to do some
// specialized image smoothing without the loss of edge sharpness that happens
// with Gaussian smoothing kernels, etc. Basic algorithm comes from Russ' Image
// Processing Handbook; proper reference for this "hybrid median" filter is
// listed there. 

int GetBoxMedian(int x, int y, int edgeLength)
{
  int i, j, halfWidth=(edgeLength-1)/2, *vert, *horiz, *diag1, *diag2, *final, median;

  vert = (int *)(malloc(edgeLength*sizeof(int)));
  horiz = (int *)(malloc(edgeLength*sizeof(int)));
  diag1 = (int *)(malloc(edgeLength*sizeof(int)));
  diag2 = (int *)(malloc(edgeLength*sizeof(int)));
  final = (int *)(malloc(5*sizeof(int)));

  final[0] = image[x][y];
  for (i=0;i<edgeLength;i++) {
    vert[i] = image[x][y-halfWidth+i];
    horiz[i] = image[x-halfWidth+i][y];
    diag1[i] = image[x-halfWidth+i][y-halfWidth+i];
    diag2[i] = image[x-halfWidth+i][y+halfWidth-i];
  }
  final[1] = Median(vert, edgeLength);
  final[2] = Median(horiz, edgeLength);
  final[3] = Median(diag1, edgeLength);
  final[4] = Median(diag2, edgeLength);
  median = Median(final, 5);
  free(vert);
  free(horiz);
  free(diag1);
  free(diag2);
  free(final);
  return(median);
}



// Function: Median
// Returns the median of a list of numbers.

int Median(int *list, int length)
{
  int result;

  SortList(list, length);
  result = list[(length-1)/2];
  return(result);
}


// Function: SortList
// Simple insertion sort, intended for short lists only...maybe good for <50
// elements, but not more since it's O(N^2) and therefore horribly inefficient.
// Longer lists should use the QuickSort algorithm below.

void SortList(int *list, int length)
{
  int i, j, *temp, value, inserted, result;

  temp = (int *)(malloc(length*sizeof(int)));
  temp[0] = list[0];
  for (i=1;i<length;i++) {
    value = list[i];
    inserted = 0;
    for (j=0;j<i;j++) {
      if (value < temp[j]) {
        InsertElement(temp, length, j, value);
        inserted = 1;
        break;
      }
    }
    if (!inserted) {
      temp[i] = value;
    }
  }
  for (i=0;i<length;i++) {
    list[i] = temp[i];
  }
  free(temp);
}

// Function: InsertElement
// Inserts an element into the list (used by the SortList function above).

void InsertElement(int *list, int length, int position, int element)
{
  int i, currVal, lastVal;

  currVal = element;
  for (i=position;i<length;i++) {
    lastVal = list[i];
    list[i] = currVal;
    currVal = lastVal;
  }
}


// Function: CreateMedianImage
// Calculates the hybrid median at each point using GetBoxMedian and achieves
// smoothing of the image. Should be used before edge finding, etc.

void CreateMedianImage(int width, int height, int edgeLength)
{
  int i, j, halfWidth=(edgeLength-1)/2;

  printf("Creating %dx%d median image\n", edgeLength, edgeLength);
  TempGetsImage(width, height);
  for (j=halfWidth;j<height-halfWidth;j++) {
    for (i=halfWidth;i<width-halfWidth;i++) {
      tempImage[i][j] = GetBoxMedian(i, j, edgeLength);
    }
  }
  ImageGetsTemp(width, height);
}




/* Function QuickSort: uses the quicksort algorithm (Numerical Recipes in C,
p. 251) to sort an array. Note that elements should be in positions 1 to
n, not 0 to n-1. 

NOTE: there is in fact a built-in C function called qsort which does
exactly this. Too bad I didn't look for it before copying all this code
out of Numerical Recipes!! */

void QuickSort(float arr[], int n)
{

  int l=1, jstack=0, j, ir, iq, i;
  int istack[NSTACK+1];
  long fx=0L;
  float a;

  ir=n;
  for (;;) {
    if (ir-l < M) {
      for (j=l+1;j<=ir;j++) {
        a=arr[j];
        for (i=j-1;arr[i]>a && i>0;i--) arr[i+1] = arr[i];
        arr[i+1] = a;
      }
      if (jstack == 0) return;
      ir = istack[jstack--];
      l = istack[jstack--];
    } else {
      i=l;
      j=ir;
      fx=(fx*FA+FC) % FM;
      iq=l+((ir-l+1)*fx)/FM;
      a=arr[iq];
      arr[iq]=arr[l];
      for (;;) {
        while (j > 0 && a < arr[j]) j--;
        if (j <= i) {
          arr[i]=a;
          break;
        }
        arr[i++] = arr[j];
        while (a > arr[i] && i<=n) i++;
        if (j <= i) {
          arr[(i=j)] = a;
          break;
        }
        arr[j--]=arr[i];
      }
      if (ir-i >= i-l) {
        istack[++jstack]=i+1;
        istack[++jstack]=ir;
        ir=i-1;
      } else {
        istack[++jstack]=l;
        istack[++jstack]=i-1;
        l=i+1;
      }
      if (jstack > NSTACK) {
	printf("NSTACK too small in QuickSort");
	exit(0);
      }
    }
  }
}


// Function: RemoveVertices
// Erases all of the pixels in the image which have too few black neighbors,
// specified by minNeighbors. For instance, triple points (in a skeletonized
// image) can be removed if minNeighbors is set to 3. This can be used as a
// precursor for more complex operations.

void RemoveVertices(int width, int height, int minNeighbors)
{
  int i, j;

  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      tempImage[i][j] = CountNeighbors(i, j);
    }
  }
  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      if (image[i][j] != 0 && tempImage[i][j] >= minNeighbors) {
	image[i][j] = 0;
      }
    }
  }
}


// Function: CreateRangeImage
// Calculates the mathematical range within a square described by edgeLength
// centered on each pixel in the image. The end result is that each pixel in
// the image is replaced by the range calculated at that pixel. Edges will have
// large ranges since there is an abrupt change in the intensities of the nearby
// pixels. In contrast, areas of uniform intensity will have a range of near
// zero. Note, however, that the range operation is not very sensitive unless
// the neighborhood is quite small, and in this case it results in some amount
// of noise.

void CreateRangeImage(int width, int height, int edgeLength)
{
  int i, j, m, n, min, max, value, halfWidth=(edgeLength-1)/2;;

  printf("Creating a %dx%d range image\n", edgeLength, edgeLength);
  for (j=MaxOffset+1;j<height-MaxOffset-1;j++) {
    for (i=MaxOffset+1;i<width-MaxOffset-1;i++) {
      min = 256;
      max = -1;
      for (n=j-halfWidth;n<=j+halfWidth;n++) {
	for (m=i-halfWidth;m<=i+halfWidth;m++) {
	  if (n!=j && m!=i) {
	    value = image[m][n];
	    min = (min<value) ? min : value;
	    max = (max>value) ? max : value;
	  }
	}
      }
      value = max-min;
      tempImage[i][j] = (value < 255) ? value : 255;
    }
  }
  ImageGetsTemp(width, height);
}



// Function: Convolve
// Convolves the image with a kernel described by edgeLength which is stored in
// the array called kernel. Calculations are done with double-precision numbers
// and scaled back to fit into the 0 - 255 range at the very end. 

void Convolve(int width, int height, int edgeLength)
{
  int i, j, m, n, halfWidth=(edgeLength-1)/2;
  double total, max=-1, min=100000, value, sum=0;

  printf("Performing convolution...\n");
  for (j=0;j<edgeLength;j++) {
    for (i=0;i<edgeLength;i++) {
      sum += kernel[i][j];
    }
  }
  for (j=MaxOffset+halfWidth;j<height-MaxOffset-halfWidth;j++) {
    for (i=MaxOffset+halfWidth;i<width-MaxOffset-halfWidth;i++) {
      total = 0;
      for (n=j-halfWidth;n<=j+halfWidth;n++) {
	for (m=i-halfWidth;m<=i+halfWidth;m++) {
	  total+=(image[m][n])*(kernel[m-i+halfWidth][n-j+halfWidth]);
	}
      }
      if (sum != 0) {
	convolution[i][j] = total/sum;
	max = (max > total/sum) ? max : total/sum;
	min = (min < total/sum) ? min : total/sum;
      } else {
	convolution[i][j] = total;
	max = (max > total) ? max : total;
	min = (min < total) ? min : total;
      }
    }
  }
  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      value = convolution[i][j];
      image[i][j] = (int)((value - min)*255/(max-min));
    }
  }
}


// Function: ReadKernel
// Reads filename, a text file which contains the values of the desired kernel,
// and stores it in the kernel array. The size of the kernel must be specified
// when this function is called and should agree (obviously) with the size of
// the kernel in the file. This function must be called before Convolve or the
// kernel will be uninitialized.

void ReadKernel(char *filename, int edgeLength)
{
  int i=0, j=0, c;
  FILE *inputFile;

  inputFile = fopen(filename, "r");
  if (inputFile == NULL) {
    printf("Could not open file %s\n", filename);
    exit(0);
  }
  for (j=0;j<edgeLength;j++) {
    for (i=0;i<edgeLength;i++) {
      fscanf(inputFile, "%d", &(kernel[i][j]));
    }
    while ((c = getc(inputFile)) != '\n') {
    }
  }
  fclose(inputFile);
  printf("Kernel %s has been successfully read\n", filename);
}


// Function: NegateKernel
// Replaces every value in kernel with its inverse.

void NegateKernel(int edgeLength)
{
  int i, j, value;

  for (j=0;j<edgeLength;j++) {
    for (i=0;i<edgeLength;i++) {
      value = kernel[i][j];
      kernel[i][j] = -value;
    }
  }
}



// Function: KirschFilter
// Applies the Kirsch edge-finding filter to the image. The spatial derivatives
// (centered at each pixel) are calculated in all four possible directions and
// the largest magnitude is kept. The end result is an image which shows large
// values where there are fast changes in intensity. Note, however, that the
// derivatives are all calculated over just 3 pixels and are therefore subject
// to some noise effects. 

void KirschFilter(int width, int height)
{
  int edgeLength=3, i, j, up, down, left, right,
      upLeft, upRight, dnLeft, dnRight, max;

  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      max=0;
      up = fabs((double)(image[i][j+1] - image[i][j-1]));
      right = fabs((double)(image[i+1][j] - image[i-1][j]));
      upRight = fabs((double)(image[i+1][j+1] - image[i-1][j+1]));
      dnRight = fabs((double)(image[i+1][j-1] - image[i-1][j-1]));
      max = (up > right) ? up : right;
      max = (max > upRight) ? max : upRight;
      max = (max > dnRight) ? max : dnRight;
      tempImage[i][j] = max;
    }
  }
  ImageGetsTemp(width, height);
  printf("finished Kirsch image\n");
}



// Function: AbsoluteImage
// Replaces any negative pixels with their inverse values.

// NOTE: I can't remember why I wrote this...I don't think it is used.

void AbsoluteImage(int width, int height)
{
  int i, j, value;

  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      if ((value = image[i][j]) < 0) {
	image[i][j] = -value;
      }
    }
  }
}
      


// Function: AutomaticThreshold
// Determines a reasonable threshold for a grayscale image based upon the
// distribution of image intensities. Calculates an image histogram (number of
// pixels as a function of intensity) and assumes that the function is near-
// continuous after smoothing. Finds the inflection point after the
// peak position and uses this point as the threshold value, binarizing
// the image. Also requires that the derivative of the curve is not
// changing excessively at the threshold point.

// NOTE: this gives repeatable results, but may result in the loss of a large
// fraction of the data...especially if the peak in the histogram is very
// sharp.

void AutomaticThreshold(int width, int height)
{
  int i, j, hist[260], value, max=0, maxPos=0, length=260, *firstD, *secondD, threshold;

  for (i=0;i<260;i++) hist[i]=0;

  for (j=MaxOffset;j<height-MaxOffset;j++) {
    for (i=MaxOffset;i<width-MaxOffset;i++) {
      value = image[i][j];
      hist[value+2]++;
    }
  }

  SmoothWave(hist, length, 5);
  firstD = FirstDerivative(hist, length);
  secondD = SecondDerivative(hist, length);
  for (i=3;i<257;i++) {
    if ((value=hist[i]) > max) {
      max = value;
      maxPos = i;
    }
  }
  if (maxPos == i) {
    printf("Automatic thresholding has failed.\n");
    exit(0);
  }
  while (secondD[maxPos++] <=0) {
    if (maxPos == i) {
      printf("Automatic thresholding has failed.\n");
      exit(0);
    }
  }
  while (firstD[maxPos] < 1.7*firstD[++maxPos]) {
//  while (hist[maxPos] > 1.25*hist[++maxPos]) {
    if (maxPos == i) {
      printf("Automatic thresholding has failed.\n");
      exit(0);
    }
  }
  threshold = maxPos-2;
  printf("Thresholding image at a value of %d\n", threshold);
  ThresholdImage(width, height, threshold);
  free(firstD);
  free(secondD);
}



// Function: AddRangeImage
// Reads in the file called "median" which is created previously with
// CreateMedianImage...this file should be well smoothed and suitable for
// edge detection. A range image is calculated and an output file can be
// written if desired. The image is thresholded, skeletonized, pruned, and
// consolidated. Vertices are removed and short segments are removed using
// FilterImage, then the surviving edges are stacked onto the contents of
// the buffer array and stored there.

void AddRangeImage(int width, int height, char *outfile)
{
  ReadBinaryFile(width, height, "median");
  CreateRangeImage(width, height, 3);
  if (strcmp(outfile, none) != 0) {
    WriteFile(width, height, outfile);
  }
  AutomaticThreshold(width, height);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);
  InvertAndFilter(width, height, none);
  RemoveVertices(width, height, 3);
  FilterImage(width, height, minDisplacement/2);
  FilterImage(width, height, minDisplacement/2);
//  ConnectEndPoints(width, height, minDisplacement+1);
//  InvertAndFilter(width, height, none);
  printf("finished range image\n");
  WriteProcessorTime();
  TempGetsBuffer(width, height);
  Overlay(width, height);
  BufferGetsImage(width, height);
}


// Function: AddKirschImage
// Reads in the file called "median" which is created previously with
// CreateMedianImage...this file should be well smoothed and suitable for
// edge detection. A Kirsch image is calculated and an output file can be
// written if desired. The image is thresholded, skeletonized, pruned, and
// consolidated. Vertices are removed and short segments are removed using
// FilterImage, then the surviving edges are stacked onto the contents of
// the buffer array and stored there.

// NOTE: thresholding is hardcoded to 10 as this is physically justifiable.

void AddKirschImage(int width, int height, char *outfile)
{
  ReadBinaryFile(width, height, "median");
  KirschFilter(width, height);
  if (strcmp(outfile, none) != 0) {
    WriteFile(width, height, outfile);
  }
  /*  AutomaticThreshold(width, height);*/
  printf("Thresholding image at a value of 10\n");
  ThresholdImage(width, height, 10);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);
  InvertAndFilter(width, height, none);
  RemoveVertices(width, height, 3);
  FilterImage(width, height, minDisplacement/2);
  FilterImage(width, height, minDisplacement/2);
//  ConnectEndPoints(width, height, minDisplacement+1);
//  InvertAndFilter(width, height, none);
  printf("finished Kirsch image\n");
  WriteProcessorTime();
  TempGetsBuffer(width, height);
  Overlay(width, height);
  BufferGetsImage(width, height);
}



// Function: AddMexHatImage
// Reads in the file called "median" which is created previously with
// CreateMedianImage...this file should be well smoothed and suitable for
// edge detection. The image is convolved with a "mexican hat" filter and
// an output file can be written if desired. The image is thresholded,
// skeletonized, pruned, and consolidated. Vertices are removed and short
// segments are removed using FilterImage, then the surviving edges are
// stacked onto the contents of the buffer array and stored there.

void AddMexHatImage(int width, int height, char *outfile)
{
  int i, j;

  ReadBinaryFile(width, height, "median");
  ReadKernel("mexHat13", 13);
  Convolve(width, height, 13);
  InvertImage(width, height);
  EraseBorder(width, height, MaxOffset+1);
  if (strcmp(outfile, none) != 0) {
    WriteFile(width, height, outfile);
  }
  AutomaticThreshold(width, height);
  Skeletonize(width, height);
  PruneImage(width, height, 1, 3, Refresh);
  InvertAndFilter(width, height, none);
  RemoveVertices(width, height, 3);
  FilterImage(width, height, minDisplacement/2);
  FilterImage(width, height, minDisplacement/2);
//  ConnectEndPoints(width, height, minDisplacement+1);
//  InvertAndFilter(width, height, none);
  printf("finished mexican hat image\n");
  WriteProcessorTime();
  TempGetsBuffer(width, height);
  Overlay(width, height);
  BufferGetsImage(width, height);
}


// Function: ZeroImage
// Initializes any integer array (image, tempImage, temp, buffer, neighbors)
// to zero. This is done before any image processing is performed.

void ZeroImage(int array[maxDim][maxDim])
{
  int i, j;
  
  for (j=0;j<maxDim;j++) {
    for (i=0;i<maxDim;i++) {
      array[i][j] = 0;
    }
  }
}



// Function: ZeroConvolution
// Initializes the convolution array...done before any image processing.

void ZeroConvolution(double array[maxDim][maxDim])
{
  int i, j;

  for (j=0;j<maxDim;j++) {
    for (i=0;i<maxDim;i++) {
      array[i][j] = 0;
    }
  }
}

