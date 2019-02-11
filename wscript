#! /usr/bin/env python
#
# wscript - waf build script
#
# Copyright 2016-2018, Daniel Kristensen, Garmin Ltd, or its subsidiaries.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import collections
import os
import shutil
import sys
import pprint
from waflib import Build, Configure, Errors, Options, Logs, Scripting
from waflib.extras import msvs

VERSION="0.0.0"
APPNAME='bfdm'
REVISION=''

# Environment prefix for app-specific settings
env_prefix = APPNAME + "_"

# Set paths for Waf
top = '.'
out = '_out'

# Define classes for custom options
CustomOptions = collections.namedtuple( 'CustomOptions', ['parameter','collection'] )
SupportedPlatform = collections.namedtuple( 'SupportedPlatform', ['name', 'cxx_target', 'description'] )
SupportedMode = collections.namedtuple( 'SupportedMode', ['name', 'msvc_defines', 'msvc_c_flags', 'msvc_cxx_flags', 'msvc_link_flags', 'description'] )

# Configure platforms; the first entry is the default if none is specified.
my_platforms = [
    SupportedPlatform( name='win-auto', cxx_target="", description="Windows (auto-detect)" ),
    SupportedPlatform( name='win32', cxx_target="x86", description="Windows 32-bit x86 architecture" ),
    SupportedPlatform( name='win64', cxx_target="x86_64", description="Windows 64-bit x86 architecture" )
    ]

# Configure modes; the first entry is the default if none is specified.
# TODO: Allow flags to be abstracted per compiler, but requires more advanced compiler detection

# Set up autoconfigure to reconfigure when wscript or dependent files change
Configure.autoconfig = True

# TODO: Can we get rid of these?  Would be nice not to depend on it
common_msvc_defines = [
    "UNICODE",
    "_UNICODE"
    ]

common_msvc_c_flags = [
    # none
    ]

common_msvc_cxx_flags = [
    "/EHsc"     # C++ exception support only
    ]
common_msvc_link_flags = [ "/WX", "/OPT:REF" ]

my_modes = [
    SupportedMode
        (
        name='development',
        msvc_defines=common_msvc_defines,
        # /RTCc is not used because the MSC STL is not guaranteed to be compliant
        msvc_c_flags=common_msvc_c_flags + [ "/Od", "/Z7", "/FS", "/RTCsu" ],
        msvc_cxx_flags=common_msvc_cxx_flags + [ "/Od", "/Z7", "/FS", "/RTCsu" ],
        msvc_link_flags=common_msvc_link_flags + [ "/DEBUG" ],
        description=""
        ),
    SupportedMode
        (
        name='release',
        msvc_defines=common_msvc_defines,
        msvc_c_flags=common_msvc_c_flags + [ "/O2" ],
        msvc_cxx_flags=common_msvc_cxx_flags + [ "/O2" ],
        msvc_link_flags=common_msvc_link_flags + [],
        description=""
        )
    ]

# Aggregate the options so default values can be detected and used automatically
my_custom_options = [
    CustomOptions( "platform", my_platforms ),
    CustomOptions( "mode", my_modes )
    ]

# This function is called first
def init( ctx ):
    pass # nothing to do here

# This function is called before any other for parsing the command-line
def options( opt ):
	# TODO: Define these in a tool that implements it more generically
    local_group = opt.add_option_group( APPNAME + " options (may be specified in configure)" )
    local_group.add_option(
        "-p", "--platform",
        dest="platform",
        type="string",
        action="store",
        help="Platform to compile for (use ? for a list, default='" + my_platforms[0].name + "')"
        )
    local_group.add_option(
        "-m", "--mode",
        dest="mode",
        type="string",
        action="store",
        help="Build mode (use ? for a list, default='" + my_modes[0].name + "')"
        )

    # Load tools required to parse option parameters (these load for all build steps).
    opt.load('compiler_cxx')
    opt.load('msvs')

