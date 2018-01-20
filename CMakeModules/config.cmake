if( NOT CONFIG_CMAKE_INCLUDED)
set(CONFIG_CMAKE_INCLUDED 1)
if(UNIX)
    SET( CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -fprofile-arcs -ftest-coverage -fno-inline -fno-inline-small-functions -fno-default-inline -O0"
            CACHE STRING "Flags used by the C++ compiler during coverage builds."
            FORCE )
    SET( CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} -fPIC -fprofile-arcs -ftest-coverage" CACHE STRING
            "Flags used by the C compiler during coverage builds."
            FORCE )
    SET( CMAKE_EXE_LINKER_FLAGS_COVERAGE
            "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -lgcov" CACHE STRING
            "Flags used for linking binaries during coverage builds."
            FORCE )
    SET( CMAKE_SHARED_LINKER_FLAGS_COVERAGE
            "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -lgcov" CACHE STRING
            "Flags used by the shared libraries linker during coverage builds."
            FORCE )

    MARK_AS_ADVANCED(
            CMAKE_CXX_FLAGS_COVERAGE
            CMAKE_C_FLAGS_COVERAGE
            CMAKE_EXE_LINKER_FLAGS_COVERAGE
            CMAKE_SHARED_LINKER_FLAGS_COVERAGE)

    #-fsanitize-address-use-after-scope
    SET( CMAKE_CXX_FLAGS_SANITIZE "${CMAKE_CXX_FLAGS_RELEASE} -g -fsanitize=address -fno-omit-frame-pointer -O0 -fno-optimize-sibling-calls"
            CACHE STRING "Flags used by the C++ compiler during coverage builds."
            FORCE )
    SET( CMAKE_C_FLAGS_SANITIZE "${CMAKE_C_FLAGS_RELEASE} -g -fsanitize=address -fno-omit-frame-pointer -O0 -fno-optimize-sibling-calls" CACHE STRING
            "Flags used by the C compiler during coverage builds."
            FORCE )
    SET( CMAKE_EXE_LINKER_FLAGS_SANITIZE
            "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -fsanitize=address" CACHE STRING
            "Flags used for linking binaries during coverage builds."
            FORCE )
    SET( CMAKE_SHARED_LINKER_FLAGS_SANITIZE
            "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -fsanitize=address" CACHE STRING
            "Flags used by the shared libraries linker during coverage builds."
            FORCE )
    MARK_AS_ADVANCED(
            CMAKE_CXX_FLAGS_SANITIZE
            CMAKE_C_FLAGS_SANITIZE
            CMAKE_EXE_LINKER_FLAGS_SANITIZE
            CMAKE_SHARED_LINKER_FLAGS_SANITIZE)

    # Update the documentation string of CMAKE_BUILD_TYPE for GUIs
    SET( CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage Sanitize."
            FORCE )

#    string( TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
#    if(${CMAKE_BUILD_TYPE_UPPER} STREQUAL "SANITIZE")
#        include(CheckCCompilerFlag)
#        include(CheckCXXCompilerFlag)
#        set (C_HAS_SANITIZE_FLAG 1)
#        #check_c_compiler_flag(-fsanitize=address C_HAS_SANITIZE_FLAG)
#        check_cxx_compiler_flag(-fsanitize=address CXX_HAS_SANITIZE_FLAG)
#        if(NOT C_HAS_SANITIZE_FLAG OR NOT CXX_HAS_SANITIZE_FLAG)
#            MESSAGE(FATAL_ERROR "compiler does not support address sanitizer")
#        endif(NOT C_HAS_SANITIZE_FLAG OR NOT CXX_HAS_SANITIZE_FLAG)
#    endif(${CMAKE_BUILD_TYPE_UPPER} STREQUAL "SANITIZE")
endif(UNIX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_STANDARD 17)

endif(NOT CONFIG_CMAKE_INCLUDED)