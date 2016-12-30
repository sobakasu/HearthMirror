//
//  winhelper.cpp
//  windows memory reading functions
//
//  Created by Istvan Fehervari on 29/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#include "winhelper.h"
#include <psapi.h>

// debug only imports
#include <stdio.h>
#include <tchar.h>
proc_address getMonoLoadAddress(HANDLE task) {

	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;
	TCHAR szModName[MAX_PATH];

	// enumerate all modules loaded by the process
	if (EnumProcessModules(task, hMods, sizeof(hMods), &cbNeeded)) {
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			GetModuleBaseName(task, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));

			//_tprintf(TEXT("%s\n"), szModName);
			if (szModName == TEXT("mono.dll")) {
				MODULEINFO modinfo;
				GetModuleInformation(task, hMods[i], &modinfo, sizeof(MODULEINFO));
				return (proc_address)modinfo.lpBaseOfDll;
			}
		}
	}
}

proc_address getMonoRootDomainAddr(HANDLE task, proc_address baseAddress) {
	 
}

