static char RCS_Id[] = "$Id: banner.c,v 1.1 2001/08/30 21:46:44 pronovic Exp $";
/*
 **********************************************************************

 BANNER - Main program file for 'banner' program

 **********************************************************************

 $GNU_Copyright-Start$

 Copyright (C) 2000 Kenneth J. Pronovici

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 $GNU_Copyright-End$

 Modification History

 * $Log: banner.c,v $
 * Revision 1.1  2001/08/30 21:46:44  pronovic
 * Initial revision
 *
 * Revision 1.2  2000/06/12 03:30:48  pronovic
 * Cleanup before final check-in.
 *
 * Revision 1.1  2000/06/12 02:57:08  pronovic
 * Initial revision
 *

 **********************************************************************
 */


/*******
  Notes
 *******/

/*
   I've used a lot of different UN*X systems, and all of them, as 
   far as I can tell, seem to provide a "banner" program that prints
   some short string in large letters.  Except Linux.  I can't seem
   to find something like that anywhere (well, there is /usr/games/banner,
   but that's different than a typical banner implementation).  So, since 
   it's such a simplistic thing to do, I wrote it myself.

   The 'banner' program prints a "banner" on the screen that corresponds 
   to the first X characters of a string entered on the command line. 

   Say we're printing "AB"... we make one call per character
   and build an array of strings from the top down.  The first time
   through, we'll have this:

      banner[0] = "   #     "
      banner[1] = "  # #    "
      banner[2] = " #   #   "
      banner[3] = "#     #  "
      banner[4] = "#######  "
      banner[5] = "#     #  "
      banner[6] = "#     #  "
 
   the second time through, we'll have this:
      
      banner[0] = "   #     ######   "
      banner[1] = "  # #    #     #  "
      banner[2] = " #   #   #     #  "
      banner[3] = "#     #  ######   "
      banner[4] = "#######  #     #  "
      banner[5] = "#     #  #     #  "
      banner[6] = "#     #  ######   "

   i.e. the first line of the "B" array is concatenated onto the 
   first line of the "A" array, the second onto the second, etc.
   to make the complete banner, line by line from top to bottom.
   The string is then printed top to bottom.

   It's pretty configurable.  The letter definitions can be changed
   by changing the array definitions in "letters.h".  The defined
   letter height, the number of spaces after each letter, the max
   letters of a given string that will be printed, etc. are all
   controlled from in there.  The only real restriction is that all
   of the letters have to be the same height.  Note, however, that 
   there is no facility for printing lower-case letters - as far as
   I can remember, Solaris and AIX don't have that, and I didn't feel
   like putting it in.  Adding it would be easy, if you want to come up
   with letter definitions for the lower-case letters.
 
   I could have made the add_to_banner() function prettier if I wanted
   to just key off of ASCII value rather than having hardcoded names
   for each letter/character.  I decided that keying off ASCII values
   would not be flexible enough.  If you want to optimize it, feel 
   free, and then write me and let me know.

   KJP 06/11/2000
*/


/****************
  Included files
 ****************/

#include "banner.h"


/**************
  Main routine
 **************/

int main(int argc, char *argv[])
{

   /*****************
     Local variables
    *****************/

   char *string = NULL;
   char *banner[BANNER_LETTER_HEIGHT];
   long i = 0;
   long working_length = 0;


   /******************
     Handle arguments
    ******************/
   /* 
      There are several possibilities for arguments.  First, they could use
      the normal case - just a string.  Second, they could use the "help" case,
      which is "--help".  Third, they might want to banner-print "--help", in
      which case argument 1 would be "--" and argument 2 would be "--help".

      Yeah, I could probably have a standard routine process this for me,
      but this was simpler for the moment (so much for adhering to 
      standards...).
   */

   if(argc < 2)
   {
      usage(argv[0]);
      exit(BANNER_ERROR_EXIT);
   }
   if(argc == 2)
   {

      if(strcmp(argv[1], BANNER_ARG_HELP_STR) == 0)
      {
         usage(argv[0]);
         exit(BANNER_NORMAL_EXIT);   /* not an error */
      }

      string = (char *)calloc(1, strlen(argv[1])+1);
      strcpy(string, argv[1]);

   }
   else if (argc == 3)
   {
      if  (strcmp(argv[1], BANNER_ARG_IGNORE_STR) == 0 
        && strcmp(argv[2], BANNER_ARG_HELP_STR) == 0)
      {
         string = (char *)calloc(1, strlen(argv[2])+1);
         strcpy(string, argv[2]);
      }
      else
      {
         usage(argv[0]);
         exit(BANNER_ERROR_EXIT);
      }
   }
   else if(argc > 3)
   {
      usage(argv[0]);
      exit(BANNER_ERROR_EXIT);
   }


   /*******
     Setup
    *******/

   convert_to_upper(string);
   init_banner(banner, BANNER_LETTER_HEIGHT);


   /*************************************
     Load each character into the banner
    *************************************/

   for(i=0; i<strlen(string) && i<BANNER_MAX_PRINT_LETTERS; i++)
   {
      add_to_banner(banner, 
                    BANNER_LETTER_HEIGHT, BANNER_SPACE_WIDTH, 
                    string[i]);
   }


   /*************************************
     And, print the banner out to STDOUT
    *************************************/

   print_banner(banner, BANNER_LETTER_HEIGHT);

}


/*************************
  print_banner subroutine
 *************************/
/* Prints banner buffer to STDOUT. */

void print_banner(char **banner, long height)
{
   int i = 0;
 
   for(i=0; i<height; i++)
   {
      printf("%s\n", banner[i]);
   }
}


/************************
  init_banner subroutine
 ************************/
/* Does initial memory allocation and clears banner buffer. */

