#pragma once
#include <wtypes.h>

BOOL EnumTypesFunc(
	HMODULE hModule,
	LPTSTR lpType,
	LONG lParam);


BOOL EnumNamesFunc(
	HMODULE hModule,
	LPCTSTR lpType,
	LPTSTR lpName,
	LONG lParam);


BOOL EnumLangsFunc(
	HMODULE hModule,
	LPCTSTR lpType,
	LPCTSTR lpName,
	WORD wLang,
	LONG lParam);

