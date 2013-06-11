/**************************************************************************
 * FreeDOS32 File System Layer                                            *
 * Wrappers for file system driver functions, SFT and JFT support         *
 * by Salvo Isaja                                                         *
 *                                                                        *
 * Copyright (C) 2002, Salvatore Isaja                                    *
 *                                                                        *
 * The FreeDOS32 File System Layer is part of FreeDOS32.                  *
 *                                                                        *
 * FreeDOS32 is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU General Public License as published by the  *
 * Free Software Foundation; either version 2 of the License, or (at your *
 * option) any later version.                                             *
 *                                                                        *
 * FreeDOS32 is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty                *
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with FreeDOS32; see the file COPYING; if not, write to the       *
 * Free Software Foundation, Inc.,                                        *
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                *
 **************************************************************************/

#ifndef __FD32_FILESYS_H
#define __FD32_FILESYS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* OPEN system call - Flags for opening mode and action taken  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Access codes */
//#define FD32_OACCESS  0x0007    /* Bit mask for access type       */
//#define FD32_OREAD    0x0000    /* Allow only reads from file     */
//#define FD32_OWRITE   0x0001    /* Allow only writes into file    */
//#define FD32_ORDWR    0x0002    /* Allow both reads and writes    */
//#define FD32_ORDNA    0x0004    /* Read only without updating the */
//                                /* last access date               */
                                
/* * * * * * * * * * * * * * * * * * * * * * * * */
/* CHMOD system call - Flags for file attributes */
/* * * * * * * * * * * * * * * * * * * * * * * * */
#define FD32_ARDONLY 0x01 /* Read only file                          */
#define FD32_AHIDDEN 0x02 /* Hidden file                             */
#define FD32_ASYSTEM 0x04 /* System file                             */
#define FD32_AVOLID  0x08 /* Volume label                            */
#define FD32_ADIR    0x10 /* Directory                               */
#define FD32_AARCHIV 0x20 /* File modified since last backup         */
#define FD32_ALNGNAM 0x0F /* Long file name directory slot (R+H+S+V) */
#define FD32_AALL    0x3F /* Select all attributes                   */
#define FD32_ANONE   0x00 /* Select no attributes                    */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Search flags for FINDFIRST/FINDNEXT and UNLINK system calls */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Allowable attributes */
#define FD32_FALLOW   0x00FF /* Allowable attributes mask */
#define FD32_FARDONLY FD32_ARDONLY
#define FD32_FAHIDDEN FD32_AHIDDEN
#define FD32_FASYSTEM FD32_ASYSTEM
#define FD32_FAVOLID  FD32_AVOLID
#define FD32_FADIR    FD32_ADIR
#define FD32_FAARCHIV FD32_AARCHIV
#define FD32_FAALL    FD32_AALL
#define FD32_FANONE   FD32_ANONE
///* Required attributes */
#define FD32_FREQUIRD 0xFF00 /* Allowable attributes mask */
#define FD32_FRRDONLY (FD32_ARDONLY << 8)
#define FD32_FRHIDDEN (FD32_AHIDDEN << 8)
#define FD32_FRSYSTEM (FD32_ASYSTEM << 8)
#define FD32_FRVOLID  (FD32_AVOLID  << 8)
#define FD32_FRDIR    (FD32_ADIR    << 8)
#define FD32_FRARCHIV (FD32_AARCHIV << 8)
#define FD32_FRALL    (FD32_AALL    << 8)
#define FD32_FRNONE   (FD32_ANONE   << 8)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* GEN_SHORT_FNAME system call - Mnemonics for flags and return values */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* TODO: Pack these huge mnemonics... */
#define FD32_GENSFN_FORMAT_MASK   (0xFF << 8)
#define FD32_GENSFN_FORMAT_FCB    (0x00 << 8)
#define FD32_GENSFN_FORMAT_NORMAL (0x01 << 8)

#define FD32_GENSFN_CASE_CHANGED  (1 << 0)
#define FD32_GENSFN_WAS_INVALID   (1 << 1)

#endif /* ifndef __FD32_FILESYS_H */

