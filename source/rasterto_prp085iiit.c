/*
 * rasterto_prp085iiit.c -- cups filter for Tysso PRP-085IIIT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (C) 2010 Oleg Kravchenko <oleg@kaa.org.ua>
 */

/*
 * Include necessary headers...
 */

#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

/*
 * Macros...
 */

#define pwrite(s,n) fwrite((s), 1, (n), stdout)


/*
 * Globals...
 */

unsigned char	*Planes[6],		/* Output buffers */
		*CompBuffer,		/* Compression buffer */
		*LineBuffers[2],	/* Line bitmap buffers */
		*LineBuffer,
		*ZeroBuffer;
int		NumPlanes,		/* Number of color planes */
		Feed,			/* Number of lines to skip */
		EjectPage;		/* Eject the page when done? */
int		DotBit,			/* Bit in buffers */
		DotBytes,		/* # bytes in a dot column */
		DotColumns,		/* # columns in 1/60 inch */
		LineCount,		/* # of lines processed */
		EvenOffset,		/* Offset into 'even' buffers */
		OddOffset,		/* Offset into 'odd' buffers */
		Shingling,		/* Shingle output? */
		Canceled;		/* Has the current job been canceled? */

unsigned char	Line = 0, First = 0, Width = 0;


/*
 * Prototypes...
 */

void	Setup(void);
void	StartPage(const ppd_file_t *ppd, const cups_page_header2_t *header);
void	EndPage(const cups_page_header2_t *header);
void	Shutdown(const cups_page_header2_t *header);

void	CancelJob(int sig);
void	CompressData(const unsigned char *line, int length, int plane,
	             int type, int xstep, int ystep);
void	OutputLine(const cups_page_header2_t *header);
void	OutputRows(const cups_page_header2_t *header, int row);


/*
 * 'Setup()' - Prepare the printer for printing.
 */

void
Setup(void)
{
}


/*
 * 'StartPage()' - Start a page of graphics.
 */

void
StartPage(const ppd_file_t         *ppd,	/* I - PPD file */
          const cups_page_header2_t *header)	/* I - Page header */
{
  int	n, t;					/* Numbers */
  int	plane;					/* Looping var */


 /*
  * Send a reset sequence.
  */

  printf("\033@");

   EjectPage = header->Margins[0] || header->Margins[1];
 
//  /*
//   * Set other stuff...
//   */
// 
//   NumPlanes = 1;
// 
//   Feed = 0;				/* No blank lines yet */
// 
//  /*
//   * Allocate memory for a line/row of graphics...
//   */
// 
//   if ((Planes[0] = malloc(header->cupsBytesPerLine)) == NULL)
//   {
//     fputs("ERROR: Unable to allocate memory!\n", stderr);
//     exit(1);
//   }
// 
//   for (plane = 1; plane < NumPlanes; plane ++)
//     Planes[plane] = Planes[0] + plane * header->cupsBytesPerLine / NumPlanes;
// 
//   if (header->cupsCompression || DotBytes)
//   {
//     if ((CompBuffer = calloc(2, header->cupsWidth)) == NULL)
//     {
//       fputs("ERROR: Unable to allocate memory!\n", stderr);
//       exit(1);
//     }
//   }
//   else
//     CompBuffer = NULL;
// 
//   if (DotBytes)
//   {
//     if ((LineBuffers[0] = calloc(DotBytes,
//                                  header->cupsWidth * (Shingling + 1))) == NULL)
//     {
//       fputs("ERROR: Unable to allocate memory!\n", stderr);
//       exit(1);
//     }
// 
//     LineBuffers[1] = LineBuffers[0] + DotBytes * header->cupsWidth;
//     DotBit         = 128;
//     LineCount      = 0;
//     EvenOffset     = 0;
//     OddOffset      = 0;
//   }
}


/*
 * 'EndPage()' - Finish a page of graphics.
 */

void
EndPage(const cups_page_header2_t *header)	/* I - Page header */
{
  int feed, line;
  
//   if (DotBytes && header)
//   {
//    /*
//     * Flush remaining graphics as needed...
//     */
// 
//     if (!Shingling)
//     {
//       if (DotBit < 128 || EvenOffset)
//         OutputRows(header, 0);
//     }
//     else if (OddOffset > EvenOffset)
//     {
//       OutputRows(header, 1);
//       OutputRows(header, 0);
//     }
//     else
//     {
//       OutputRows(header, 0);
//       OutputRows(header, 1);
//     }
//   }

 /*
  * Eject the current page...
  */

  /*if (EjectPage)
    putchar(12);		/* Form feed */

  fflush(stdout);

//  /*
//   * Free memory...
//   */
// 
//   free(Planes[0]);
// 
//   if (CompBuffer)
//     free(CompBuffer);
// 
//   if (DotBytes)
//     free(LineBuffers[0]);
}
 

