/*
   minizip.c
   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant
*/

#include "PAL_platformheader.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string>


#if defined(PAL_WIN32_RUN)
# include <direct.h>
# include <io.h>
#endif

#include "zip.h"
#include "unzip.h"		//	.../3RDPartySoftware/zlib/minizip

#include "CZip.h"

#if defined(PAL_WIN32_RUN)
#define USEWIN32IOAPI
#include "iowin32.h"
#endif

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "ULong.h"

namespace Hobart
{

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)


int check_exist_file(const char *filename)
{
	FILE* ftestexist;
	int ret = 1;
	ftestexist = fopen(filename,"rb");
	if (ftestexist==NULL)
		ret = 0;
	else
		fclose(ftestexist);
	return ret;
};

int CZip::zipDir(const char * dir, const char * dest_file)
{
	int opt_overwrite=0;
	int opt_compress_level=Z_DEFAULT_COMPRESSION;
	// LINUX_FIX int zipfilenamearg = 0;
	int err=0;
	int size_buf=0;
	void* buf=NULL;
	const char* password=NULL;

#if !defined ( __linux__ )
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
#endif

	char path[MAX_PATH];  // directory specification  
	strcpy (path, dir);

#if defined(PAL_WIN32_RUN)
	strncat (path, PATH_SEPARATOR, sizeof(PATH_SEPARATOR));
	strncat (path, "*", 1);
#endif

	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (buf==NULL)
	{
		printf("Error allocating memory\n");
		return ZIP_INTERNALERROR;
	}

	{
		zipFile zf;
#ifdef USEWIN32IOAPI
		zlib_filefunc_def ffunc;
		fill_win32_filefunc(&ffunc);
		zf = zipOpen2(dest_file,/*(opt_overwrite==2) ? 2 :*/ 0,NULL,&ffunc);
#else
		zf = zipOpen(dest_file,(opt_overwrite==2) ? 2 : 0);
#endif

		if (zf == NULL)
			err= ZIP_ERRNO;

#if defined ( __linux__ )
		std::vector<std::string> dirlist = PAL_absclass::PAL_getDirectoryList( const_cast< char *>(path));
#else
		hFind = FindFirstFileA(path, &FindFileData);
#endif	  // __linux__
		std::string full_path = dir;
#if defined(LINUX_FIX)
		full_path += PATH_SEPARATOR;
#else
		full_path += "\\";
#endif

#if defined ( __linux__ )
		std::vector<std::string>::iterator w;
		for(w = dirlist.begin(); w != dirlist.end(); w++) 
#else
		while (hFind != INVALID_HANDLE_VALUE && err == ZIP_OK  )
#endif	  // __linux__
		{
#if !defined ( __linux__ )
			if ( FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
#endif
			{
				FILE * fin = NULL;
				int size_read;
				std::string filename;
#if !defined ( __linux__ )
				filename = full_path + FindFileData.cFileName;
#else
				filename = full_path + *w;
#endif
				const char* filenameinzip = filename.c_str();
				zip_fileinfo zi;
				unsigned long crcFile=0;

				zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
					zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
				zi.dosDate = 0;
				zi.internal_fa = 0;
				zi.external_fa = 0;

				PAL_FILETIME local_time;
#if !defined ( __linux__ )
				PAL_absclass::PAL_fileTimeToLocalFileTime(&(FindFileData.ftLastWriteTime),&local_time);
#else
				struct stat st;
				stat(filename.c_str(), &st);
				PAL_FILETIME pts;
				int64_t time =(st.st_mtime * RATE_DIFF) + EPOCH_DIFF;
				pts.dwHighDateTime = time >> 32;
				pts.dwLowDateTime = (DWORD)time;

				PAL_absclass::PAL_fileTimeToLocalFileTime(&(pts),&local_time);
#endif	  // __linux__
				PAL_absclass::PAL_fileTimeToDosDateTime(&local_time,((LPWORD) &zi.dosDate)+1,((LPWORD) &zi.dosDate )+0);

				err = zipOpenNewFileInZip3(zf,filenameinzip,&zi,
						NULL,0,NULL,0,NULL /* comment*/,
						(opt_compress_level != 0) ? Z_DEFLATED : 0,
						opt_compress_level,0,
						/* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
						-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
						password,crcFile);

				if (err == ZIP_OK)
				{
					fin = fopen(filenameinzip,"rb");
					if (fin==NULL)
						err=ZIP_ERRNO;
				}

				if (err == ZIP_OK)
					do
					{
						err = ZIP_OK;
						size_read = (int)fread(buf,1,size_buf,fin);
						if (size_read < size_buf)
							if (feof(fin)==0)
							{
								err = ZIP_ERRNO;
							}

						if (size_read>0)
						{
							err = zipWriteInFileInZip (zf,buf,size_read);
						}
					} while ((err == ZIP_OK) && (size_read>0));

				if (fin)
					fclose(fin);

				if (err<0)
					err=ZIP_ERRNO;
				else
				{
					err = zipCloseFileInZip(zf);
				}
			}

#if !defined ( __linux__ )
			if (FindNextFileA(hFind, &FindFileData) == 0)
				break;			
#endif
		}
		err = zipClose(zf,NULL);
	}

	free(buf);
	return err;
}

/* change_file_date : change the date/time of a file
	filename : the filename of the file where date/time must be modified
	dosdate : the new date at the MSDos format (4 bytes)
	tmu_date : the SAME new date at the tm_unz format */
void change_file_date(
		const char *filename, 
		uLong dosdate,
		tm_unz /* LINUX_FIX tmu_date */)
{
#if defined ( __linux__ )
	PAL_FILETIME tm;
	struct utimbuf times;
	PAL_absclass::PAL_dosDateTimeToFileTime( (WORD) (dosdate>>16), (WORD)dosdate, &tm);
	times.actime = times.modtime = (int64_t)(tm.dwHighDateTime)<<32;
	times.actime = times.modtime |= tm.dwLowDateTime;
	utime(filename, &times);
#else
	HANDLE hFile;
	FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

	hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
			0,NULL,OPEN_EXISTING,0,NULL);
	GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
	DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
	LocalFileTimeToFileTime(&ftLocal,&ftm);
	SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
	CloseHandle(hFile);
#endif	  // __linux__
}


/* mymkdir and change_file_date are not 100 % portable
   As I don't know well Unix, I wait feedback for the unix portion */


int makedir (char *newdir)
	//char *newdir;
{
	char *buffer ;
	char *p;
	int  len = (int)strlen(newdir);

	if (len <= 0)
		return 0;

	buffer = (char*)malloc(len+1);
	strcpy(buffer,newdir);

	if (buffer[len-1] == '/') {
		buffer[len-1] = '\0';
	}
#if defined ( __linux__ )
	if (mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
#else
	if (mkdir(buffer) == 0)
#endif	  // __linux__
	{
		free(buffer);
		return 1;
	}

	p = buffer+1;
	while (1)
	{
		char hold;

		while(*p && *p != '\\' && *p != '/')
			p++;
		hold = *p;
		*p = 0;
#if defined ( __linux__ )
		if ((mkdir(buffer, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) && (errno == ENOENT))
#else
		if ((mkdir(buffer) == -1) && (errno == ENOENT))
#endif	  // __linux__
		{
			free(buffer);
			return 0;
		}
		if (hold == 0)
			break;
		*p++ = hold;
	}
	free(buffer);
	return 1;
}


int do_extract_currentfile(
		unzFile uf,
		const int * popt_extract_without_path, 
		int* /* LINUX_FIX popt_overwrite */,
		const char *password)
{
	char filename_inzip[256];
	char* filename_withoutpath;
	char* p;
	int err=UNZ_OK;
	FILE *fout=NULL;
	void* buf;
	uInt size_buf;

	unz_file_info file_info;
	// LINUX_FIX uLong ratio=0;
	err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

	if (err!=UNZ_OK)
	{
		return err;
	}

	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (buf==NULL)
	{
		return UNZ_INTERNALERROR;
	}

	p = filename_withoutpath = filename_inzip;
	while ((*p) != '\0')
	{
		if (((*p)=='/') || ((*p)=='\\'))
			filename_withoutpath = p+1;
		p++;
	}

	if ((*filename_withoutpath)=='\0')
	{
		if ((*popt_extract_without_path)==0)
		{
#if defined __linux__
			mkdir(filename_inzip, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
			mkdir(filename_inzip);
#endif
		}
	}
	else
	{
		const char* write_filename;
		int skip=0;

		if ((*popt_extract_without_path)==0)
			write_filename = filename_inzip;
		else
			write_filename = filename_withoutpath;

		err = unzOpenCurrentFilePassword(uf,password);

		if ((skip==0) && (err==UNZ_OK))
		{
			fout=fopen(write_filename,"wb");

			/* some zipfile don't contain directory alone before file */
			if ((fout==NULL) && ((*popt_extract_without_path)==0) &&
					(filename_withoutpath!=(char*)filename_inzip))
			{
				char c=*(filename_withoutpath-1);
				*(filename_withoutpath-1)='\0';
				makedir(( char * )write_filename);
				*(filename_withoutpath-1)=c;
				fout=fopen(write_filename,"wb");
			}
		}

		if (fout!=NULL)
		{
			do
			{
				err = unzReadCurrentFile(uf,buf,size_buf);
				if (err<0)
				{
					break;
				}
				if (err>0)
					if (fwrite(buf,err,1,fout)!=1)
					{
						err=UNZ_ERRNO;
						break;
					}
			}
			while (err>0);
			if (fout)
			{
				fflush(fout);
				fclose(fout);
			}

			if (err==0)
				change_file_date(write_filename,file_info.dosDate,
						file_info.tmu_date);
		}

		if (err==UNZ_OK)
		{
			err = unzCloseCurrentFile (uf);
		}
		else
			unzCloseCurrentFile(uf); /* don't lose the error */
	}

	free(buf);
	return err;
}


int do_extract(unzFile uf,int opt_extract_without_path,int opt_overwrite,const char * password)
{
	uLong i;
	unz_global_info gi;
	int err;
	// LINUX_FIX FILE* fout=NULL;

	err = unzGetGlobalInfo (uf,&gi);

	for (i=0;i<gi.number_entry;i++)
	{
		if (do_extract_currentfile(
					uf,
					&opt_extract_without_path,
					&opt_overwrite,
					password) != UNZ_OK)
			break;

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK)
			{
				break;
			}
		}
	}

	return 0;
}

int do_extract_a_singleFile(unzFile uf,int opt_extract_without_path,int opt_overwrite,const char * password, const char *targetFile)
{
	uLong i;
	unz_global_info gi;
	int err;
	// LINUX_FIX FILE* fout=NULL;
	char filename_inzip[256];
	unz_file_info file_info;

	err = unzGetGlobalInfo (uf,&gi);

//	Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  Entries in archive = ") + Integer::toString(gi.number_entry) + HobString(", targetFile is ") + HobString(targetFile));
	for (i=0;i<gi.number_entry;i++)
	{
		err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

//		if (err != UNZ_OK)
//			Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  i = ") + ULong::toString(i) + HobString(", unzGetCurrentFileInfo() returned ") + Integer::toString(err));
//		if (err==UNZ_OK)
//			Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  filename_inzip = ") +  HobString(filename_inzip));
		if ((err==UNZ_OK) && (strcmp (filename_inzip, targetFile) == 0))
		{
			Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  Extracting ") + HobString(filename_inzip) + HobString(",  opt_extract_without_path = ") + Integer::toString(opt_extract_without_path) + HobString(",  opt_overwrite = ") + Integer::toString(opt_overwrite));
			do_extract_currentfile(uf,&opt_extract_without_path,
					&opt_overwrite,
					password);
			break;
		}

		if ((i+1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
//			if (err != UNZ_OK)
//				Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  i = ") + ULong::toString(i) + HobString(", unzGoToNextFile() returned ") + Integer::toString(err));
			if (err!=UNZ_OK)
			{
				Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  Error at unzGoToNextFile()!"));
				break;
			}
		}
	}

//	Debug::trace("VersionManager.upgrade", HobString("CZip::deasf():  i = ") + ULong::toString(i) + HobString(", Returning ") + Integer::toString(err));
	return err;		//	Geez, let's return the stupid error and not just 0. Zero tells us nothing!
}

int CZip::unZip( const char * zipfilename, const char * dirname)
{
	const char *password=NULL;
	char filename_try[MAXFILENAME+16] = "";
	// LINUX_FIX int opt_do_list=0;
	int opt_do_extract=1;
	int opt_do_extract_withoutpath=0;
	int opt_overwrite=1;
	// LINUX_FIX int opt_extractdir=0;
	unzFile uf=NULL;

	int ret_value = 0;

	if (zipfilename!=NULL)
	{
#ifdef USEWIN32IOAPI
		zlib_filefunc_def ffunc;
#endif

		strncpy(filename_try, zipfilename,MAXFILENAME-1);
		filename_try[ MAXFILENAME ] = '\0';

#ifdef USEWIN32IOAPI
		fill_win32_filefunc(&ffunc);
		uf = unzOpen2(zipfilename,&ffunc);
#else
		uf = unzOpen(zipfilename);
#endif
		if (uf==NULL)
		{
			strcat(filename_try,".zip");
#ifdef USEWIN32IOAPI
			uf = unzOpen2(filename_try,&ffunc);
#else
			uf = unzOpen(filename_try);
#endif
		}
	}

	if ( dirname != NULL )
#if defined ( __linux__ )
		mkdir( dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );
#else
		_mkdir( dirname);
#endif

	if (uf==NULL)
	{
		return 1;
	}

	char  current_dir[MAX_PATH];
	PAL_absclass::PAL_getCurrentDirectoryA( MAX_PATH, current_dir );
	std::string dir_save = current_dir;

	if (opt_do_extract==1)
	{
		if ( !PAL_absclass::PAL_setCurrentDirectoryA( dirname ) )
			return -1;		

		ret_value = do_extract(uf,opt_do_extract_withoutpath,opt_overwrite,password);

		PAL_absclass::PAL_setCurrentDirectoryA( dir_save.c_str() );
	}
	unzCloseCurrentFile(uf);
	unzClose( uf );
	return ret_value;
}

int CZip::unZipSingleFile( const char * zipfilename, const char * targetFile, const char * dirname)
{
	Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile(") + zipfilename + ", " + targetFile + ", " + dirname + ")");
	const char *password=NULL;
	char filename_try[MAXFILENAME+16] = "";
	// LINUX_FIX int opt_do_list=0;
	int opt_do_extract=1;
	int opt_do_extract_withoutpath=0;
	int opt_overwrite=1;
	// LINUX_FIX int opt_extractdir=0;
	unzFile uf=NULL;

	int ret_value = 0;

	if (!targetFile)
	{
		Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  No target file. Exiting and returning -2!"));
		return -2;
	}

	if (zipfilename!=NULL)
	{
#ifdef USEWIN32IOAPI
		zlib_filefunc_def ffunc;
#endif

		strncpy(filename_try, zipfilename,MAXFILENAME-1);
		filename_try[ MAXFILENAME ] = '\0';
	Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  filename_try = ") + filename_try);

#ifdef USEWIN32IOAPI
		fill_win32_filefunc(&ffunc);
		uf = unzOpen2(zipfilename,&ffunc);
#else
		uf = unzOpen(zipfilename);
#endif
		if (uf==NULL)
		{
			strcat(filename_try,".zip");
	Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  Could not unzOpen. Now filename_try = ") + filename_try);
#ifdef USEWIN32IOAPI
			uf = unzOpen2(filename_try,&ffunc);
#else
			uf = unzOpen(filename_try);
#endif
		}
	}

	if ( dirname != NULL )
	{
#if defined ( __linux__ )
		Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  mkdir(") + dirname + ", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)");
		mkdir( dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
		_mkdir( dirname );
#endif	  // __linux__
	}

	if (uf==NULL)
	{
		Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  uf = NULL. Exiting and returning 1!"));
		return 1;
	}
	Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  So far, so good."));

	char  current_dir[MAX_PATH];
	PAL_absclass::PAL_getCurrentDirectoryA( MAX_PATH, current_dir );
	std::string dir_save = current_dir;

	if (opt_do_extract==1)
	{
		if ( !PAL_absclass::PAL_setCurrentDirectoryA( dirname ) )
		{
			Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  setCurrentDirectory ") + dirname + " failed. Exiting and returning -1!");
			return -1;
		}

		ret_value = do_extract_a_singleFile(uf,opt_do_extract_withoutpath,opt_overwrite,password,targetFile);
		Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  do_extract_a_singleFile returned ") + Integer::toString(ret_value));

		PAL_absclass::PAL_setCurrentDirectoryA( dir_save.c_str() );
	}
	unzCloseCurrentFile(uf);
	unzClose( uf );
	Debug::trace("VersionManager.upgrade", HobString("CZip::unZipSingleFile():  Done. Return ") + Integer::toString(ret_value));
	return ret_value;
}

}//end of namespace
