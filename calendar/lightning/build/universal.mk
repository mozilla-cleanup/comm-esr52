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
# The Original Code is the Calendar code
#
# The Initial Developer of the Original Code is
#  Michiel van Leeuwen <mvl@exedo.nl>.
# Portions created by the Initial Developer are Copyright (C) 2007
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   Philipp Kewisch <mozilla@kewis.ch>
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

ifndef OBJDIR
OBJDIR_ARCH_1 = $(MOZ_OBJDIR)/$(firstword $(MOZ_BUILD_PROJECTS))
OBJDIR_ARCH_2 = $(MOZ_OBJDIR)/$(word 2,$(MOZ_BUILD_PROJECTS))
DIST_ARCH_1 = $(OBJDIR_ARCH_1)/mozilla/dist
DIST_ARCH_2 = $(OBJDIR_ARCH_2)/mozilla/dist
DIST_UNI = $(DIST_ARCH_1)/universal
OBJDIR = $(OBJDIR_ARCH_1)
endif

include $(OBJDIR)/config/autoconf.mk

postflight_all:
	mkdir -p $(DIST_UNI)/xpi-stage
	rm -rf $(DIST_UNI)/xpi-stage/lightning*
	cp -R $(DIST_ARCH_1)/xpi-stage/lightning $(DIST_UNI)/xpi-stage
	rm -f $(DIST_UNI)/xpi-stage/lightning/components/components.manifest
	platform=`$(PYTHON) $(TOPSRCDIR)/calendar/lightning/build/get-platform.py \
		$(DIST_ARCH_1)/xpi-stage/lightning`; \
	mkdir -p $(DIST_UNI)/xpi-stage/lightning/components/$$platform; \
	mv $(DIST_UNI)/xpi-stage/lightning/components/*.dylib \
		$(DIST_UNI)/xpi-stage/lightning/components/$$platform; \
	$(foreach dylib,$(wildcard $(DIST_ARCH_1)/xpi-stage/lightning/components/*.dylib),echo binary-component $$platform/$(notdir $(dylib)) ABI=$$platform >> $(DIST_UNI)/xpi-stage/lightning/components/components.manifest)
	platform=`$(PYTHON) $(TOPSRCDIR)/calendar/lightning/build/get-platform.py \
		$(DIST_ARCH_2)/xpi-stage/lightning`; \
	mkdir -p $(DIST_UNI)/xpi-stage/lightning/components/$$platform; \
	cp $(DIST_ARCH_2)/xpi-stage/lightning/components/*.dylib \
		$(DIST_UNI)/xpi-stage/lightning/components/$$platform; \
	$(foreach dylib,$(wildcard $(DIST_ARCH_2)/xpi-stage/lightning/components/*.dylib),echo binary-component $$platform/$(notdir $(dylib)) ABI=$$platform >> $(DIST_UNI)/xpi-stage/lightning/components/components.manifest)
	$(PYTHON) $(TOPSRCDIR)/build/merge-installrdf.py \
		$(DIST_ARCH_1)/xpi-stage/lightning \
		$(DIST_ARCH_2)/xpi-stage/lightning \
		> $(DIST_UNI)/xpi-stage/lightning/install.rdf
	cd $(DIST_UNI)/xpi-stage/lightning && $(ZIP) -qr ../lightning.xpi *