void init_banner(char **banner, long height)
{
   int i = 0;
 
   for(i=0; i<height; i++)
   {
      banner[i] = (char *)calloc(1, BANNER_DEF_SIZE*sizeof(char));
   }
}


/******************************
  convert_to_upper subroutine
 ******************************/
/* Destructively converts a string to all UPPER-CASE letters. */

void convert_to_upper(char *string)
{
   long i = 0;
   
   for(i=0; i<strlen(string); i++)
   {
      string[i] = toupper(string[i]);
   }
}


/******************
  usage subroutine
 ******************/

void usage(char *program)
{
   printf("Usage:\t%s \"string to print... (max %d characters)\" [--help]\n"
          "\nThis $Revision: 1.1 $ from $Date: 2001/08/30 21:46:44 $.\n"
          "Copyright (c) %s %s <%s>.\n"
          "Distributed under the GNU General Public License.\n"
          "See %s for details on the GNU GPL.\n",
           program, BANNER_MAX_PRINT_LETTERS,
           BANNER_COPYRIGHT_DATE, BANNER_AUTHOR, 
           BANNER_AUTHOR_EMAIL, BANNER_GNU_URL);
   return;
}


/**************************
  add_to_banner subroutine
 **************************/
/* This tacks together the pieces into a complete banner.  */

void add_to_banner(char **banner, long height, long space, char letter)
{

   long i = 0;
   long j = 0;
   char **working_char = NULL;
   long new_length = 0;

   switch(letter)
   {
   case 'A':
      working_char = char_a;
      break;
   case 'B':
      working_char = char_b;
      break;
   case 'C':
      working_char = char_c;
      break;
   case 'D':
      working_char = char_d;
      break;
   case 'E':
      working_char = char_e;
      break;
   case 'F':
      working_char = char_f;
      break;
   case 'G':
      working_char = char_g;
      break;
   case 'H':
      working_char = char_h;
      break;
   case 'I':
      working_char = char_i;
      break;
   case 'J':
      working_char = char_j;
      break;
   case 'K':
      working_char = char_k;
      break;
   case 'L':
      working_char = char_l;
      break;
   case 'M':
      working_char = char_m;
      break;
   case 'N':
      working_char = char_n;
      break;
   case 'O':
      working_char = char_o;
      break;
   case 'P':
      working_char = char_p;
      break;
   case 'Q':
      working_char = char_q;
      break;
   case 'R':
      working_char = char_r;
      break;
   case 'S':
      working_char = char_s;
      break;
   case 'T':
      working_char = char_t;
      break;
   case 'U':
      working_char = char_u;
      break;
   case 'V':
      working_char = char_v;
      break;
   case 'W':
      working_char = char_w;
      break;
   case 'X':
      working_char = char_x;
      break;
   case 'Y':
      working_char = char_y;
      break;
   case 'Z':
      working_char = char_z;
      break;
   case '0':
      working_char = char_0;
      break;
   case '1':
      working_char = char_1;
      break;
   case '2':
      working_char = char_2;
      break;
   case '3':
      working_char = char_3;
      break;
   case '4':
      working_char = char_4;
      break;
   case '5':
      working_char = char_5;
      break;
   case '6':
      working_char = char_6;
      break;
   case '7':
      working_char = char_7;
      break;
   case '8':
      working_char = char_8;
      break;
   case '9':
      working_char = char_9;
      break;
   case '#':
      working_char = char_hash;
      break;
   case '~':
      working_char = char_tild;
      break;
   case '!':
      working_char = char_excl;
      break;
   case '@':
      working_char = char_atsg;
      break;
   case '$':
      working_char = char_dolr;
      break;
   case '%':
      working_char = char_prct;
      break;
   case '^':
      working_char = char_crrt;
      break;
   case '&':
      working_char = char_ampr;
      break;
   case '*':
      working_char = char_star;
      break;
   case ')':
      working_char = char_rpar;
      break;
   case '(':
      working_char = char_lpar;
      break;
   case '_':
      working_char = char_undr;
      break;
   case '+':
      working_char = char_plus;
      break;
   case '=':
      working_char = char_equl;
      break;
   case '}':
      working_char = char_rbrc;
      break;
   case '{':
      working_char = char_lbrc;
      break;
   case ']':
      working_char = char_rbrk;
      break;
   case '[':
      working_char = char_lbrk;
      break;
   case '|':
      working_char = char_pipe;
      break;
   case '\\':
      working_char = char_lsls;
      break;
   case ':':
      working_char = char_coln;
      break;
   case ';':
      working_char = char_scln;
      break;
   case '\"':
      working_char = char_quot;
      break;
   case '\'':
      working_char = char_squt;
      break;
   case '`':
      working_char = char_btck;
      break;
   case '>':
      working_char = char_rarw;
      break;
   case '<':
      working_char = char_larw;
      break;
   case '.':
      working_char = char_perd;
      break;
   case ',':
      working_char = char_coma;
      break;
   case '?':
      working_char = char_ques;
      break;
   case '/':
      working_char = char_rsls;
      break;
   case '-':
      working_char = char_dash;
      break;
   default:
      /* If we don't know about it, we'll just ignore it. */
      return;
   }

   for(i=0; i<height; i++)
   {
      new_length = strlen(banner[i])*sizeof(char) +         /* current banner */
                   strlen(working_char[i])*sizeof(char) +   /* new character  */
                   space*sizeof(char) +                     /* the spaces     */
                   1;                                       /* \0 character   */

      banner[i] = (char *)realloc(banner[i], new_length);
      strcat(banner[i], working_char[i]);

      for(j=0; j<space; j++)
      {
         strcat(banner[i], BANNER_SPACE_STR);
      }
   }
}

