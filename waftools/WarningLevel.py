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

import re
from waflib import TaskGen, Logs, Errors

LEVEL_RE = re.compile("warning_level_([^_]*)_(.*)")

config_levels = dict()

@TaskGen.feature('warning-level')
@TaskGen.before_method('process_source')
def apply_warnings(self):
    level_str = getattr(self, "warning_levels", None)
    if( level_str is None ):
        raise Errors.WafError("WarningLevel: missing warning_levels for {}".format(self.name))

    level_list = str.split(level_str)
    Logs.debug("WarningLevel: levels applied = {}".format(level_str))

    # Use or get the defined levels
    config = config_levels.get(self.name, None)
    if not config:
        config = dict()
        Logs.debug("WarningLevel: Building configs for {}".format(self.name))
        for var in self.env:
            if not var.startswith("warning_level_"):
                continue
            m = LEVEL_RE.match(var)
            if not m:
                raise Errors.WafError("WarningLevel: Invalid level: {}".format(var))

            g = m.groups()
            if len(g) != 2:
                raise Errors.WafError("WarningLevel: Invalid level: {}".format(var))

            level = g[0]
            flags = g[1]
            if level not in config:
                config[level] = dict()

            config[level][flags] = self.env[var]

        config_levels[self.name] = config

    # Apply settings applicable to this file
    for level_to_apply in level_list:
        Logs.debug("WarningLevel: Applying state: {}".format(level_to_apply))
        if level_to_apply not in config:
            raise Errors.WafError("WarningLevel: Undefined level: {}".format(level_to_apply))
        for flag, value in config[level_to_apply].items():
            Logs.debug( "WarningLevel:   {}={}".format(flag, value))
            self.env.append_unique( flag, value )
