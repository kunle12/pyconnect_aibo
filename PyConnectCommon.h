/*
 *  PyConnectCommon.h
 *  
 *  Copyright 2006, 2007, 2013, 2014 Xun Wang.
 *  This file is part of PyConnect.
 *
 *  PyConnect is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PyConnect is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PyConnectCommon_h_DEFINED
#define PyConnectCommon_h_DEFINED

#ifdef PYTHON_SERVER
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include "Python.h"
#endif
#include <string>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PYCONNECT_MSG_BUFFER_SIZE 10240

#ifdef OPENR_OBJECT
// define missing struct sockaddr_in
#define AF_INET   2
/* POSIX.1g specifies this type name for the `sa_family' member.  */
typedef unsigned short int sa_family_t;

/* This macro is used to declare the initial common members
   of the data types used for socket addresses, `struct sockaddr',
   `struct sockaddr_in', `struct sockaddr_un', etc.  */

#define __SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family

#define __SOCKADDR_COMMON_SIZE  (sizeof (unsigned short int))

struct sockaddr {
    __SOCKADDR_COMMON (sa_);    /* Common data: address family and length.  */
    char sa_data[14];           /* Address data.  */
};
struct in_addr {
  unsigned int s_addr;
};
struct sockaddr_in {
  __SOCKADDR_COMMON (sin_);
  unsigned short sin_port;                 /* Port number.  */
  struct in_addr sin_addr;            /* Internet address.  */

  /* Pad to size of `struct sockaddr'.  */
  unsigned char sin_zero[sizeof (struct sockaddr) -
                         __SOCKADDR_COMMON_SIZE -
                         sizeof (unsigned short) -
                         sizeof (struct in_addr)];
};
#endif

#ifdef RELEASE
#define PYCONNECT_LOGGING_INIT
#define PYCONNECT_LOGGING_DECLARE( LOGNAME )
#if defined( WIN32 ) || defined( SUN_COMPILER )
#define DEBUG_MSG( ... )
#define ERROR_MSG( ... )
#define WARNING_MSG( ... )
#define INFO_MSG( ... )
#else
#define DEBUG_MSG( MSG... )
#define ERROR_MSG( MSG... )
#define WARNING_MSG( MSG... )
#define INFO_MSG( MSG... )
#endif
#define PYCONNECT_LOGGING_FINI
#else
#ifdef OPENR_LOG
#define PYCONNECT_LOGGING_INIT
#define DEBUG_MSG( MSG... ) OSYSDEBUG(( MSG ))
#define ERROR_MSG( MSG... ) OSYSLOG1(( osyslogERROR, MSG ))
#define WARNING_MSG( MSG... ) OSYSLOG1(( osyslogWARNING, MSG ))
#define INFO_MSG( MSG... ) OSYSLOG1(( osyslogINFO, MSG ))
#else
#define PYCONNECT_LOGGING_DECLARE( LOGNAME ) \
  FILE * s_pyconnectlog = NULL; \
  char * logFileName = (char *)LOGNAME

extern FILE * s_pyconnectlog;
#define PYCONNECT_LOGGING_INIT \
  s_pyconnectlog = fopen( logFileName, "a" )

//#define s_pyconnectlog stdout

#if defined( WIN32 ) || defined( SUN_COMPILER )
#define DEBUG_MSG(...) \
fprintf( s_pyconnectlog, "DEBUG: " ); \
fprintf( s_pyconnectlog, __VA_ARGS__ ); \
fflush( s_pyconnectlog )

#define INFO_MSG(...) \
fprintf( s_pyconnectlog, "INFO: " ); \
fprintf( s_pyconnectlog, __VA_ARGS__ ); \
fflush( s_pyconnectlog )

#define WARNING_MSG(...) \
fprintf( s_pyconnectlog, "WARNING: " ); \
fprintf( s_pyconnectlog, __VA_ARGS__ ); \
fflush( s_pyconnectlog )

#define ERROR_MSG(...) \
fprintf( s_pyconnectlog, "ERROR: " ); \
fprintf( s_pyconnectlog, __VA_ARGS__ ); \
fflush( s_pyconnectlog )
#else // !WIN32
#define DEBUG_MSG( MSG... ) \
fprintf( s_pyconnectlog, "DEBUG: " ); \
fprintf( s_pyconnectlog, MSG ); \
fflush( s_pyconnectlog )

#define INFO_MSG( MSG... ) \
fprintf( s_pyconnectlog, "INFO: " ); \
fprintf( s_pyconnectlog, MSG ); \
fflush( s_pyconnectlog )

#define WARNING_MSG( MSG... ) \
fprintf( s_pyconnectlog, "WARNING: " ); \
fprintf( s_pyconnectlog, MSG ); \
fflush( s_pyconnectlog )

#define ERROR_MSG( MSG... ) \
fprintf( s_pyconnectlog, "ERROR: " ); \
fprintf( s_pyconnectlog, MSG ); \
fflush( s_pyconnectlog )
#endif

