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

  
 
 
#define JSON_PARSER_PROTOCOL_GUID \
{ 0x462C5BBA, 0x2F3D, 0x4BDD, { 0xBC, 0x37, 0x6A, 0x1A, 0xED, 0x3D, 0xDE, 0x79 }}

extern EFI_GUID gJsonParserProtocolGuid;


// *****************************************************************************
// Related Definitions - Public defines, typedefs and structs
// *****************************************************************************

typedef struct _JSON_PARSER_PROTOCOL JSON_PARSER_PROTOCOL; 
    
/* Types and enums */
typedef UINT64 SIZE_T;
typedef struct json_object_t JSON_Object;
typedef struct json_array_t  JSON_Array;
typedef struct json_value_t  JSON_Value;

typedef enum json_value_type {
    JSONError   = 0,
    JSONNull    = 1,
    JSONString  = 2,
    JSONNumber  = 3,
    JSONObject  = 4,
    JSONArray   = 5,
    JSONBoolean = 6
};

typedef INTN JSON_Value_Type;

    
typedef enum json_result_t {
    JSONSuccess = 0,
    JSONFailure = 0xff
};

typedef INTN JSON_Status;
    
typedef VOID * (*JSON_Malloc_Function)(SIZE_T);
typedef VOID   (*JSON_Free_Function)(void *);

/* Call only once, before calling any other function from parson API. If not called, malloc and free
   from stdlib will be used for all allocations */
void Json_Set_Allocation_Functions(JSON_Malloc_Function malloc_fun, JSON_Free_Function free_fun);
    
/* Parses first JSON value in a file, returns NULL in case of error */
JSON_Value * Json_Parse_File(CONST CHAR8 *Filename);

/* Parses first JSON value in a file and ignores comments (/ * * / and //),
   returns NULL in case of error */
JSON_Value * Json_Parse_File_With_Comments(CONST CHAR8 *Filename);
    
/*  Parses first JSON value in a string, returns NULL in case of error */
JSON_Value * Json_Parse_String(CONST CHAR8 *string);

/*  Parses first JSON value in a string and ignores comments (/ * * / and //),
    returns NULL in case of error */
JSON_Value * json_parse_string_with_comments(CONST CHAR8 *string);
    
/* Serialization */
SIZE_T      json_serialization_size(CONST JSON_Value *value); /* returns 0 on fail */
JSON_Status json_serialize_to_buffer(CONST JSON_Value *value, CHAR8 *buf, SIZE_T buf_size_in_bytes);
JSON_Status json_serialize_to_file(CONST JSON_Value *value, CONST CHAR8 *filename);
CHAR8 *      json_serialize_to_string(CONST JSON_Value *value);

/* Pretty serialization */
SIZE_T      json_serialization_size_pretty(CONST JSON_Value *value); /* returns 0 on fail */
JSON_Status json_serialize_to_buffer_pretty(CONST JSON_Value *value, CHAR8 *buf, SIZE_T buf_size_in_bytes);
JSON_Status json_serialize_to_file_pretty(CONST JSON_Value *value, CONST CHAR8 *filename);
CHAR8 *      json_serialize_to_string_pretty(CONST JSON_Value *value);

void        json_free_serialized_string(CHAR8 *string); /* frees string from json_serialize_to_string and json_serialize_to_string_pretty */

/* Comparing */
INTN  json_value_equals(CONST JSON_Value *a, CONST JSON_Value *b);
    
/* Validation
   This is *NOT* JSON Schema. It validates json by checking if object have identically 
   named fields with matching types.
   For example schema {"name":"", "age":0} will validate 
   {"name":"Joe", "age":25} and {"name":"Joe", "age":25, "gender":"m"},
   but not {"name":"Joe"} or {"name":"Joe", "age":"Cucumber"}.
   In case of arrays, only first value in schema is checked against all values in tested array.
   Empty objects ({}) validate all objects, empty arrays ([]) validate all arrays,
   null validates values of every type.
 */
JSON_Status json_validate(CONST JSON_Value *schema, CONST JSON_Value *value);
    
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
  ); /* returns 0xff on fail */

typedef
INTN
(EFI_API JSON_OBJECT_GET_BOOLEAN)(
  CONST JSON_Object *Object,
  CONST CHAR8 *Name
  ); /* returns 0xff on fail */

/* dotget functions enable addressing values with dot notation in nested objects,
 just like in structs or c++/java/c# objects (e.g. objectA.objectB.value).
 Because valid names in JSON can contain dots, some values may be inaccessible
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
  
  


/* Functions to get available names */
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
    
    
/* Creates new name-value pair or frees and replaces old value with a new one. 
 * json_object_set_value does not copy passed value so it shouldn't be freed afterwards. */
typedef
JSON_STATUS
(EFIAPI *JSON_OBJECT_SET_VALUE)(
  JSON_Object *object,
  CONST CHAR8 *Name,
  JSON_Value *Value
  );




