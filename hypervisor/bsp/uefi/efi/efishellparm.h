/** @file
  EFI_SHELL_PARAMETERS_PROTOCOL as defined in the UEFI Shell 2.0 specification.

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
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


/*
 * This is based on ShellPkg/Include/Protocol/EfiShellParameters.h from EDK II.
 */

#ifndef __EFI_SHELL_PARAMETERS_PROTOCOL__
#define __EFI_SHELL_PARAMETERS_PROTOCOL__


// EDK2's ShellBase.h
typedef VOID *SHELL_FILE_HANDLE;

#define EFI_SHELL_PARAMETERS_PROTOCOL_GUID \
  { \
  0x752f3136, 0x4e16, 0x4fdc, { 0xa2, 0x2a, 0xe5, 0xf4, 0x68, 0x12, 0xf4, 0xca } \
  }

typedef struct _EFI_SHELL_PARAMETERS_PROTOCOL {
  ///
  /// Points to an Argc-element array of points to NULL-terminated strings containing
  /// the command-line parameters. The first entry in the array is always the full file
  /// path of the executable. Any quotation marks that were used to preserve
  /// whitespace have been removed.
  ///
  CHAR16 **Argv;

  ///
  /// The number of elements in the Argv array.
  ///
  UINTN Argc;

  ///
  /// The file handle for the standard input for this executable. This may be different
  /// from the ConInHandle in EFI_SYSTEM_TABLE.
  ///
  SHELL_FILE_HANDLE StdIn;

  ///
  /// The file handle for the standard output for this executable. This may be different
  /// from the ConOutHandle in EFI_SYSTEM_TABLE.
  ///
  SHELL_FILE_HANDLE StdOut;

  ///
  /// The file handle for the standard error output for this executable. This may be
  /// different from the StdErrHandle in EFI_SYSTEM_TABLE.
  ///
  SHELL_FILE_HANDLE StdErr;
} EFI_SHELL_PARAMETERS_PROTOCOL;

#endif
