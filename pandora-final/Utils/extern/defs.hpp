#pragma once
#include <cstdint>
#include <Windows.h>
#include <winternl.h>

#define FILE_OPEN 0x00000001
#define FILE_NON_DIRECTORY_FILE 0x00000040
#define FILE_OPEN_BY_FILE_ID 0x00002000

#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080

typedef struct _API_SET_NAMESPACE {
   ULONG Version;
   ULONG Size;
   ULONG Flags;
   ULONG Count;
   ULONG EntryOffset;
   ULONG HashOffset;
   ULONG HashFactor;
} API_SET_NAMESPACE, * PAPI_SET_NAMESPACE;

typedef struct _API_SET_NAMESPACE_ENTRY {
   ULONG Flags;
   ULONG NameOffset;
   ULONG NameLength;
   ULONG HashedLength;
   ULONG ValueOffset;
   ULONG ValueCount;
} API_SET_NAMESPACE_ENTRY, * PAPI_SET_NAMESPACE_ENTRY;

typedef struct _RTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION {
   HANDLE ReflectionProcessHandle;
   HANDLE ReflectionThreadHandle;
   CLIENT_ID ReflectionClientId;
} RTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION, * PRTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION;

using NtAllocateVirtualMemory = NTSTATUS( NTAPI* )( HANDLE process_handle, PVOID* base_address,
													std::uint32_t* bits, size_t* size, std::uint32_t type, std::uint32_t access );

using NtReadVirtualMemory = NTSTATUS( NTAPI* )( HANDLE process_handle, PVOID base_address, PVOID buffer,
												ULONG number_of_bytes_to_read, PULONG number_of_bytes_readed );

using NtWriteVirtualMemory = NTSTATUS( NTAPI* )( HANDLE process_handle, PVOID base_address, PVOID buffer,
												 ULONG number_of_bytes_to_write, PULONG number_of_bytes_written );

using NtFreeVirtualMemory = NTSTATUS( NTAPI* )( HANDLE process_handle, PVOID* base_address, PSIZE_T region_size, ULONG free_type );

using NtProtectVirtualMemory = NTSTATUS( NTAPI* )( HANDLE process_handle, PVOID* base_address, PULONG number_of_bytes_to_protect,
												   ULONG new_access_protection, PULONG old_access_protection );

using NtYieldExecution = NTSTATUS( NTAPI* )( );

using NtCreateThreadEx = NTSTATUS( NTAPI* )( PHANDLE thread_handle, ACCESS_MASK desired_access, POBJECT_ATTRIBUTES object_attributes,
											 HANDLE process_handle, PVOID start_routine, PVOID argument, ULONG create_flags,
											 ULONG_PTR zero_bits, SIZE_T stack_size, SIZE_T maximum_stack_size, PVOID attribute_list );

using NtResumeThread = NTSTATUS( NTAPI* )( HANDLE thread_handle, PULONG suspend_count );

using NtGetContextThread = NTSTATUS( NTAPI* )( HANDLE thread_handle, PCONTEXT context );

using NtSetContextThread = NTSTATUS( NTAPI* )( HANDLE thread_handle, PCONTEXT context );

using RtlCreateProcessReflection = NTSTATUS( NTAPI* )( HANDLE process_handle, ULONG flags, PVOID start_routine,
													   PVOID start_context, HANDLE event_handle, PRTLP_PROCESS_REFLECTION_REFLECTION_INFORMATION reflection_information );

// ReSharper restore CppInconsistentNaming

static const auto current_process = reinterpret_cast< HANDLE >( -1 );