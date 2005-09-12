/*
    Shared Class Facility (SCF)
    Copyright (C) 1999 by Andrew Zabolotny

    Cross-platform shared library management
*/

#ifndef __SHAREDLIB_H__
#define __SHAREDLIB_H__

typedef void *tLibraryHandle;

/**
 * Load a shared library and return a library handle,
 * which is used later to query and unload the library.
 * iName is the FULL path to the library.
 */
extern tLibraryHandle fLoadLibrary (const char* iName);

/**
 * Return a pointer to a symbol within given shared library.
 * Note that generally Crystal Space needs just one exported symbol
 * from every shared library; the symbol is called <library>_GetClassTable.
 * If your OS is short on features, you may implement querying of just
 * this symbol.
 */
extern void *fGetLibrarySymbol (tLibraryHandle Handle, const char *iName);

/**
 * Unload a shared library given its handle.
 * The function returns false on error.
 */
extern bool fUnloadLibrary (tLibraryHandle Handle);

/**
 * Add one element to shared library search path;
 * the path should end in '/' or whatever the path separator is,
 * that is, it should be immediately prependable to shared library name.
 */
extern void fAddLibraryPath (const char *iPath);

/**
 * Find a shared library in library search path and load it.
 * Same as csLoadLibrary except that you give just the name of the
 * module, without any prefix/suffix.
 */
extern tLibraryHandle fFindLoadLibrary (const char *iModule);

/**
 * Same but you give the possible suffix and prefix. This is usually called
 * by the system-dependent implementation of csFindLoadLibrary, and not
 * by the user. iPrefix can be either NULL or something like "lib";
 * the routine tries both with (if it is not NULL) and without prefix.
 * Same about iSuffix - it can be something like ".dll" or ".so", but
 * not NULL (because all OSes use some suffix for shared libs).
 */
extern tLibraryHandle fFindLoadLibrary (const char *iPrefix,
  const char *iName, const char *iSuffix);

/**
 * Print out the latest dynamic loader error.
 * This is not strictly required (and on some platforms its just a empty
 * routine) but sometimes it helps to find problems.
 */
extern void fPrintLibraryError (const char *iModule);

#endif // __SHAREDLIB_H__
