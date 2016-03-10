
#ifndef __JSON_PARSER_DXE_H__
#define __JSON_PARSER_DXE_H__

  
#include <Protocol/JsonParser.h>
#include <Protocol/SimpleFileSystem.h>

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>


// *****************************************************************************
// private definitions and data structures
// *****************************************************************************

#define JSON_PARSER_DRIVER_VERSION 0x01000000

#define JSON_PARSER_PRIVATE_SIGNATURE SIGNATURE_32 ('J', 'S', 'O', 'N')

#define JOSN_PARSER_PRIVATE_FROM_THIS(a) \
            CR(a, JSON_PARSER_PRIVATE_DATA, \
                Json, \
                JSON_PARSER_PRIVATE_SIGNATURE)


/// @brief Private data managed by the iDRAC Inventory PCI Driver.
typedef struct {
  UINTN                     Signature;
  UINT32                    Version;
  EFI_HANDLE                DriverHandle;
  JSON_PARSER_PROTOCOL      Json;
} JSON_PARSER_PRIVATE_DATA;



typedef union
{
  INTN  value;
  struct
  {
    UINT32 msw;
    UINT32 lsw;
  } parts;
} ieee_double_shape_type;

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)          \
do {                \
  ieee_double_shape_type gh_u;          \
  gh_u.value = (d);           \
  (i) = gh_u.parts.msw;           \
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)         \
do {                \
  ieee_double_shape_type gl_u;          \
  gl_u.value = (d);           \
  (i) = gl_u.parts.lsw;           \
} while (0)

/* Set the more significant 32 bits of a double from an int.  */

#define SET_HIGH_WORD(d,v)          \
do {                \
  ieee_double_shape_type sh_u;          \
  sh_u.value = (d);           \
  sh_u.parts.msw = (v);           \
  (d) = sh_u.value;           \
} while (0)


VOID EFIAPI SafreFreePool(IN VOID *Buffer)
{
    if(Buffer != NULL)
    {
        FreePool(Buffer);
    }
}

UINTN EFIAPI AsciiStringLength (IN CONST CHAR8 *String)
{
    UINTN   Length;
    if(!String)
    {
        return 0;
    }
    Length = AsciiStrLen(String);
    return Length;
}


#define isspace(X)              ((X) == ' ' || (X) == '\t' || (X) == '\n' || (X) == '\r')

#define IsXDigit(ch) (((ch>='A' && ch <='F') || ch>='a' && ch <='f' || (ch>='0' && ch <='9')) ? 1 : 0)

//File APIs
#define SEEK_SET                0
#define SEEK_CUR                1
#define SEEK_END                2

#define EOF                     -1

#define malloc                 AllocateZeroPool
#define free                   SafreFreePool
#define strstr                 AsciiStrStr
#define strlen                 AsciiStringLength
#define strncpy                AsciiStrnCpy
#define strchr(str,ch)         ScanMem8((VOID *)(str),AsciiStrSize(str),(UINT8)ch)
#define memcpy                 CopyMem
#define memset                 SetMem
#define strcmp                 AsciiStrCmp
#define strncmp                AsciiStrnCmp

#define FILE                   EFI_FILE  
//#define double                 INTN



/* Call only once, before calling any other function from parson API. If not called, malloc and free
   from stdlib will be used for all allocations */
void json_set_allocation_functions(JSON_Malloc_Function malloc_fun, JSON_Free_Function free_fun);
    
///* Parses first JSON value in a file, returns NULL in case of error */
//JSON_Value * json_parse_file(const CHAR8*filename);
JSON_Value * 
json_parse_file( 
  const CHAR8 *filename
  );

//
///* Parses first JSON value in a file and ignores comments (/ * * / and //),
//   returns NULL in case of error */
//JSON_Value * json_parse_file_with_comments(const CHAR8*filename);
//
///*  Parses first JSON value in a string, returns NULL in case of error */
JSON_Value * json_parse_string(const CHAR8 *string);
//
///*  Parses first JSON value in a string and ignores comments (/ * * / and //),
//    returns NULL in case of error */
JSON_Value * json_parse_string_with_comments(const CHAR8*string);
    
