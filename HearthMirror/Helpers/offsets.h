//
//  HMOffsets.h
//  MonoReader
//
//  Created by Istvan Fehervari on 23/12/2016.
//  Copyright © 2016 com.ifehervari. All rights reserved.
//

#ifndef offsets_h
#define offsets_h

#ifdef __APPLE__

#define kMonoDomainDomainAssemblies 0x84
#define kMonoAssemblyName 0x8
#define kMonoAssemblyImage 0x40

#define kMonoImageClassCache 0x2a0
#define kMonoInternalHashTableSize 0xc
#define kMonoInternalHashTableNum_entries 0x10
#define kMonoInternalHashTableTable 0x14

#define kMonoClassNextClassCache 0x98
#define kMonoClassName 0x28
#define kMonoClassNameSpace 0x2c
#define kMonoClassNestedIn 0x20
#define kMonoClassRuntimeInfo 0x94
#define kMonoClassRuntimeInfoDomainVtables 0x4
#define kMonoClassBitfields 0x14
#define kMonoClassSizes 0x50
#define kMonoClassParent 0x1c
#define kMonoClassByvalArg 0x7c
#define kMonoClassFields 0x6c
#define kMonoClassFieldCount 0x5c

#define kMonoClassFieldSizeof 0x10
#define kMonoClassFieldType 0x0
#define kMonoClassFieldName 0x4
#define kMonoClassFieldParent 0x8
#define kMonoClassFieldOffset 0xc

#define kMonoTypeAttrs 0x4
#define kMonoTypeType 0x6
#define kMonoTypeSizeof 0x8
#define kMonoVTableData 0xc

#else

#define kImageDosHeader_e_lfanew 0x3c
#define kImageNTHeadersSignature 0x0
#define kImageNTHeadersMachine 0x4
#define kImageNTHeadersExportDirectoryAddress 0x78
#define kImageExportDirectoryNumberOfFunctions 0x14
#define kImageExportDirectoryAddressOfFunctions 0x1c
#define kImageExportDirectoryAddressOfNames 0x20
#define kMonoDomainSizeof 0x144
#define kMonoDomainDomainAssemblies 0x6c
#define kMonoAssemblyName 0x8
#define kMonoAssemblyImage 0x40

#define kMonoImageClassCache 0x2a0
#define kMonoInternalHashTableSize 0xc
#define kMonoInternalHashTableTable 0x14

#define kMonoClassNextClassCache 0xa8
#define kMonoClassName 0x30
#define kMonoClassNameSpace 0x34
#define kMonoClassNestedIn 0x28
#define kMonoClassRuntimeInfo 0xa4
#define kMonoClassRuntimeInfoDomainVtables 0x4
#define kMonoClassBitfields  0x14
#define kMonoClassSizes 0x58
#define kMonoClassParent 0x24
#define kMonoClassByvalArg 0x88
#define kMonoClassFields 0x74

#define kMonoClassFieldCount 0x64
#define kMonoClassFieldSizeof 0x10
#define kMonoClassFieldType 0x0
#define kMonoClassFieldName 0x4
#define kMonoClassFieldParent 0x8
#define kMonoClassFieldOffset 0xc

#define kMonoTypeAttrs 0x4
//#define kMonoTypeType 0x6
#define kMonoTypeSizeof 0x8
#define kMonoVTableData 0xc

#endif


#ifdef USE_FAKE_TYPES
#include <pthread.h>
#include <glib.h>

typedef enum {
    MONO_TYPE_END        = 0x00,
    MONO_TYPE_VOID       = 0x01,
    MONO_TYPE_BOOLEAN    = 0x02,
    MONO_TYPE_CHAR       = 0x03,
    MONO_TYPE_I1         = 0x04,
    MONO_TYPE_U1         = 0x05,
    MONO_TYPE_I2         = 0x06,
    MONO_TYPE_U2         = 0x07,
    MONO_TYPE_I4         = 0x08,
    MONO_TYPE_U4         = 0x09,
    MONO_TYPE_I8         = 0x0a,
    MONO_TYPE_U8         = 0x0b,
    MONO_TYPE_R4         = 0x0c,
    MONO_TYPE_R8         = 0x0d,
    MONO_TYPE_STRING     = 0x0e,
    MONO_TYPE_PTR        = 0x0f,
    MONO_TYPE_BYREF      = 0x10,
    MONO_TYPE_VALUETYPE  = 0x11,
    MONO_TYPE_CLASS      = 0x12,
    MONO_TYPE_VAR	     = 0x13,
    MONO_TYPE_ARRAY      = 0x14,
    MONO_TYPE_GENERICINST= 0x15,
    MONO_TYPE_TYPEDBYREF = 0x16,
    MONO_TYPE_I          = 0x18,
    MONO_TYPE_U          = 0x19,
    MONO_TYPE_FNPTR      = 0x1b,
    MONO_TYPE_OBJECT     = 0x1c,
    MONO_TYPE_SZARRAY    = 0x1d,
    MONO_TYPE_MVAR	     = 0x1e,
    MONO_TYPE_CMOD_REQD  = 0x1f,
    MONO_TYPE_CMOD_OPT   = 0x20,
    MONO_TYPE_INTERNAL   = 0x21,
    
    MONO_TYPE_MODIFIER   = 0x40,
    MONO_TYPE_SENTINEL   = 0x41,
    MONO_TYPE_PINNED     = 0x45,
    
    MONO_TYPE_ENUM       = 0x55
} FakeMonoTypeEnum;

