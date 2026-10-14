
#include <vxWorks.h>
//#include <ioLib.h>
#include <dirent.h>
#include <stat.h>
#include <fioLib.h>
#include <stdio.h>
#include <usrLib.h>
#include <string.h>
#include <sysLib.h>
#include <string>
#include <iosLib.h>
#include <nfsDrv.h>


extern "C" STATUS copyDirectoryPath(char *sourceDir, char *destDir, BOOL cleanFirst, BOOL makeDestDir);
STATUS makeComplexDirectory(const char *fullPath);
STATUS createSingleDir(const char *fullPath, char *dirPath);
STATUS copyDirEntries(char * sourceDir, char *destinationDir);
STATUS clearDirectory(const char * sourceDir, BOOL recursive);
STATUS clearSingleDirectory(const char * sourceDir, BOOL recursive);
BOOL findFile(const char * startDir, const char *fileName, BOOL recursive, char *filePath);
BOOL fileMatch(const char *searchFileName, const char *targetFileName);
STATUS getPath(const string& fullPath, string& Path);
STATUS getNFSDirectory(void);
STATUS copy_NFSFile(char *sourceDir, char *destDir);
STATUS copy_NFSDirectory(char *sourceDir, char *destDir, BOOL cleanFirst);

#define MAXPATH 256

char nfsDriveName[3] = "";

STATUS copy_NFSDirectory(char *sourceDir, char *destDir, BOOL cleanFirst)
{
char fullSourcePath[MAXPATH];

	
	if (nfsDriveName[0] == NULL)
	{
		if (getNFSDirectory() == ERROR_CODE)
			return (ERROR_CODE);
	}

	strcpy (fullSourcePath, nfsDriveName);
	strcat (fullSourcePath, sourceDir);
	return (copyDirectoryPath(fullSourcePath, destDir, cleanFirst, TRUE));

}

STATUS copy_NFSFile(char *sourceDir, char *destDir)
{
char fullSourcePath[MAXPATH];

	
	if (nfsDriveName[0] == NULL)
	{
		if (getNFSDirectory() == ERROR_CODE)
			return (ERROR_CODE);
	}

	strcpy (fullSourcePath, nfsDriveName);
	strcat (fullSourcePath, sourceDir);

	return (copy(fullSourcePath, destDir));

}


STATUS getNFSDirectory(void)
{
char device[6] = "/null";
char *pTail;
DEV_HDR * pHdr;
int nfsDrvNum;

	nfsDrvNum = nfsDrvNumGet();

	pHdr = iosDevFind (device, &pTail);

	while ((pHdr != NULL)&&(pHdr->drvNum != nfsDrvNum))
	{
		pHdr = iosNextDevGet(pHdr);

	}
	
	if ((pHdr != NULL)&&(pHdr->drvNum == nfsDrvNum))
	{
		strncpy (nfsDriveName, pHdr->name, 2);
		nfsDriveName[2] = 0;
		printf ("drive name = (%s)\n", nfsDriveName);
		return (OK);
	}

	return (ERROR_CODE);
}



STATUS copyDirectoryPath(char *sourceDir, char *destDir, BOOL cleanFirst, BOOL makeDestDir)
{
STATUS st = OK;

	printf ("Copying %s to %s\r\n", sourceDir, destDir);

	if (cleanFirst)
	{
		st = clearDirectory(destDir, TRUE);

		if (st == ERROR_CODE)
			return (st);
	}

	if (makeDestDir)
		st = makeComplexDirectory(destDir);

	if (st == OK)
		return (copyDirEntries(sourceDir, destDir));
	else
		return (ERROR_CODE);

}


STATUS makeComplexDirectory(const char *fullPath)
{
char *ptr;


	ptr = strchr (fullPath, '/');

	if (ptr == fullPath)
		ptr++;

	ptr = strchr (ptr, '/');
	ptr++;

	return createSingleDir(fullPath, ptr);
	   
}

