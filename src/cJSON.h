/*
  版权所有 （c） 2009-2017 Dave Gamble 和 cJSON 贡献者

  特此免费授予任何获得副本的人的许可
  本软件和相关文档文件（以下简称“软件”）的处理
  不受限制地在软件中，包括但不限于权利
  使用、复制、修改、合并、发布、分发、再许可和/或出售
  软件的副本，并允许软件所针对的人
  为此，须符合以下条件：

  以上版权声明及本许可声明应包含在
  软件的所有副本或大部分。

  本软件按“原样”提供，不提供任何形式的明示或
  默示，包括但不限于适销性保证，
  适用于特定目的和非侵权。在任何情况下，
  作者或版权所有者对任何索赔、损害或其他责任负责
  责任，无论是在合同诉讼、侵权行为还是其他诉讼中，由以下原因引起的：
  因软件、使用或其他交易而产生或与之相关
  软件。
*/

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* 在为 Windows 编译时，我们指定一个特定的调用约定，以避免从具有不同默认调用约定的项目中调用我们的问题。 对于窗口，您有 3 个定义选项：

CJSON_HIDE_SYMBOLS - 在您不想 dllexport 元件的情况下定义此项
CJSON_EXPORT_SYMBOLS - 当您想要 dllexport 符号时，在库构建时定义此项（默认）
CJSON_IMPORT_SYMBOLS - 如果要 dllimport 元件，请定义此项

对于支持 visibility 属性的 *nix 构建，你可以通过以下方式定义类似的行为

通过将 Default Visibility 设置为 Hidden，方法是将
-fvisibility=hidden （用于 gcc）
或
-xldscope=hidden （用于 Sun CC）
至 CFLAGS

然后使用 CJSON_API_VISIBILITY 标志以 CJSON_EXPORT_SYMBOLS 的方式 “导出” 相同的符号

*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* export symbols 时，这对于复制粘贴 C 和头文件是必需的 */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* ! windows */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* 项目版本 */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 18

#include <stddef.h>

/* cJSON Types: */
#define cJSON_Invalid (0)
#define cJSON_False  (1 << 0)
#define cJSON_True   (1 << 1)
#define cJSON_NULL   (1 << 2)
#define cJSON_Number (1 << 3)
#define cJSON_String (1 << 4)
#define cJSON_Array  (1 << 5)
#define cJSON_Object (1 << 6)
#define cJSON_Raw    (1 << 7) /* 原始 JSON */

#define cJSON_IsReference 256
#define cJSON_StringIsConst 512

/* cJSON 结构： */
typedef struct cJSON
{
    /* next/prev 允许你遍历数组/对象链。或者，使用 GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* 数组或对象项将具有一个子指针，指向数组/对象中的项链。 */
    struct cJSON *child;

    /* 项目的类型，如上所述。 */
    int type;

    /* 项目的字符串（如果 type==cJSON_String 且 type == cJSON_Raw */
    char *valuestring;
    /* 写入 valueint 已弃用，请改用 cJSON_SetNumberValue */
    int valueint;
    /* 项目的编号（如果 type==cJSON_Number */
    double valuedouble;

    /* 项的名称字符串（如果此项是对象的子项或位于对象的子项列表中）。 */
    char *string;
} cJSON;

typedef struct cJSON_Hooks
{
      /* malloc/free 是 Windows 上的 CDECL，而不管编译器的默认调用约定如何，因此请确保钩子允许直接传递这些函数。 */
      void *(CJSON_CDECL *malloc_fn)(size_t sz);
      void (CJSON_CDECL *free_fn)(void *ptr);
} cJSON_Hooks;

typedef int cJSON_bool;
/* 限制数组/对象在 cJSON 拒绝解析之前可以嵌套的深度。
 * 这是为了防止堆栈溢出。*/
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* 限制循环引用的长度，可以在 cJSON 拒绝解析它们之前。
 * 这是为了防止堆栈溢出。*/
#ifndef CJSON_CIRCULAR_LIMIT
#define CJSON_CIRCULAR_LIMIT 10000
#endif

/* 以字符串形式返回 cJSON 的版本 */
CJSON_PUBLIC(const char*) cJSON_Version(void);

/* 向 cJSON 提供 malloc、realloc 和 free 函数 */
CJSON_PUBLIC(void) cJSON_InitHooks(cJSON_Hooks* hooks);