#define PYCONNECT_LOGGING_FINI fclose( s_pyconnectlog )
#endif
#endif // define RELEASE

#ifndef DEFINED_ENCRYPTION_KEY
#pragma message ( "Make sure you change the encryption key for network communication. \
                   To disable this warning, set DEFINED_ENCRYPTION_KEY macro to true." )
#endif

namespace pyconnect {

const int MAX_STR_LENGTH = 32767;
const char PYCONNECT_MSG_INIT  = '#';
const char PYCONNECT_MSG_END   = '@';

typedef enum {
  MODULE_DISCOVERY      = 0x1,
  MODULE_DECLARE        = 0x2,
  MODULE_ASSIGN_ID      = 0x3,
  ATTR_METD_EXPOSE      = 0x4,
  CALL_ATTR_METD        = 0x5,
  CALL_ATTR_METD_NOCB   = 0x6,
  ATTR_METD_RESP        = 0x7,
  ATTR_VALUE_UPDATE     = 0x8,
  GET_ATTR_METD_DESC    = 0x9,
  ATTR_METD_DESC_RESP   = 0xa,
  MODULE_SHUTDOWN       = 0xb,
  SERVER_SHUTDOWN       = 0xc,
  PEER_SERVER_DISCOVERY = 0xd, // TODO: to be implemented.
  PEER_SERVER_MSG       = 0xe
} PyConnectMsg;

typedef enum {
  NO_ERRORS             = 0x0,
  NO_ATTR_METD          = 0x1,
  STR_TOO_LONG          = 0x2,
  INDEX_MISMATCH        = 0x4,
  NO_PYCONNECT_OBJECT   = 0x8,
  METD_EXCEPTION        = 0xa,
  MSG_CORRUPTED         = 0xf
} PyConnectMsgStatus;

class PyConnectType {
public:
  typedef enum {
    INT  = 0,
    FLOAT  = 1,
    DOUBLE  = 2, // same as float for Python in fact.
    STRING  = 3,
    BOOL  = 4,
    COMPOSITE = 5,
    PyVOID  = 6 // silly window compile breaks if defined as VOID
  } Type;
  
  static std::string typeName( Type type );
  static Type typeName( const char * type );
#ifdef PYTHON_SERVER
  static int validateTypeAndSize( PyObject * obj, Type type );
  static PyObject * defaultValue( Type type );
  static void packToStr( PyObject * arg, Type type, unsigned char * & dataPtr );
  static PyObject * unpackStr( Type type, unsigned char * & dataPtr, int & remainingLength );
#endif
};

template <class DataType> static int packToLENumber( const DataType & v, unsigned char * & dataPtr )
{
  // TODO: optimise this operation
  int dataLength = sizeof( DataType );
#ifdef WITH_BIG_ENDIAN
  char * valPtr = (char *)&v;
  for (int i = dataLength-1;i >= 0;i--) {
//  for (int i = 0;i < dataLength;i++) {
//    *dataPtr = (char)( (v >> (8*(dataLength - i - 1))) & 0xff );
    *dataPtr = (*(valPtr+i) & 0xFF);
    dataPtr++;
  }
#else
  memcpy( dataPtr, (char *)&v, dataLength );
  dataPtr += dataLength;
#endif
  return dataLength;
}

template <class DataType> static void unpackLENumber( DataType & val, unsigned char * & dataPtr, int & remainingLength )
{
  // TODO: optimise this operation
  int dataLength = sizeof( DataType );
#ifdef WITH_BIG_ENDIAN
  // convert to Big Endian
  char * valPtr = (char *)&val;
  for (int i = dataLength-1;i >= 0;i--) {
    *(valPtr+i) = (*dataPtr & 0xFF);
    dataPtr++;
  }
#else
  memcpy( (char *)&val, dataPtr, dataLength );
  dataPtr += dataLength;
#endif
  remainingLength -= dataLength;
}

void packString( unsigned char * str, int length, unsigned char * & dataBufPtr,
        bool extendSize = false );
std::string unpackString( unsigned char * & dataBufPtr, int & remainingBytes,
               bool extendSize = false );
int unpackStrToInt( unsigned char * & str, int & remainingBytes );
int packIntToStr( int num, unsigned char * & str );
int packedIntLen( int num );

#ifdef __cplusplus
extern "C" {
#endif
  unsigned char * decodeBase64( char * input, size_t * outLen );
  char * encodeBase64( const unsigned char * input, size_t length );
  void endecryptInit();
  void endecryptFini();
  int decryptMessage( const unsigned char * origMesg, int origMesgLength, unsigned char ** decryptedMesg, int * decryptedMesgLength );
  int encryptMessage( const unsigned char * origMesg, int origMesgLength, unsigned char ** encryptedMesg, int * encryptedMesgLength );
  int secureSHA256Hash( const unsigned char * password, const int pwlen, unsigned char * code );
  
#ifdef __cplusplus
}
#endif
  
} // namespace pyconnect

#endif  // PyConnectCommon_h_DEFINED