STATUS createSingleDir(const char *fullPath, char *dirPath)
{
char *ptr;
char directory[MAXPATH];
STATUS st;

	ptr = strchr (dirPath, '/');

	if (ptr != 0)
	{
		strncpy (directory, fullPath, (int)ptr - (int)fullPath);
		directory[(int)ptr - (int)fullPath] = 0;
	}
	else
	{
		st = mkdir ((char *)fullPath);
		return OK;
	}


	st = mkdir (directory);

	ptr++;
	if ((st == OK) && (strlen(ptr) != 0))
     return createSingleDir (fullPath, ptr);
	else
		return st;

}


STATUS copyDirEntries(char * sourceDir, char *destinationDir)
{

	char sourceEntryName[MAXPATH], destinationEntryName[MAXPATH];
	struct stat dirEntStat;
	STATUS st;
	char name[100];
	DIR *pDir;
	struct dirent *pEntry;


	pDir = opendir(sourceDir);
	printf("\n");

	if (pDir != NULL)
	{

		do
		{
			errno = OK;

			pEntry = readdir(pDir);

			if (pEntry != NULL)
			{		
				sprintf(sourceEntryName, "%s/%s", sourceDir, pEntry->d_name);

				if (stat(sourceEntryName, &dirEntStat) == ERROR_CODE)
				{
					printf("%s ... ", pEntry->d_name);

					printf("\n*** Error reading stat structure for %d ***\n", errno);
					printErrno(errno);
					printf("\nstat.deviceID %ld", dirEntStat.st_dev);
					printf("\nstat.serialNumber %ld", dirEntStat.st_ino);
					printf("\nstat.mode %d  %04x", dirEntStat.st_mode, dirEntStat.st_mode);
					printf("\nstat.num of links %d", dirEntStat.st_nlink);
					printf("\nstat.userID %d", dirEntStat.st_uid);
					printf("\nstat.groupID %d", dirEntStat.st_gid);
					printf("\nstat.specialfile deviceID %ld", dirEntStat.st_rdev);
					printf("\nstat.filesize %ld", dirEntStat.st_size);
					printf("\nstat.accesstime %ld", dirEntStat.st_atime);
					printf("\nstat.modtime %ld", dirEntStat.st_mtime);
					printf("\nstat.changetime %ld", dirEntStat.st_ctime);
					printf("\nstat.blocksize %ld", dirEntStat.st_blksize);
					printf("\nstat.blocks %ld", dirEntStat.st_blocks);
					printf("\nstat.attributes %d", dirEntStat.st_attrib);
  
					perror("stat");


						sprintf(destinationEntryName, "%s/%s", destinationDir, pEntry->d_name);

						st = copy (sourceEntryName, destinationEntryName);
						if (st == ERROR_CODE)
						{
							printf("\n*** Error copying regular file %s ***\n", pEntry->d_name);
							perror("copyfile");
							closedir(pDir);
							return st;
						}
						else
							printf("OK  ");


				}
				else
				{
					if (S_ISREG(dirEntStat.st_mode))
					{
						sprintf(destinationEntryName, "%s/%s", destinationDir, pEntry->d_name);

						printf("%s ... ", pEntry->d_name);

						st = copy (sourceEntryName, destinationEntryName);
						if (st == ERROR_CODE)
						{
							printf("\n*** Error copying regular file %s ***\n", pEntry->d_name);
							perror("copyfile");
							closedir(pDir);
							return st;
						}
/*
						else
							printf("OK  ");
*/
					}
					else if (S_ISDIR(dirEntStat.st_mode))
					{
						if ((strcmp(pEntry->d_name,".") != 0) && (strcmp(pEntry->d_name, "..") != 0))
						{
							sprintf(destinationEntryName, "%s/%s", destinationDir, pEntry->d_name);
							st = mkdir (destinationEntryName);
							if (st != ERROR_CODE)
								st = copyDirEntries(sourceEntryName, destinationEntryName);

							if (st == ERROR_CODE)
							{
								printf("\n*** Error copying directory %s ***\n", pEntry->d_name);
								perror("copydir");
								closedir(pDir);
								return st;
							}
/*
							else
								printf("OK  ");
*/
						}
					}
					else
					{
						printf("\n*** Not going to copy %s ***\n", sourceEntryName);
					}
				}
			}
		}
		while (pEntry != NULL);

	}

	closedir(pDir);

	return OK;	
}