/* Serialization */
SIZE_T      json_serialization_size(const JSON_Value *value); /* returns 0 on fail */
JSON_Status json_serialize_to_buffer(const JSON_Value *value, CHAR8*buf, SIZE_T buf_size_in_bytes);
JSON_Status json_serialize_to_file(const JSON_Value *value, const CHAR8*filename);
CHAR8*      json_serialize_to_string(const JSON_Value *value);

/* Pretty serialization */
SIZE_T      json_serialization_size_pretty(const JSON_Value *value); /* returns 0 on fail */
JSON_Status json_serialize_to_buffer_pretty(const JSON_Value *value, CHAR8*buf, SIZE_T buf_size_in_bytes);
JSON_Status json_serialize_to_file_pretty(const JSON_Value *value, const CHAR8*filename);
CHAR8*      json_serialize_to_string_pretty(const JSON_Value *value);

void        json_free_serialized_string(CHAR8*string); /* frees string from json_serialize_to_string and json_serialize_to_string_pretty */

/* Comparing */
INTN  json_value_equals(const JSON_Value *a, const JSON_Value *b);
    
/* Validation
   This is *NOT* JSON Schema. It validates Json by checking if Object have identically 
   Named fields with matching types.
   For example schema {"Name":"", "age":0} will validate 
   {"Name":"Joe", "age":25} and {"Name":"Joe", "age":25, "gender":"m"},
   but not {"Name":"Joe"} or {"Name":"Joe", "age":"Cucumber"}.
   In case of arrays, only first value in schema is checked against all values in tested array.
   Empty Objects ({}) validate all Objects, empty arrays ([]) validate all arrays,
   null validates values of every type.
 */
JSON_Status Json_validate(CONST JSON_Value *schema, CONST JSON_Value *value);
    

/*
 * JSON Object
 */
JSON_Value  * json_object_get_value  (const JSON_Object *object, const CHAR8 *name);
const CHAR8  * json_object_get_string (const JSON_Object *object, const CHAR8 *name);
JSON_Object * json_object_get_object (const JSON_Object *object, const CHAR8 *name);
JSON_Array  * json_object_get_array  (const JSON_Object *object, const CHAR8 *name);
INTN         json_object_get_number (const JSON_Object *object, const CHAR8 *name); /* returns 0 on fail */
INTN          json_object_get_boolean(const JSON_Object *object, const CHAR8 *name); /* returns -1 on fail */

/* dotget functions enable addressing values with dot notation in nested objects,
 just like in structs or c++/java/c# objects (e.g. objectA.objectB.value).
 Because valid names in JSON can contain dots, some values may be inaccessible
 this way. */
JSON_Value  * json_object_dotget_value  (const JSON_Object *object, const CHAR8 *name);
const CHAR8  * json_object_dotget_string (const JSON_Object *object, const CHAR8 *name);
JSON_Object * json_object_dotget_object (const JSON_Object *object, const CHAR8 *name);
JSON_Array  * json_object_dotget_array  (const JSON_Object *object, const CHAR8 *name);
INTN         json_object_dotget_number (const JSON_Object *object, const CHAR8 *name); /* returns 0 on fail */
INTN           json_object_dotget_boolean(const JSON_Object *object, const CHAR8 *name); /* returns -1 on fail */

/* Functions to get available names */
SIZE_T        json_object_get_count(const JSON_Object *object);
const CHAR8  * json_object_get_name (const JSON_Object *object, SIZE_T index);
    
/* Creates new name-value pair or frees and replaces old value with a new one. 
 * json_object_set_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_object_set_value(JSON_Object *object, const CHAR8 *name, JSON_Value *value);
JSON_Status json_object_set_string(JSON_Object *object, const CHAR8 *name, const CHAR8 *string);
JSON_Status json_object_set_number(JSON_Object *object, const CHAR8 *name, INTN  number);
JSON_Status json_object_set_boolean(JSON_Object *object, const CHAR8 *name, INTN boolean);
JSON_Status json_object_set_null(JSON_Object *object, const CHAR8 *name);

/* Works like dotget functions, but creates whole hierarchy if necessary.
 * json_object_dotset_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_object_dotset_value(JSON_Object *object, const CHAR8 *name, JSON_Value *value);
JSON_Status json_object_dotset_string(JSON_Object *object, const CHAR8 *name, const CHAR8 *string);
JSON_Status json_object_dotset_number(JSON_Object *object, const CHAR8 *name, INTN  number);
JSON_Status json_object_dotset_boolean(JSON_Object *object, const CHAR8 *name, INTN boolean);
JSON_Status json_object_dotset_null(JSON_Object *object, const CHAR8 *name);

/* Frees and removes name-value pair */
JSON_Status json_object_remove(JSON_Object *object, const CHAR8 *name);

