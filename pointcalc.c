/**
 * @brief Sums up the points for all found FKP worksheets in 
 * the folder where the program is executed
 * @author(s) hope15, ...
 * @date 2016-12-01 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>

/* === Macros === */
#define BUFFER_SIZE (50)
#define MAX_FNAME (50)

/* === Global variables === */

/* Name of the program */
const char *pname;

/* The line for that we are searching in the worksheets*/
const char *point_line = "Punkte gesamt: ";

/* === Prototypes === */

/**
 * @brief Exits the program with the given exit code
 * and prints an error message
 * @param exitcode The exitcode with that the program should exit
 * @param fmt format string, error_message
 */
static void error_exit (int exitcode, const char *fmt, ...);

/**
 * @brief Tries to convert a string to an integer 
 * @param input The string that should be converted
 * @returns An integer if the conversion was successful,
 * if an error occurs a error message is printed 
 * and the program exits with EXIT_FAILURE
 */
static int string_to_int (char *input);

/**
 * @brief Extracts the points of a worksheet 
 * @detail To work correctly the file must contain
 * the sub string that is specified in 'point_line' 
 * and the points must follow right after it
 * @returns The points read from the worksheet
 */
static int get_points (char *filename);


static void
error_exit (int exitcode, const char *fmt, ...)
{
  va_list ap;
  (void) fprintf (stderr, "%s: ", pname);
  if (fmt != NULL)
    {
      va_start (ap, fmt);
      (void) vfprintf (stderr, fmt, ap);
      va_end (ap);
    }
  if (errno != 0)
    {
      (void) fprintf (stderr, ": %s", strerror (errno));
    }
  (void) fprintf (stderr, "\n");

  exit (exitcode);
}

static int
string_to_int (char *input)
{
  errno = 0;
  int value = -1;
  char *endptr;
  value = strtol (input, &endptr, 10);

  if ((errno == ERANGE
       && (value == LONG_MAX || value == LONG_MIN))
      || (errno != 0 && value == 0))
    {
      error_exit (EXIT_FAILURE,
		  "Error while trying to convert number with strtol");

    }
  if (endptr == input)
    {
      error_exit (EXIT_FAILURE, "No digits were found");
    }

  /* Does not matter
     if (*endptr != '\0')
     {
     error_exit (EXIT_FAILURE, "There were characters behind the number");
     }
   */

  return value;
}


static int
get_points (char *filename)
{
  FILE *f;

  char buffer[BUFFER_SIZE];

  int found_points = -1;

  if ((f = fopen (filename, "r")) == NULL)
    {
      error_exit (EXIT_FAILURE, "Error opening file %s", filename);
    }

  while (fgets (buffer, BUFFER_SIZE, f) != NULL)
    {
      if (strstr (buffer, point_line) != NULL)
	{
	  //We expect that the number is right after the string
	  found_points = string_to_int (buffer + strlen (point_line));
	}
    }

  if (found_points < 0)
    {
      (void) fprintf (stderr,
		      "\nNOTICE: Line containing the points not found in '%s', returning 0",
		      filename);
      found_points = 0;
    }

  // No check here because we're only reading
  (void) fclose (f);

  return found_points;
}


/**
 * @brief Sums up the points form the worksheets and
 * prints the result 
 * @param argc Argument counter
 * @param argv Argument vector
 */
int
main (int argc, char **argv)
{

  if (argc > 0)
    {
      pname = argv[0];
    }

  if (argc != 1)
    {
      (void) fprintf (stderr, "Usage: %s\n", pname);
      return EXIT_FAILURE;
    }

  double points = 0;

  DIR *d;
  struct dirent *dir;

  // open the local folder
  d = opendir (".");

  if (d == NULL)
    {
      error_exit (EXIT_FAILURE, "Error opening dir for reading contents");
    }

  // If we have two versions of the evaluation we have to calculate 
  // the total -> (points for first + points for second) / 2
  int sub_points = 0;

  while ((dir = readdir (d)) != NULL)
    {

      // Check if we can find the files we're searching for
      if (strstr (dir->d_name, "hs_1.out") != NULL)
	{
	  sub_points += get_points (dir->d_name);

	  char s_file[MAX_FNAME];

	  (void) strncpy (s_file, dir->d_name, MAX_FNAME);

	  // Replace 'hs_1.out' with 'hs_2.out'
	  char *endPtr = strstr (s_file, "hs_1.out");
	  *(endPtr + 3) = '2';

	  // Check if the file exists and we can read from it
	  // Not safe (better check directly in fopen()), 
	  // but for our purpose irrelevant
	  if (access (s_file, R_OK) == 0)
	    {
	      sub_points += get_points (s_file);
	      points += sub_points / 2.0;
	    }
	  else
	    {
	      points += sub_points;
	    }

	  sub_points = 0;
	}

    }

  (void) closedir (d);

  (void) fprintf (stdout, "\nTotal points: %lf\n", points);
}
