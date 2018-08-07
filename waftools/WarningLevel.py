#! /usr/bin/env python
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


from waflib import TaskGen, Logs, Errors

@TaskGen.feature('warning-level')
@TaskGen.before_method('process_source')
def apply_warnings(self):
    level_str = getattr( self, "warning_levels", None )
    if( level_str is None ):
        raise Errors.WafError( "WarningLevel: missing warning_levels for %s" % ( self.name ) )

    level_list = str.split( level_str )
    Logs.debug( "WarningLevel: levels applied = %s" % ( level_str ) )
    for level_to_apply in level_list:
        Logs.debug( "Applying state: %s" % ( level_to_apply ) )
        for i in [ "cxxflags", "linkflags" ]:
            config_name = "warning_level_%s_%s" % ( level_to_apply, i )
            flag = getattr( self.env, config_name, None )
            if( flag is not None ):
                self.env.append_unique( i, flag )
