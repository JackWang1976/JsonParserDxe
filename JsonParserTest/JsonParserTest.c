/** @file
  Json Parser Test

  Copyright (c) 2016,
 Dell Inc. All rights reserved 
**/

//-----------------------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------------------
#include <Uefi/UefiSpec.h>
#include <Protocol/FirmwareManagement.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Library/ShellLib.h>
#include <Protocol/EfiShellParameters.h>
#include <Protocol/EfiShellInterface.h>
#include <Guid/DellBios.h>
#include <Library/DellShellLib.h>
#include <EfiFlash.h>

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
UINTN               Column, Row;
//UINTN               Callbacks;
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
GetNewBiosImage (
  IN    EFI_HANDLE      ImageHandle,
  IN    CHAR16          *FName,
  IN OUT VOID           **FirmwareImageBuffer,
  OUT   UINTN           *FSize
)
{
    EFI_STATUS          Status      = EFI_SUCCESS;
    EFI_FILE_HANDLE     FileHandle  = NULL;


    // Open the source BIOS file.
    Status = ShellOpenFileByName(FName, &FileHandle, EFI_FILE_MODE_READ, 0);
    if ( EFI_ERROR(Status) )    {
        Print(L"Error:  Can not open %s\n", FName);
        return Status;
    }
    else    {
        DebugPrint((L"Success opening %s\n", FName));
    }

    // Get the size of the BIOS size.  It must be the same size as the current
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
                                    FirmwareImageBuffer
                                    );

    if (*FirmwareImageBuffer == NULL) {
        Print(L" **FAIL: Error allocating a buffer for the image file %s.\n", FName);
        return Status;
    }

    // Load BIOS.HDR into the allocated memory.
    Status = ShellReadFile(FileHandle, FSize, *FirmwareImageBuffer);

    if (EFI_ERROR(Status)) {
        Print(L" **FAIL: Error reading the image file %s.\n", FName);
        return Status;
    }

    // Close the BIOS file.
    if ( FileHandle)    {
        Status = ShellCloseFile(&FileHandle);
        if (EFI_ERROR(Status)) {
            Print(L" **FAIL: Cannot close the firmware image file %s.\n", FName);
            return Status;
        }
        DebugPrint((L"   INFO: %X bytes of %s read into memory at %X\n\n", *FSize, FName, *FirmwareImageBuffer));
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
  EFI_STATUS                          Status;
 
  mSystemTable = SystemTable;

  // Print the utitlity name and version as defined in EfiFlash.h
  Print(TITLE_BANNER);

//<= JFDDebug Start  4/20/2010   10:17:39AM
//     CpuDeadLoop();
////=> JFDDebug End  4/20/2010   10:17:39AM

    // Get the input parameters into ProgramOptions.
    if ((Status = HandleInputParameters(&ProgramOptions, &Argv, ImageHandle)) != EFI_SUCCESS)  {
        return Status;
    }

    DebugPrint((L"ProgramOptions = %0.8x\n", ProgramOptions.iProgramOptions));

    // Copy the program options into the VendorCode structure for the Firmware
    // Management Protocol SetImage.
    VendorCode.Version = 0;
    VendorCode.Parameters = 0;
    if (ProgramOptions.iProgramOptions != 0)    {
        VendorCodePtr = &VendorCode;
        if ((ProgramOptions.iProgramOptions & WIPECLEAN) != 0)  {
            VendorCode.Parameters |= PARAMETER_WIPE_CLEAN;
        }
        if ((ProgramOptions.iProgramOptions & FORCETYPE) != 0)  {
            VendorCode.Parameters |= PARAMETER_FORCE_TYPE;
        }
        if ((ProgramOptions.iProgramOptions & FORCEIT) != 0)  {
            VendorCode.Parameters |= PARAMETER_FORCE_IT;
        }
    }

    // The name of the BIOS .hdr file must be the first argument in the command line.
    FName = Argv[1];

    DebugPrint((L"FLASH file name:  %s\n", FName));



//1. Locate the FW Management Protocol
//   This is done by LocateHandleBuffer
  NoHandles = 0;
  HandleBuffer = NULL;
  Status = SystemTable->BootServices->LocateHandleBuffer(
                                          ByProtocol,
                                          &gEfiFirmwareManagementProtocolGuid,
                                          NULL,
                                          &NoHandles,
                                          &HandleBuffer
                                          );
  if (EFI_ERROR(Status)) {
      Print(L"*** ERROR: No Firmware Management Protocol instances were found");
      return EFI_NOT_FOUND;
  }

  if (NoHandles < 1) {
      Print(L"***ERROR: No Dell Firmware Management protocol instances.\n");
      return EFI_NOT_FOUND;
  }

  DebugPrint((L"%d Firmware Management Protocol instances.\n", NoHandles));


//2. We have found the Handles.. Now fetch the protocol
//   At the end of this, the FW management protocol functions will be available in
//   FirmwareManagementProtocol
  for (k = 0; k < NoHandles ; k++)    {
    DebugPrint((L"             #%d: ", k + 1));
    FirmwareManagementProtocol = NULL;
    Status = SystemTable->BootServices->HandleProtocol(
                                                       HandleBuffer[k],
                                                       &gEfiFirmwareManagementProtocolGuid,
                                                       (VOID *)&FirmwareManagementProtocol
                                                       );

    if (EFI_ERROR(Status) || FirmwareManagementProtocol == NULL) {
      Print(L"***ERROR: Could not fetch the protocol with HandleProtocol\n");
      continue;
    }

//3. Get the FW image info
    ImageInfoSize = sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR);
    Status = FirmwareManagementProtocol->GetImageInfo(
                                                     FirmwareManagementProtocol,
                                                     &ImageInfoSize,
                                                     &FwImageInfo,
                                                     &DescriptorVersion,
                                                     &DescriptorNumber,
                                                     &DescriptorSize,
                                                     &PackageVersion,
                                                     &PackageVersionName
                                                     );
    if (EFI_ERROR(Status)) {
      Print(L"***ERROR: FirmwareManagementProtocol->GetImageInfo failed.\n");
      continue;
    }

    if (sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR) == ImageInfoSize) {
      DebugPrint ((L" %2d: %s \nPointer=%x Handle=%x\n", FwImageInfo.ImageIndex, FwImageInfo.ImageIdName, &FirmwareManagementProtocol, HandleBuffer[k]));
      if (CompareGuid(&(FwImageInfo.ImageTypeId), &gDellBiosGuid) == TRUE) {
        // We found the BIOS Firmware Management Protocol.
        DebugPrint((L"Image Guid/ID match: %.-36g\n", FwImageInfo.ImageTypeId));
        DebugPrint((L"Found BIOS Firmware Management Protocol.\n"));
        BiosFMP = FirmwareManagementProtocol;
//      break;
      }
      else  {
        Print(L"***ERROR: Image Guid/ID mismatch: %.-36g\n",FwImageInfo.ImageTypeId);
        Print(L"***ERROR: VersionName = %s\n", FwImageInfo.VersionName);
      }
      DebugPrint((L"Package Version = %X\n", PackageVersion));
      DebugPrint((L"Package Version Name = %s.\n", PackageVersionName));

    }
    else {
      Print(L"***ERROR: Unknown Other Image, InfoSize = %d [not %d]\n", ImageInfoSize, sizeof(EFI_FIRMWARE_IMAGE_DESCRIPTOR));
    }
  }

  if (NULL == BiosFMP)  {
    return  EFI_NOT_FOUND;
  }

  // Get the input parameters into ProgramOptions.
  if ((Status = HandleInputParameters(&ProgramOptions, &Argv, ImageHandle)) != EFI_SUCCESS)  {
    return Status;
  }
  DebugPrint((L"\nProgramOptions = %0.8x\n", ProgramOptions.iProgramOptions));

  // Copy the program options into the VendorCode structure for the Firmware
  // Management Protocol SetImage.
  VendorCode.Version = 0;
  VendorCode.Parameters = 0;
  if (ProgramOptions.iProgramOptions != 0)    {
    VendorCodePtr = &VendorCode;
    if ((ProgramOptions.iProgramOptions & WIPECLEAN) != 0)  {
      VendorCode.Parameters |= PARAMETER_WIPE_CLEAN;
    }
    if ((ProgramOptions.iProgramOptions & FORCETYPE) != 0)  {
      VendorCode.Parameters |= PARAMETER_FORCE_TYPE;
    }
    if ((ProgramOptions.iProgramOptions & FORCEIT) != 0)  {
      VendorCode.Parameters |= PARAMETER_FORCE_IT;
    }
  }

  // The name of the BIOS .hdr file must be the first argument in the command line.
  FName = Argv[1];

  DebugPrint((L"FLASH file name:  %s\n", FName));



  return Status;
}
