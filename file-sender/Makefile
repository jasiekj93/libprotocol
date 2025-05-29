# ------------------------------------------------
# @author Adrian Szczepanski
# @date 06-03-2023
# ------------------------------------------------

# Platform
# Uncomment if program platform must be diffrent than the solution
# PLATFORM := Pc32

# Target
target := file-sender

# Directories
project_dir ?= ..
make_dir := $(project_dir)/make
external_dir := $(project_dir)/external

# Sources 
cxx_sources :=  \
$(wildcard *.cpp) \

# c_sources := \

# asm_sources := \

# Includes
cxx_includes := \
-I. \

# Defines
cxx_defs := \

# internal libraries
required_internal_libraries = \

# libraries
required_libraries = \
protocol \

# external libraries
#	e.g. if include flag is '-I$(external_dir)/libFoo'
#	write 'libFoo'
external_library_include_path = \
etl/include \

external_library_flags = \

#	e.g. if link flag is '-L$(external_dir)/libFoo/lib'
#	write 'libFoo/lib'
external_library_paths = \

# Linker script
ldscript = 

# Uncomment if gcc should be used for compilation *.c files
# use_gcc_for_c_files := 1

# Uncomment if you want to add custom flags for gcc or g++
# custom_c_flags :=
# custom_cxx_flags :=

# Uncomment if you want to add library flags and the end of the linking command (e.g. -pthread)
# terminating_libs := 

# Append rest of Makefile from template
include $(make_dir)/Program.mk