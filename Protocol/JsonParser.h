/*
 Parson ( http://kgabis.github.com/parson/ )
 Copyright (c) 2012 - 2016 Krzysztof Gabis
 
 Permission is hereby granted, free of CHAR8ge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

  
 // The version of the protocol installed by the driver.
#define DELL_JSON_PARSER_PROTOCOL_VERSION 0x01000000
 
#define JSON_PARSER_PROTOCOL_GUID \
{ 0x462C5BBA, 0x2F3D, 0x4BDD, { 0xBC, 0x37, 0x6A, 0x1A, 0xED, 0x3D, 0xDE, 0x79 }}

//extern EFI_GUID gJsonParserProtocolGuid;


// *****************************************************************************
// Related Definitions - Public defines, typedefs and structs
// *****************************************************************************

typedef struct _JSON_PARSER_PROTOCOL JSON_PARSER_PROTOCOL; 
    
/* Types and enums */
typedef UINT64 SIZE_T;
typedef struct Json_Object_T JSON_Object;
typedef struct Json_Array_T  JSON_Array;
typedef struct Json_Value_T  JSON_Value;

typedef enum Json_value_type {
    JSONError   = 0,
    JSONNull    = 1,
    JSONString  = 2,
    JSONNumber  = 3,
    JSONObject  = 4,
    JSONArray   = 5,
    JSONBoolean = 6
};

typedef INTN JSON_Value_Type;

    
typedef enum Json_result_t {
    JSONSuccess = 1,
    JSONFailure = 0
};

typedef INTN JSON_Status;
    
typedef VOID * (*JSON_Malloc_Function)(SIZE_T);
typedef VOID   (*JSON_Free_Function)(void *);


typedef
UINT32
(EFIAPI *JSON_PARSER_GET_VERSION)(
  VOID
);


/* Parses first JSON value in a file, returns NULL in case of error */
typedef
JSON_Value *
(EFIAPI *DELL_JSON_PARSE_FILE)(
  const CHAR8 *filename
);

/* Parses first JSON value in a file and ignores comments (/ * * / and //),
   returns NULL in case of error */
typedef
JSON_Value *
(EFIAPI *JSON_PARSE_FILE_WITH_COMMENTS)(
  const CHAR8 *filename
);
    
/*  Parses first JSON value in a string, returns NULL in case of error */
typedef
JSON_Value *
(EFIAPI *JSON_PARSE_STRING)(
  const CHAR8 *string
);

/*  Parses first JSON value in a string and ignores comments (/ * * / and //),
    returns NULL in case of error */
typedef
JSON_Value *
(EFIAPI *JSON_PARSE_STRING_WITH_COMMENTS)(
  const CHAR8 *string
);




/*
 * JSON Object
 */
typedef
JSON_Value  * 
(EFIAPI *JSON_OBJECT_GET_VALUE)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );
typedef
CONST CHAR8 *
(EFIAPI *JSON_OBJECT_GET_STRING)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );


typedef
JSON_Object *
(EFIAPI *JSON_OBJECT_GET_OBJECT)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );

typedef
JSON_Array *
(EFIAPI *JSON_OBJECT_GET_ARRAY)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );


typedef
double        
(EFIAPI *JSON_OBJECT_GET_NUMBER)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  ); /* returns 0x1 on fail */

typedef
INTN
(EFIAPI *JSON_OBJECT_GET_BOOLEAN)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  ); /* returns 0x1 on fail */

/* dotget functions enable addressing values with dot notation in nested Objects,
 just like in structs or c++/java/c# Objects (e.g. ObjectA.ObjectB.value).
 Because valid Names in JSON can contain dots, some values may be inaccessible
 this way. */

typedef
JSON_Value *
(EFIAPI *JSON_OBJECT_DOTGET_VALUE)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );

typedef
CONST 
CHAR8 *
(EFIAPI *JSON_OBJECT_DOTGET_STRING)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );
  