# This function is called to update the configuration
def configure( conf ):
    # check and save configuration options
    check_arguments()
    for v in my_custom_options:
        option_value = getattr( Options.options, v.parameter )
        conf.env[env_prefix + v.parameter] = ""
        if( None != option_value ):
            Logs.debug( "options: presetting " + v.parameter + " = '" + option_value + "'" )
            conf.env[env_prefix + v.parameter] = option_value
        else:
            Logs.debug( "options: no preset value for " + v.parameter )

    # Load MSVC tool
    conf.load('msvc')
    conf.load('globitool', tooldir='_out/globifest/waf')

    # Load the WarningLevel tool
    conf.load('WarningLevel', tooldir='waftools')
    conf.env.append_unique( "warning_level_high_cxxflags", [ "/W4" ] )
    conf.env.append_unique( "warning_level_max_cxxflags", [
        "/Wall", # Highest warning level

        # Disable warnings:
        "/wd4514",  # unreferenced inline function has been removed (caused by STL headers)
        "/wd4710",  # function not inlined (caused by STL headers)
        "/wd4373",  # previous versions of the compiler did not override when parameters only differed by const/volatile qualifiers
                    # (caused by gmock and extraneous const value qualifiers on declarations)
        "/wd4820",  # 'x' bytes padding added after data member 'y'
        "/wd4625",  # copy constructor was implicitly defined as deleted
        "/wd4626",  # assignment operator was implicitly defined as deleted
        "/wd5026",  # move constructor was implicitly defined as deleted
        "/wd5027"   # move assignment operator was implicitly defined as deleted
        ] )
    conf.env.append_unique( "warning_level_warnings-as-errors_cxxflags", [ "/WX" ] )
    conf.env.append_unique( "warning_level_warnings-as-errors_linkflags", [ "/WX" ] )

    # Configure flags for each mode; tools are loaded once before the environment is copied to
    # avoid multiple configuration loops
	# TODO: Do this in the platform/mode tool
    base_env = conf.env
    for p in my_platforms:
        for m in my_modes:
            variant_name = p.name + "_" + m.name
            Logs.debug( APPNAME + ": configuring variant " + variant_name )

            # Derive from the base ConfigSet and detach.  This creates a new ConfigSet, which is
            # then used as the current set.
            new_env = base_env.derive()
            new_env.detach()
            conf.setenv( env_prefix + variant_name, new_env )
            conf.env.append_unique( 'DEFINES', m.msvc_defines )
            conf.env.append_unique( 'CFLAGS', m.msvc_c_flags )
            conf.env.append_unique( 'CXXFLAGS', m.msvc_cxx_flags )
            conf.env.append_unique( 'LINKFLAGS', m.msvc_link_flags )

# Holds effective options calculated in build step
effective_options = {}

# This function is called to perform build actions
def build( bld ):
	# TODO: When implementing the platform/mode tool, this if/else should go away
	# * platform/mode will set configurations and platforms for msvs
    if not bld.variant:
        # When custom variant commands are not provided,
        # select the appropriate variant from the options

        check_arguments()

        if( bld.cmd == "msvs" ):
            # One solution is generated for all variants, so this is done here instead of running
            # multiple msvs_<variant> commands.
            Logs.debug( APPNAME + ": Generating msvs solution..." )
            bld.waf_command = "waf.bat --platform $(Platform) --mode $(Configuration)"
            bld.solution_name = out + os.path.sep + APPNAME + ".sln"
            bld.projects_dir = bld.srcnode.make_node(out + os.path.sep + "vsproj")
            bld.projects_dir.mkdir()

            # Set up configurations
            vs_configs = list()
            for m in my_modes:
                vs_configs.append( m.name )
            bld.configurations = vs_configs

            # Set up platforms (these are not officially-supported platforms, but MSVS is just a
			# GUI in this case since waf generates an NMake solution; it simplifies the selection
			# of platform/mode combinations).
            vs_platforms = list()
            for p in my_platforms:
                vs_platforms.append( p.name )
            bld.platforms = vs_platforms

            # msvs uses the build targets to generate projects
            add_taskgens( bld )
        else:
            # This breaks up single non-variant commands like "build" into variant-specific commands

            # Iterate over options to determine which to use for each
            for v in my_custom_options:
                override_option = getattr( Options.options, v.parameter )
                saved_option = getattr( bld.env, env_prefix + v.parameter )
                if( override_option != None ):
                    Logs.debug( "options: using command line parameter for " + v.parameter )
                    effective_options[v.parameter] = override_option
                elif( saved_option != "" ):
                    Logs.debug( "options: using preconfigured parameter for " + v.parameter )
                    effective_options[v.parameter] = saved_option
                else:
                    Logs.debug( "options: using default parameter for " + v.parameter )
                    effective_options[v.parameter] = v.collection[0].name
                Logs.info( "options: " + v.parameter + " = '" + effective_options[v.parameter] + "'" )

            variant_name = effective_options["platform"] + "_" + effective_options["mode"]
            variant_cmd = bld.cmd + "_" + variant_name
            Logs.debug( APPNAME + ": executing " + variant_cmd  )

            # TODO: Turn the parameters into lists to allow multiple variants to be built at once
            Scripting.run_command( variant_cmd )

            # Create dummy taskgens when individual targets are specified, to avoid errors when
            # attempting to run the taskgens for those specific targets in this general-purpose
            # build context.  The real taskgen will be run in the variant-specific context.
            if bld.targets and bld.targets != '*':
                for tgt in bld.targets.split(','):
                    bld( name=tgt )
    else:
        Logs.debug( APPNAME + ": chaining command " + bld.cmd )
		# TODO: platform/mode tool should set the parameters in the build context derivative
		# instead of having the effective_parameters global
        if( not effective_options ):
            Logs.error( "Please use the 'build' command" )

        # Override the target according to the platform chosen
        if( bld.options.msvc_targets != "" ):
            Logs.warning( "Use --platform option to specify MSVC target" )
        platform = lookup_option_value( my_platforms, effective_options["platform"] )
        bld.options.msvc_targets = platform.cxx_target

        # Override the environment for the specific build variant prior to creating TaskGens
        mode = lookup_option_value( my_modes, effective_options["mode"] )
        variant_name = platform.name + "_" + mode.name
        bld.env = bld.all_envs[env_prefix + variant_name]

        # Add TaskGen objects to the build for this variant
		# TODO: platform/mode should have a callback registered to call for each build variant
        add_taskgens( bld )