STATUS clearDirectory(const char * sourceDir, BOOL recursive)
{

	STATUS st;
	DIR *pDir;


	pDir = opendir((char *)sourceDir);

	if (pDir != NULL)
	{

		st = clearSingleDirectory(sourceDir, recursive);
		if (st != ERROR_CODE)
			st = rmdir ((char *)sourceDir);

		closedir(pDir);

	}


	return (st);	
}


STATUS clearSingleDirectory(const char * sourceDir, BOOL recursive)
{

	char sourceEntryName[MAXPATH], destinationEntryName[MAXPATH];
	struct stat dirEntStat;
	STATUS st;
	char name[100];
	DIR *pDir;
	struct dirent *pEntry;


	pDir = opendir((char *)sourceDir);

	if (pDir != NULL)
	{

		do
		{
			errno = OK;

			pEntry = readdir(pDir);

			if (pEntry != NULL)
			{		
				sprintf(sourceEntryName, "%s/%s", sourceDir, pEntry->d_name);

				if (stat(sourceEntryName, &dirEntStat) == OK)
				{
					if (S_ISREG(dirEntStat.st_mode))
					{

						st = rm (sourceEntryName);
						if (st == ERROR_CODE)
						{
							closedir(pDir);
							return st;
						}
					}
				
					else if ((S_ISDIR(dirEntStat.st_mode)) && recursive)
					{
						if ((strcmp(pEntry->d_name,".") != 0) && (strcmp(pEntry->d_name, "..") != 0))
						{
							
							st = clearSingleDirectory(sourceEntryName, recursive);
							if (st != ERROR_CODE)
								st = rmdir (sourceEntryName);
							if (st == ERROR_CODE)
							{
								closedir(pDir);
								return st;
							}
						}
					}
				}
			}
		}
		while (pEntry != NULL);

		closedir(pDir);

	}


	return (st);	
}


BOOL findFile(const char * startDir, const char *fileName, BOOL recursive, char *filePath)
{

char sourceEntryName[MAXPATH];
struct stat dirEntStat;
STATUS st;
DIR *pDir;
struct dirent *pEntry;

	pDir = opendir((char *)startDir);

	if (pDir != NULL)
	{
		do
		{
			pEntry = readdir(pDir);

			if (pEntry != NULL)
			{		
				sprintf(sourceEntryName, "%s/%s", startDir, pEntry->d_name);

				if (stat(sourceEntryName, &dirEntStat) == OK)
				{
					if (S_ISREG(dirEntStat.st_mode))
					{
						if (fileMatch(pEntry->d_name, fileName))
						{
							strcpy(filePath,sourceEntryName);
 							closedir(pDir);
							return (TRUE);
						}

					}
				
					else if ((S_ISDIR(dirEntStat.st_mode)) && recursive)
					{
						if ((strcmp(pEntry->d_name,".") != 0) && (strcmp(pEntry->d_name, "..") != 0))
							if (findFile(sourceEntryName, fileName, recursive, filePath))
							{
								closedir(pDir);
								return (TRUE);
							}

					}
				}
			}
		}
		while (pEntry != NULL);

		closedir(pDir);

	}

	return (FALSE);	
}


BOOL fileMatch(const char *searchFileName, const char *targetFileName)
{
const char *targetTmpPtr = targetFileName;
const char *searchTmpPtr = searchFileName;

	if (targetFileName[0] == '*')
	{
		for (; (searchTmpPtr[0] != '.'); searchTmpPtr++);
		{
			if (searchTmpPtr[0] == 0)
			return (FALSE);
		}
		searchTmpPtr++;
		targetTmpPtr+=2;
	}
	
	if (strcmp(searchTmpPtr,targetTmpPtr) == 0)
		return (TRUE);
	else
		return (FALSE);
}

/*STATUS getPath(const string& fullPath, string& Path)
  this function simply extracts the directory path from the
  full path of a file. in other words, which directory contains
  the specified file.*/

STATUS getPath(const string& fullPath, string& Path)
{
string::size_type length = fullPath.size();
int i;

	for (i = length-1; (i>=0)&&(fullPath[i] != '\\')&&(fullPath[i] != '/'); i--);

	if (i < 0)
		return (ERROR_CODE);

	Path = fullPath.substr(0, i + 1);

	return (OK);
}