typedef enum {
    MONO_TABLE_MODULE,
    MONO_TABLE_TYPEREF,
    MONO_TABLE_TYPEDEF,
    MONO_TABLE_FIELD_POINTER,
    MONO_TABLE_FIELD,
    MONO_TABLE_METHOD_POINTER,
    MONO_TABLE_METHOD,
    MONO_TABLE_PARAM_POINTER,
    MONO_TABLE_PARAM,
    MONO_TABLE_INTERFACEIMPL,
    MONO_TABLE_MEMBERREF,
    MONO_TABLE_CONSTANT,
    MONO_TABLE_CUSTOMATTRIBUTE,
    MONO_TABLE_FIELDMARSHAL,
    MONO_TABLE_DECLSECURITY,
    MONO_TABLE_CLASSLAYOUT,
    MONO_TABLE_FIELDLAYOUT,
    MONO_TABLE_STANDALONESIG,
    MONO_TABLE_EVENTMAP,
    MONO_TABLE_EVENT_POINTER,
    MONO_TABLE_EVENT,
    MONO_TABLE_PROPERTYMAP,
    MONO_TABLE_PROPERTY_POINTER,
    MONO_TABLE_PROPERTY,
    MONO_TABLE_METHODSEMANTICS,
    MONO_TABLE_METHODIMPL,
    MONO_TABLE_MODULEREF,
    MONO_TABLE_TYPESPEC,
    MONO_TABLE_IMPLMAP,
    MONO_TABLE_FIELDRVA,
    MONO_TABLE_UNUSED6,
    MONO_TABLE_UNUSED7,
    MONO_TABLE_ASSEMBLY,
    MONO_TABLE_ASSEMBLYPROCESSOR,
    MONO_TABLE_ASSEMBLYOS,
    MONO_TABLE_ASSEMBLYREF,
    MONO_TABLE_ASSEMBLYREFPROCESSOR,
    MONO_TABLE_ASSEMBLYREFOS,
    MONO_TABLE_FILE,
    MONO_TABLE_EXPORTEDTYPE,
    MONO_TABLE_MANIFESTRESOURCE,
    MONO_TABLE_NESTEDCLASS,
    MONO_TABLE_GENERICPARAM,
    MONO_TABLE_METHODSPEC,
    MONO_TABLE_GENERICPARAMCONSTRAINT
    
#define MONO_TABLE_LAST MONO_TABLE_GENERICPARAMCONSTRAINT
#define MONO_TABLE_NUM (MONO_TABLE_LAST + 1)
    
} FakeMonoMetaTableEnum;

struct MonoTableInfo {
    const char *base;
    unsigned int       rows     : 24;
    unsigned int       row_size : 8;
    uint32_t   size_bitfield;
};

typedef struct {
    const char* data;
    uint32_t  size;
} MonoStreamHeader;

typedef void* (*MonoInternalHashKeyExtractFunc) (void* value);
typedef void** (*MonoInternalHashNextValueFunc) (void* value);

struct MonoInternalHashTable
{
    GHashFunc hash_func;
    MonoInternalHashKeyExtractFunc key_extract;
    MonoInternalHashNextValueFunc next_value;
    int size;
    int num_entries;
    gpointer *table;
};

typedef struct _WapiCriticalSection
{
    guint32 depth;
    pthread_mutex_t mutex;
} CRITICAL_SECTION;

//typedef mono_mutex_t CRITICAL_SECTION;

