# ------------------------------------------------
# @author Adrian Szczepanski
# @date 06-03-2022
# ------------------------------------------------

# Platform
# Uncomment if program platform must be diffrent than the solution
# PLATFORM := Pc32

# Target
library_name := protocol

# Directories
project_dir ?= ..
make_dir := $(project_dir)/make
external_dir := $(project_dir)/external

# Sources 
cxx_sources :=  \
$(wildcard libprotocol/*.cpp) \
$(wildcard libprotocol/state/*.cpp) \

# c_sources := \

# asm_sources := \

# Includes
cxx_includes := \
-I. \

# Defines
cxx_defs := \

# external libraries
#	e.g. if include flag is '-I$(external_dir)/libFoo'
#	write 'libFoo'
export external_library_include_path = \
etl/include \

# 	for testing purpose - use only PC 32-bit version
export external_library_flags = \

# 	for testing purposes - use only PC 32-bit version
#	e.g. if link flag is '-L$(external_dir)/libFoo/lib'
#	write 'libFoo/lib'
export external_library_paths = \

# Install configuration
installed_include_files = \

installed_include_directories = \

# Uncomment if gcc should be used for compilation *.c files
# use_gcc_for_c_files := 1

# Uncomment if you want to add custom flags for gcc or g++
# custom_c_flags :=
# custom_cxx_flags :=

# Append rest of Makefile from template
include $(make_dir)/SoftwareLib.mk