def dist(ctx):
    raise Errors.WafError("Dist not supported yet")

# Create classes for each variant based off of Waf's internal classes
# This is necessary because WAF doesn't like having a single command for build variants
# TODO: Should be able to remove this by resetting BuildContext variables and calling callback
for p in my_platforms:
    for m in my_modes:
        variant_name = p.name + "_" + m.name
        for ctx in ( Build.BuildContext, Build.CleanContext, Build.InstallContext, Build.UninstallContext ):
            name = ctx.__name__.replace('Context','').lower()
            class tmp(ctx):
                cmd = name + '_' + variant_name
                variant = variant_name

# LOCAL METHODS BELOW THIS LINE #

def add_taskgens( bld ):
    # Common
    bld.GLOBITOOL(
        target="Common",
        project="proj/Common.gproj",
        tgt_params=dict(
            warning_levels="max warnings-as-error",
            features="cxx warning-level"
            ),
        lnk_params=dict(
            warning_levels="max warnings-as-error",
            features="cxx cxxstlib warning-level"
            )
        )

    # Building googletest separately to apply different warning level
    bld.GLOBITOOL(
        target="GoogleTest",
        project="proj/GoogleTest.gproj",
        tgt_params=dict(
            warning_levels="high",
            features="cxx warning-level"
            )
        )

    bld.GLOBITOOL(
        target="BfsdlTests",
        project="proj/BfsdlTests.gproj",
        use="Common GoogleTest",
        tgt_params=dict(
            warning_levels="max warnings-as-error",
            features="cxx warning-level"
            ),
        lnk_params=dict(
            warning_levels="max warnings-as-error",
            features="cxx cxxprogram warning-level",
            subsystem = "CONSOLE"
            )
        )

def check_arguments():
    for v in my_custom_options:
        if( getattr( Options.options, v.parameter) == None ):
            pass
        elif( getattr( Options.options, v.parameter) == "?" ):
            print( "Available " + v.parameter + " values:" )
            list_parameter_values( v.collection )
            sys.exit(0)
        elif( lookup_option_value( v.collection, getattr( Options.options, v.parameter) ) == None ):
            print( "Unknown value for --" + v.parameter + ", select one of the following:")
            list_parameter_values( v.collection )
            raise Errors.WafError("Bad argument to --" + v.parameter + ": '" + getattr( Options.options, v.parameter) + "'" )

def list_parameter_values( collection ):
    for v in collection:
        print( "  " + v.name + "\t" + v.description )

def lookup_option_value( collection, value ):
    for v in collection:
        if( v.name == value ):
            return v
    return None