typedef struct _MonoImage {
    int   ref_count;
    void *raw_data_handle;
    char *raw_data;
    uint32_t raw_data_len;
    uint8_t raw_buffer_used    : 1;
    uint8_t raw_data_allocated : 1;
    
#ifdef USE_COREE
    uint8_t is_module_handle : 1;
    uint8_t has_entry_point : 1;
#endif
    uint8_t dynamic : 1;
    uint8_t ref_only : 1;
    uint8_t uncompressed_metadata : 1;
    uint8_t checked_module_cctor : 1;
    uint8_t has_module_cctor : 1;
    uint8_t idx_string_wide : 1;
    uint8_t idx_guid_wide : 1;
    uint8_t idx_blob_wide : 1;
    uint8_t core_clr_platform_code : 1;
    char *name;
    const char *assembly_name;
    const char *module_name;
    char *version;
    int16_t md_version_major, md_version_minor;
    char *guid;
    void *image_info;
    void          *mempool;
    char                *raw_metadata;
    MonoStreamHeader     heap_strings;
    MonoStreamHeader     heap_us;
    MonoStreamHeader     heap_blob;
    MonoStreamHeader     heap_guid;
    MonoStreamHeader     heap_tables;
    const char          *tables_base;
    MonoTableInfo        tables [MONO_TABLE_LAST + 1];
    void **references;
    void **modules;
    uint32_t module_count;
    int *modules_loaded;
    void  **files;
    void* aot_module;
    void  *assembly;
    void *method_cache;
    MonoInternalHashTable class_cache;
    void *methodref_cache;
    void *field_cache;
    void *typespec_cache;
    GHashTable *memberref_signatures;
    GHashTable *helper_signatures;
    GHashTable *method_signatures;
    GHashTable *name_cache;
    GHashTable *array_cache;
    GHashTable *ptr_cache;
    
    GHashTable *szarray_cache;
    CRITICAL_SECTION szarray_cache_lock;
    GHashTable *delegate_begin_invoke_cache;
    GHashTable *delegate_end_invoke_cache;
    GHashTable *delegate_invoke_cache;
    GHashTable *runtime_invoke_cache;
    GHashTable *delegate_abstract_invoke_cache;
    GHashTable *runtime_invoke_direct_cache;
    GHashTable *runtime_invoke_vcall_cache;
    GHashTable *managed_wrapper_cache;
    GHashTable *native_wrapper_cache;
    GHashTable *native_wrapper_aot_cache;
    GHashTable *remoting_invoke_cache;
    GHashTable *synchronized_cache;
    GHashTable *unbox_wrapper_cache;
    GHashTable *cominterop_invoke_cache;
    GHashTable *cominterop_wrapper_cache;
    GHashTable *thunk_invoke_cache;
    GHashTable *ldfld_wrapper_cache;
    GHashTable *ldflda_wrapper_cache;
    GHashTable *stfld_wrapper_cache;
    GHashTable *isinst_cache;
    GHashTable *castclass_cache;
    GHashTable *proxy_isinst_cache;
    GHashTable *rgctx_template_hash;
    GHashTable *generic_class_cache;
    void *property_hash;
    void *reflection_info;
    void *user_info;
    void *dll_map;
    void *interface_bitset;
    GSList *reflection_info_unregister_classes;
    CRITICAL_SECTION    lock;
} FakeMonoImage;

typedef struct {
    unsigned int required : 1;
    unsigned int token    : 31;
} FakeMonoCustomMod;

struct _MonoClass;
typedef _MonoClass FakeMonoClass;

#ifndef MONO_ZERO_LEN_ARRAY
#ifdef __GNUC__
#define MONO_ZERO_LEN_ARRAY 0
#else
#define MONO_ZERO_LEN_ARRAY 1
#endif
#endif

typedef struct {
    unsigned int required : 1;
    unsigned int token    : 31;
} MonoCustomMod;

typedef struct _MonoType {
    union {
        FakeMonoClass *klass; // for VALUETYPE and CLASS
        _MonoType *type;   // for PTR
        void *array; // for ARRAY
        void *method;
        void *generic_param; // for VAR and MVAR
        void *generic_class; // for GENERICINST
    } data;
    uint32_t attrs    : 16;
    FakeMonoTypeEnum type     : 8;
    unsigned int num_mods : 6;
    unsigned int byref    : 1;
    unsigned int pinned   : 1;
    MonoCustomMod modifiers [MONO_ZERO_LEN_ARRAY];
} FakeMonoType;

struct _MonoClassField {
    FakeMonoType        *type;
    const char      *name;
    FakeMonoClass       *parent;
    int              offset;
};

#if defined(_MSC_VER)
#   define USE_UINT8_BIT_FIELD(type, field) uint8 field
#else
#   define USE_UINT8_BIT_FIELD(type, field) type field
#endif