typedef
JSON_Object *
(EFIAPI *JSON_OBJECT_DOTGET_OBJECT)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );
typedef
JSON_Array *
(EFIAPI *JSON_OBJECT_DOTGET_ARRAY)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );


typedef
INTN
(EFIAPI *JSON_OBJECT_DOTGET_NUMBER)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  ); /* returns 0 on fail */

typedef
INTN
(EFIAPI *JSON_OBJECT_DOTGET_BOOLEAN)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  );/* returns 0xff on fail */
  
  


/* Functions to get available Names */
typedef
SIZE_T
(EFIAPI *JSON_OBJECT_GET_COUNT)(
  CONST JSON_Object *Object
  );
  
typedef
CONST CHAR8 *
(EFIAPI *JSON_OBJECT_GET_NAME)(
  CONST JSON_Object *Object,
  SIZE_T Index
  );
    
    
/* Creates new Name-value pair or frees and replaces old value with a new one. 
 * Json_Object_set_value does not copy passed value so it shouldn't be freed afterwards. */
typedef
JSON_Status
(EFIAPI *JSON_OBJECT_SET_VALUE)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  JSON_Value *Value
  );


typedef
JSON_Status
(EFIAPI *JSON_OBJECT_SET_STRING)(
  JSON_Object *Object, 
  CONST CHAR8 *Name,
  CONST CHAR8 *String
  );

typedef
JSON_Status
(EFIAPI *JSON_OBJECT_SET_NUMBER)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  double Number
  );

//check.
typedef
JSON_Status
(EFIAPI *JSON_OBJECT_SET_BOOLEAN)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  INTN  Boolean
  );


typedef
JSON_Status
(EFIAPI *JSON_OBJECT_SET_NULL)(
  JSON_Object *Object,
  CONST CHAR8 *Name
  );

/* Works like dotget functions, but creates whole hierarchy if necessary.
 * Json_Object_Dotset_value does not copy passed value so it shouldn't be freed afterwards. */
typedef
JSON_Status
(EFIAPI *JSON_OBJECT_DOTSET_VALUE)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  JSON_Value *Value
  );

typedef
JSON_Status
(EFIAPI *JSON_OBJECT_DOTSET_STRING)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  CONST CHAR8 *String);

typedef
JSON_Status
(EFIAPI *JSON_OBJECT_DOTSET_NUMBER)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  double Number);

typedef
JSON_Status
(EFIAPI *JSON_OBJECT_DOTSET_BOOLEAN)(
  JSON_Object *Object,
  CONST CHAR8 *Name,
  INTN  Boolean);

typedef
JSON_Status
(EFIAPI *JSON_OBJECT_DOTSET_NULL)(
  JSON_Object *Object,
  CONST CHAR8 *Name);

/* Frees and removes Name-value pair */
typedef
JSON_Status
(EFIAPI *JSON_OBJECT_REMOVE)(
  JSON_Object *Object,
  CONST CHAR8 *Name);

/* Works like dotget function, but removes Name-value pair only on exact match. */
typedef
JSON_Status
(EFIAPI *JSON_OBJECT_DOTREMOVE)(
  JSON_Object *Object,
  CONST CHAR8 *Key);

/* Removes all Name-value pairs in Object */
typedef
JSON_Status
(EFIAPI *JSON_OBJECT_CLEAR)(
  JSON_Object *Object);
    
/* 
 *JSON Array 
 */
typedef
JSON_Value  * 
(EFIAPI *JSON_ARRAY_GET_VALUE)(
  CONST JSON_Array *Array,
  SIZE_T index);

typedef
CONST CHAR8  * 
(EFIAPI *JSON_ARRAY_GET_STRING)(
  CONST JSON_Array *Array,
  SIZE_T index);

typedef
JSON_Object * 
(EFIAPI *JSON_ARRAY_GET_OBJECT)(
  CONST JSON_Array *Array,
  SIZE_T index);
typedef
JSON_Array  * 
(EFIAPI *JSON_ARRAY_GET_ARRAY)(
  CONST JSON_Array *Array,
  SIZE_T index);