/* Works like dotget function, but removes name-value pair only on exact match. */
JSON_Status json_object_dotremove(JSON_Object *object, const CHAR8 *key);

/* Removes all name-value pairs in object */
JSON_Status json_object_clear(JSON_Object *object);
    
/* 
 *JSON Array 
 */
JSON_Value  * json_array_get_value  (const JSON_Array *array, SIZE_T index);
const CHAR8  * json_array_get_string (const JSON_Array *array, SIZE_T index);
JSON_Object * json_array_get_object (const JSON_Array *array, SIZE_T index);
JSON_Array  * json_array_get_array  (const JSON_Array *array, SIZE_T index);
INTN         json_array_get_number (const JSON_Array *array, SIZE_T index); /* returns 0 on fail */
INTN           json_array_get_boolean(const JSON_Array *array, SIZE_T index); /* returns -1 on fail */
SIZE_T        json_array_get_count  (const JSON_Array *array);
    
/* Frees and removes value at given index, does nothing and returns JSONFailure if index doesn't exist.
 * Order of values in array may change during execution.  */
JSON_Status json_array_remove(JSON_Array *array, SIZE_T i);

/* Frees and removes from array value at given index and replaces it with given one.
 * Does nothing and returns JSONFailure if index doesn't exist. 
 * json_array_replace_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_array_replace_value(JSON_Array *array, SIZE_T i, JSON_Value *value);
JSON_Status json_array_replace_string(JSON_Array *array, SIZE_T i, const CHAR8* string);
JSON_Status json_array_replace_number(JSON_Array *array, SIZE_T i, INTN  number);
JSON_Status json_array_replace_boolean(JSON_Array *array, SIZE_T i, INTN boolean);
JSON_Status json_array_replace_null(JSON_Array *array, SIZE_T i);

/* Frees and removes all values from array */
JSON_Status json_array_clear(JSON_Array *array);

/* Appends new value at the end of array.
 * json_array_append_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_array_append_value(JSON_Array *array, JSON_Value *value);
JSON_Status json_array_append_string(JSON_Array *array, const CHAR8 *string);
JSON_Status json_array_append_number(JSON_Array *array, INTN  number);
JSON_Status json_array_append_boolean(JSON_Array *array, INTN boolean);
JSON_Status json_array_append_null(JSON_Array *array);
    
/*
 *JSON Value
 */
JSON_Value * json_value_init_object (void);
JSON_Value * json_value_init_array  (void);
JSON_Value * json_value_init_string (const CHAR8 *string); /* copies passed string */
JSON_Value * json_value_init_number (INTN  number);
JSON_Value * json_value_init_boolean(INTN boolean);
JSON_Value * json_value_init_null   (void);
JSON_Value * json_value_deep_copy   (const JSON_Value *value);
void         json_value_free        (JSON_Value *value);

JSON_Value_Type json_value_get_type   (const JSON_Value *value);
JSON_Object *   json_value_get_object (const JSON_Value *value);
JSON_Array  *   json_value_get_array  (const JSON_Value *value);
const CHAR8  *   json_value_get_string (const JSON_Value *value);
INTN           json_value_get_number (const JSON_Value *value);
INTN             json_value_get_boolean(const JSON_Value *value);

/* Same as above, but shorter */
JSON_Value_Type json_type   (const JSON_Value *value);
JSON_Object *   json_object (const JSON_Value *value);
JSON_Array  *   json_array  (const JSON_Value *value);
const CHAR8  *   json_string (const JSON_Value *value);
INTN           json_number (const JSON_Value *value);
INTN             json_boolean(const JSON_Value *value);
    

#endif