struct _MonoClass {
    uint32_t element_class;
    uint32_t cast_class;
    uint32_t supertypes;
    uint16_t     idepth;
    uint8_t     rank;
    int32_t        instance_size;
    USE_UINT8_BIT_FIELD(uint32_t, inited          : 1);
    USE_UINT8_BIT_FIELD(uint32_t, init_pending    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, size_inited     : 1);
    USE_UINT8_BIT_FIELD(uint32_t, valuetype       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, enumtype        : 1);
    USE_UINT8_BIT_FIELD(uint32_t, blittable       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, unicode         : 1);
    USE_UINT8_BIT_FIELD(uint32_t, wastypebuilder  : 1);
    uint8_t min_align;
    USE_UINT8_BIT_FIELD(uint32_t, packing_size    : 4);
    USE_UINT8_BIT_FIELD(uint32_t, ghcimpl         : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_finalize    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, marshalbyref    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, contextbound    : 1);
    USE_UINT8_BIT_FIELD(uint32_t, delegate        : 1);
    USE_UINT8_BIT_FIELD(uint32_t, gc_descr_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_cctor       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_references  : 1);
    USE_UINT8_BIT_FIELD(uint32_t, has_static_refs : 1);
    USE_UINT8_BIT_FIELD(uint32_t, no_special_static_fields : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_com_object   : 1);
    USE_UINT8_BIT_FIELD(uint32_t, nested_classes_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, interfaces_inited : 1);
    USE_UINT8_BIT_FIELD(uint32_t, simd_type       : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_generic      : 1);
    USE_UINT8_BIT_FIELD(uint32_t, is_inflated     : 1);
    
    uint8_t     exception_type;
    
    uint32_t  parent;
    uint32_t  nested_in;
    
    uint32_t image;
    uint32_t name;
    uint32_t name_space;
    
    uint32_t    type_token;
    int32_t        vtable_size;
    
    uint16_t      interface_count;
    uint16_t     interface_id;
    uint16_t     max_interface_id;
    
    uint16_t     interface_offsets_count;
    uint32_t interfaces_packed;
    uint32_t interface_offsets_packed;
    uint32_t interface_bitmap;
    
    uint32_t interfaces;
    
    union {
        int32_t class_size;
        int32_t element_size;
        int32_t generic_param_token;
    } sizes;
    
    uint32_t    flags;
    struct {
        uint32_t first, count;
    } field, method;

    uint32_t marshal_info;
    
    uint32_t fields;
    
    uint32_t methods;

    FakeMonoType this_arg;
    FakeMonoType byval_arg;
    
    uint32_t generic_class;
    uint32_t generic_container;
    
    uint32_t reflection_info;
    
    uint32_t gc_descr;
    
    uint32_t runtime_info;
 
    uint32_t next_class_cache;
    uint32_t vtable;
    uint32_t ext;
    
    uint32_t user_data;
};

typedef struct _MonoDomain {
    CRITICAL_SECTION    lock; // this should be 48 bytes
    void        *mp;
    void    *code_mp;

#define MONO_DOMAIN_FIRST_OBJECT setup
    void *setup;
    void      *domain;
    void     *default_context;
    void      *out_of_memory_ex;
    void      *null_reference_ex;
    void      *stack_overflow_ex;
    void      *divide_by_zero_ex;

    void         *typeof_void;
    
#define MONO_DOMAIN_FIRST_GC_TRACKED env
    void     *env;
    void     *ldstr_table;
    void     *type_hash;
    void     *refobject_hash;
    gpointer           *static_data_array;
    void    *type_init_exception_hash;
    void     *delegate_hash_table;
#define MONO_DOMAIN_LAST_GC_TRACKED delegate_hash_table
    guint32            state;
    gint32             domain_id;
    gint32             shadow_serial;
    GSList             *domain_assemblies;
    void       *entry_assembly;
    char               *friendly_name;
    GHashTable         *class_vtable_hash;
    GHashTable         *proxy_vtable_hash;
    MonoInternalHashTable jit_code_hash;
    CRITICAL_SECTION    jit_code_hash_lock;
    int		    num_jit_info_tables;
    void *
    volatile          jit_info_table;
    GSList		   *jit_info_free_queue;
    gchar **search_path;
    gchar *private_bin_path;
    void         *create_proxy_for_type_method;
    void         *private_invoke_method;
    GHashTable         *special_static_fields;
    GHashTable         *finalizable_objects_hash;
#ifndef HAVE_SGEN_GC
    GHashTable         *track_resurrection_objects_hash;
    GHashTable         *track_resurrection_handles_hash;
#endif
    CRITICAL_SECTION   finalizable_objects_hash_lock;
    CRITICAL_SECTION    assemblies_lock;
    
    GHashTable	   *method_rgctx_hash;
    
    GHashTable	   *generic_virtual_cases;
    void **thunk_free_lists;
    GHashTable	*class_custom_attributes;
    gpointer runtime_info;
    int					threadpool_jobs;
    void*				cleanup_semaphore; // !!!! Inaccurate, it was HANDLE
    gpointer            finalize_runtime_invoke;
    gpointer            capture_context_runtime_invoke;
    gpointer            capture_context_method;
    void *socket_assembly;
    FakeMonoClass *sockaddr_class;
    void *sockaddr_data_field;
    FakeMonoClass           **static_data_class_array;
} FakeMonoDomain;
#endif // fake types

#endif /* offsets_h */
