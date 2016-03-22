/** @file
  Json Parser Test

  Copyright (c) 2016,
 Dell Inc. All rights reserved 
**/

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include <Uefi/UefiSpec.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/ShellLib.h>
#include <Protocol/EfiShellParameters.h>
#include <Protocol/EfiShellInterface.h>
#include <Protocol/JsonParser.h>
#include <Guid/DellBios.h>
#include <Library/DellShellLib.h>
#include <JsonParserTest.h>

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
EFI_SYSTEM_TABLE   *mSystemTable;

//-----------------------------------------------------------------------------
//#define DebugPrint(x)
#define DebugPrint(x) Print x
//-----------------------------------------------------------------------------

/**
 PrintUsage
  Description: Prints usage.

  @param    None.
  @return   Nothing.
**/
VOID
PrintUsage(
    VOID
)
{
    Print(
        L"Usage: JaonParserTest.efi file_name \n"
        L"  Valid options are:\n"
        L"      -ReadBios (Get the BIOS header file from the target system)\n"
        L"    Note: Options are case sensitive\n\n"
        L"    Example: JaonParserTest file_name.json\n"
//        L"    Note: HDR file has to be present in same directory as the utility.\n\n"
        );
}


/**
    HandleInputParameters

  Validate input parameters and set appropriate flags. Display usage if needed.

  @param  ProgramOptionsPtr  =  Pointer to PROGRAM_OPTIONS_UNION; an integer
                                bit-field structure to store the requested
                                program options.
  @param   Argv              =  The address to a pointer to the array of string
                                pointers passed as command line arguments.
  @param   ImageHandle       =  Image Handle for this utility.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @return Values from ShellCommandLineParse.

**/
#if 0
EFI_STATUS
HandleInputParameters (
  IN OUT    PROGRAM_OPTIONS_UNION *ProgramOptionsPtr,
  OUT       CHAR16                ***Argv,
  IN        EFI_HANDLE             ImageHandle
)
{
    // Local Variables
    EFI_STATUS      Status = EFI_SUCCESS;
    VALIDOPTIONS        validOptions[] = VALID_OPTIONS_DATA;
    UINTN               counter, counter1;
    UINTN               Argc = 0;


    // Get the command line arguments in the C style Argv and Argc.
    Status = GetArgvArgc(ImageHandle, gST, Argv, &Argc);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    // Display usage if no parameters passed.
    if (Argc <= 1) {
        PrintUsage();
        return EFI_INVALID_PARAMETER;
    }

    //
    // Parse command line to get input parameters.
    //
    for ( counter = 2; counter < Argc; counter ++ )  {
        for ( counter1 = 0; counter1 < sizeof(validOptions)/sizeof(validOptions[0]); counter1 ++)    {
            if ( !StrCmp(validOptions[counter1].OptionName, (*Argv)[counter]))    {
                ProgramOptionsPtr->iProgramOptions |= validOptions[counter1].OptionValue;
                Status = EFI_SUCCESS;
                break;
            }
            Status = EFI_INVALID_PARAMETER;
        }
        if ( Status != EFI_SUCCESS ) {
            Print(L"\nInvalid option: %s\n", (*Argv)[counter]);
            break;
        }
    }

    // If the user asked for help, print the usage message
    if ( ProgramOptionsPtr->sProgramOptions.displayHelp )   {
        PrintUsage();
        Status = EFI_INVALID_PARAMETER;
    }

    return Status;
}

#endif
EFI_STATUS
GetValue (
  JSON_Value *Value,
  UINTN Nesting
  )
{
  JSON_Array *Array = NULL;
  JSON_Object *Object = NULL;

  switch (pJson->json_value_get_type(Value)) {
    case JSONString:
      Print(L"Value: %a \n", pJson->json_value_get_string(Value));
      break;
    case JSONNumber:
      break;  
    case JSONObject:
      Object = pJson->json_value_get_object(Value);
      ParserObjectName(Object, Nesting+1)
      break;

    case JSONArray:
      Array = pJson->json_value_get_array(Value);
      ParserArrayName(Array, Nesting+1)
      break;
    default:
      break;


  }


}

EFI_STATUS
ParserArrayName (
  JSON_Array *Array,
  UINTN Nesting
  )
{
  JSON_Value          *Value = NULL;

  if (Array = NULL) {
    return EFI_UNSUPPORT;
  }

  Print(L"Array Count = %d \n", pJson->json_array_get_count(Array));

  for (Index = 0; Index < pJson->json_array_get_count(Array); Index++) {
    Value = json_array_get_value(Array, Index);
    GetValue(Value, Nesting)
  }


}


EFI_STATUS
ParserObjectName (
  JSON_Object Object,
  UINTN Nesting
  )
{
  JSON_Value          *Value = NULL;

  if (Object == NULL) {
    return EFI_UNSUPPORT;
  }

  Print(L"Nest:%d , Object Count = %d \n", Nest, pJson->json_object_get_count(Object));
  

  for (Index = 0; Index < pJson->json_object_get_count(Object); Index++) {
    Print(L"Object Name: %a,", pJson->json_object_get_name(Object, Index));
    Value = pJson->json_object_get_value(Object, pJson->json_object_get_name(Object, Index));

    GetValue(Value, Nesting)

  }//for.

}

