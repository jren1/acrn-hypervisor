/** @file
  SHELL_INTERFACE_PROTOCOL from EDK shell (no spec).

  Shell Interface - additional information (over image_info) provided
  to an application started by the shell.

  ConIo provides a file-style interface to the console.

  The shell interface's and data (including ConIo) are only valid during
  the applications Entry Point.  Once the application returns from it's
  entry point the data is freed by the invoking shell.

  Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
/*
 * Copyright (c) 2011, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *    * Neither the name of Intel Corporation nor the names of its
 *      contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <efi.h>
#include <efilib.h>
#include "efilinux.h"
#include "stdlib.h"
#include "efishellparm.h"
#include "efishellintf.h"
#include "boot.h"


#ifndef MAX_ARGV_CONTENTS_SIZE
# define MAX_CMDLINE_SIZE 1024
#endif
#ifndef MAX_ARGC
# define MAX_CMDLINE_ARGC 32
#endif

/*
  Parse LoadedImage options area, called only in case the regular
  shell protos are not available.

  Format of LoadedImage->LoadOptions appears to be a
  single-space-separated list of args (looks like the shell already
  pre-parses the input, it apparently folds several consecutive spaces
  into one):
    argv[0] space argv[1] (etc.) argv[N] space \0 cwd \0 other data
  For safety, we support the trailing \0 without a space before, as
  well as several consecutive spaces (-> several args).
*/
static
INTN
GetShellArgcArgvFromLoadedImage(
    EFI_HANDLE ImageHandle,
    CHAR16 **ResultArgv[]
    )
{
  EFI_STATUS Status;
  EFI_LOADED_IMAGE *LoadedImage = NULL;
  static CHAR16 ArgvContents[MAX_CMDLINE_SIZE];
  static CHAR16 *Argv[MAX_CMDLINE_ARGC], *ArgStart, *c;
  UINTN Argc = 0, BufLen;

  Status = uefi_call_wrapper(BS->OpenProtocol, 6,
                             ImageHandle,
                             &LoadedImageProtocol,
                             &LoadedImage,
                             ImageHandle,
                             NULL,
                             EFI_OPEN_PROTOCOL_GET_PROTOCOL
                             );
  if (EFI_ERROR(Status))
    return -1;

  BufLen = LoadedImage->LoadOptionsSize;
  if (BufLen < 2)  /* We are expecting at least a \0 */
    return -1;
  else if (BufLen > sizeof(ArgvContents))
    BufLen = sizeof(ArgvContents);

  CopyMem(ArgvContents, LoadedImage->LoadOptions, BufLen);
  ArgvContents[MAX_CMDLINE_SIZE - 1] = L'\0';

  for (c = ArgStart = ArgvContents ; *c != L'\0' ; ++c) {
    if (*c == L' ') {
      *c = L'\0';
      if (Argc < MAX_CMDLINE_ARGC) Argv[Argc++] = ArgStart;
      ArgStart = c + 1;
    }
  }

  if ((*ArgStart != L'\0') && (Argc < MAX_CMDLINE_ARGC))
    Argv[Argc++] = ArgStart;

  // Print(L"Got argc/argv from loaded image proto\n");
  *ResultArgv = Argv;
  return Argc;
}

INTN GetShellArgcArgv(EFI_HANDLE ImageHandle, CHAR16 **Argv[])
{
  // Code inspired from EDK2's
  // ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.c (BSD)
  EFI_STATUS Status;
  static const EFI_GUID EfiShellParametersProtocolGuid
      = EFI_SHELL_PARAMETERS_PROTOCOL_GUID;
  static const EFI_GUID ShellInterfaceProtocolGuid
      = SHELL_INTERFACE_PROTOCOL_GUID;
  EFI_SHELL_PARAMETERS_PROTOCOL *EfiShellParametersProtocol = NULL;
  EFI_SHELL_INTERFACE *EfiShellInterfaceProtocol = NULL;

  Status = uefi_call_wrapper(BS->OpenProtocol, 6,
                             ImageHandle,
                             (EFI_GUID*)&EfiShellParametersProtocolGuid,
                             (VOID **)&EfiShellParametersProtocol,
                             ImageHandle,
                             NULL,
                             EFI_OPEN_PROTOCOL_GET_PROTOCOL
                             );
  if (!EFI_ERROR(Status))
  {
    // use shell 2.0 interface
    // Print(L"Got argc/argv from shell intf proto\n");
    *Argv = EfiShellParametersProtocol->Argv;
    return EfiShellParametersProtocol->Argc;
  }

  // try to get shell 1.0 interface instead.
  Status = uefi_call_wrapper(BS->OpenProtocol, 6,
                             ImageHandle,
                             (EFI_GUID*)&ShellInterfaceProtocolGuid,
                             (VOID **)&EfiShellInterfaceProtocol,
                             ImageHandle,
                             NULL,
                             EFI_OPEN_PROTOCOL_GET_PROTOCOL
                             );
  if (!EFI_ERROR(Status))
  {
    // Print(L"Got argc/argv from shell params proto\n");
    *Argv = EfiShellInterfaceProtocol->Argv;
    return EfiShellInterfaceProtocol->Argc;
  }

  // shell 1.0 and 2.0 interfaces failed
  return GetShellArgcArgvFromLoadedImage(ImageHandle, Argv);
}
