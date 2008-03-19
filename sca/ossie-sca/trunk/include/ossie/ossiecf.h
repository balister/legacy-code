/****************************************************************************

Copyright 2004, Virginia Polytechnic Institute and State University

This file is part of the OSSIE Core Framework.

OSSIE Core Framework is free software; you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

OSSIE Core Framework is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with OSSIE Core Framework; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the OSSIECF_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// OSSIECF_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MS_dotNET
#ifdef OSSIECF_EXPORTS
#define OSSIECF_API __declspec(dllexport)
#else                                             /*  */
#define OSSIECF_API __declspec(dllimport)
#endif                                            /*  */
#else                                             /*  */
#define OSSIECF_API
#endif                                            /*  */
/*
// This class is exported from the ossiecf.dll
class OSSIECF_API COssiecf {
public:
    COssiecf(void);
    // TODO: add your methods here.
};

extern OSSIECF_API int nOssiecf;

OSSIECF_API int fnOssiecf(void);
*/
