# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 1.1/GPL 2.0/LGPL 2.1
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is the Mozilla build system.
#
# The Initial Developer of the Original Code is
# the Mozilla Foundation <http://www.mozilla.org/>.
# Portions created by the Initial Developer are Copyright (C) 2008
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   Robert Kaiser <kairo@kairo.at> (Initial Code)
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

MOZ_SUITE = 1
DEFINES += -DMOZ_SUITE=1

# On trunk we need to link the windows shell service into seamonkey.exe
ifdef BUILD_STATIC_LIBS
ifndef MOZILLA_1_9_1_BRANCH
BUILD_STATIC_SHELL = 1
DEFINES += -DBUILD_STATIC_SHELL=1
endif
endif

# Make the whole tree rebuild if app-config.mk changes
# Use MOZ_BUILD_APP to make life easy
ifeq (,$(wildcard $(topsrcdir)/$(MOZ_BUILD_APP)/app-config.mk))
#Fail if normal means of finding app-config.mk does not work
$(error Somehow we got included but we can't find ourselves...)
else
GLOBAL_DEPS += $(topsrcdir)/$(MOZ_BUILD_APP)/app-config.mk
endif