/* 内存管理：调用者始终负责从 cJSON_Parse（使用 cJSON_Delete）和 cJSON_Print（使用 stdlib free、cJSON_Hooks.free_fn 或 cJSON_free 视情况而定）的所有变体中释放结果。例外情况是 cJSON_PrintPreallocated，其中调用方对缓冲区负有全部责任。 */
/* 提供一个 JSON 块，这将返回一个您可以查询的 cJSON 对象。 */
CJSON_PUBLIC(cJSON *) cJSON_Parse(const char *value);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts 允许您要求（并检查）JSON 是否以 null 结尾，并检索指向解析的最终字节的指针。 */
/* 如果您在 return_parse_end 中提供 ptr 并且解析失败，则 return_parse_end 将包含指向错误的指针，因此将匹配 cJSON_GetErrorPtr（）。 */
CJSON_PUBLIC(cJSON *) cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated);
CJSON_PUBLIC(cJSON *) cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated);

/* 将 cJSON 实体渲染为文本以进行传输/存储。 */
CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item);
/* 将 cJSON 实体渲染为文本以进行传输/存储，无需任何格式设置。 */
CJSON_PUBLIC(char *) cJSON_PrintUnformatted(const cJSON *item);
/* 使用缓冲策略将 cJSON 实体渲染为文本。prebuffer 是对最终大小的猜测。猜得好可以减少重新分配。fmt=0 给出 unformatted， =1 给出格式化 */
CJSON_PUBLIC(char *) cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt);
/* 使用已在内存中分配的具有给定长度的缓冲区将 cJSON 实体呈现为文本。成功时返回 1，失败时返回 0。 */
/* 注意：cJSON 在估计它将使用多少内存时并不总是 100% 准确，因此为了安全起见，请分配比实际需要多 5 个字节 */
CJSON_PUBLIC(cJSON_bool) cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format);
/* 删除 cJSON 实体和所有子实体。 */
CJSON_PUBLIC(void) cJSON_Delete(cJSON *item);

/* 返回数组（或对象）中的项数。 */
CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array);
/* 从数组 “array” 中检索项目编号 “index”。如果不成功，则返回 NULL。 */
CJSON_PUBLIC(cJSON *) cJSON_GetArrayItem(const cJSON *array, int index);
/* 从 object 中获取项 “string”。不区分大小写。 */
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItem(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON *) cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
CJSON_PUBLIC(cJSON_bool) cJSON_HasObjectItem(const cJSON *object, const char *string);
/* 用于分析失败的解析。这将返回指向 parse 错误的指针。你可能需要回头看几个字符才能理解它。当 cJSON_Parse（） 返回 0 时定义。0 表示 cJSON_Parse（） 成功。 */
CJSON_PUBLIC(const char *) cJSON_GetErrorPtr(void);

/* 检查监控项类型并返回其值 */
CJSON_PUBLIC(char *) cJSON_GetStringValue(const cJSON * const item);
CJSON_PUBLIC(double) cJSON_GetNumberValue(const cJSON * const item);