typedef
INTN        
(EFIAPI *JSON_ARRAY_GET_NUMBER)(
  CONST JSON_Array *Array,
  SIZE_T index); /* returns 0 on fail */

typedef
INTN          
(EFIAPI *JSON_ARRAY_GET_BOOLEAN)(
  CONST JSON_Array *Array,
  SIZE_T index); /* returns -1 on fail */

typedef
SIZE_T
(EFIAPI *JSON_ARRAY_GET_COUNT)(
  CONST JSON_Array *Array);
    
/* Frees and removes value at given index, does nothing and returns JSONFailure if index doesn't exist.
 * Order of values in Array may change during execution.  */
JSON_Status JSON_ARRAY_REMOVE(JSON_Array *Array, SIZE_T i);

/* Frees and removes from Array value at given index and replaces it with given one.
 * Does nothing and returns JSONFailure if index doesn't exist. 
 * Json_Array_replace_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status JSON_ARRAY_REPLACE_VALUE(JSON_Array *Array, SIZE_T i, JSON_Value *value);
JSON_Status JSON_ARRAY_REPLACE_STRING(JSON_Array *Array, SIZE_T i, CONST CHAR8* string);
JSON_Status JSON_ARRAY_REPLACE_NUMBER(JSON_Array *Array, SIZE_T i, double number);
JSON_Status JSON_ARRAY_REPLACE_BOOLEAN(JSON_Array *Array, SIZE_T i, INTN boolean);
JSON_Status JSON_ARRAY_REPLACE_NULL(JSON_Array *Array, SIZE_T i);

/* Frees and removes all values from Array */
JSON_Status JSON_ARRAY_CLEAR(JSON_Array *Array);

/* Appends new value at the end of Array.
 * Json_Array_append_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status JSON_ARRAY_APPEND_VALUE(JSON_Array *Array, JSON_Value *value);
JSON_Status JSON_ARRAY_APPEND_STRING(JSON_Array *Array, CONST CHAR8 *string);
JSON_Status JSON_ARRAY_APPEND_NUMBER(JSON_Array *Array, double number);
JSON_Status JSON_ARRAY_APPEND_BOOLEAN(JSON_Array *Array, INTN boolean);
JSON_Status JSON_ARRAY_APPEND_NULL(JSON_Array *Array);
    
/*
 *JSON Value
 */
JSON_Value * JSON_VALUE_INIT_OBJECT (void);
JSON_Value * JSON_VALUE_INIT_ARRAY  (void);
JSON_Value * JSON_VALUE_INIT_STRING (CONST CHAR8 *string); /* copies passed string */
JSON_Value * JSON_VALUE_INIT_NUMBER (double number);
JSON_Value * JSON_VALUE_INIT_BOOLEAN(INTN boolean);
JSON_Value * JSON_VALUE_INIT_NULL   (void);
JSON_Value * JSON_VALUE_DEEP_COPY   (CONST JSON_Value *value);
void         JSON_vALUE_FREE        (JSON_Value *value);

typedef
JSON_Value_Type 
(EFIAPI *JSON_VALUE_GET_TYPE)(
  CONST JSON_Value *value);

typedef
JSON_Object *
(EFIAPI *JSON_VALUE_GET_OBJECT)(
  CONST JSON_Value *value);

typedef
JSON_Array  *
(EFIAPI *JSON_VALUE_GET_ARRAY)(
  CONST JSON_Value *value);

typedef
CONST CHAR8  *
(EFIAPI *JSON_VALUE_GET_STRING)(
  CONST JSON_Value *value);

typedef
INTN          
(EFIAPI *JSON_VALUE_GET_NUMBER)(
  CONST JSON_Value *value);

typedef
INTN 
(EFIAPI *JSON_VALUE_GET_BOOLEAN)(
  CONST JSON_Value *value);