/*
 * 'Shutdown()' - Shutdown the printer.
 */

void
Shutdown(const cups_page_header2_t *header)
{

    int line, feed = header->cupsHeight / 8;

    while(feed > 0)
    {
      line = feed > 200 ? 200 : feed;

      printf("\x1b\x4a%c", line);

      feed -= line;
    }

  /*
  * Cut the paper
  */

  if (header->CutMedia)
    pwrite("\x1d\x56\x00", 3);

  /*
  * Send a reset sequence.
  */

  printf("\033@");

  fflush(stdout);
}


/*
 * 'CancelJob()' - Cancel the current job...
 */

void
CancelJob(int sig)			/* I - Signal */
{
  (void)sig;

  Canceled = 1;
}

/*
 * 'main()' - Main entry and processing of driver.
 */

int					/* O - Exit status */
main(int  argc,				/* I - Number of command-line arguments */
     char *argv[])			/* I - Command-line arguments */
{
  int			fd;		/* File descriptor */
  cups_raster_t		*ras;		/* Raster stream for printing */
  cups_page_header2_t	header;		/* Page header from file */
  ppd_file_t		*ppd;		/* PPD file */
  int			page;		/* Current page */
  int			y;		/* Current line */
#if defined(HAVE_SIGACTION) && !defined(HAVE_SIGSET)
  struct sigaction action;		/* Actions for POSIX signals */
#endif /* HAVE_SIGACTION && !HAVE_SIGSET */


 /*
  * Make sure status messages are not buffered...
  */

  setbuf(stderr, NULL);

 /*
  * Check command-line...
  */

  if (argc < 6 || argc > 7)
  {
   /*
    * We don't have the correct number of arguments; write an error message
    * and return.
    */

    fprintf(stderr, "Usage: %s job-id user title copies options [file]\n",
            argv[0]);
    return (1);
  }

 /*
  * Open the page stream...
  */

  if (argc == 7)
  {
    if ((fd = open(argv[6], O_RDONLY)) == -1)
    {
      perror("ERROR: Unable to open raster file - ");
      sleep(1);
      return (1);
    }
  }
  else
    fd = 0;

  ras = cupsRasterOpen(fd, CUPS_RASTER_READ);

 /*
  * Register a signal handler to eject the current page if the
  * job is cancelled.
  */

  Canceled = 0;

#ifdef HAVE_SIGSET /* Use System V signals over POSIX to avoid bugs */
  sigset(SIGTERM, CancelJob);
#elif defined(HAVE_SIGACTION)
  memset(&action, 0, sizeof(action));

  sigemptyset(&action.sa_mask);
  action.sa_handler = CancelJob;
  sigaction(SIGTERM, &action, NULL);
#else
  signal(SIGTERM, CancelJob);
#endif /* HAVE_SIGSET */

  /*
   * Initialize the print device...
   */
 
  ppd = ppdOpenFile(getenv("PPD"));


  Setup();
 
  /*
   * Process pages as needed...
   */

   page = 0;

   while (cupsRasterReadHeader2(ras, &header))
   {
      /*
      * Write a status message with the page number and number of copies.
      */

      if (Canceled)
	break;

      page ++;

      fprintf(stderr, "PAGE: %d %d\n", page, header.NumCopies);

      /*
      * Start the page...
      */
 
     StartPage(ppd, &header);
 
      /*
      * Loop for each line on the page...
      */
 
//       #define STRUCT_PRINTF(s) printf(#s " = %d\n", s)
//       puts("test");
//       STRUCT_PRINTF(header.cupsHeight);
//       STRUCT_PRINTF(header.cupsWidth);
//       STRUCT_PRINTF(header.cupsBytesPerLine);
//       STRUCT_PRINTF(header.HWResolution[0]);
//       STRUCT_PRINTF(header.HWResolution[1]);
//       STRUCT_PRINTF(header.AdvanceDistance);
//       STRUCT_PRINTF(header.Collate);
//       STRUCT_PRINTF(header.CutMedia);
//       STRUCT_PRINTF(header.Duplex);
//       STRUCT_PRINTF(header.HWResolution[0]);
//       STRUCT_PRINTF(header.HWResolution[1]);
//       STRUCT_PRINTF(header.ImagingBoundingBox[0]);
//       STRUCT_PRINTF(header.ImagingBoundingBox[1]);
//       STRUCT_PRINTF(header.ImagingBoundingBox[2]);
//       STRUCT_PRINTF(header.ImagingBoundingBox[3]);
//       STRUCT_PRINTF(header.InsertSheet);
//       STRUCT_PRINTF(header.ManualFeed);
//       STRUCT_PRINTF(header.Margins[0]);
//       STRUCT_PRINTF(header.Margins[1]);
//       STRUCT_PRINTF(header.MediaPosition);
//       STRUCT_PRINTF(header.MediaWeight);
//       STRUCT_PRINTF(header.MirrorPrint);
//       STRUCT_PRINTF(header.NegativePrint);
//       STRUCT_PRINTF(header.NumCopies);
//       STRUCT_PRINTF(header.Orientation);
//       STRUCT_PRINTF(header.OutputFaceUp);
//       STRUCT_PRINTF(header.PageSize[0]);
//       STRUCT_PRINTF(header.PageSize[1]);
//       STRUCT_PRINTF(header.Separations);
//       STRUCT_PRINTF(header.TraySwitch);
//       STRUCT_PRINTF(header.Tumble);
//       STRUCT_PRINTF(header.cupsBitsPerColor);
//       STRUCT_PRINTF(header.cupsBitsPerPixel);
//       STRUCT_PRINTF(header.cupsBytesPerLine);
//       STRUCT_PRINTF(header.cupsCompression);
//       STRUCT_PRINTF(header.cupsMediaType);
//       STRUCT_PRINTF(header.cupsNumColors);
//       STRUCT_PRINTF(header.cupsRowCount);
//       STRUCT_PRINTF(header.cupsRowFeed);
//       STRUCT_PRINTF(header.cupsRowStep);


      for (y = 0, Line = 1; y < header.cupsHeight; y ++)
      {
	  /*
	   * Let the user know how far we have progressed...
	   */

	  if (Canceled)
	    break;
 
	  if ((y & 127) == 0)
	    fprintf(stderr, "INFO: Printing page %d, %d%% complete...\n", page,
		    100 * y / header.cupsHeight);

	  /*
	  * Read a line of graphics...
	  */
 
	  if(!(LineBuffer = malloc(header.cupsBytesPerLine)))
	  {
	      fputs("ERROR: Unable to allocate memory!\n", stderr);
	      exit(1);
	  }

	  if(!(ZeroBuffer = malloc(header.cupsBytesPerLine)))
	  {
	      fputs("ERROR: Unable to allocate memory!\n", stderr);
	      exit(1);
	  }
	  memset(ZeroBuffer, 0, header.cupsBytesPerLine);

	  if(cupsRasterReadPixels(ras, LineBuffer, header.cupsBytesPerLine) < 1)
	      break;

	  if(memcmp(ZeroBuffer, LineBuffer, header.cupsBytesPerLine) == 0 && Line == 1)
	    continue;

	  if(Line == 1)
	  {
	    Line = (header.cupsHeight - y > 24) ? 24 : header.cupsHeight - y;
	    Width = (header.cupsBytesPerLine > 72) ? 72 : header.cupsBytesPerLine;

//	    printf("width %d line %d header.cupsBytesPerLine %d", Width, Line, header.cupsBytesPerLine);

//	    printf("\x1D\x76\x30\x00%c\x00%c\x00", Width, Line);
	    pwrite("\x1D\x76\x30\x00", 4);
	    pwrite(&Width, 1);
	    pwrite("\x00", 1);
	    pwrite(&Line, 1);
	    pwrite("\x00", 1);
	  }
	  else
	    -- Line;

	  pwrite(LineBuffer, Width);


	  if(Line == 1)
   	    pwrite("\x1B\x4A\x00", 3);
	  
	  free(LineBuffer);

//      if (cupsRasterReadPixels(ras, Planes[0], header.cupsBytesPerLine) < 1)
//        break;
 
	  /*
	  * Write it to the printer...
	  */
 
//       //OutputLine(&header);
      }
 
    /*
     * Eject the page...
     */
 
     EndPage(&header);
// 
     if (Canceled)
       break;
   }
 
  /*
   * Shutdown the printer...
   */
 
   Shutdown(&header);
 
   ppdClose(ppd);
 
  /*
   * Close the raster stream...
   */
 
   cupsRasterClose(ras);
   if (fd != 0)
     close(fd);
 
  /*
   * If no pages were printed, send an error message...
   */
 
   if (page == 0)
     fputs("ERROR: No pages found!\n", stderr);
   else
     fputs("INFO: Ready to print.\n", stderr);

   return (page == 0);
}