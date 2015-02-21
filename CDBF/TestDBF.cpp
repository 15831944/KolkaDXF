/****************************************************************************/
/*******    TestDBF.cpp : sample application for the CDBFile object  ********/
/****************************************************************************/
/*
  
    Copyright (C) 1997 Hervé GOURMELON, ENSSAT
    gourmelon@enssat.fr
    http://www.enssat.fr

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program (COPYING.TXT); if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "cdbfile.h" 


// A simple function that displays the raw contents of the data, truncating
// the length of the displayed records to 75 characters. An index Ind is passed
// as a parameter, which indicates from which record the list should be
// displayed.
void  Display(CDBFile *Dbase, unsigned long Ind)
{
	int i; 
	char Page[23][80];

	printf("*o:OpenFile; r/R:load one/all records; e:edit; d:del; s:sort; x:exit; i:index *\n");
	Dbase->GetAtRecord(Ind);
	for (i=0; i<23; i++)
	{	
		Dbase->DumpCurrentContents(75, Page[i]);
		printf("%d\t%s\n", Dbase->GetRecordNum(), Page[i]);
		Dbase->GetNextRecord();
	}
}


void main()
{
CDBFile Base;
unsigned long Index=1;


char c=' ';

printf("TestDBF version 1.0a, Copyright (C) 1997  Herve GOURMELON \n");
printf("TestDBF comes with ABSOLUTELY NO WARRANTY; it is licensed \n");
printf("under the GNU Public License.  This is free software, and \n");
printf("you are welcome to  redistribute  it under certain condi- \n");
printf("tions; see the 'COPYING' file for details, or the website \n");
printf("http://www.fsf.org. \n");
printf("Strike SPACE to continue...\n");
c=(char)getch();



while ((c!='x')||(c!='q'))
{
	switch(c)
	{
	case 'o':	// Opening the file
	{
	 	char Path[256];
		printf("Type in the complete path to the file :");
		scanf("%s", Path);
		if (Base.OpenFile(Path)==FALSE) exit(1);
		else Display(&Base,Index);
		break;
	}
	case 'r':	// Reading one record from the file; loading it into memory
	if (Base.IsOpen()==FALSE) break;
	else
	{
		unsigned long RecNum;
		printf("Type the number of the record within the file : ");
		scanf("%d", &RecNum);
		Base.LoadRecord(RecNum);
		Display(&Base, Index);
	}
	break;
	case 'R':	// Loading the whole file into the memory
	if (Base.IsOpen()==FALSE) break;
	else
	{
		unsigned long RecNum;
		RecNum=Base.LoadFileToMemory();
		Display(&Base, Index);
	}
	break;
	case 'e': 	// Editing a record
	{
		unsigned short i, Fields;
		unsigned long RecNum;
		printf("Type the number of the record within the file : ");
		scanf("%d", &RecNum);
		Base.GetAtRecord(RecNum);
		Fields=Base.GetFieldCount();
		for (i=1; i<=Fields; i++)
		{
			switch(Base.GetFieldType(i))
			{
			case 'N':
				if (Base.GetFieldDecCount(i)==0)
				{
					long Res1;
					printf("Enter the new value (int) for field %d : ", i);
					scanf("%ld", &Res1);
					Base.SetFieldValue(i, (void*)&Res1);
					break;
				}
				else
				{
					double Res1;
					printf("Enter the new value (float) for field %d : ", i);
					scanf("%lf", &Res1);
					Base.SetFieldValue(i, (void*)&Res1);
					break;
				}
			case 'C':
			default :
				char Res2[256];
				printf("Enter the new value (string, 255 char max.) : ");
				scanf("%s", Res2);
				Base.SetFieldValue(i, (void*)&Res2);
				break;
			}
		}
		Display(&Base, RecNum);
		break;
	}
	case 'd' :	// Delete a record
	{
		unsigned long RecNum;
		printf("Type the number of the record within the file : ");
		scanf("%d", &RecNum);
		Base.GetAtRecord(RecNum);
		Base.DeleteCurrentRec();
		Display(&Base, RecNum);
		break;
	}
	case 's' :	// Sort all records
	{
		unsigned short Crit1;
		printf("Type the number of the criterium field : ");
		scanf("%d", &Crit1);

		Base.SortOn(Crit1/*, Crit2*/);
		Display(&Base, Index);
		break;
	}
	case 'i' :	 // Displaying from record #index
		printf("Enter the display index : ");
		scanf("%lu", &Index);
		Display(&Base, Index);
		break;
	case 'w' :	 // write modified records (only if FullFileInMemory==FALSE)
		Base.WriteModified();
		break;
	case 'W' :	 // save the entire file as... (if FullFileInMemory==TRUE)
	{
	 	char Path[256];
		printf("Type in the complete path to the file :");
		scanf("%s", Path);
		Base.WriteAllToFile(Path);
		break;
	}

	case ' ' :
		Display(&Base, Index);
		break;
	case 'x' : exit(0);
	}// end switch
	c=(char)getch();
}//end while

}
			
			
		