/* Same as above, but shorter */
JSON_Value_Type JSON_TYPE   (CONST JSON_Value *value);
JSON_Object *   JSON_OBJECT (CONST JSON_Value *value);
JSON_Array  *   JSON_ARRAY  (CONST JSON_Value *value);
CONST CHAR8  *   JSON_STRING (CONST JSON_Value *value);
double          JSON_NUMBER (CONST JSON_Value *value);
INTN            JSON_BOOLEAN(CONST JSON_Value *value);
    



// *****************************************************************************
// JSON Parser protocol interface definition
// *****************************************************************************
//typedef 
struct _JSON_PARSER_PROTOCOL {
  // Getters and setters
  JSON_PARSER_GET_VERSION         get_version;
  DELL_JSON_PARSE_FILE            json_parse_file;
  JSON_PARSE_FILE_WITH_COMMENTS   json_parse_file_with_comments;
  JSON_PARSE_STRING               json_parse_string;
  JSON_PARSE_STRING_WITH_COMMENTS json_parse_string_with_comments;

  JSON_OBJECT_GET_VALUE json_object_get_value;
  JSON_OBJECT_GET_STRING json_object_get_string;
  JSON_OBJECT_GET_OBJECT json_object_get_object;
  JSON_OBJECT_GET_ARRAY json_object_get_array;
  JSON_OBJECT_GET_NUMBER json_object_get_number;
  JSON_OBJECT_GET_BOOLEAN json_object_get_boolean;

  JSON_OBJECT_DOTGET_VALUE json_object_dotget_value;
  JSON_OBJECT_DOTGET_STRING json_object_dotget_string;
  JSON_OBJECT_DOTGET_OBJECT json_object_dotget_object;
  JSON_OBJECT_DOTGET_ARRAY json_object_dotget_array;
  JSON_OBJECT_DOTGET_NUMBER json_object_dotget_number;
  JSON_OBJECT_DOTGET_BOOLEAN json_object_dotget_boolean;

  JSON_OBJECT_GET_COUNT json_object_get_count;
  JSON_OBJECT_GET_NAME json_object_get_name;

  JSON_ARRAY_GET_VALUE json_array_get_value;
  JSON_ARRAY_GET_STRING json_array_get_string;
  JSON_ARRAY_GET_OBJECT json_array_get_object;
  JSON_ARRAY_GET_ARRAY json_array_get_array;
  JSON_ARRAY_GET_NUMBER json_array_get_number;
  JSON_ARRAY_GET_BOOLEAN json_array_get_boolean;
  JSON_ARRAY_GET_COUNT json_array_get_count;

  JSON_VALUE_GET_TYPE json_value_get_type;
  JSON_VALUE_GET_OBJECT json_value_get_object;
  JSON_VALUE_GET_ARRAY json_value_get_array;
  JSON_VALUE_GET_STRING json_value_get_string;
  JSON_VALUE_GET_NUMBER json_value_get_number;
  JSON_VALUE_GET_BOOLEAN json_value_get_boolean;
  
  JSON_OBJECT_SET_VALUE json_object_set_value;
  JSON_OBJECT_SET_STRING json_object_set_string;
  JSON_OBJECT_SET_NUMBER json_object_set_number;
  JSON_OBJECT_SET_BOOLEAN json_object_set_boolean;
  JSON_OBJECT_SET_NULL json_object_set_null;

  JSON_OBJECT_DOTSET_VALUE json_object_dotset_value;
  JSON_OBJECT_DOTSET_STRING json_object_dotset_string;
  JSON_OBJECT_DOTSET_NUMBER json_object_dotset_number;
  JSON_OBJECT_DOTSET_BOOLEAN json_object_dotset_boolean;
  JSON_OBJECT_DOTSET_NULL json_object_dotset_null;
  
  JSON_OBJECT_REMOVE json_object_remove;
  JSON_OBJECT_DOTREMOVE json_object_dotremove;


};

extern EFI_GUID gJsonParserProtocolGuid;

//extern EFI_GUID gDellEfiJsonProtocolGuid;

#endif
