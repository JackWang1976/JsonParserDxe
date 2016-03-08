//----------------------------------------------------------------------------
//Inclusion guard
#ifndef     JSON_PARSER_TSET_H__
#define JSON_PARSER_TSET_H__    1
//----------------------------------------------------------------------------
/** @file
  JSON_PARSER_TEST.h

  Abstract:
    Contains definitions for the corresponding application.

 Copyright (c) 2016, Dell Inc.
 All rights reserved
**/


// ****************************************************************************
// Related Definitiions
// ****************************************************************************
#define     TITLE_BANNER    L"Dell JSON Utility version 0.01\n"



// Initialization for the VALIDOPTIONS structure array
#define VALID_OPTIONS_DATA {                    \
                    {L"-h", DISPLAYHELP},       \
                    {L"-?", DISPLAYHELP},       \
                    {L"/?", DISPLAYHELP},       \
                    {L"/h", DISPLAYHELP},       \
                }

// ****************************************************************************
// Function Prototypes
// ****************************************************************************
VOID PrintUsage(VOID);


//----------------------------------------------------------------------------
//End of Inclusion guard.
//DO NOT ADD MORE CODE AFTER THIS LINE.
#endif  // JSON_PARSER_TSET_H__
//----------------------------------------------------------------------------