JSON_Status json_object_set_string(JSON_Object *object, CONST CHAR8 *name, CONST CHAR8 *string);
JSON_Status json_object_set_number(JSON_Object *object, CONST CHAR8 *name, double number);
JSON_Status json_object_set_boolean(JSON_Object *object, CONST CHAR8 *name, INTNboolean);
JSON_Status json_object_set_null(JSON_Object *object, CONST CHAR8 *name);

/* Works like dotget functions, but creates whole hierarchy if necessary.
 * json_object_dotset_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_object_dotset_value(JSON_Object *object, CONST CHAR8 *name, JSON_Value *value);
JSON_Status json_object_dotset_string(JSON_Object *object, CONST CHAR8 *name, CONST CHAR8 *string);
JSON_Status json_object_dotset_number(JSON_Object *object, CONST CHAR8 *name, double number);
JSON_Status json_object_dotset_boolean(JSON_Object *object, CONST CHAR8 *name, INTNboolean);
JSON_Status json_object_dotset_null(JSON_Object *object, CONST CHAR8 *name);

/* Frees and removes name-value pair */
JSON_Status json_object_remove(JSON_Object *object, CONST CHAR8 *name);

/* Works like dotget function, but removes name-value pair only on exact match. */
JSON_Status json_object_dotremove(JSON_Object *object, CONST CHAR8 *key);

/* Removes all name-value pairs in object */
JSON_Status json_object_clear(JSON_Object *object);
    
/* 
 *JSON Array 
 */
JSON_Value  * json_array_get_value  (CONST JSON_Array *array, SIZE_T index);
CONST CHAR8  * json_array_get_string (CONST JSON_Array *array, SIZE_T index);
JSON_Object * json_array_get_object (CONST JSON_Array *array, SIZE_T index);
JSON_Array  * json_array_get_array  (CONST JSON_Array *array, SIZE_T index);
double        json_array_get_number (CONST JSON_Array *array, SIZE_T index); /* returns 0 on fail */
INTN          json_array_get_boolean(CONST JSON_Array *array, SIZE_T index); /* returns -1 on fail */
SIZE_T        json_array_get_count  (CONST JSON_Array *array);
    
/* Frees and removes value at given index, does nothing and returns JSONFailure if index doesn't exist.
 * Order of values in array may change during execution.  */
JSON_Status json_array_remove(JSON_Array *array, SIZE_T i);

/* Frees and removes from array value at given index and replaces it with given one.
 * Does nothing and returns JSONFailure if index doesn't exist. 
 * json_array_replace_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_array_replace_value(JSON_Array *array, SIZE_T i, JSON_Value *value);
JSON_Status json_array_replace_string(JSON_Array *array, SIZE_T i, CONST CHAR8* string);
JSON_Status json_array_replace_number(JSON_Array *array, SIZE_T i, double number);
JSON_Status json_array_replace_boolean(JSON_Array *array, SIZE_T i, INTNboolean);
JSON_Status json_array_replace_null(JSON_Array *array, SIZE_T i);

/* Frees and removes all values from array */
JSON_Status json_array_clear(JSON_Array *array);

/* Appends new value at the end of array.
 * json_array_append_value does not copy passed value so it shouldn't be freed afterwards. */
JSON_Status json_array_append_value(JSON_Array *array, JSON_Value *value);
JSON_Status json_array_append_string(JSON_Array *array, CONST CHAR8 *string);
JSON_Status json_array_append_number(JSON_Array *array, double number);
JSON_Status json_array_append_boolean(JSON_Array *array, INTNboolean);
JSON_Status json_array_append_null(JSON_Array *array);
    
/*
 *JSON Value
 */
JSON_Value * json_value_init_object (void);
JSON_Value * json_value_init_array  (void);
JSON_Value * json_value_init_string (CONST CHAR8 *string); /* copies passed string */
JSON_Value * json_value_init_number (double number);
JSON_Value * json_value_init_boolean(INTNboolean);
JSON_Value * json_value_init_null   (void);
JSON_Value * json_value_deep_copy   (CONST JSON_Value *value);
void         json_value_free        (JSON_Value *value);

JSON_Value_Type json_value_get_type   (CONST JSON_Value *value);
JSON_Object *   json_value_get_object (CONST JSON_Value *value);
JSON_Array  *   json_value_get_array  (CONST JSON_Value *value);
CONST CHAR8  *   json_value_get_string (CONST JSON_Value *value);
double          json_value_get_number (CONST JSON_Value *value);
INTN            json_value_get_boolean(CONST JSON_Value *value);

/* Same as above, but shorter */
JSON_Value_Type json_type   (CONST JSON_Value *value);
JSON_Object *   json_object (CONST JSON_Value *value);
JSON_Array  *   json_array  (CONST JSON_Value *value);
CONST CHAR8  *   json_string (CONST JSON_Value *value);
double          json_number (CONST JSON_Value *value);
INTN            json_boolean(CONST JSON_Value *value);
    
#ifdef __cplusplus
}
#endif

#endif