/**
  GetNewBiosImage
    Opens a disk file and reads in a BIOS image to memory.

    @param      ImageHandle     Handle for this utility.
    @param      *FName          The File Name of the new BIOS image
    @param      **FirmwareImageBuffer   A pointer to the Buffer that will hold
                                the BIOS image.  This buffer is not allocated
                                on entry.  This routine will allocate the buffer
                                and copy the new BIOS image from the disk to this
                                buffer.
                THE CALLER MUST FREE THE MEMORY ALLOCATED BY THIS ROUTINE FOR
                FirmwareImageBuffer.

    @retval     EFI_SUCCESS     If the new BIOS image was read into memory.
    @retval     EFI_NOT_FOUND   If we cannot find the file.

**/
EFI_STATUS
OpenJsonFile (
  IN    EFI_HANDLE      ImageHandle,
  IN    CHAR16          *FName,
  IN OUT VOID           **FileBuffer,
  OUT   UINTN           *FSize
)
{
    EFI_STATUS          Status      = EFI_SUCCESS;
    EFI_FILE_HANDLE     FileHandle  = NULL;


    // Open the source file.
    Status = ShellOpenFileByName(FName, &FileHandle, EFI_FILE_MODE_READ, 0);
    if ( EFI_ERROR(Status) )    {
        Print(L"Error:  Can not open %s\n", FName);
        return Status;
    }
    else    {
        DebugPrint((L"Success opening %s\n", FName));
    }

    // Get the size of the file.  It must be the same size as the current
    // size returned from the Firmware Management Protocol plus the RBU header.
    Status = ShellGetFileSize(FileHandle, (UINT64*) FSize);
    if ( EFI_ERROR(Status) )    {
        ShellCloseFile(&FileHandle);
        Print(L"Could not read the size of %s\n", FName);
        return  Status;
    }

    DebugPrint((L"Size:  %s = %X\n", FName, *FSize));

    // Allocate a buffer for reading bios.HDR
    Status = gST->BootServices->AllocatePool(
                                    EfiLoaderData,
                                    *FSize,
                                    FileBuffer
                                    );

    if (*FileBuffer == NULL) {
        Print(L" **FAIL: Error allocating a buffer for this JSON file %s.\n", FName);
        return Status;
    }

    // Load file into the allocated memory.
    Status = ShellReadFile(FileHandle, FSize, *FileBuffer);

    if (EFI_ERROR(Status)) {
        Print(L" **FAIL: Error reading the file %s.\n", FName);
        return Status;
    }

    // Close the BIOS file.
    if ( FileHandle)    {
        Status = ShellCloseFile(&FileHandle);
        if (EFI_ERROR(Status)) {
            Print(L" **FAIL: Cannot close this file %s.\n", FName);
            return Status;
        }
        DebugPrint((L"   INFO: %X bytes of %s read into memory at %X\n\n", *FSize, FName, *FileBuffer));
    }

    return  Status;
}



//-----------------------------------------------------------------------------
// Application Entry Point
//-----------------------------------------------------------------------------
/**
    EfiFlashAppEntry

    Entry point for the EfiFlash utility.

  @param  ImageHandle  The image handle of the UEFI Application.
  @param  SystemTable  A pointer to the EFI System Table.

  @retval  0               The application exited normally.
  @retval  Other           An error occurred.

**/
EFI_STATUS
EfiJsonTestAppEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  CHAR8               *FileBuffer = NULL; 
  CHAR16              *FName = NULL;
  CHAR16              **Argv = NULL;
  JSON_PARSER_PROTOCOL *pJson = NULL;
  UINTN               FSize  = 0;    // Size of the new file.
  JSON_Value          *JsonValue = NULL;
  UINTN               Argc = 0;

  mSystemTable = SystemTable;

  // Print the utitlity name and version as defined in EfiFlash.h
  Print(TITLE_BANNER);
//Price debug++
     CpuDeadLoop();
//

    // Get the input parameters into ProgramOptions.

    // Get the command line arguments in the C style Argv and Argc.
    Status = GetArgvArgc(ImageHandle, gST, &Argv, &Argc);
    if (EFI_ERROR (Status)) {
        return Status;
    }

    // Display usage if no parameters passed.
    if (Argc <= 1) {
        PrintUsage();
        return EFI_INVALID_PARAMETER;
    }

  // The name of file must be the first argument in the command line.
  FName = Argv[1];
  DebugPrint((L"JSON file name:  %s\n", FName));


  // Open JSON file and put to buffer...
  Status = OpenJsonFile(ImageHandle, FName, &FileBuffer, &FSize);
  if (EFI_ERROR(Status)) {
      Print(L"**FAIL: Unable to load %s into buffer.\n",FName);
      return Status;;
  }
  DebugPrint((L"File loaded at %X\n", FileBuffer));

  Status = gST->BootServices->LocateProtocol (
                              &gJsonParserProtocolGuid,
                              NULL,
                              &pJson
                              );

  if (EFI_ERROR(Status) || pJson->Get_Version() == 0 ) {
      Print(L"**FAIL: Can't to locate JSON parser protocol successful.\n");
      return Status;;
  }

  // Parser JOSN string with Json Parser Protocol.
  JsonValue = pJson->json_parse_string(FileBuffer);
  if (JsonValue == NULL) {
      Print(L"JsonValue = NULL \n");
  }

  if ( JsonValue != 0 && JsonValue->type == JSONObject) { //start is object
    ParserObjectName(pJson->json_value_get_object(JsonValue), 0)
  }

  if ( JsonValue != 0 && JsonValue->type == JSONArray) { //start is Array
    ParserArrayName(pJson->json_value_get_Array(JsonValue), 0)
  }

  if (JsonValue != NULL) {
    pJson->json_value_free(JsonValue);
  }


  if (FileBuffer != NULL) {
    FreePool(FileBuffer);
  }

  return Status;
}