/* 这些函数检查项目的类型 */
CJSON_PUBLIC(cJSON_bool) cJSON_IsInvalid(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsFalse(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsTrue(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsBool(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNull(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsNumber(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsString(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsArray(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsObject(const cJSON * const item);
CJSON_PUBLIC(cJSON_bool) cJSON_IsRaw(const cJSON * const item);

/* 这些调用会创建相应类型的 cJSON 项。 */
CJSON_PUBLIC(cJSON *) cJSON_CreateNull(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateTrue(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateFalse(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateBool(cJSON_bool boolean);
CJSON_PUBLIC(cJSON *) cJSON_CreateNumber(double num);
CJSON_PUBLIC(cJSON *) cJSON_CreateString(const char *string);
/* 原始 JSON */
CJSON_PUBLIC(cJSON *) cJSON_CreateRaw(const char *raw);
CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void);
CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void);

/* 创建一个字符串，其中 valuestring 引用一个字符串，因此
 * 它不会被 cJSON_Delete 释放 */
CJSON_PUBLIC(cJSON *) cJSON_CreateStringReference(const char *string);
/* 创建一个仅引用其元素的对象/数组，以便
 * 他们不会被 cJSON_Delete 释放 */
CJSON_PUBLIC(cJSON *) cJSON_CreateObjectReference(const cJSON *child);
CJSON_PUBLIC(cJSON *) cJSON_CreateArrayReference(const cJSON *child);

/* 这些实用程序将创建一个 count 项的 Array。
 * 参数 count 不能大于 number 数组中的元素数，否则数组访问将超出范围。
CJSON_PUBLIC(cJSON *) cJSON_CreateIntArray(const int *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateFloatArray(const float *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateDoubleArray(const double *numbers, int count);
CJSON_PUBLIC(cJSON *) cJSON_CreateStringArray(const char *const *strings, int count);

/* 将 item 追加到指定的数组/对象。 */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
/* 当 string 绝对是 const （即 literals 或 as good as） 并且肯定会在 cJSON 对象中幸存下来时，请使用此函数。
 * 警告：使用此功能时，请确保始终检查之前（item->type & cJSON_StringIsConst）为零。
 * 写入 'item->string' */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObjectCS(cJSON *object, const char *string, cJSON *item);
/* 将对 item 的引用附加到指定的数组/对象。当您想将现有 cJSON 添加到新的 cJSON 中，但又不想损坏现有 cJSON 时，请使用此选项。 */
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
CJSON_PUBLIC(cJSON_bool) cJSON_AddItemReferenceToObject(cJSON *object, const char *string, cJSON *item);

/* 从 Arrays/Objects 中删除/分离项目。 */
CJSON_PUBLIC(cJSON *) cJSON_DetachItemViaPointer(cJSON *parent, cJSON * const item);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(void) cJSON_DeleteItemFromArray(cJSON *array, int which);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObjectCaseSensitive(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObject(cJSON *object, const char *string);
CJSON_PUBLIC(void) cJSON_DeleteItemFromObjectCaseSensitive(cJSON *object, const char *string);

/* 更新数组项。 */
CJSON_PUBLIC(cJSON_bool) cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* 将预先存在的项向右移动。 */
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemViaPointer(cJSON * const parent, cJSON * const item, cJSON * replacement);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
CJSON_PUBLIC(cJSON_bool) cJSON_ReplaceItemInObjectCaseSensitive(cJSON *object,const char *string,cJSON *newitem);

/* 复制 cJSON 项 */
CJSON_PUBLIC(cJSON *) cJSON_Duplicate(const cJSON *item, cJSON_bool recurse);
/* Duplicate 将在新内存中创建一个与您传递的 cJSON 项相同的新 cJSON 项，该项将
 * 需要发布。使用 recurse！=0 时，它将复制连接到该项目的任何子项。
 * item->next 和 ->prev 指针在从 Duplicate 返回时始终为零。*/
/* 以递归方式比较两个 cJSON 项是否相等。如果 a 或 b 为 NULL 或 invalid，则它们将被视为不相等。
 * case_sensitive 确定对象键是区分大小写 （1） 还是不区分大小写 （0） */
CJSON_PUBLIC(cJSON_bool) cJSON_Compare(const cJSON * const a, const cJSON * const b, const cJSON_bool case_sensitive);

/* 缩小字符串，从字符串中删除空白字符（例如 ' '、'\t'、'\r'、'\n'）。
 * 输入指针 json 不能指向只读地址区域，例如字符串常量， 
 * 但应指向可读和可写的地址区域。*/
CJSON_PUBLIC(void) cJSON_Minify(char *json);

/* 用于同时创建项和向对象添加项的辅助函数。
 * 它们返回添加的项或在失败时返回 NULL。*/
CJSON_PUBLIC(cJSON*) cJSON_AddNullToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddTrueToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddFalseToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddBoolToObject(cJSON * const object, const char * const name, const cJSON_bool boolean);
CJSON_PUBLIC(cJSON*) cJSON_AddNumberToObject(cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(cJSON*) cJSON_AddRawToObject(cJSON * const object, const char * const name, const char * const raw);
CJSON_PUBLIC(cJSON*) cJSON_AddObjectToObject(cJSON * const object, const char * const name);
CJSON_PUBLIC(cJSON*) cJSON_AddArrayToObject(cJSON * const object, const char * const name);

/* 当分配一个整数值时，它也需要传播到 valuedouble。 */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* cJSON_SetNumberValue 宏的帮助程序 */
CJSON_PUBLIC(double) cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
/* 更改 cJSON_String 对象的 valuestring，仅在对象类型为 cJSON_String 时生效 */
CJSON_PUBLIC(char*) cJSON_SetValuestring(cJSON *object, const char *valuestring);

/* 如果对象不是布尔类型，则不执行任何操作并返回cJSON_Invalid否则返回新类型*/
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False|cJSON_True))) ? \
    (object)->type=((object)->type &(~(cJSON_False|cJSON_True)))|((boolValue)?cJSON_True:cJSON_False) : \
    cJSON_Invalid\
)

/* 用于迭代数组或对象的宏 */
#define cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free 对象，这些对象使用已通过 cJSON_InitHooks 设置的 malloc/free 函数 */
CJSON_PUBLIC(void *) cJSON_malloc(size_t size);
CJSON_PUBLIC(void) cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif
